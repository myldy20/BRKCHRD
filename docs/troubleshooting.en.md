# BRKCHRD 0.5.1 Troubleshooting — English

## PortMaster entry is missing

Verify the launcher and executable:

```bash
ssh root@DEVICE_IP '
  ls -l /userdata/roms/ports/BRKCHRD.sh \
        /userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
  chmod +x /userdata/roms/ports/BRKCHRD.sh \
           /userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
  sync
'
```

Refresh the game list or reboot. The release archive must be extracted into `/userdata/roms/ports/`, not copied there unopened.

## NextUI card or image is missing

Verify exact case-sensitive paths:

```text
Tools/tg5040/BRKCHRD.pak/launch.sh
Tools/tg5040/.media/BRKCHRD.png
```

The image is a sibling of the Pak, not a file inside it.

## Immediate exit

PortMaster log:

```bash
ssh root@DEVICE_IP 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

NextUI log:

```text
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

Common causes are a missing executable, wrong architecture, missing execute permission, SDL initialisation failure or an incomplete extraction.

## L1 and L2 are reversed

Expected 0.5.1 roles:

```text
L1 press = cycle CHORD / SOUND / PERF FX
L2 hold  = alternate layer
```

Toggle `SWAP L1/L2` in Settings when the physical firmware order is reversed. `SWAP R1/R2` performs the equivalent correction on the right side.

## L1 does not show PERF FX

Check `PERF FX` in Settings. When it is Off, L1 intentionally cycles CHORD ↔ SOUND only. Turning it On restores the three-screen cycle.

## A chord changes octave depending on the route

Check `VOICE LEAD`:

- Off: the same button, bank, colour, octave and preset must always return the same notes;
- On: route-sensitive inversions are intentional, though the result remains anchored near the selected octave.

For an exact octave test, turn Voice Lead Off and compare the same button before and after one front octave press. Every note should move twelve semitones.

## A DARK chord sounds dissonant

Several DARK colours are deliberately tense: Cluster, Tritone, Crunch and augmented structures are not intended to sound consonant on every preset. Compare BASE or CLASSIC before reporting a construction problem.

A useful chord bug report includes key, face button, base/R1/R2 bank, palette, D-pad direction, octave, Voice Lead state and preset.

## Preset text is clipped or a value bar disappears

These were 0.5.0 UI defects corrected in 0.5.1. Confirm the splash/header reports 0.5.1 and replace the executable if it still reports 0.5.0.

In 0.5.1:

- Preset uses a large dedicated card;
- values have right padding;
- an active parameter has a white bar;
- Settings says `SAVE AND EXIT`.

## Chord colour disappears in SOUND

SOUND does not reset the stored colour. Return to CHORD and inspect the active label. A selected colour returns to BASE only when the same direction is pressed again while its palette is active.

Holding L2 only displays the alternate palette; it does not change the stored colour until a direction is pressed.

## Performance FX stay active

The base chain should be restored when:

- D-pad returns to centre;
- L1 leaves PERF FX;
- PERF FX is disabled;
- Select opens Settings;
- the application exits normally.

Opening Settings forces restoration. Include the exact direction and log if a gesture remains stuck.

## Reverse-ish or Shimmer-ish are not literal

These labels describe musical gestures made from the current lightweight Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb blocks. They are not dedicated reverse-buffer or pitch-shimmer processors.

## Wrong ABXY position lights up

Expected physical layout:

```text
        X
    Y       A
        B
```

Knulli may expose different SDL logical letters. BRKCHRD translates by physical position. If the wrong screen position responds, attach the opening controller mapping and raw input lines from the log.

## All-notes-off

Hold Start for about 0.85 seconds. This stops voices immediately. There is no latch mode.

## Crackles, overheating or high CPU

1. set FX1 and FX2 to Off;
2. turn UI Motion Off;
3. use PAD with a short Release and BASE chord;
4. compare a three-note Keys preset with a wide Pad or PERF FX;
5. test headphones and the built-in speaker separately;
6. report preset, chord, play mode, effects, hardware volume and observed CPU load.

## Reset configuration

PortMaster:

```bash
ssh root@DEVICE_IP '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.bak
'
```

NextUI: rename `SD/.userdata/tg5040/brkchrd/brkchrd.cfg`.

## Useful issue report

Include device, firmware/NextUI version, exact release archive, log, configuration or relevant settings, physical input sequence, expected result, actual result, output route, and photo/video for layout defects. Describe a minimal reproducible sequence rather than only the final sound.
