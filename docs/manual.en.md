# BRKCHRD 0.4.0 Manual — English

## What the instrument is

BRKCHRD is a live harmony instrument for TrimUI Brick. The four physical ABXY buttons play functional chords. A stored chord colour changes their voicing, R1/R2 temporarily select alternate scale-degree banks, and three D-pad modes provide harmony selection, synthesis editing and momentary performance effects.

The design goal is to let the player keep making sound while changing the instrument. ABXY therefore remain active in CHORD, SOUND, PERF FX and Settings.

## First minute

1. Launch BRKCHRD. It starts in CHORD mode.
2. Play physical B, A, Y and X. In the default CORE bank they are I, V, vi and IV.
3. Use D-pad Right in the default CLASSIC palette to select maj7/min7.
4. Press L2 once. SOUND opens, but the stored maj7/min7 colour remains active for every ABXY chord.
5. Use Up/Down to select Tone, Body or Motion and Left/Right to edit.
6. Hold L1. The left panel changes to Attack, Release, Spread, BPM and Play Mode.
7. Release L1 and press L2 again. PERF FX opens.
8. Hold a D-pad direction to apply a momentary effect combination; release it to restore the normal effects.
9. Hold L1 in PERF FX to reveal the second, more extreme performance bank.
10. Hold R1 or R2 while playing to use the configured alternate chord banks.
11. Press Select to open Settings and assign the normal/held colour palettes and chord banks.

## The three D-pad modes

### CHORD

CHORD mode stores one palette plus one D-pad direction. That selection is applied to all later chords until another colour is chosen or BASE is restored.

The palette currently visible does not have to match the palette of the stored colour. This is deliberate:

- select maj7 in CLASSIC;
- hold L1 to inspect DARK;
- release L1 without choosing anything;
- maj7 remains active.

If a direction is selected while L1 is held, its L1 palette becomes the stored colour even after L1 is released.

### SOUND

SOUND divides the synthesis parameters into two momentary banks.

Normal bank:

- Preset
- Tone
- Body
- Motion

L1-held bank:

- Attack
- Release
- Spread
- BPM
- Play Mode

Up/Down selects a row. Left/Right edits it. Holding Left/Right accelerates after a short delay. ABXY and R1/R2 remain playable, so every edit can be auditioned immediately with the active chord colour.

### PERF FX

PERF FX treats the D-pad as an eight-direction momentary effect surface.

The application snapshots the two base effect slots when the first direction is pressed. It then replaces them with the selected performance combination. Releasing all D-pad directions restores the snapshot exactly.

Normal bank examples:

- Reverse-ish: delay plus controlled crushing;
- Stutter and Chop: fast amplitude gating and sample-rate reduction;
- Crush and Drive: destructive distortion combinations;
- Wash and Deep Echo: long spatial tails;
- Phase: strong phase/chorus movement.

L1-held bank examples:

- Abyss: maximal delay and reverb;
- Ratetrap and Glitch: rapid gating and low-resolution destruction;
- Broken and Doom: distorted, unstable combinations;
- Shimmer-ish, Tunnel and Alien: extreme modulation and space.

The labels describe the musical gesture rather than claiming exact EP-133 algorithms. The current DSP uses combinations of BRKCHRD's existing effects. Future engines can add true reverse buffers, granular repeat and dedicated beat slicing without changing the control layout.

## Chord colours

The three palettes are CLASSIC, EXTENDED and DARK. Settings assigns one palette to the normal CHORD layer and one to L1 hold.

The stored colour remains active in SOUND, PERF FX and Settings. It changes only when another CHORD direction is selected or the active direction is pressed again to return to BASE.

## Chord banks

Three bank assignments are stored:

- Base bank
- R1-held bank
- R2-held bank

Each assignment can be CORE, DIATONIC+ or BORROWED.

| Physical button | CORE | DIATONIC+ | BORROWED |
| --- | --- | --- | --- |
| B, bottom | I | ii | i |
| A, right | V | iii | iv |
| Y, left | vi | vii° | ♭VI |
| X, top | IV | ♭VII | ♭III |

R1/R2 are momentary. Releasing them returns to the base assignment. Automatic voice leading continues across bank changes.

## Play modes

Start cycles:

- PAD — all chord notes together;
- STRUM — notes enter in sequence;
- ARP — repeating arpeggio;
- PULSE — rhythmic chord pulses.

Start hold performs all-notes-off. Start+Select saves and exits.

## Settings

Select opens a full-screen settings menu. The menu is intentionally separate from performance so the rear buttons can remain momentary instruments.

Settings include:

- default colour palette;
- L1-held colour palette;
- base, R1-held and R2-held chord banks;
- Type, Amount and Colour for base FX1 and FX2;
- key and octave;
- UI motion;
- swap L1/L2;
- swap R1/R2.

The SWAP controls compensate for firmware differences without changing the global Knulli controller mapping.

## Base effects versus performance effects

FX1 and FX2 in Settings are the permanent serial effects:

```text
SYNTH → FX1 → FX2 → MASTER
```

PERF FX temporarily replaces both slots and restores them on release. Exiting PERF FX, opening Settings or changing D-pad mode also forces restoration, preventing a performance effect from becoming stuck.

## Octave controls

The two glowing front buttons lower and raise octave. If a chord is held, it is rebuilt immediately. The setting range is −2 to +2 relative to the engine's base register.

## Saving

Select closes Settings and writes the current configuration. Start+Select saves and exits. The text file stores all palettes, banks, sound macros, base effects, key, octave, play mode, current D-pad mode, UI motion and rear-button swap options.

## Diagnostics

The log records the detected controller, SDL mapping, raw input events and the expected v0.4 control roles. The mapping confirmed during development exposes front buttons as shoulders, L1/R1 as stick buttons and L2/R2 as trigger buttons. Use the log before changing global firmware mappings.

## Current limitations

- performance FX use combinations of the current effect algorithms rather than dedicated granular/reverse DSP;
- in-app labels are English, while external documentation is bilingual;
- no MIDI input/output yet;
- no user-named preset slots yet;
- final speaker voicing, latency and CPU limits require physical testing.
