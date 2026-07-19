# Installing BRKCHRD 0.3 on TrimUI Brick / Knulli

## Requirements

- TrimUI Brick running Knulli/PortMaster;
- the `brkchrd-v030-portmaster-aarch64` artifact from a successful GitHub Actions run;
- approximately 2 MB free space plus configuration and log files;
- SSH is optional but is the fastest update method.

GitHub wraps the artifact in an outer ZIP. The actual installation archive inside it is:

```text
brkchrd-v0.3.0-portmaster.zip
```

## Install through SSH

With a `trimui` alias in `~/.ssh/config`:

```bash
scp ~/Downloads/brkchrd-v0.3.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.3.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Without an alias:

```bash
scp ~/Downloads/brkchrd-v0.3.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/
```

Then connect:

```bash
ssh root@10.53.219.134
```

And run:

```bash
unzip -o /userdata/system/brkchrd-v0.3.0-portmaster.zip \
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

`conf/` is created or reused at runtime. Installing with `unzip -o` does not delete settings.

## Back up settings

```bash
ssh trimui '
  rm -rf /userdata/system/brkchrd-conf-backup &&
  cp -a /userdata/roms/ports/brkchrd/conf \
    /userdata/system/brkchrd-conf-backup
'
```

## Startup and input log

View it remotely:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Copy it to a Mac:

```bash
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v030.log
```

Version 0.3 logs the SDL mapping and the first raw button/axis events. This is especially useful when testing another Knulli release.

## Uninstall

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
