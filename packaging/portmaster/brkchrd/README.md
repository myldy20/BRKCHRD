# BRKCHRD 0.5.0

Extract this package into the PortMaster ports directory. Full English and Russian documentation is included under `brkchrd/docs/`.

TrimUI Brick controls:

- glowing front left/right: octave down/up;
- **press L1:** cycle CHORD → SOUND → PERF FX;
- **hold L2:** alternate colour, sound or performance-FX layer;
- hold R1/R2: configured alternate chord banks;
- D-pad: select chord colour, edit sound or hold momentary performance FX;
- ABXY: play functional chords;
- Select: open/close Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

New Settings options:

- `VOICE LEAD = OFF`: deterministic register; octave buttons move every note exactly 12 semitones;
- `VOICE LEAD = ON`: route-sensitive nearby inversions, anchored to the selected octave;
- `PERF FX = OFF`: remove PERF FX from the L1 cycle;
- base/L2 colour palettes, base/R1/R2 chord banks, two base FX slots, key, octave, UI motion and rear-button swaps.

Sixteen factory presets use instrument-aware chord spacing. Pad/Choir can use wide extensions, Heavy removes muddy low extensions and Bass uses root/fifth/octave.

The second output meter has been removed. Normal launch shows a two-second Myldy design splash.

Runtime files:

```text
brkchrd/conf/brkchrd.cfg
brkchrd/conf/brkchrd.log
```
