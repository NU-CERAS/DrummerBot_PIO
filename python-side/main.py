import sys
import mido
from bisect import bisect_right
from collections import deque

DEFAULT_TEMPO_USPB = 500_000  # 120 BPM (already in your file)

def adjust_vel(vel):
    if(vel < 40):
        return 40
    elif (vel > 120):
        return 120
    else:
        return vel

def velocity_delay_map(vel):
    return 0.5*(vel-40) + 30

def is_off(msg):
    # Treat explicit note_off and note_on with velocity 0 as OFF
    return (msg.type == "note_off") or (msg.type == "note_on" and msg.velocity == 0)

def is_on(msg):
    return (msg.type == "note_on" and msg.velocity > 0)

def build_tempo_map(mid):
    """Return sorted list of (tick, us_per_beat). Starts with default at tick 0."""
    changes = [(0, DEFAULT_TEMPO_USPB)]
    for track in mid.tracks:
        t = 0
        for msg in track:
            t += msg.time
            if msg.type == "set_tempo":
                changes.append((t, msg.tempo))
    changes.sort(key=lambda x: x[0])
    # collapse duplicates at same tick (keep last)
    out = []
    for tick, uspb in changes:
        if out and out[-1][0] == tick:
            out[-1] = (tick, uspb)
        else:
            out.append((tick, uspb))
    return out

def tempo_at_tick(tempo_map, tick):
    idx = bisect_right([t for t, _ in tempo_map], tick) - 1
    if idx < 0: idx = 0
    return tempo_map[idx][1]

def ms_to_ticks(ms, ticks_per_beat, us_per_beat):
    return int(round(mido.second2tick(ms / 1000.0, ticks_per_beat, us_per_beat)))
    adjusted = list(adjusted)  # shallow copy so we can edit tuples

def shorten_prev_if_overlap(adjusted, pad_ms=10, tempo_map=None, ticks_per_beat=None):
    """
    For each note, if a new ON happens before the previous OFF, shorten that previous OFF to
    (new_ON_tick - pad_ms) using the local tempo at that ON's tick.

    Handles multiple overlaps by queueing a cutoff for each new ON that arrives while the note
    is still 'open'. Each OFF consumes one pending cutoff (FIFO).
    """
    if tempo_map is None or ticks_per_beat is None:
        raise ValueError("shorten_prev_if_overlap needs tempo_map and ticks_per_beat when using pad_ms.")

    adjusted = list(adjusted)

    # Most recent ON for this note (the *current* instance)
    current_on = {}  # note -> tick

    # For each note, a FIFO of desired cutoff ticks for the *previous* instances
    pending_cutoffs = {}  # note -> deque([cut_tick1, cut_tick2, ...])

    for i, (tick, msg) in enumerate(adjusted):
        if is_on(msg):
            note = msg.note
            if note in current_on:
                # There's already an open instance; create a cutoff for that previous one
                uspb = tempo_at_tick(tempo_map, tick)
                pad_ticks = ms_to_ticks(pad_ms, ticks_per_beat, uspb)
                desired = max(0, tick - pad_ticks)

                # Clamp so we never end before that previous instance actually started
                desired = max(desired, current_on[note])

                dq = pending_cutoffs.setdefault(note, deque())
                dq.append(desired)

            # This ON becomes the current instance
            current_on[note] = tick

        elif is_off(msg):
            note = msg.note
            dq = pending_cutoffs.get(note)

            if dq and len(dq) > 0:
                # This OFF closes the *previous* instance; apply the queued cutoff
                new_off_tick = dq.popleft()
                # Only move earlier, never later
                if new_off_tick <= tick:
                    adjusted[i] = (new_off_tick, msg)
                # Do NOT clear current_on; the current instance (latest ON) is still open
            else:
                # No pending cutoff -> this OFF closes the current instance
                current_on.pop(note, None)

    # After moving OFFs, re-sort to keep OFFs before ONs at ties
    adjusted.sort(key=lambda x: (x[0], 0 if is_off(x[1]) else 1))
    return adjusted

def collect_note_events(mid):
    """Return a list of (abs_tick, mido.Message) for ON/OFF notes across all tracks."""
    events = []
    for track in mid.tracks:
        t = 0
        for msg in track:
            t += msg.time
            if msg.type in ("note_on", "note_off"):
                events.append((t, msg.copy(time=0)))
    events.sort(key=lambda x: (x[0], 0 if x[1].type == "note_off" else 1))
    return events


def write_notes(events, ticks_per_beat, out_path, mid):
    """Write notes to a new MIDI with a default tempo and a single note track."""
    out = mido.MidiFile(type=1, ticks_per_beat=ticks_per_beat)
    meta = mido.MidiTrack()
    notes = mido.MidiTrack()
    out.tracks.append(meta)
    out.tracks.append(notes)

    # simple default tempo so timing is well-defined
    meta.append(mido.MetaMessage("set_tempo", tempo=DEFAULT_TEMPO_USPB, time=0))

    tempo_map = build_tempo_map(mid) if mid is not None else [(0, DEFAULT_TEMPO_USPB)]

    #Shift note ONs by their respective vel using the local tempo at that tick
    adjusted = []
    for abs_tick, msg in events:
        new_tick = abs_tick
        if msg.type == "note_on" and msg.velocity > 0:
            uspb = tempo_at_tick(tempo_map, abs_tick)
            LEAD_MS = velocity_delay_map(adjust_vel(msg.velocity))
            lead_ticks = ms_to_ticks(LEAD_MS, ticks_per_beat, uspb)
            new_tick = max(0, abs_tick - lead_ticks)
        adjusted.append((new_tick, msg.copy(time=0)))

    # after you filled `adjusted = [(new_tick, msg.copy(time=0)) ...]`
    adjusted.sort(key=lambda x: (x[0], 0 if is_off(x[1]) else 1))
    adjusted = shorten_prev_if_overlap(
        adjusted,
        pad_ms=10,
        tempo_map=tempo_map,
        ticks_per_beat=ticks_per_beat
        )

# then convert to deltas and write out, as you already do
    last_tick = 0
    for abs_tick, msg in adjusted:
        delta = max(0, abs_tick - last_tick)
        last_tick = abs_tick
        notes.append(msg.copy(time=delta))


    # end-of-track markers
    meta.append(mido.MetaMessage("end_of_track", time=0))
    notes.append(mido.MetaMessage("end_of_track", time=0))
    out.save(out_path)

def main():
    if len(sys.argv) != 3:
        print("Usage: python main.py input.mid output.mid")
        sys.exit(1)
    in_path, out_path = sys.argv[1], sys.argv[2]
    mid = mido.MidiFile(in_path)
    events = collect_note_events(mid)
    write_notes(events, mid.ticks_per_beat, out_path, mid)
    print(f"Cloned {len(events)} note events → {out_path}")

if __name__ == "__main__":
    main()
