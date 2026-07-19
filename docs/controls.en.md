# Controls — English

## The actual TrimUI Brick layout

BRKCHRD 0.3 follows the signals that Knulli really exposes on TrimUI Brick instead of pretending the device is a generic Xbox controller:

- the two glowing front buttons arrive as left and right shoulder controls;
- rear L1 and L2 share one left-side channel;
- rear R1 and R2 share one right-side channel;
- SDL face-button names are swapped relative to the letters printed on the Brick.

The application handles these differences directly.

## Main controls

| Physical control | Action |
| --- | --- |
| glowing front button, left | octave down |
| glowing front button, right | octave up |
| either rear left button: L1 or L2 | next left-panel mode |
| either rear right button: R1 or R2 | next right-panel mode |
| Start | next PAD / STRUM / ARP / PULSE mode |
| short Select press | latch on/off |
| hold Select | disable latch and stop every note immediately |
| Start + Select | save settings and exit |

The last rear side pressed becomes focused. Its panel receives a bright double outline and the D-pad controls that panel.

## Left panel

Rear L1/L2 cycle through:

1. **CLASSIC** — standard chord colours;
2. **EXTENDED** — extended, jazz and modal colours;
3. **DARK** — power, open, cluster, tritone and other heavy variants;
4. **SOUND** — synthesis engine and macro parameters;
5. **SYSTEM** — key, octave display and interface-motion level.

In CLASSIC / EXTENDED / DARK, the D-pad selects a cell in the equal 3×3 colour grid. Press the armed direction again to return to BASE. While a chord is held, the D-pad temporarily changes its colour; releasing the D-pad returns to the armed colour.

In SOUND and SYSTEM:

- D-pad Up/Down selects a row;
- D-pad Left/Right edits the value;
- held Left/Right accelerates.

## Right panel

Rear R1/R2 cycle through:

1. **CORE** — main chord bank;
2. **DIATONIC+** — additional diatonic degrees;
3. **BORROWED** — borrowed minor and modal degrees;
4. **FX 1** — first effect slot;
5. **FX 2** — second effect slot;
6. **MASTER** — master level and signal path.

Entering CORE / DIATONIC+ / BORROWED makes that bank active. It remains active while effects are being edited.

In FX 1 and FX 2:

- Up/Down selects TYPE, AMOUNT or COLOUR;
- Left/Right edits the selected value.

In MASTER, Left/Right adjusts master level.

## Physical ABXY chord layout

The interface now shows the letters printed on the Brick rather than SDL's logical button names.

| Physical position | Button | CORE | DIATONIC+ | BORROWED |
| --- | --- | --- | --- | --- |
| bottom | B | I | ii | i |
| right | A | V | iii | iv |
| left | Y | vi | vii° | ♭VI |
| top | X | IV | ♭VII | ♭III |

ABXY remain playable in every left- and right-panel mode, so sound and effect edits can be auditioned immediately.

## Chord colours

### CLASSIC

Up — major/minor flip; Up-right — dominant/minor 7; Right — maj7/min7; Down-right — add9/min9; Down — sus4; Down-left — 6/sus2; Left — dark/dim; Up-left — augmented.

### EXTENDED

ADD11, DOM9, 6/9, MIN11, 7SUS, HALF-DIM, mMAJ7 and LYDIAN.

### DARK

POWER, CRUNCH 7#9, QUARTAL, CLUSTER add♭9, OPEN no-third, MIN6, TRITONE and AUGMAJ7.

## Desktop mapping

| Keyboard | TrimUI Brick |
| --- | --- |
| Z / X / A / S | bottom B / right A / left Y / top X |
| arrows | D-pad |
| Q | next left-panel mode |
| W | next right-panel mode |
| E / R | octave down / up |
| Enter | Start |
| Space | Select |
| Esc | exit |

## Input diagnostics

At startup, the application writes the controller name, SDL mapping, controller-button events and the first raw button/axis events to `brkchrd.log`. This makes firmware-specific mapping differences diagnosable without guessing from labels.
