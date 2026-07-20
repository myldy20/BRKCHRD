# BRKCHRD 0.4.0

Extract this package into the PortMaster ports directory. Full English and Russian documentation is included under `brkchrd/docs/`.

TrimUI Brick controls:

- glowing front left/right: octave down/up;
- hold L1: alternate layer for CHORD, SOUND or PERF FX;
- press L2: cycle CHORD → SOUND → PERF FX;
- hold R1/R2: configured alternate chord banks;
- D-pad: select chord colour, edit sound or hold momentary performance FX;
- ABXY: play functional chords at all times;
- Select: open/close Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

Settings configure the default/L1 colour palettes, base/R1/R2 chord banks, two base effect slots, key, octave, UI motion and rear-button swaps.

PERF FX temporarily replaces the two base effects only while the D-pad is held. Releasing the D-pad, changing mode or opening Settings restores the saved base chain.

Runtime files:

```text
brkchrd/conf/brkchrd.cfg
brkchrd/conf/brkchrd.log
```

The log includes the SDL mapping, raw-input trace and expected v0.4 roles.
