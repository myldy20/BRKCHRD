# BRKCHRD 0.5.2 Controls — English

## TrimUI Brick mapping

On the tested Knulli controller profile:

- front illuminated left/right: octave down/up;
- rear L1: cycle the D-pad screen;
- rear L2: hold the alternate layer;
- rear R1/R2: hold alternate chord banks;
- ABXY: play the functional chord layout.

Use `SWAP L1/L2` or `SWAP R1/R2` if a firmware exposes the rear controls in the opposite physical order.

## Global controls

| Control | Action |
| --- | --- |
| Front illuminated left/right | Octave down/up |
| L1 press | CHORD → SOUND → PERF FX |
| L2 hold | Alternate palette, parameter layer or PERF FX bank |
| R1/R2 hold | Temporary configured chord bank |
| ABXY hold | Play chords |
| Select | Open or close Settings |
| Start tap | Next PAD / STRUM / ARP / PULSE mode |
| Start hold | All notes off |
| Start + Select | Save and exit |

When `PERF FX = OFF`, L1 cycles only CHORD ↔ SOUND.

## LANGUAGE / ЯЗЫК

- `EN`: English interface;
- `RU`: Russian interface with native Cyrillic bitmap glyphs.

The change is immediate and persists after restart. Preset names and chord symbols remain their standard names.

## CHORD mode

The normal and L2-held palettes can each be CLASSIC, EXTENDED or DARK. Hold L2 while using the D-pad to access the alternate palette.

### CHORD DPAD = TOGGLE

- press a direction to store its colour;
- the colour remains active after release and on other screens;
- press the same direction again in the same palette to return to BASE;
- selecting from the L2 palette stores that palette and colour after L2 is released.

### CHORD DPAD = HOLD

- centred D-pad means BASE;
- hold a direction to apply its colour temporarily;
- release the D-pad to return immediately to BASE;
- press or release a second direction to move into or out of a diagonal;
- a sounding chord is rebuilt on every gesture change;
- L2 chooses the alternate palette for the duration of the hold.

HOLD does not overwrite the colour stored by TOGGLE. Returning to TOGGLE restores the stored selection.

## SOUND mode

Normal layer:

| Up/Down selection | Left/Right action |
| --- | --- |
| Preset | Previous / next factory sound |
| Tone | Darker / brighter |
| Body | Less / more weight |
| Motion | Less / more movement |

L2-held layer:

| Up/Down selection | Left/Right action |
| --- | --- |
| Attack | Faster / slower attack |
| Release | Shorter / longer tail |
| Spread | Narrower / wider |
| BPM | Lower / higher tempo |
| Play Mode | Previous / next mode |

Holding Left/Right accelerates editing. ABXY and R1/R2 remain playable.

## PERF FX mode

Hold a direction to substitute a temporary two-effect gesture. Release it to restore the saved FX1/FX2 chain. Hold L2 for the second gesture bank.

## Settings

- Up/Down: select a row;
- Left/Right: change the value;
- Select: close and save;
- Start+Select: save and exit.

Important options include `VOICE LEAD`, `PERF FX`, `UI MOTION`, button swaps, `LANGUAGE / ЯЗЫК` and `CHORD DPAD`.

## Desktop test mapping

| Keyboard | Handheld equivalent |
| --- | --- |
| Z / X / A / S | Physical B / A / Y / X |
| `[` / `]` | Octave down / up |
| Q | L1 mode cycle |
| W | L2 alternate layer |
| E / R | R1 / R2 banks |
| Arrow keys | D-pad |
| Enter | Start |
| Space | Select |
| Escape | Exit |
