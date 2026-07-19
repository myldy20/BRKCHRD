# Troubleshooting — English

## The application does not appear under Ports

Check the launcher path and executable bit:

```bash
ssh trimui 'ls -l "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64'
```

Repair permissions:

```bash
ssh trimui 'chmod +x "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64 && sync'
```

Refresh the game list or reboot.

## Immediate exit

Read the log:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Typical causes are a missing binary, incompatible architecture, SDL audio/video initialisation failure or an incomplete PortMaster control environment.

## Front or rear buttons perform the wrong action

Version 0.3 expects the mapping observed on TrimUI Brick with Knulli:

- glowing front-left button — octave down;
- glowing front-right button — octave up;
- rear L1 and L2 — one left-side channel that advances the left-panel mode;
- rear R1 and R2 — one right-side channel that advances the right-panel mode.

Inspect the beginning of the log:

```bash
ssh trimui 'sed -n "1,120p" /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Look for:

```text
controller:
mapping:
controller button down:
raw button down:
raw axis:
```

BRKCHRD records only a limited number of raw events so the log cannot grow indefinitely.

## X/Y or A/B labels appear swapped

Version 0.3 displays the physical Brick labels:

```text
        X
    Y       A
        B
```

Internally, Knulli may expose bottom B as SDL A, right A as SDL B, left Y as SDL X and top X as SDL Y. That is expected: BRKCHRD translates the positional SDL mapping back to the letters printed on the device.

If pressing a physical position highlights a different on-screen position, attach `brkchrd.log`; that indicates another firmware mapping rather than a label issue.

## The D-pad edits the wrong side

The D-pad belongs to the most recently selected side. A bright double outline shows focus:

- after a rear L press, the left panel is focused;
- after a rear R press, the right panel is focused.

In CLASSIC / EXTENDED / DARK and chord-bank views, the D-pad selects chord colour. In SOUND / SYSTEM / FX it selects and edits parameters.

## Return to BASE

In a colour view, press the currently armed D-pad direction again. There is no separate RESET COLOUR row in version 0.3.

## Emergency all-notes-off

Hold Select for approximately 0.85 seconds. BRKCHRD disables latch, clears held-chord state, stops every voice and displays `ALL NOTES OFF`.

## Crackles or high CPU use

- reduce effect amounts, especially two delays/reverbs in series;
- avoid maximum Release with dense six-note extended chords during initial testing;
- test once with both effects Off;
- compare the built-in speaker with headphones;
- report engine, chord colour, play mode, effect settings and observed load.

Interface animation does not run in the audio graph, but SYSTEM → UI MOTION → OFF is available for a clean comparison.

## Reset configuration

```bash
ssh trimui 'mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.bak'
```

The next launch uses factory defaults.

## Useful bug report

Include:

- Knulli version;
- exact archive name;
- `brkchrd.log`;
- the response of every physical button;
- a photo of any clipped or overlapping text;
- speaker or headphones;
- approximate latency;
- crackles and CPU load;
- active left- and right-panel modes.
