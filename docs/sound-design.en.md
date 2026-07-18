# Sound design — English

## Ten synthesis engines

The presets are not parameter variations of one oscillator pair. Each uses a separate synthesis path:

1. **Velvet Poly — Dual Analog:** saw, triangle and controllable sub/pulse body.
2. **Dust Piano — Electric Tines:** phase-modulated tine fundamental with a decaying high transient.
3. **Chapel Organ — Additive Organ:** several harmonic sine drawbars and long sustain.
4. **String Haze — String Ensemble:** detuned saw/triangle ensemble with wide modulation.
5. **Tape Choir — Formant Choir:** moving vowel balance, drift and controlled breath noise.
6. **Wire Pluck — Resonant Pluck:** rapidly decaying harmonic/noise excitation.
7. **Frozen Glass — Phase Mod:** changing modulation index and metallic upper partials.
8. **Doom Stack — Power Stack:** square, saw, octave/sub content and nonlinear saturation.
9. **Smoke Reed — Pulse Reed:** moving pulse width with sine/triangle body.
10. **Sub Altar — Sub Harmonics:** fundamental, sub-octave and fifth-related low mass.

## SOUND macros

| Macro | Meaning |
| --- | --- |
| Tone | filter opening and high-frequency harmonic content |
| Body | low/mid harmonic weight and model-specific density |
| Motion | drift, ensemble movement and modulation speed |
| Attack | onset time from immediate to slow swell |
| Release | tail length after releasing a chord |
| Spread | detune and stereo width |
| BPM | arpeggio, pulse and time-dependent performance speed |
| Play Mode | PAD, STRUM, ARP or PULSE |

Preset selection loads a complete factory state, including macros and effects. Any later edit is stored in `brkchrd.cfg`.

## Effects

Two serial effect slots are available. Every slot has:

- **Type:** OFF, CHORUS, PHASER, TREMOLO, DRIVE, CRUSHER, DELAY or REVERB;
- **Amount:** wet depth/intensity;
- **Colour:** the effect-specific secondary control, such as rate, delay time, reverb size or crusher rate.

The chain is:

```text
voices → model filter/body → FX1 → FX2 → master soft limiter → SDL audio
```

## Gain and safety

The engine normalises very dense extended chords, applies a master control and finishes with soft clipping. The UI meter is a smoothed peak indicator, not a calibrated dB meter. Speaker-safe defaults are conservative; DOOM STACK can still become loud when DRIVE and MASTER are raised together.
