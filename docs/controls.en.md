# BRKCHRD 0.4.0 Controls — English

## Hardware mapping confirmed on TrimUI Brick

The Knulli controller mapping used for BRKCHRD 0.4 exposes the controls separately:

- glowing front-left: SDL left shoulder;
- glowing front-right: SDL right shoulder;
- rear L1: SDL left-stick button;
- rear L2: SDL left trigger;
- rear R1: SDL right-stick button;
- rear R2: SDL right trigger;
- D-pad: hat directions;
- printed ABXY letters use the Brick's physical layout rather than SDL's logical names.

If a firmware presents L1/L2 or R1/R2 in the opposite physical order, use `SWAP L1/L2` or `SWAP R1/R2` in Settings.

## Main controls

| Physical control | Action |
| --- | --- |
| glowing front-left | octave down |
| glowing front-right | octave up |
| L1 hold | temporary alternate layer for the current D-pad mode |
| L2 press | cycle `CHORD → SOUND → PERF FX` |
| R1 hold | configured alternate chord bank 1 |
| R2 hold | configured alternate chord bank 2 |
| ABXY | play functional chords |
| Start tap | cycle PAD / STRUM / ARP / PULSE |
| Start hold | all notes off |
| Select | open or close Settings |
| Start + Select | save and exit |

Latch is intentionally absent. Releasing the last held ABXY button releases the chord.

## CHORD mode

The D-pad selects a chord colour. The choice is stored independently from the currently visible palette.

- tap a direction to select that colour;
- tap the same direction while the same palette is displayed to return to BASE;
- play any ABXY chord: the stored colour is applied;
- switch to SOUND or PERF FX: the stored colour remains active;
- hold L1: show the alternate palette;
- release L1: return to the default palette without erasing the stored colour.

Settings independently define:

- `BASE COLOUR`: palette shown normally;
- `L1 COLOUR`: palette shown while L1 is held.

Both can be CLASSIC, EXTENDED or DARK.

### CLASSIC directions

| Direction | Colour |
| --- | --- |
| Up-left | augmented |
| Up | major/minor flip |
| Up-right | dominant 7 / minor 7 |
| Left | dark minor / diminished |
| Right | major 7 / minor 7 |
| Down-left | sixth / sus2 |
| Down | sus4 |
| Down-right | add9 / minor 9 |

### EXTENDED directions

ADD11, DOM9, 6/9, MIN11, 7SUS, HALF-DIM, mMAJ7 and LYDIAN.

### DARK directions

POWER, CRUNCH 7#9, QUARTAL, CLUSTER add♭9, OPEN no-third, MIN6, TRITONE and AUGMAJ7.

## SOUND mode

Press L2 once from CHORD. ABXY remain playable and the stored chord colour does not change.

### Normal layer

- Preset
- Tone
- Body
- Motion

### L1-held layer

- Attack
- Release
- Spread
- BPM
- Play Mode

Use D-pad Up/Down to select a row. Use Left/Right to edit. Holding Left/Right accelerates the change.

L1 is momentary: releasing it returns to the normal parameter bank while preserving all edits.

## PERF FX mode

Press L2 again from SOUND. This mode turns the D-pad into a momentary performance-effects controller.

- hold a D-pad direction: the assigned two-effect combination becomes active;
- move to another direction: the combination changes immediately;
- release the D-pad: the original base FX1 and FX2 settings are restored exactly;
- hold L1: use the second, more extreme FX bank;
- release L1 while holding a direction: switch back to the normal FX bank.

### Normal performance bank

Reverse-ish, Stutter, Chop, Crush, Drive, Wash, Deep Echo and Phase combinations.

### L1-held performance bank

Abyss, Ratetrap, Glitch, Broken, Doom, Shimmer-ish, Tunnel and Alien combinations.

These names describe performance gestures. The current engine implements them by combining the available Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb algorithms. They are momentary and do not overwrite the saved base effects.

## Chord banks

Settings independently define:

- `BASE BANK`: no rear R button held;
- `R1 BANK`: R1 held;
- `R2 BANK`: R2 held.

Each can be CORE, DIATONIC+ or BORROWED.

| Physical position | Button | CORE | DIATONIC+ | BORROWED |
| --- | --- | --- | --- | --- |
| bottom | B | I | ii | i |
| right | A | V | iii | iv |
| left | Y | vi | vii° | ♭VI |
| top | X | IV | ♭VII | ♭III |

R1 and R2 are momentary. Releasing them returns to BASE BANK. If a chord is already sounding, the voicing updates to the newly selected bank.

## Settings

Press Select to open the full-screen Settings menu. D-pad Up/Down selects a row; Left/Right changes the value.

Available settings:

1. Base colour palette
2. L1-held colour palette
3. Base chord bank
4. R1-held chord bank
5. R2-held chord bank
6. FX1 type
7. FX1 amount
8. FX1 colour
9. FX2 type
10. FX2 amount
11. FX2 colour
12. Key
13. Octave
14. UI motion
15. Swap L1/L2
16. Swap R1/R2

Select closes Settings and saves the current configuration. Start+Select saves and exits.

## Front octave controls

The glowing front-left and front-right controls change octave down/up. If a chord is held, it is rebuilt immediately in the new octave.

## Desktop test mapping

| Keyboard | Handheld action |
| --- | --- |
| Z / X / A / S | physical B / A / Y / X chord buttons |
| arrows | D-pad |
| Q hold | L1 |
| W press | L2 |
| E hold | R1 |
| R hold | R2 |
| [ / ] | octave down / up |
| Enter | Start |
| Space | Select |
| Esc | exit |

## Diagnostics

`brkchrd.log` records the SDL controller mapping, raw button/axis events and the expected v0.4 roles. Use it together with the two SWAP settings instead of globally remapping Knulli.
