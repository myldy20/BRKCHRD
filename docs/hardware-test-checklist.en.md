# BRKCHRD 0.2 hardware test checklist

This checklist is intended for real-device validation on a TrimUI Brick running Knulli. It complements the automated Linux and AArch64 tests: CI can prove that the program builds, starts and packages correctly, but only the handheld can confirm controller mapping, latency, screen readability and audio stability.

## 1. Installation and launch

- Extract the PortMaster archive into `/userdata/roms/ports/`.
- Confirm that these files exist:
  - `/userdata/roms/ports/BRKCHRD.sh`
  - `/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64`
- Confirm that both files are executable.
- Refresh the game list or reboot.
- Launch **Ports → BRKCHRD**.
- Confirm that the application reaches the PLAY page without returning to the frontend.
- Confirm that the first audible chord does not require opening a settings screen.

## 2. Display and layout

Inspect every page: PLAY, SOUND, FX and SYSTEM.

- No text overlaps another label, value, panel border or footer.
- Long engine and effect names remain inside their assigned fields.
- The active page, active row and current value are visually obvious.
- The selected chord and armed D-pad colour are readable from normal handheld distance.
- The 512×384 logical layout scales cleanly to the Brick's 1024×768 display.
- The bottom help line changes with the current page and does not cover content.
- Rapid page changes do not leave stale text or visual fragments.

Record the page name and exact labels if any overlap remains.

## 3. Face buttons and chord banks

Test A, B, X and Y on every page.

- Each face button starts a chord immediately.
- Releasing the button releases the chord when latch is off.
- Holding L while pressing a face button selects the secondary diatonic bank.
- Holding R while pressing a face button selects the borrowed bank.
- Fast changes between face buttons do not leave stuck notes.
- Voice leading avoids unnecessary octave jumps between common progressions.

Suggested progression tests:

- A → B → X → Y
- A → Y → B → A
- L+A → L+B → A
- R+A → R+B → Y

## 4. D-pad colour workflow

On the PLAY page, the D-pad must control chord colour only. It must not change key or octave.

- Press Up with no chord held, then press A: the first chord uses the armed colour.
- Repeat for all eight directions, including diagonals.
- Press the same direction again: colour returns to BASE.
- Arm POWER and verify that it can be the first chord played.
- Hold a chord and change direction: the sounding chord updates without a stuck previous chord.
- The armed direction remains visible after releasing the chord.
- Key and octave remain unchanged during all PLAY-page D-pad tests.

## 5. Glowing FX buttons

Knulli may expose the glowing face-panel FX buttons as L3/R3 buttons or as trigger axes. BRKCHRD accepts both representations.

- FX1 selects the EXTENDED colour palette.
- FX2 selects the DARK colour palette.
- FX1 + FX2 returns to CLASSIC and BASE.
- The on-screen palette indicator follows the physical buttons.
- The buttons work both before and during a held chord.
- No unrelated rear button is required for this function.

If a button does not respond, record which physical button was pressed and attach the controller log described below.

## 6. Page navigation and performance controls

With no chord held:

- Tap L to move to the previous page.
- Tap R to move to the next page.
- Holding L or R for a chord bank must not accidentally change page.
- Start cycles PAD, STRUM, ARP and PULSE.
- Select toggles latch.
- Start + Select saves settings and exits cleanly.

After restarting, confirm that key, octave, engine parameters, effects, palette, armed direction, BPM, play mode and latch state were restored as documented.

## 7. Sound engines

Test all ten engines with the same chord and with effects temporarily disabled:

1. Velvet Poly
2. Dust Piano
3. Chapel Organ
4. String Haze
5. Tape Choir
6. Wire Pluck
7. Frozen Glass
8. Doom Stack
9. Smoke Reed
10. Sub Altar

For each engine, check:

- It has a clearly different attack, spectrum or movement from adjacent engines.
- Tone, Body and Motion produce useful changes across most of their range.
- Attack and Release reach both short and sustained settings without clicks.
- Spread creates width without obvious phase cancellation on the built-in speaker.
- Low notes remain controlled and do not produce permanent limiter pumping.
- High notes do not become painfully loud compared with low notes.

Note the engine and parameter values for any weak, redundant or unpleasant range.

## 8. Effects

Test FX1 and FX2 separately, then in series.

Available algorithms:

- Off
- Chorus
- Phaser
- Tremolo
- Drive
- Crusher
- Delay
- Reverb

For each slot:

- Type changes immediately without a burst or stuck tail.
- Amount moves from effectively dry to clearly audible.
- Colour changes a meaningful algorithm-specific parameter.
- Delay repeats follow BPM changes.
- Reverb and delay tails remain audible after chord release.
- Drive and Crusher do not create uncontrolled DC offset or extreme volume jumps.
- Two active effects do not cause persistent crackling or UI lag.

## 9. Audio latency and stability

Use headphones if possible and also test the built-in speaker.

- Note the perceived delay between button press and sound.
- Play repeated short chords in PAD and STRUM modes.
- Hold six-note extended chords while switching pages.
- Run ARP and PULSE for at least five minutes.
- Change engine and effects during sustained playback.
- Listen for underruns, crackles, periodic gaps, stuck notes and sudden level jumps.
- Confirm that exiting stops audio and returns control to Knulli.

Record whether problems occur on speaker, headphones or both.

## 10. Performance and thermal check

Run BRKCHRD continuously for at least 15 minutes.

- UI remains responsive.
- No increasing audio latency appears over time.
- Memory use does not grow continuously.
- The device does not become unusually hot.
- Battery drain is reasonable for an active audio application.
- Returning from Knulli suspend does not leave BRKCHRD or audio in a broken state.

## 11. Logs to collect

Primary runtime log:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

Saved settings:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
```

Useful commands:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log ~/Downloads/
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.cfg ~/Downloads/
```

For a controller-mapping problem, also run the controller diagnostic described in the troubleshooting guide and include its complete output.

## 12. Recommended bug report format

```text
BRKCHRD version:
Knulli version:
Page:
Physical control:
Expected result:
Actual result:
Reproduction steps:
Does it happen every time:
Speaker / headphones:
Relevant engine and parameter values:
Relevant FX settings:
Attached log files:
Photo or video:
```

One reproducible problem per report is preferable to a long mixed list. Include a short video for controller or layout issues whenever possible.
