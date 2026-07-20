# BRKCHRD 0.5.1 Hardware Test Checklist

CI proves that the code builds, starts, renders audio and packages correctly. This checklist validates the real handheld controls, screen, latency, sound balance and stability.

## 1. Install and launch

- install the 0.5.1 PortMaster or NextUI package;
- verify executable permissions;
- confirm the two-second BRKCHRD / Myldy design splash;
- confirm the header reports 0.5.1;
- confirm ABXY produces sound immediately.

## 2. 0.5.1 interface corrections

Open SOUND normal layer:

- every factory preset name stays inside the dedicated 52-pixel card;
- the name is clearly larger than parameter text;
- Tone, Body and Motion values have visible right padding;
- selected Tone, Body or Motion keeps a white bar;
- the character line does not overlap the last parameter.

Open SOUND with L2 held:

- Attack, Release and Spread bars remain visible when selected;
- BPM and Play Mode remain inside the right boundary.

Open Settings:

- all values have right padding;
- no value touches or crosses the frame;
- footer reads `START+SELECT: SAVE AND EXIT` with no missing glyph;
- scrolling reaches all 18 rows.

## 3. Physical face buttons

Expected CORE layout:

```text
        X = IV
Y = vi          A = V
        B = I
```

Test every position in CORE, DIATONIC+ and BORROWED. Screen highlight and sounding degree must match the physical position. Fast transitions must not leave stuck notes.

## 4. Front octave controls

With Voice Lead Off:

- front-left lowers octave;
- front-right raises octave;
- limits are −2 and +2;
- the same held chord rebuilds immediately;
- every voice changes exactly twelve semitones;
- bank, colour and D-pad screen remain unchanged.

Repeat with Voice Lead On and confirm the result stays near the requested register.

## 5. L1 and L2

With `SWAP L1/L2 = OFF`:

- press L1: CHORD → SOUND → PERF FX → CHORD;
- hold L2 in CHORD: alternate palette appears only while held;
- hold L2 in SOUND: Attack/Release/Spread/BPM/Play Mode appear;
- hold L2 in PERF FX: alternate gesture bank appears.

If the physical order is reversed, enable `SWAP L1/L2` and repeat. The setting must persist after restart.

## 6. Chord colour persistence

- select a CLASSIC colour;
- enter SOUND and PERF FX: the colour remains active;
- hold/release L2 without selecting: the colour does not change;
- select a colour from the L2 palette: it remains after L2 is released;
- display the same palette and press the same direction again: BASE returns.

## 7. Deterministic and live voicing

Voice Lead Off:

- play the same face button after different routes through ABXY;
- MIDI/register result must remain identical;
- repeat for several presets and banks.

Voice Lead On:

- different routes may create different inversions;
- no result should jump outside a reasonable range around the selected octave;
- Bass remains root/fifth/octave;
- Heavy avoids dense low extensions.

## 8. SOUND editing

Normal layer:

- Up/Down selects Preset, Tone, Body and Motion;
- Left/Right edits;
- held Left/Right accelerates;
- ABXY and R1/R2 remain active;
- changing preset immediately updates its voicing profile and level.

L2 layer:

- separate row memory is preserved;
- Attack, Release, Spread, BPM and Play Mode edit correctly;
- BPM changes ARP/PULSE timing;
- releasing L2 returns to the normal layer without losing edits.

## 9. R1/R2 banks

Assign CORE, DIATONIC+ and BORROWED to base/R1/R2.

- R1 and R2 change labels and notes only while held;
- R2 has priority when both are held;
- a held chord rebuilds cleanly;
- `SWAP R1/R2` corrects reversed firmware order and persists.

## 10. PERF FX

With PERF FX On:

- test all eight normal and eight L2 gestures;
- effect is active only while a direction is held;
- moving between directions updates immediately;
- centre restores FX1/FX2;
- opening Settings, leaving the screen or disabling PERF FX restores FX1/FX2;
- fast gestures do not leave a stuck effect.

With PERF FX Off:

- L1 cycles CHORD ↔ SOUND only;
- ordinary FX1/FX2 continue to work.

## 11. Settings and persistence

- edit both palettes, all banks, FX1/FX2, key, octave, Voice Lead, PERF FX, UI Motion and swaps;
- close Settings and restart;
- every value is restored;
- Start+Select saves and exits;
- config survives a package upgrade.

## 12. Sixteen presets

Test BASE, a seventh and one wide extension on all presets through:

- built-in speaker;
- headphones;
- external output where available.

Record harshness, weak fundamentals, level jumps, mud, excessive stereo spread or ineffective parameters. Pay special attention to Doom Chords, Sub Altar, Frozen Glass and the choir patches.

## 13. Latency and stability

- estimate button-to-sound latency with headphones;
- play rapid PAD and STRUM patterns;
- run ARP and PULSE for at least five minutes;
- perform repeated PERF FX gestures for five minutes;
- leave the application running for at least fifteen minutes;
- observe CPU, memory, temperature, battery drain, underruns and increasing latency;
- test suspend/resume where practical;
- confirm exit silences audio and returns to the frontend.

## 14. Branding and package integration

PortMaster:

- game entry displays the BRKCHRD cover;
- `screenshot.jpg` is 640×480;
- package includes GPL, NOTICE and RU/EN docs.

NextUI:

- Tools card displays `.media/BRKCHRD.png`;
- configuration is stored outside the Pak;
- replacing the Pak preserves settings.

## 15. Files to collect

PortMaster:

```bash
scp root@DEVICE_IP:/userdata/roms/ports/brkchrd/conf/brkchrd.log ./
scp root@DEVICE_IP:/userdata/roms/ports/brkchrd/conf/brkchrd.cfg ./
```

For an issue, include device, firmware/frontend version, exact archive, control sequence, Voice Lead/PERF FX state, preset, chord, output route, observed CPU/latency and photo/video for UI defects.
