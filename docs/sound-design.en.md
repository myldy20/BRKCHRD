# BRKCHRD 0.4.0 Sound Design — English

## Ten synthesis engines

The presets use distinct synthesis paths rather than different labels for one oscillator pair:

1. **Velvet Poly — Dual Analog:** saw, triangle and controllable sub/pulse body.
2. **Dust Piano — Electric Tines:** phase-modulated tine fundamental with a decaying high transient.
3. **Chapel Organ — Additive Organ:** harmonic sine drawbars and long sustain.
4. **String Haze — String Ensemble:** detuned saw/triangle ensemble with wide modulation.
5. **Tape Choir — Formant Choir:** moving vowel balance, drift and controlled breath noise.
6. **Wire Pluck — Resonant Pluck:** rapidly decaying harmonic/noise excitation.
7. **Frozen Glass — Phase Mod:** changing modulation index and metallic upper partials.
8. **Doom Stack — Power Stack:** square, saw, octave/sub content and nonlinear saturation.
9. **Smoke Reed — Pulse Reed:** moving pulse width with sine/triangle body.
10. **Sub Altar — Sub Harmonics:** fundamental, sub-octave and fifth-related low mass.

## SOUND parameter layers

SOUND uses L1 as a momentary layer switch.

Normal layer:

| Parameter | Meaning |
| --- | --- |
| Preset | complete factory starting point |
| Tone | filter opening and high-frequency harmonic content |
| Body | low/mid harmonic weight and model-specific density |
| Motion | drift, ensemble movement and modulation speed |

L1-held layer:

| Parameter | Meaning |
| --- | --- |
| Attack | onset time from immediate to slow swell |
| Release | tail length after releasing a chord |
| Spread | detune and stereo width |
| BPM | ARP, PULSE and tempo-dependent effect timing |
| Play Mode | PAD, STRUM, ARP or PULSE |

The chord colour and chord-bank state are independent from SOUND. A stored maj7, power, open or other colour therefore remains audible while these parameters are edited.

## Base effects

Two serial base effect slots are configured in Settings. Every slot has:

- **Type:** OFF, CHORUS, PHASER, TREMOLO, DRIVE, CRUSHER, DELAY or REVERB;
- **Amount:** wet depth or intensity;
- **Colour:** effect-specific secondary control such as rate, delay time, reverb size or crusher rate.

The permanent chain is:

```text
voices → model filter/body → FX1 → FX2 → master soft limiter → SDL audio
```

## Performance FX

PERF FX is separate from the base effect configuration. On the first D-pad press, BRKCHRD snapshots FX1 and FX2. While a direction is held it substitutes a two-effect performance combination. Releasing all directions restores the snapshot.

Normal bank gestures:

- **Reverse-ish:** dark delay with controlled crushing;
- **Stutter:** rapid tremolo gating;
- **Chop:** harder gating plus crusher;
- **Crush:** strong crusher and drive;
- **Drive:** saturated drive with moving phase;
- **Wash:** large reverb with chorus;
- **Deep Echo:** long delay into reverb;
- **Phase:** deep phaser plus chorus.

L1-held bank gestures:

- **Abyss:** maximum reverb and delay;
- **Ratetrap:** very fast gating with short echo;
- **Glitch:** severe crusher plus gating;
- **Broken:** low-resolution crusher and extreme phase;
- **Doom:** maximum drive into dark reverb;
- **Shimmer-ish:** wide chorus and large reverb;
- **Tunnel:** very long delay with resonant movement;
- **Alien:** extreme phase plus crusher.

The names describe musical behaviour. They are not claimed clones of EP-133 algorithms. The current implementation combines BRKCHRD's existing DSP blocks; dedicated reverse, granular and beat-repeat engines are a future extension.

## Restoration guarantees

Performance FX are restored when:

- the D-pad returns to centre;
- L2 changes to another D-pad mode;
- Select opens Settings;
- the program exits.

This prevents a temporary destructive effect from overwriting the user's normal effect chain.

## Gain and safety

Dense extended chords are normalised, followed by the base/performance effect chain and soft clipping. The UI meter is a smoothed peak indicator, not a calibrated dB meter. Doom Stack combined with high Drive, Crusher or performance FX can still be substantially louder; begin testing at a moderate hardware volume.
