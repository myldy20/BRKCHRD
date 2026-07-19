# BRKCHRD 0.3.0 manual — English

## What the instrument is

BRKCHRD is not a chord encyclopaedia or a tracker. It is a live harmony instrument for TrimUI Brick. Four face buttons select functional chords, the D-pad colours them, and two independent screen halves expose harmony, synthesis and effects without interrupting performance.

## First minute

1. Launch BRKCHRD. The left panel starts in CLASSIC and the right panel in CORE.
2. Press the physical B, A, Y and X buttons. In CORE they play I, V, vi and IV.
3. Press either rear L button twice: the left panel moves CLASSIC → EXTENDED → DARK.
4. Press D-pad Up. POWER is now armed in DARK.
5. Play any chord: it begins as a power chord immediately.
6. Press Up again to return to BASE.
7. The glowing front-left button lowers the octave; the glowing front-right button raises it.
8. Either rear R button cycles the right panel through chord banks, FX 1, FX 2 and MASTER.
9. Start changes PAD / STRUM / ARP / PULSE. A short Select press toggles latch.

## Two independent panels

The interface always contains two equal panels. The last rear side pressed receives a bright double outline and D-pad focus.

### Left panel

Rear L1 and L2 share the same left-side signal and cycle through:

- CLASSIC;
- EXTENDED;
- DARK;
- SOUND;
- SYSTEM.

CLASSIC, EXTENDED and DARK define eight directional colours plus BASE. SOUND opens the engine and synthesis macros. SYSTEM contains key, octave status and interface motion.

### Right panel

Rear R1 and R2 share the same right-side signal and cycle through:

- CORE;
- DIATONIC+;
- BORROWED;
- FX 1;
- FX 2;
- MASTER.

Entering one of the first three sections selects that bank. The selected bank remains active while effects are edited.

## Chord buttons

The Brick cap layout is:

```text
        X
    Y       A
        B
```

The core bank is:

```text
        X = IV
    Y = vi    A = V
        B = I
```

The interface deliberately shows the physical Brick letters rather than SDL's logical names, which Knulli swaps in pairs.

## Chord colour

In any colour mode or chord-bank view, the D-pad can arm a colour before the next chord. Press the same direction again to return to BASE.

While a chord is held, D-pad movement changes its colour immediately and temporarily. Releasing the direction returns to the armed colour.

### CLASSIC

Standard minor/major flip, sevenths, add9, sus, sixth, diminished and augmented variants.

### EXTENDED

ADD11, DOM9, 6/9, MIN11, 7SUS, HALF-DIM, mMAJ7 and LYDIAN.

### DARK

POWER, CRUNCH 7#9, QUARTAL, CLUSTER, OPEN, MIN6, TRITONE and AUGMAJ7.

## SOUND

Rows:

- ENGINE;
- TONE;
- BODY;
- MOTION;
- ATTACK;
- RELEASE;
- SPREAD;
- BPM;
- PLAY MODE.

Up/Down selects a row and Left/Right edits it. Holding Left/Right accelerates. ABXY remain playable, so every change can be auditioned immediately.

## FX 1 and FX 2

Each serial effect slot exposes:

- TYPE — algorithm;
- AMOUNT — depth;
- COLOUR — algorithm-specific second dimension.

Available algorithms: Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb.

## MASTER

MASTER controls final level after both effects and visualises:

```text
SYNTH → FX 1 → FX 2 → MASTER
```

## SYSTEM

- KEY — tonal centre;
- OCTAVE — current shift, changed by the glowing front controls;
- UI MOTION — OFF / LOW / FULL;
- VERSION — application version.

RESET COLOUR and PANIC menu rows no longer exist. BASE is restored by tapping the armed D-pad direction again. Hold Select for an emergency stop.

## Latch and all-notes-off

A short Select press toggles latch. With latch enabled, the last chord continues after ABXY are released.

Holding Select for approximately 0.85 seconds:

- disables latch;
- stops every voice;
- clears held-chord state;
- displays `ALL NOTES OFF`.

## Voice leading

Each new chord is inverted and octave-shifted to reduce movement from the previous voicing. The displayed name is the harmonic identity; the sounding voicing may not be root position.

## Save and exit

Start + Select saves:

- key and octave;
- current panel modes;
- bank and palette;
- armed colour;
- engine and synthesis macros;
- both effect slots;
- BPM, play mode and latch;
- interface-motion level.

The application then exits normally.

## Diagnostics

`brkchrd.log` includes the controller name, SDL mapping and a limited raw button/axis trace. If another Knulli release changes the layout, the actual codes can be recovered without a separate diagnostic application.

## Current limitations

- in-app labels are English; all external documentation is bilingual;
- no MIDI input/output yet;
- no user preset slots yet;
- no per-effect delay subdivision selector;
- other handhelds and firmwares remain unverified;
- final speaker voicing, latency and CPU limits require physical testing.
