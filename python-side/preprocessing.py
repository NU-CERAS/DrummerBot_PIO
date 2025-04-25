import mido
from mido import Message, MidiFile, MidiTrack
from collections import defaultdict

# Processes midi file and pushes back drum hits by given amount so they create sound at the right time
def preprocess_midi(mid, drum_delays, max_delta):        
    adjusted_events = defaultdict(list)
    current_time = 0
    for msg in mid:
        current_time += msg.time
        if msg.type == 'note_on':  
            note = msg.note
            if note in drum_delays:
                adjusted_time = current_time - drum_delays[note]
                adjusted_events[note].append((adjusted_time, msg.velocity))

    all_events = []
    for note, events in adjusted_events.items():
        for time, velocity in events: # events is a list of the adjusted time and velocity 
            all_events.append((time, note, velocity))

    all_events.sort()
    commands = []
    for time, note, velocity in all_events:
        commands.append([round(time + max_delta, 5), note, velocity])
    return commands

# Creates midi file based on commands
def createProcessedMidi(commands, bpm):
    midi = MidiFile()
    track = MidiTrack()
    midi.tracks.append(track)
    prev_time = commands[0][0]

    for time, note, velocity in commands:
        ticks = int((time - prev_time) * midi.ticks_per_beat * bpm / 60)
        print("Ticks: " + str(ticks) + "\t" + "Note: " + str(note) + "\t" + "Velocity: " + str(velocity))

        if velocity > 0:  # Note on
            track.append(Message('note_on', channel=9, note=note, velocity=velocity, time=ticks))
        else:  # Note off
            track.append(Message('note_off', channel=9, note=note, velocity=0, time=ticks))
        prev_time = time

    midi.save('OUTPUT.mid')

# Finds all Note numbers used in midi track
def getNotes(mid):
    notes = set() 
    for msg in mid:        
        if msg.type == 'note_on':
            note = msg.note
            notes.add(note)
    return notes

# Prints out motor commands in Time, Note, Velocity format
def printCommands(commands):
    for time, note, velocity in commands:
        print("Time: " + str(time) + "\t" + "Note: " + str(note) + "\t" + "Velocity: " + str(velocity))

# Returns bpm for midi track. If none is found, returns a default bpm
def getBPM(midi_file):
    for track in midi_file.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                return mido.tempo2bpm(msg.tempo)
    return 120 # if nothing found, return 120bpm 

def main():

    # MIDI FILE PATH
    midi_file = mido.MidiFile(r'Latin9.mid')  

    # ADD MOTOR-TO-SOUND DELAYS HERE (in milliseconds)
    drum_delays = {
        36: 0, # Bass Drum 1
        37: 0, # Side Stick
        40: 0, # Electric Snare
        41: 0, # Low Floor Tom
        42: 0, # Closed Hi Hat
        43: 0, # High Floor Tom
        45: 0, # Low Tom
        51: 0  # Ride Cymbal 1 
    }
    max_delta = max(drum_delays.values())
    
    commands = preprocess_midi(midi_file, drum_delays, max_delta)
    createProcessedMidi(commands, getBPM(midi_file))    

if __name__ == "__main__":
    main()