# BRKCHRD 0.5.1 Controls — English

## Confirmed TrimUI Brick mapping

On the tested Knulli controller profile:

- front illuminated left: SDL left shoulder;
- front illuminated right: SDL right shoulder;
- rear L1: SDL left-stick button;
- rear L2: SDL left trigger;
- rear R1: SDL right-stick button;
- rear R2: SDL right trigger;
- printed ABXY follow the physical Brick labels, not SDL logical letters.

Use `SWAP L1/L2` or `SWAP R1/R2` when a firmware exposes the physical order differently.

## Global controls

| Control | Tap / hold | Action |
| --- | --- | --- |
| Front illuminated left | Tap | Octave down |
| Front illuminated right | Tap | Octave up |
| L1 | Tap | Cycle CHORD → SOUND → PERF FX |
| L2 | Hold | Alternate layer for the current D-pad screen |
| R1 | Hold | Temporary chord bank assigned to R1 |
| R2 | Hold | Temporary chord bank assigned to R2 |
| ABXY | Hold | Play functional chords |
| Select | Tap | Open or close Settings |
| Start | Tap | Next PAD / STRUM / ARP / PULSE mode |
| Start | Hold | All notes off |
| Start + Select | Chord | Save configuration and exit |

When `PERF FX = OFF`, L1 cycles CHORD ↔ SOUND only.

## CHORD mode

D-pad directions select one stored harmonic colour. The selection remains active while SOUND, PERF FX or Settings is shown.

- press an unselected direction: store that colour;
- press the currently selected direction again: return to BASE;
- hold L2 while selecting: use the configured alternate palette;
- the normal and L2 palettes can each be CLASSIC, EXTENDED or DARK.

## SOUND mode

Normal layer:

| Up/Down selection | Left/Right action |
| --- | --- |
| Preset | Previous / next factory sound |
| Tone | Darker / brighter |
| Body | Less / more body |
| Motion | Less / more movement |

L2-held layer:

| Up/Down selection | Left/Right action |
| --- | --- |
| Attack | Faster / slower attack |
| Release | Shorter / longer release |
| Spread | Narrower / wider |
| BPM | Lower / higher tempo |
| Play Mode | Previous / next mode |

Holding Left/Right accelerates after a short delay. ABXY remain playable during editing.

## PERF FX mode

Hold a direction to engage a temporary two-effect gesture. Release the direction to restore the normal FX1/FX2 chain. Hold L2 for the second, more extreme bank.

The centre position is dry with respect to performance substitution; it does not bypass the configured base effects.

## Chord-bank modifiers

R1 and R2 are momentary. Each can be assigned CORE, DIATONIC+ or BORROWED. If both are held, R2 has priority. The display and any sounding chord update immediately.

## Settings navigation

- Up/Down: select row;
- Left/Right: change value;
- hold Left/Right: accelerated repeat where applicable;
- Select: close and save;
- Start+Select: save and exit.

All values are right-aligned with safe frame padding in 0.5.1.

## Desktop test mapping

| Keyboard | Handheld equivalent |
| --- | --- |
| Z / X / A / S | Physical B / A / Y / X chord buttons |
| `[` / `]` | Octave down / up |
| Q | L1 mode cycle |
| W | L2 alternate layer |
| E / R | R1 / R2 chord banks |
| Arrow keys | D-pad |
| Enter | Start |
| Space | Select |
| Escape | Exit |
