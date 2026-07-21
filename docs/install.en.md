# Installing BRKCHRD 0.6.0 on TrimUI Brick / Knulli

## Download the package

Download the ready-to-install build here:

**[`brkchrd-v0.6.0-portmaster.zip`](https://github.com/myldy20/BRKCHRD/releases/download/v0.6.0/brkchrd-v0.6.0-portmaster.zip)**

Alternatively, open the [latest release](https://github.com/myldy20/BRKCHRD/releases/latest), expand **Assets** and choose `brkchrd-v0.6.0-portmaster.zip`.

> Do not use the green **Code → Download ZIP** button. That archive contains source code, not the ready-to-install application.

## Requirements

- TrimUI Brick or another compatible AArch64 handheld;
- Knulli with PortMaster support;
- `brkchrd-v0.6.0-portmaster.zip` from the link above;
- approximately 5 MB of free space.

## Install from the SD card

1. Shut down the handheld cleanly and mount its SD card.
2. Extract the **contents** of `brkchrd-v0.6.0-portmaster.zip` into:

```text
/userdata/roms/ports/
```

The resulting installation includes:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/cover.jpg
/userdata/roms/ports/brkchrd/docs/
```

Return the card, boot Knulli and refresh the game list if BRKCHRD is not visible immediately.

## Install over SSH

Assuming the archive is in `~/Downloads` and the handheld IP is `10.53.219.134`:

```bash
scp ~/Downloads/brkchrd-v0.6.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/

ssh root@10.53.219.134 '
  unzip -o /userdata/system/brkchrd-v0.6.0-portmaster.zip \
    -d /userdata/roms/ports/ &&
  chmod +x /userdata/roms/ports/BRKCHRD.sh &&
  chmod +x /userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64 &&
  sync
'
```

Refresh Ports or reboot after installation.

## Upgrade

Extract 0.6.0 over an existing installation. The configuration format is compatible and missing settings use safe defaults.

Back up the configuration before resetting or testing factory defaults:

```bash
ssh root@10.53.219.134 '
  cp /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.backup \
     2>/dev/null || true
'
```

To reset BRKCHRD, rename `brkchrd.cfg`; the application will create a new file on the next save.

## Runtime data

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

The language, CHORD DPAD mode, selected UI palette and all other settings are stored in this configuration.

## Uninstall

Back up `brkchrd.cfg` if required, then remove:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/
```

For launch failures, see [Troubleshooting](troubleshooting.en.md). Physical release validation is documented separately in the contributor hardware checklist.
