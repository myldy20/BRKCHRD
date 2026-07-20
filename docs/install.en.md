# Installing BRKCHRD 0.5.1 on TrimUI Brick / Knulli

## Requirements

- TrimUI Brick or another compatible AArch64 handheld;
- Knulli with PortMaster support;
- the `brkchrd-v0.5.1-portmaster.zip` release file;
- approximately 5 MB of free space;
- SSH is optional but is the fastest method for development builds.

## Method A: SD-card installation

1. Shut down the handheld cleanly and remove the SD card.
2. Open the PortMaster ports directory.
3. Extract the **contents** of `brkchrd-v0.5.1-portmaster.zip` into:

```text
/userdata/roms/ports/
```

4. Confirm these paths exist:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/cover.jpg
```

5. Return the card, boot Knulli and refresh the game list if BRKCHRD is not visible immediately.

The package also contains top-level `screenshot.jpg` and `cover.jpg` for PortMaster metadata and the complete documentation under `brkchrd/docs/`.

## Method B: SSH installation

Assuming the archive is in `~/Downloads` and the handheld IP is `10.53.219.134`:

```bash
scp ~/Downloads/brkchrd-v0.5.1-portmaster.zip \
  root@10.53.219.134:/userdata/system/

ssh root@10.53.219.134 '
  unzip -o /userdata/system/brkchrd-v0.5.1-portmaster.zip \
    -d /userdata/roms/ports/ &&
  chmod +x /userdata/roms/ports/BRKCHRD.sh &&
  chmod +x /userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64 &&
  sync
'
```

Refresh Ports or reboot after installation.

## Upgrading from 0.5.0

The executable and documentation can be overwritten in place. The configuration format remains compatible.

Before testing factory defaults, preserve the old file:

```bash
ssh root@10.53.219.134 '
  cp /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.backup \
     2>/dev/null || true
'
```

To start with defaults, rename the configuration:

```bash
ssh root@10.53.219.134 '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v051 \
     2>/dev/null || true
'
```

## Runtime data

The launcher sets HOME and XDG directories to the persistent package location:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

Do not place personal settings inside the executable directory outside `conf/`, because a future package extraction may overwrite them.

## Uninstall

Back up `brkchrd.cfg` if required, then remove:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/
```

## First verification

1. Confirm the two-second logo appears.
2. Open SOUND and verify the preset name stays inside the large card.
3. Select Motion and confirm its bar remains visible.
4. Open Settings and confirm values have right padding and the footer says `SAVE AND EXIT`.
5. Test Start+Select and verify configuration is restored on the next launch.

For launch failures, see [Troubleshooting](troubleshooting.en.md).
