# Installation on TrimUI Brick / Knulli

## Requirements

- TrimUI Brick running Knulli/PortMaster;
- AArch64 build artifact from a successful GitHub Actions run;
- approximately 2 MB free space plus configuration/log files;
- SSH is optional but recommended for rapid testing.

## Install through SSH

Assume the downloaded inner package is named `brkchrd-v0.2.0-portmaster.zip` and the console is available as `trimui` in `~/.ssh/config`.

```bash
scp ~/Downloads/brkchrd-v0.2.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.2.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Without an SSH alias:

```bash
scp ~/Downloads/brkchrd-v0.2.0-portmaster.zip root@10.53.219.134:/userdata/system/
```

## Expected installation tree

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/docs/
/userdata/roms/ports/brkchrd/licenses/
/userdata/roms/ports/brkchrd/conf/
```

`conf/` is created or reused at runtime. Reinstalling with `unzip -o` does not intentionally delete the configuration file.

## Updating

Back up settings if desired:

```bash
ssh trimui 'cp -a /userdata/roms/ports/brkchrd/conf /userdata/system/brkchrd-conf-backup'
```

Then install the new ZIP over the existing directory. The configuration format is text and unknown keys are ignored.

## Uninstall

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
