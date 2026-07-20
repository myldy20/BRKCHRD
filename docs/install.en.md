# Installing BRKCHRD 0.5.0 on TrimUI Brick / Knulli

Download the `brkchrd-v050-portmaster-aarch64` artifact from a successful GitHub Actions run. GitHub supplies an outer artifact ZIP; inside it is:

```text
brkchrd-v0.5.0-portmaster.zip
```

## SSH installation

```bash
scp ~/Downloads/brkchrd-v0.5.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/

ssh root@10.53.219.134 '
  unzip -o /userdata/system/brkchrd-v0.5.0-portmaster.zip \
    -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Expected files:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

## Updating from 0.4

The 0.5 configuration reader keeps compatible key, octave, palette, bank, sound and base-FX values. New defaults when absent:

```text
VOICE LEAD = OFF
PERF FX = ON
```

The physical duties of L1/L2 changed in 0.5. L1 now changes D-pad mode and L2 holds the alternate layer. Existing `SWAP L1/L2` remains available for firmware-order differences.

For a completely clean test:

```bash
ssh root@10.53.219.134 '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v050 2>/dev/null || true
'
```

## Logs

```bash
ssh root@10.53.219.134 \
  'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Copy to Mac:

```bash
scp root@10.53.219.134:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v050.log
```
