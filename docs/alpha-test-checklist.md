# BRKCHRD 0.1.0 alpha test checklist

## Launch and audio

- application appears under Ports and reaches the main screen;
- audio starts without crackles or a long delay;
- Start+Select saves configuration and exits cleanly;
- `conf/brkchrd.log` contains no SDL or audio-device errors.

## Controls

- A/B/X/Y trigger I/V/vi/IV;
- L1 and R1 expose the two alternate chord banks;
- L2 and R2 select the extended and dark D-pad palettes;
- all eight D-pad directions work while a chord is held;
- D-pad changes key and octave while no chord is held;
- Start cycles Pad, Strum, Arp and Pulse;
- Select toggles latch;
- L1+R1 and L2+R2 change synth presets.

## Performance

- no stuck notes after quick chord changes;
- no audible buffer underruns during six-note chords and effects;
- voice leading feels smoother than fixed root-position chords;
- UI remains responsive while several notes and effect tails are active.
