# BRKCHRD 0.5.0 Controls — English

## Physical mapping

The confirmed TrimUI Brick / Knulli mapping exposes all rear controls separately:

- glowing front-left/right: SDL left/right shoulder;
- rear L1/R1: SDL left/right stick buttons;
- rear L2/R2: SDL left/right triggers;
- printed ABXY letters are translated by physical position.

`SWAP L1/L2` and `SWAP R1/R2` compensate for firmware variants that report the upper/lower order differently.

## Main controls

| Physical control | Action |
| --- | --- |
| glowing front-left | octave down |
| glowing front-right | octave up |
| **L1 press** | cycle D-pad mode |
| **L2 hold** | temporary alternate layer of the current D-pad mode |
| R1 hold | configured alternate chord bank 1 |
| R2 hold | configured alternate chord bank 2 |
| ABXY | play functional chords |
| Start tap | PAD / STRUM / ARP / PULSE |
| Start hold | all notes off |
| Select | open/close Settings |
| Start + Select | save and exit |

Latch is absent. Releasing the last held ABXY button releases the chord.

## L1: D-pad mode

With PERF FX enabled, each new L1 press cycles:

```text
CHORD → SOUND → PERF FX → CHORD
```

With PERF FX disabled:

```text
CHORD ↔ SOUND
```

L1 changes mode once per press, not repeatedly while held.

## L2: alternate layer

L2 is momentary:

- CHORD: show/select the configured alternate colour palette;
- SOUND: show/edit Attack, Release, Spread, BPM and Play Mode;
- PERF FX: use the second, more extreme performance-effect bank.

Releasing L2 returns to the normal layer without erasing edits or the stored chord colour.

## CHORD mode

The D-pad stores one colour. It applies to every ABXY chord until another colour is selected or BASE is restored.

- tap a direction to select its colour;
- tap the same direction again while its palette is visible to return to BASE;
- hold L2 to show the alternate palette;
- selecting a colour while L2 is held stores it after L2 is released;
- merely viewing the L2 palette does not change the current chord.

Settings independently define `BASE COLOUR` and `L2 COLOUR` as CLASSIC, EXTENDED or DARK.

### Quality-aware extensions

Version 0.5 no longer forces major extensions onto every scale degree. Examples:

- major degree + 9th → dominant 9 or major 9 according to direction;
- minor degree + 9th → minor 9;
- diminished degree + seventh → diminished 7 or half-diminished;
- sixth/add9 colours retain the degree's major, minor or diminished quality.

## SOUND mode

ABXY, R1/R2 and the selected chord colour remain active while editing.

Normal layer:

- Preset
- Tone
- Body
- Motion

L2-held layer:

- Attack
- Release
- Spread
- BPM
- Play Mode

D-pad Up/Down selects a row. Left/Right changes the value. Holding Left/Right accelerates editing.

Changing Preset also changes the voicing profile used for later chords. A currently held chord is rebuilt on the next musical input; release and press it again when comparing presets.

## PERF FX mode

When enabled, holding a D-pad direction temporarily replaces base FX1/FX2 with a performance combination. Releasing all directions restores the base effects exactly.

L2 selects the alternate performance bank while held. Opening Settings, changing mode or exiting also restores the base chain.

Set `PERF FX = OFF` in Settings to remove this mode from the L1 cycle.

## R1/R2 chord banks

Settings independently assign `BASE BANK`, `R1 BANK` and `R2 BANK` as CORE, DIATONIC+ or BORROWED.

| Physical position | Button | CORE | DIATONIC+ | BORROWED |
| --- | --- | --- | --- | --- |
| bottom | B | I | ii | i |
| right | A | V | iii | iv |
| left | Y | vi | vii° | ♭VI |
| top | X | IV | ♭VII | ♭III |

R1/R2 are momentary. A held chord rebuilds immediately when the active bank changes.

## VOICE LEAD

`VOICE LEAD` controls whether chord register depends on the previous chord.

### OFF — default

- the same button/settings always produce the same notes;
- clockwise and counter-clockwise routes do not change the result;
- front octave buttons move the entire chord by exactly 12 semitones;
- the selected preset still applies its instrument-specific spacing.

### ON

- BRKCHRD searches nearby inversions for smoother transitions;
- route order can change inversion/register intentionally;
- unlike 0.4, candidates stay near the octave requested by the front controls;
- turning the option Off immediately restores deterministic behaviour on the next chord.

## Instrument-aware voicing

The preset chooses a profile automatically:

- Keys/Organ: clear shell voicings;
- Pad/Choir: wider voicings, up to five notes;
- Pluck: higher, less crowded voicings;
- Heavy: power/shell voicings with reduced low extensions;
- Bass: root, fifth and octave only.

This affects note placement, not the displayed chord name.

## Settings

Select opens the full-screen menu. Up/Down selects a row; Left/Right edits.

1. Base colour
2. L2 colour
3. Base bank
4. R1 bank
5. R2 bank
6–11. Base FX1/FX2 settings
12. Key
13. Octave
14. Voice Lead
15. PERF FX
16. UI Motion
17. Swap L1/L2
18. Swap R1/R2

Select closes and saves. Start+Select saves and exits.

## Front octave controls

The glowing buttons change octave within −2…+2. In deterministic mode the sounding notes change by exactly ±12 semitones. A currently held chord is rebuilt immediately.

## Desktop mapping

| Keyboard | Brick control |
| --- | --- |
| Z / X / A / S | physical B / A / Y / X |
| arrows | D-pad |
| Q press | L1 mode switch |
| W hold | L2 alternate layer |
| E hold | R1 |
| R hold | R2 |
| [ / ] | octave down / up |
| Enter | Start |
| Space | Select |
| Esc | exit |
