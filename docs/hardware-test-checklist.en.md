# BRKCHRD 0.4.0 Hardware Test Checklist

Automated CI proves that the code builds, starts and packages. This checklist validates the real TrimUI Brick controls, screen, latency, DSP behaviour and thermal stability.

## 1. Install and launch

- Install `brkchrd-v0.4.0-portmaster.zip` into `/userdata/roms/ports/`.
- Confirm `BRKCHRD.sh` and `brkchrd-sdl.aarch64` are executable.
- Launch **Ports → BRKCHRD**.
- Confirm the header shows `V0.4.0` and `CHORD`.
- Confirm ABXY plays immediately.

## 2. Layout and typography

Check CHORD, SOUND, PERF FX and Settings.

- Left and right performance panels remain equal.
- All eight directional cells plus the centre cell are equal.
- All four chord buttons are equal and match physical X/Y/A/B positions.
- Long preset/effect names stay inside fields.
- Microtext remains compact and readable.
- Footer labels do not overlap.
- Toast messages remain inside their box.
- FULL, LOW and OFF UI motion do not leave stale pixels.

Photograph any overlap and record the active D-pad mode and L1 state.

## 3. Physical face buttons

Expected CORE layout:

```text
        X = IV
Y = vi          A = V
        B = I
```

Test each position in CORE, DIATONIC+ and BORROWED. The matching screen button must highlight. Test fast patterns and simultaneous transitions; no notes should stick.

## 4. Front octave controls

- front-left lowers octave;
- front-right raises octave;
- limits are −2 and +2;
- a held chord rebuilds immediately;
- no bank, palette or D-pad mode changes.

## 5. L1 and L2 identity

With both SWAP options Off:

- hold L1 in CHORD: alternate palette appears only while held;
- release L1: default palette returns;
- press L2: header advances CHORD → SOUND → PERF FX → CHORD;
- holding L2 must not repeatedly skip modes.

If the two physical buttons are reversed, enable `SWAP L1/L2` and repeat. Confirm the new order persists after restart.

## 6. CHORD colour persistence

Configure BASE COLOUR = CLASSIC and L1 COLOUR = DARK.

- select maj7 in CLASSIC;
- hold and release L1 without selecting anything: maj7 remains active;
- enter SOUND: every chord remains maj7;
- enter PERF FX: every chord remains maj7;
- return to CHORD: the active status still shows maj7;
- hold L1, select POWER, release L1: POWER remains active;
- hold L1 and press the POWER direction again: BASE returns.

Repeat with EXTENDED as either layer.

## 7. SOUND normal layer

Without L1 held:

- Up/Down selects Preset, Tone, Body and Motion;
- Left/Right edits;
- held Left/Right accelerates;
- ABXY continues to play;
- R1/R2 bank changes continue to work;
- chord colour never resets.

## 8. SOUND L1 layer

Hold L1:

- panel changes to Attack, Release, Spread, BPM and Play Mode;
- Up/Down keeps a separate row position from the normal layer;
- Left/Right edits correctly;
- releasing L1 returns to the normal bank without losing edits;
- changing BPM affects ARP/PULSE timing.

## 9. R1 and R2 banks

Configure BASE BANK = CORE, R1 BANK = DIATONIC+, R2 BANK = BORROWED.

- hold R1: all ABXY labels and sounds change to DIATONIC+;
- release R1: CORE returns;
- hold R2: BORROWED appears;
- release R2: CORE returns;
- change R1/R2 while a chord is held: the chord rebuilds without a stuck old voice;
- if physical order is wrong, enable `SWAP R1/R2` and repeat.

## 10. PERF FX normal bank

Enter PERF FX with L1 released. Test all eight directions:

- effect starts only while direction is held;
- moving between directions changes immediately;
- returning to centre restores both base FX slots;
- ABXY remains responsive;
- repeated fast gestures do not leave a stuck effect;
- base effect settings displayed in Settings remain unchanged.

Listen specifically to Reverse-ish, Stutter, Chop, Crush, Drive, Wash, Deep Echo and Phase.

## 11. PERF FX L1 bank

Hold L1 and test all eight alternate gestures: Abyss, Ratetrap, Glitch, Broken, Doom, Shimmer-ish, Tunnel and Alien.

- L1 changes the bank while a D-pad direction is held;
- releasing L1 returns to the normal performance bank;
- releasing D-pad restores base effects;
- opening Settings while a gesture is active restores base effects;
- pressing L2 while active restores base effects before changing mode.

## 12. Settings

Press Select.

- Settings opens instead of toggling latch.
- Up/Down reaches all 16 rows.
- Left/Right edits palettes, banks, both base FX, key, octave, UI motion and swaps.
- ABXY remains playable.
- Select closes and saves.
- Restart restores every edited value.
- Old latch state never returns.

## 13. Start behaviour

- tap Start: PAD → STRUM → ARP → PULSE;
- hold Start about 0.85 seconds: `ALL NOTES OFF` appears and every voice stops;
- releasing after a long hold must not also change play mode;
- Start + Select saves and exits.

## 14. Sound engines and base FX

With base FX Off, compare all ten engines on one chord and octave. Record redundant, weak or unpleasant parameter ranges.

Then test every base effect type separately and in series. Check tails, level jumps, crackles, stereo collapse and speaker/headphone differences.

## 15. Latency and stability

- estimate button-to-sound latency with headphones;
- play rapid short chords in PAD and STRUM;
- run ARP/PULSE for at least five minutes;
- perform repeated PERF FX gestures for at least five minutes;
- leave the application running for at least 15 minutes;
- watch CPU, memory, heat, battery drain, underruns and increasing latency;
- test suspend/resume if practical;
- verify exit silences audio and returns to Knulli.

## 16. Files to collect

```bash
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log ~/Downloads/brkchrd-v040.log
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.cfg ~/Downloads/brkchrd-v040.cfg
```

Report BRKCHRD/Knulli versions, physical control, SWAP settings, D-pad mode, L1/R1/R2 state, stored chord colour, engine, base FX, speaker/headphones, latency/CPU and a short video for control or layout issues.
