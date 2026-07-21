# BRKCHRD 0.6.0 Contributor Hardware QA Checklist

This document is for a contributor, developer or release tester validating a physical TrimUI Brick. Ordinary users do not need to complete it. Automated CI already covers compilation, unit tests, headless SDL startup, audio rendering and package structure.

## Installation and basic operation

- install the 0.6.0 PortMaster or NextUI package;
- confirm the application opens and ABXY produces sound;
- confirm the header reports 0.6.0;
- confirm exit returns cleanly to the frontend and silences audio.

## English and Russian interface

Set `LANGUAGE / ЯЗЫК` to EN and RU in turn.

- the interface changes without restarting;
- Cyrillic letters render as letters rather than question marks or broken bytes;
- no translated label crosses its panel boundary;
- header, CHORD, SOUND, PERF FX, Settings, footer and toast messages follow the selected language;
- the language persists after restart;
- all five UI palettes apply immediately and persist; LOGO uses the charcoal/cream project colours;
- the next startup logo uses the saved UI palette;
- preset names and chord symbols remain readable.

## Settings layout

- scrolling reaches all 21 rows;
- `LANGUAGE / ЯЗЫК`, `CHORD DPAD` and `UI PALETTE` are reachable;
- values remain inside the right boundary in both languages;
- the selected row remains legible;
- Select closes and saves;
- Start+Select saves and exits.

## CHORD DPAD = TOGGLE

- pressing a direction stores its colour without opening a popup over the chord labels;
- pressing the same direction again returns to BASE;
- the stored colour remains active in SOUND and PERF FX;
- selecting from the L2 palette remains active after L2 is released;
- changing banks or octave rebuilds a sounding chord without stuck notes.

## CHORD DPAD = HOLD

- centred D-pad produces BASE;
- holding a direction applies its colour immediately without opening a popup over the chord labels;
- releasing it returns a sounding chord immediately to BASE;
- adding and releasing a second axis moves cleanly into and out of diagonals;
- L2 selects the alternate palette while held;
- entering SOUND, PERF FX or Settings restores BASE;
- returning to TOGGLE restores the earlier stored toggle colour;
- fast repeated gestures do not leave a stale colour or stuck voice.

## Face buttons and chord banks

Expected CORE layout:

```text
        X = IV
Y = vi          A = V
        B = I
```

Test CORE, DIATONIC+ and BORROWED. R1/R2 must act only while held; R2 has priority when both are held. `SWAP R1/R2` must correct reversed firmware order and persist.

## L1, L2 and octave controls

- L1 cycles CHORD → SOUND → PERF FX, or CHORD ↔ SOUND when PERF FX is Off;
- L2 selects the alternate layer on every screen;
- `SWAP L1/L2` corrects reversed physical order;
- front controls change octave within −2…+2;
- with Voice Lead Off, every voice moves exactly twelve semitones;
- with Voice Lead On, voicings remain near the requested register.

## SOUND and presets

- all sixteen preset names fit the large card;
- Tone, Body, Motion, Attack, Release and Spread bars remain visible when selected;
- BPM and Play Mode stay inside the frame in EN and RU;
- ABXY and R1/R2 remain responsive while editing;
- compare BASE, a seventh and a wide extension on speaker, headphones and external output where available;
- note harshness, mud, weak fundamentals, level jumps, excessive spread or ineffective parameters.

## PERF FX and saved effects

- test all eight normal and eight L2 gestures;
- a gesture lasts only while its direction is held;
- release, screen change, Settings and disabling PERF FX restore FX1/FX2;
- fast gestures do not leave a stuck effect;
- PERF FX Off removes only the performance screen, not the saved effects.

## Stability

- play fast PAD and STRUM passages;
- run ARP and PULSE for at least five minutes;
- perform repeated chord-colour and PERF FX gestures;
- observe CPU, memory, temperature, battery drain, underruns and increasing latency;
- test suspend/resume where practical.

## Package integration

PortMaster must show the BRKCHRD cover and include the 640×480 screenshot, RU/EN documentation, GPL and NOTICE. NextUI must show `.media/BRKCHRD.png`, store configuration outside the Pak and preserve it when the Pak is replaced.

A useful issue report includes the device, firmware/frontend version, exact archive, language, CHORD DPAD mode, control sequence, preset, chord, output route, log, observed CPU/latency and photo or video for UI defects.
