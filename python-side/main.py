import sys
import mido
from bisect import bisect_right

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

def resolve_overlaps(adjusted):
    """
    Shorten previous note's OFF to the next ON if they overlap.
    Works per NOTE NUMBER (i.e., one 'actuator' per MIDI note).
    `adjusted` is a list of (abs_tick, msg) sorted by time.
    Returns a new adjusted list (with OFF times possibly pulled earlier),
    then re-sorted (OFF before ON at identical ticks).
    """
    # For each note number, remember the most recent ON and the index of its OFF event (if seen).
    state = {}  # note -> {"last_on_tick": int or None, "off_idx": int or None, "off_tick": int or None}

    # We will modify in place; make a shallow copy of tuples with messages preserved
    adjusted = list(adjusted)

    for i, (tick, msg) in enumerate(adjusted):
        if is_on(msg):
            note = msg.note
            st = state.get(note)
            if st and st.get("off_idx") is not None:
                prev_off_idx = st["off_idx"]
                prev_off_tick = st["off_tick"]
                # If the new ON starts before the previous OFF ends, shorten the previous OFF
                if tick < prev_off_tick:
                    # Replace the previous OFF event's time with the new ON tick
                    prev_off_msg = adjusted[prev_off_idx][1]
                    adjusted[prev_off_idx] = (tick, prev_off_msg)
                # Reset OFF tracking; we’re starting a new note window
                state[note] = {"last_on_tick": tick, "off_idx": None, "off_tick": None}
            else:
                # No prior OFF recorded; just start a new note window
                state[note] = {"last_on_tick": tick, "off_idx": None, "off_tick": None}

        elif is_off(msg):
            note = msg.note
            st = state.get(note)
            # Record where this OFF lives so a future overlapping ON can pull it earlier
            if st is None:
                state[note] = {"last_on_tick": None, "off_idx": i, "off_tick": tick}
            else:
                st["off_idx"] = i
                st["off_tick"] = tick
        else:
            # ignore other events
            pass

    # Re-sort after any OFF-time edits; also enforce OFF before ON when equal
    adjusted.sort(key=lambda x: (x[0], 0 if is_off(x[1]) else 1))
    return adjusted



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


def collect_note_events(mid):
    """Return a list of (abs_tick, mido.Message) for NOTE ON/OFF across all tracks."""
    events = []
    for track in mid.tracks:
        t = 0
        for msg in track:
            t += msg.time
            if msg.type in ("note_on", "note_off"):
                # clone the message with time=0; we’ll set proper deltas later
                events.append((t, msg.copy(time=0)))
    # Sort by absolute tick; at same tick, emit note_off before note_on
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
    adjusted = resolve_overlaps(adjusted)

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
