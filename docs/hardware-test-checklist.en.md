# BRKCHRD 0.3 hardware test checklist

This checklist is for a physical TrimUI Brick running Knulli. Automated tests verify compilation, startup and packaging; the handheld must still validate layout, real controller mapping, latency, sound balance and thermal behaviour.

## 1. Installation and launch

- Install `brkchrd-v0.3.0-portmaster.zip` into `/userdata/roms/ports/`.
- Confirm:
  - `/userdata/roms/ports/BRKCHRD.sh`
  - `/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64`
- Confirm both are executable.
- Refresh the game list or reboot.
- Open **Ports → BRKCHRD**.
- Confirm the application reaches the dual-panel performance screen.
- Confirm a chord can be played immediately without opening an editor.

## 2. Fixed layout and typography

Inspect every left mode: CLASSIC, EXTENDED, DARK, SOUND and SYSTEM.

Inspect every right mode: CORE, DIATONIC+, BORROWED, FX 1, FX 2 and MASTER.

Confirm:

- the two main panels have equal width and height;
- all nine colour cells are equal;
- all four chord buttons are equal;
- no label crosses a cell, panel, footer or meter;
- long engine and effect names remain clipped inside their fields;
- large chord text remains prominent;
- microtext is compact rather than loosely tracked;
- the focused side has a clearly visible double outline;
- footer text remains inside the footer;
- no stale pixels remain after rapid mode changes;
- 512×384 scales cleanly to the Brick's 1024×768 display.

Photograph any overlap and record both active panel modes.

## 3. Physical face-button translation

The screen must match the labels printed on the Brick:

```text
        X
    Y       A
        B
```

In CORE, confirm:

- physical bottom B plays I;
- physical right A plays V;
- physical left Y plays vi;
- physical top X plays IV.

Repeat in DIATONIC+ and BORROWED. Pressing a button must highlight the matching physical screen position, not merely the SDL semantic name.

Test rapid progressions:

- B → A → Y → X;
- B → X → A → B;
- change bank, then repeat the same physical pattern.

No stuck notes or unexpected octave jumps should remain.

## 4. Glowing front octave buttons

- glowing front-left lowers octave by one;
- glowing front-right raises octave by one;
- range stops safely at −2 and +2;
- the header and SYSTEM octave row update immediately;
- a held chord revoices immediately without leaving old notes;
- the front buttons do not change chord bank, palette or panel mode.

## 5. Rear left channel

Press both physical L1 and L2 separately. Each must advance exactly one left-panel mode:

```text
CLASSIC → EXTENDED → DARK → SOUND → SYSTEM → CLASSIC
```

Confirm:

- L1 and L2 produce the same result;
- one physical press never skips two modes;
- the left panel receives the focus outline;
- D-pad editing applies to the left panel after the press;
- no right-panel state changes.

## 6. Rear right channel

Press both physical R1 and R2 separately. Each must advance exactly one right-panel mode:

```text
CORE → DIATONIC+ → BORROWED → FX 1 → FX 2 → MASTER → CORE
```

Confirm:

- R1 and R2 produce the same result;
- one press never skips two modes;
- CORE / DIATONIC+ / BORROWED immediately become the active bank;
- entering FX or MASTER preserves the last selected bank;
- the right panel receives D-pad focus;
- no left-panel state changes.

## 7. D-pad colour workflow

With CLASSIC, EXTENDED or DARK visible—or while a chord-bank view is focused:

- arm all eight directions, including diagonals;
- press the armed direction again and confirm BASE returns;
- arm POWER before the first chord and confirm the first chord is a power chord;
- hold a chord and move the D-pad: the sounding chord changes temporarily;
- release the direction: the armed colour returns;
- key and octave must not change.

## 8. SOUND and SYSTEM editing

In SOUND:

- Up/Down selects ENGINE, TONE, BODY, MOTION, ATTACK, RELEASE, SPREAD, BPM and PLAY MODE;
- Left/Right edits the selected row;
- held Left/Right accelerates;
- ABXY continue to play during editing.

In SYSTEM:

- KEY changes correctly;
- OCTAVE is status-only and follows the front buttons;
- UI MOTION cycles OFF / LOW / FULL;
- VERSION shows 0.3.0;
- there are no unexplained RESET COLOUR or PANIC rows.

## 9. FX and MASTER editing

For both FX slots:

- Up/Down selects TYPE, AMOUNT and COLOUR;
- Left/Right edits immediately;
- the response curve changes visibly;
- Off is effectively dry;
- no type change creates a burst or stuck voice.

In MASTER:

- Left/Right adjusts final level;
- the signal path remains readable;
- the output meter follows actual sound.

Test Off, Chorus, Phaser, Tremolo, Drive, Crusher, Delay and Reverb, first separately and then in series.

## 10. Reactive interface

At UI MOTION FULL, confirm:

- background depth moves smoothly without distracting flicker;
- active chord buttons pulse;
- the output meter reacts to dynamics;
- FX curves move with effect state;
- current chord and play mode update immediately.

Compare FULL, LOW and OFF. Audio timing and sound must remain unchanged.

## 11. Select, latch and emergency stop

- short Select toggles latch once;
- with latch on, a released chord continues;
- hold Select for about 0.85 seconds;
- `ALL NOTES OFF` appears;
- latch turns off;
- every voice stops;
- no held face-button state remains;
- releasing Select after the long press must not toggle latch back on.

Start must cycle PAD, STRUM, ARP and PULSE. Start + Select must save and exit cleanly.

## 12. Sound engines and effects

With both effects Off, compare all ten engines using the same chord and octave:

1. Velvet Poly
2. Dust Piano
3. Chapel Organ
4. String Haze
5. Tape Choir
6. Wire Pluck
7. Frozen Glass
8. Doom Stack
9. Smoke Reed
10. Sub Altar

For each engine, note attack, spectral identity, useful macro ranges, low/high balance and any redundant or unpleasant region.

Then enable effects and check tails, level jumps, crackles and stereo behaviour on both speaker and headphones.

## 13. Latency, stability and thermal behaviour

- estimate button-to-sound latency with headphones;
- play repeated short chords in PAD and STRUM;
- run ARP and PULSE for at least five minutes;
- edit sound and effects during sustained playback;
- run continuously for at least 15 minutes;
- watch for underruns, clicks, stuck notes, rising latency, UI slowdown, memory growth and unusual heat;
- test Knulli suspend/resume if practical;
- confirm exit stops audio and returns control to Knulli.

## 14. Logs and bug report

Collect:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.log
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
```

Commands:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log ~/Downloads/
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.cfg ~/Downloads/
```

Recommended report:

```text
BRKCHRD version:
Knulli version:
Left-panel mode:
Right-panel mode:
Focused side:
Physical control:
Expected result:
Actual result:
Reproduction steps:
Speaker / headphones:
Engine and parameters:
FX settings:
Attached log:
Photo or video:
```

Prefer one reproducible issue per report. A short video is particularly useful for controller and animation problems.
