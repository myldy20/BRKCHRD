# Troubleshooting — English

## The application does not appear under Ports

Check the launcher path and executable bit:

```bash
ssh trimui 'ls -l "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64'
```

Repair permissions:

```bash
ssh trimui 'chmod +x "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64 && sync'
```

Refresh the game list or reboot.

## Immediate exit

Read the log:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Typical causes are a missing binary, incompatible architecture, SDL audio/video initialisation failure or an incomplete PortMaster control environment.

## FX1/FX2 do not respond

BRKCHRD accepts L3/R3 and trigger-axis mappings. If neither works, capture raw input information with Knulli controller mapping or run an SDL event diagnostic. Do not remap the whole controller blindly: that can swap Start/Select or physical face-button positions for every PortMaster application.

## X and Y appear swapped

Knulli may use positional Xbox-style mapping rather than printed labels. BRKCHRD follows SDL GameController's A/B/X/Y semantic mapping. First verify Knulli Controller Mapping. A project-specific printed-label override can be added later if the device log proves it is required.

## Crackles or high CPU use

- reduce FX amounts, especially two reverbs/delays in series;
- avoid maximum Release with dense six-note extended chords during testing;
- close Wi-Fi-heavy background activity;
- inspect the log for audio-device mismatch;
- report preset, chord colour, mode and FX settings together with the observed CPU load.

## Reset configuration

```bash
ssh trimui 'mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.bak'
```

The next launch uses factory defaults.

## Useful bug report

Include firmware version, exact archive name, `brkchrd.log`, what each physical button produced, headphones/speaker, audible latency, crackles, and a photo of any layout overlap.
