import sys
import mido

DEFAULT_TEMPO_USPB = 500_000  # 120 BPM

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

def write_notes(events, ticks_per_beat, out_path):
    """Write notes to a new MIDI with a default tempo and a single note track."""
    out = mido.MidiFile(type=1, ticks_per_beat=ticks_per_beat)
    meta = mido.MidiTrack()
    notes = mido.MidiTrack()
    out.tracks.append(meta)
    out.tracks.append(notes)

    # simple default tempo so timing is well-defined
    meta.append(mido.MetaMessage("set_tempo", tempo=DEFAULT_TEMPO_USPB, time=0))

    last_tick = 0
    for abs_tick, msg in events:
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
    write_notes(events, mid.ticks_per_beat, out_path)
    print(f"Cloned {len(events)} note events → {out_path}")

if __name__ == "__main__":
    main()
