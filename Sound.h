/* Sound.h
    Copyright (c) 2007-2012 by Timothy J. Weber, tw@timothyweber.org.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


	Sound generation through an arbitrary port pin, using a timer.
      Requires Timer 1, and sole use of the interrupt handler for the required deterministic latency.
	
	Requires SoundConsts.h defined, from the template.
*/

#ifndef __SOUND_H
#define __SOUND_H

// Starts playing a square wave with the given period (in microseconds) .
// Will continue playing until this is called again with a different period.
// A period of 0 indicates silence.
void StartSound(unsigned short periodUs);

// Starts playing a square wave with the given period (in microseconds) .
// After the given duration (in milliseconds), the sound will end.
// Requires calling UpdateSoundMs once per millisecond.
void PlaySound(unsigned short periodUs, unsigned short durationMs);

// Call this from the interrupt handler to process the timer interrupt.
// Best if nothing else is called from the interrupt handler.
void SoundInterrupt(void);

// Call this once per millisecond, to advance the timer for sound durations.
// Can be called from an interrupt handler.
void UpdateSoundMs(void);

// Call this AS WELL, pretty frequently, but not from an interrupt handler.
void UpdateSong(void);

// Makes a short click.
void PlayClick(void);

// Starts playing a song written out in a string of note names.
// Plain letters signify diatonic (C major) notes over two octaves,
// lowercase for the lower octave (c-b) and uppercase for the upper octave (C-B).
// E.g., an ascending scale is "cdefgabC".
// The default duration is an eighth note at 120 bpm.
// Duration is changed for subsequent notes using '8', '4', '2', and '1'
// for eighth, quarter, half, and whole notes (4/4), or '6' for sixteenths or '3' for thirty-seconds.
// A dot after the duration signifies a dotted note, and is reset for the next note automatically.
// Accidentals can be prefixed as '+' for sharp and '-' for flat,
// and affect only the following note.
// A space (' ') signifies a rest, and uses the current duration.
// Default is staccato; can be changed with '_' for legato and '^' for detached, which persist.
void PlaySong(const char* song);

// Returns true if we're currently playing an individual sound, or if we're in silence that's part of a song.
unsigned char IsSoundPlaying(void);

// Stops playing all sounds and songs.
void TurnOffAllSound(void);

#endif
