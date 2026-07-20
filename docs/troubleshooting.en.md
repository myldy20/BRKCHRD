# BRKCHRD 0.4.0 Troubleshooting — English

## The application does not appear under Ports

```bash
ssh trimui 'ls -l "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64'
ssh trimui 'chmod +x "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64 && sync'
```

Refresh the game list or reboot.

## Immediate exit

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Typical causes are a missing binary, incompatible architecture, SDL audio/video initialisation failure or an incomplete PortMaster environment.

## L1 behaves like L2, or R1 behaves like R2

Open Settings and toggle:

- `SWAP L1/L2`
- `SWAP R1/R2`

The confirmed mapping exposes all four rear buttons separately, but another Knulli/controller order may reverse the two controls on one side. The SWAP settings change only BRKCHRD and are safer than global remapping.

Expected v0.4 roles:

```text
left shoulder / right shoulder = front octave controls
left stick button              = L1
left trigger                   = L2
right stick button             = R1
right trigger                  = R2
```

Inspect the first log lines:

```bash
ssh trimui 'sed -n "1,160p" /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Look for `mapping:`, `roles:`, `controller button down:`, `raw button down:` and `raw axis:`.

## L2 does not change D-pad mode

L2 changes mode only on a new press and while Settings is closed. The header should cycle:

```text
CHORD → SOUND → PERF FX
```

If holding the intended L2 instead changes the left layer, turn on `SWAP L1/L2`.

## Chord colour disappears in SOUND

The selected colour should remain stored. SOUND does not intentionally reset it. Return to CHORD and inspect the active label. A colour is reset only by pressing the same direction again while its palette is displayed.

Note that simply holding L1 shows another palette but does not select anything.

## Performance FX remain stuck

PERF FX should restore base FX when:

- the D-pad returns to centre;
- L2 changes mode;
- Select opens Settings;
- the program exits normally.

If a direction appears released but the effect remains, capture `brkchrd.log` and state the exact D-pad direction. Use Start hold for all-notes-off; opening Settings also forces FX restoration.

## Performance FX sound unlike the label

The v0.4 labels describe musical gestures. Reverse-ish and Shimmer-ish are combinations of the current Delay/Reverb/Crusher/Chorus blocks, not dedicated reverse-buffer or pitch-shimmer algorithms. This is a known DSP limitation rather than a controller error.

## ABXY labels or positions are wrong

The intended physical layout is:

```text
        X
    Y       A
        B
```

Internally Knulli may expose bottom B as SDL A, right A as SDL B, left Y as SDL X and top X as SDL Y. BRKCHRD translates by position. If the wrong on-screen position lights up, attach the log; that indicates another firmware mapping.

## Return to BASE

In CHORD mode, display the palette containing the stored direction and press that direction again. If the stored colour came from the L1 palette, hold L1 while pressing it again.

## All-notes-off

Hold Start for approximately 0.85 seconds. Latch no longer exists in 0.4.

## Crackles or high CPU use

- test with base FX1/FX2 set to Off;
- avoid maximum Release with dense extended chords during initial testing;
- compare normal operation with PERF FX held;
- test UI Motion at Off;
- compare built-in speaker and headphones;
- report engine, chord colour, bank, play mode, D-pad direction and CPU load.

## Reset configuration

```bash
ssh trimui 'mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.bak'
```

The next launch uses factory defaults.

## Useful bug report

Include Knulli version, exact archive name, `brkchrd.log`, physical button and expected role, active D-pad mode, L1 state, R1/R2 state, stored chord colour, base FX settings, speaker/headphones, latency, crackles, CPU load and a photo/video for layout or control problems.
