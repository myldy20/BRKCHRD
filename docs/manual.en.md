# BRKCHRD 0.5.2 Manual — English

## What BRKCHRD is

BRKCHRD is a live harmony and songwriting instrument for TrimUI Brick. The four face buttons form a functional chord set rather than a list of unrelated chord names. The D-pad changes chord colour, edits the synth or performs temporary effects, while R1/R2 open alternate chord banks for as long as they are held.

The application runs entirely on the handheld. No computer, MIDI controller or network connection is required after installation.

## Quick start

1. Play B, A, Y and X. In the default CORE bank they are I, V, vi and IV.
2. In CHORD, use the D-pad to add a harmonic colour.
3. Press L1 to move between CHORD, SOUND and PERF FX.
4. In SOUND, use Up/Down to choose a parameter and Left/Right to edit it. Hold L2 for the second parameter layer.
5. Open Settings with Select to choose the language, D-pad behaviour, chord banks, effects and voicing options.

## Language

Set `LANGUAGE / ЯЗЫК` to `EN` or `RU` in Settings. The interface changes immediately and the choice is saved. The Russian mode uses native Cyrillic glyphs in the built-in bitmap font; it is not transliteration.

Preset names and standard chord symbols remain their original musical names so that patches and chords can be discussed consistently in either language.

## Chord banks

The right panel always shows the current face-button layout. In CORE:

```text
        X = IV
Y = vi          A = V
        B = I
```

R1 and R2 are momentary bank modifiers. Settings can assign CORE, DIATONIC+ or BORROWED independently to the base, R1 and R2 states. A sounding chord is rebuilt immediately when the bank changes.

## Chord colours and CHORD DPAD

CHORD offers three palettes:

- **CLASSIC:** practical triads, sevenths, sixths, sus and add9 colours;
- **EXTENDED:** ninths, elevenths, 6/9, half-diminished and Lydian-style structures;
- **DARK:** power, quartal, tritone and deliberately tense colours.

The normal and L2-held palettes are configured independently. Minor and diminished scale degrees receive musically appropriate minor or diminished extensions.

`CHORD DPAD` determines how a colour is applied:

### TOGGLE

Press a direction to store that colour. It remains active while you play, enter SOUND or use PERF FX. Press the same direction again in the same palette to return to BASE.

### HOLD

The chord is BASE while the D-pad is centred. Hold a direction to apply its colour only for the duration of the hold; release it to return immediately to BASE. Diagonals respond naturally when a second direction is pressed or released. A sounding chord is rebuilt as the gesture changes.

Switching back to TOGGLE restores the previously stored toggle colour; HOLD does not erase it.

## Stable and live voicing

### Voice Lead Off — default

The same face button with the same bank, colour, octave and preset always produces the same notes. Front octave controls move every voice by exactly twelve semitones.

### Voice Lead On

BRKCHRD searches nearby inversions relative to the previous chord. Different routes through ABXY may produce different voicings, but the result remains anchored near the octave selected by the front controls.

## Instrument-aware voicing

The displayed chord symbol and the played orchestration are deliberately separate. Each preset can omit redundant notes or spread voices differently:

- Keys and Organ: compact shell voicings;
- Pad and Choir: wider structures up to five notes;
- Pluck: fewer voices in a higher register;
- Heavy: controlled power/shell structures;
- Bass: root, fifth and octave.

## Sound editing

The normal SOUND layer contains Preset, Tone, Body and Motion. Hold L2 for Attack, Release, Spread, BPM and Play Mode.

The preset has a dedicated large card. Parameter values have safe right padding, and the selected continuous parameter keeps a visible white value bar.

## Play modes

- **PAD:** all notes start together;
- **STRUM:** notes enter in sequence;
- **ARP:** notes are played one at a time;
- **PULSE:** the whole chord is retriggered rhythmically.

Tap Start to change mode. Hold Start to stop all voices.

## Effects

FX1 and FX2 are saved serial effects. PERF FX temporarily substitutes a two-effect gesture while a D-pad direction is held and restores FX1/FX2 on release.

Set `PERF FX = OFF` to remove PERF FX from the L1 cycle without disabling FX1 or FX2.

## Settings

Settings contains twenty rows:

- base and L2 colour palettes;
- base, R1 and R2 chord banks;
- FX1/FX2 type, amount and colour;
- key and octave;
- Voice Lead and PERF FX;
- UI Motion;
- rear-button swaps;
- `LANGUAGE / ЯЗЫК`;
- `CHORD DPAD`.

Use Up/Down to select and Left/Right to change a value. Select closes and saves. Start+Select saves and exits.

## Files

PortMaster configuration and log:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

NextUI configuration and log:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

See [Controls](controls.en.md), [Sound design](sound-design.en.md), [Installation](install.en.md), [NextUI installation](install.nextui.en.md) and [Troubleshooting](troubleshooting.en.md) for focused guides. The hardware checklist is intended for contributors and release testing, not normal use.
