# BRKCHRD 0.5.1 Manual — English

## 1. What BRKCHRD is

BRKCHRD is a live harmony and songwriting instrument for TrimUI Brick. It is not a menu of unrelated chord names: the four face buttons form a functional set that can be played as a compact progression instrument. The D-pad changes harmonic colour, edits synthesis parameters or performs momentary effects, while R1/R2 expose temporary alternate chord banks.

The application runs entirely on the handheld. No MIDI controller, computer or network connection is required after installation.

## 2. First minute

1. Launch the application and wait for the two-second Myldy design splash.
2. In the default CORE bank play B, A, Y and X. They represent I, V, vi and IV.
3. In CHORD mode press a D-pad direction to select a chord colour. Press the selected direction again to return to BASE.
4. Press L1 to enter SOUND. ABXY remain playable and keep the selected chord colour.
5. Use Up/Down to select Preset, Tone, Body or Motion and Left/Right to change it.
6. Hold L2 to edit Attack, Release, Spread, BPM and Play Mode.
7. Press L1 again to enter PERF FX, unless it has been disabled in Settings.
8. Open Settings with Select and choose whether Voice Lead should be deterministic or expressive.

## 3. Chord banks

The active face-button layout is always shown on the right. In the default CORE bank:

```text
        X = IV
Y = vi          A = V
        B = I
```

R1 and R2 are momentary modifiers. Settings independently assign CORE, DIATONIC+ or BORROWED to the base, R1 and R2 positions. Holding a rear button immediately rebuilds a currently sounding chord with the selected bank.

## 4. Chord colours

CHORD mode offers three palettes:

- **CLASSIC:** practical triads, sevenths, sixths, sus chords and add9 colours;
- **EXTENDED:** ninths, elevenths, 6/9, half-diminished and Lydian-style structures;
- **DARK:** power, quartal, cluster, tritone and deliberately tense colours.

The colour engine preserves the quality of the current scale degree. A minor degree receives minor extensions; a diminished degree receives diminished or half-diminished structures. Some DARK colours are intentionally dissonant rather than errors.

The normal and L2-held palettes are configured independently.

## 5. Stable and live voicing

### Voice Lead Off — default

The same face button with the same bank, colour, octave and preset always produces the same MIDI notes. The order in which chords were played does not matter. The front octave buttons move every note by exactly twelve semitones.

Use this mode when learning the instrument, recording repeatable parts or comparing presets.

### Voice Lead On

The instrument searches nearby inversions relative to the previously played chord. Clockwise and counter-clockwise movement through ABXY may therefore produce different voicings. Unlike the old 0.4 behaviour, the search remains anchored near the octave selected by the front controls.

Use this mode when smooth movement and evolving inversions are more important than exact repeatability.

## 6. Instrument-aware voicing

The harmonic chord and the played orchestration are deliberately separate. The displayed symbol remains correct, while each preset profile can omit redundant notes or spread voices differently:

- Keys and Organ: compact, clear shell voicings;
- Pad and Choir: wider structures up to five notes;
- Pluck: fewer voices in a higher useful register;
- Heavy: power/shell structures without dense low extensions;
- Bass: root, fifth and octave.

This is why the same `m11` can remain lush on strings but controlled on a sub or distorted patch.

## 7. Sound editing

In SOUND, the normal layer contains:

- **Preset:** one of sixteen factory sounds;
- **Tone:** spectral brightness and filter position;
- **Body:** low-mid weight, oscillator mixture or model-specific density;
- **Motion:** drift, modulation depth and movement.

Hold L2 for:

- **Attack:** fade-in time;
- **Release:** tail after key release;
- **Spread:** detune and stereo distribution;
- **BPM:** timing for ARP, PULSE and time-dependent gestures;
- **Play Mode:** PAD, STRUM, ARP or PULSE.

Version 0.5.1 gives the preset a dedicated large card. Parameter values are right-aligned with safe padding, and the selected parameter keeps a visible white bar.

## 8. Play modes

- **PAD:** all chord notes start together;
- **STRUM:** notes enter in sequence, with spacing influenced by Motion;
- **ARP:** one note at a time, cycling through the current voicing;
- **PULSE:** the whole chord is retriggered rhythmically.

Tap Start to cycle play modes. Hold Start to stop every active voice immediately.

## 9. Effects

Two serial base slots, FX1 and FX2, support Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb. Their type, amount and colour are edited in Settings.

PERF FX is a momentary performance layer. A held D-pad direction temporarily replaces the base pair with a gesture combination. Releasing the D-pad, leaving the screen or opening Settings restores the saved base chain exactly.

Set `PERF FX = OFF` to remove PERF FX from the L1 cycle. This does not disable FX1 or FX2.

## 10. Settings

Settings contains:

- base and L2 colour palettes;
- base, R1 and R2 chord banks;
- FX1/FX2 type, amount and colour;
- musical key and octave;
- Voice Lead On/Off;
- PERF FX On/Off;
- UI Motion Off/Low/Full;
- Swap L1/L2 and Swap R1/R2 for firmware mapping differences.

Use Up/Down to select a row and Left/Right to edit it. Percentages and values are right-aligned inside the frame. The footer reads `START+SELECT: SAVE AND EXIT`.

## 11. Saving and files

Closing Settings saves the current state. Start+Select saves and exits from the application.

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

## 12. Exit and recovery

- Start+Select: save and exit;
- Start hold: emergency all-notes-off;
- if the program does not launch, read the runtime log before reinstalling;
- to test factory defaults, rename `brkchrd.cfg` rather than deleting it permanently.

See [Controls](controls.en.md), [Sound design](sound-design.en.md), [Installation](install.en.md), [NextUI installation](install.nextui.en.md) and [Troubleshooting](troubleshooting.en.md) for focused guides.
