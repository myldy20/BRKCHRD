# BRKCHRD 0.2.0 manual — English

## Purpose

BRKCHRD is a performance instrument, not a chord encyclopaedia menu. The main workflow is: choose a functional chord, arm a colour, optionally hold an alternate bank, and play. Harmony remains transposable because buttons represent scale functions rather than fixed notes.

## First minute

1. Open PLAY.
2. Press A, B, X and Y to hear I, V, vi and IV.
3. Tap FX2 to choose DARK.
4. Tap D-pad Up to arm POWER.
5. Play any face button: it starts as a power chord immediately.
6. Tap Up again to return to BASE.
7. Tap rear R to open SOUND, select a row with Up/Down and edit it with Left/Right.
8. Tap rear R again for FX.
9. Start changes play mode; Select toggles latch.

## Pages

### PLAY

The left panel shows the current chord-colour wheel. The right panel shows the active chord bank and face-button diamond. The bottom strip shows the sounding chord and the most important performance hints.

### SOUND

Factory preset plus Tone, Body, Motion, Attack, Release, Spread, BPM and Play Mode. Selecting another preset replaces the whole sound state; edit macros after selecting the desired base preset.

### FX

Two serial slots. Type selects the algorithm, Amount controls depth and Colour controls the algorithm's second dimension. Master is after both slots.

### SYSTEM

Key and octave live here deliberately. RESET COLOUR returns CLASSIC + BASE. PANIC clears all voices. ABOUT shows the product version and hardware-input notes.

## Latch behaviour

With latch off, releasing the last face button releases the chord. With latch on, the chord remains until another chord replaces it or PANIC is used. ARP and PULSE keep using the latched note set.

## Voice leading

Each new chord is inverted and octave-shifted to reduce movement from the previous one. The displayed chord name remains the harmonic identity, while the sounding voicing may not be root position.

## Save behaviour

Start+Select exits through the normal path and writes `brkchrd.cfg`. Abrupt power loss or forced termination may lose the latest edits. The file is human-readable and can be backed up over SSH.

## Current limitations

- English in-app labels only in 0.2.0; all external documentation is bilingual.
- no MIDI input/output yet;
- no user preset slots yet;
- no per-effect tempo subdivision selector;
- controller aliases cover the two Brick mappings already observed, but other firmwares remain unverified;
- final speaker voicing and CPU limits require physical testing.
