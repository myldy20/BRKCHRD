# Installing BRKCHRD 0.4.0 on TrimUI Brick / Knulli

## Requirements

- TrimUI Brick running Knulli/PortMaster;
- the `brkchrd-v040-portmaster-aarch64` artifact from a successful GitHub Actions run;
- approximately 2 MB free space plus configuration and log files;
- SSH is optional but is the fastest update method.

GitHub wraps the artifact in an outer ZIP. The actual installation archive inside it is:

```text
brkchrd-v0.4.0-portmaster.zip
```

## Install through SSH

With a `trimui` alias in `~/.ssh/config`:

```bash
scp ~/Downloads/brkchrd-v0.4.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.4.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Without an alias:

```bash
scp ~/Downloads/brkchrd-v0.4.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/
```

Then connect and install:

```bash
ssh root@10.53.219.134
unzip -o /userdata/system/brkchrd-v0.4.0-portmaster.zip \
  -d /userdata/roms/ports/
chmod +x "/userdata/roms/ports/BRKCHRD.sh"
chmod +x /userdata/roms/ports/brkchrd/*.aarch64
sync
reboot
```

## Expected installation tree

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/docs/
/userdata/roms/ports/brkchrd/licenses/
/userdata/roms/ports/brkchrd/conf/
```

Installing with `unzip -o` preserves `conf/` and the existing text configuration.

## Updating from 0.3

Version 0.4 reads useful 0.3 values such as key, octave, engine parameters and base effects. Old panel-mode and latch values are ignored or replaced by the new D-pad workflow. Latch is always disabled.

Back up the current configuration before testing:

```bash
ssh trimui '
  rm -rf /userdata/system/brkchrd-conf-backup &&
  cp -a /userdata/roms/ports/brkchrd/conf \
    /userdata/system/brkchrd-conf-backup
'
```

To force all 0.4 defaults without deleting the old file:

```bash
ssh trimui '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v040
'
```

## Controller order

The confirmed mapping provides four separate rear controls. If L1 acts like L2 or R1 acts like R2, open Settings and toggle:

- `SWAP L1/L2`
- `SWAP R1/R2`

Do not globally remap the Knulli controller before trying these options.

## Logs

View the log remotely:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Copy it to a Mac:

```bash
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v040.log
```

The log contains the SDL mapping, raw button/axis events and the expected roles for front octave buttons, L1/L2 and R1/R2.

## Uninstall

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
