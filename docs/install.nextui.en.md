# Installing BRKCHRD 0.6.0 on NextUI

BRKCHRD ships as a native Pak for the `tg5040` platform:

```text
brkchrd-v0.6.0-nextui-tg5040.zip
```

## Package layout

```text
Tools/tg5040/BRKCHRD.pak/
Tools/tg5040/.media/BRKCHRD.png
```

The Pak contains the launcher, AArch64 executable, RU/EN documentation and license files. The sibling `.media/BRKCHRD.png` is displayed on the NextUI tool card.

## Install

1. Shut down the device and mount its SD card.
2. Extract the archive to the root of the card while preserving `Tools/tg5040/`.
3. Return the card and open Tools.

The resulting paths are:

```text
SD/Tools/tg5040/BRKCHRD.pak/launch.sh
SD/Tools/tg5040/BRKCHRD.pak/brkchrd-sdl.aarch64
SD/Tools/tg5040/.media/BRKCHRD.png
```

## Persistent data

The launcher stores user data outside the Pak:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

This preserves the selected language, CHORD DPAD mode and all other settings when the Pak is replaced.

## Upgrade

Extract the 0.6.0 archive over the existing `Tools/tg5040/` tree. Do not remove `.userdata` unless factory defaults are required.

To reset only BRKCHRD, rename:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
```

## Uninstall

Remove:

```text
SD/Tools/tg5040/BRKCHRD.pak/
SD/Tools/tg5040/.media/BRKCHRD.png
```

Optionally remove `SD/.userdata/tg5040/brkchrd/` after backing up the configuration.

## Troubleshooting

- card image missing: verify the exact case and path `.media/BRKCHRD.png`;
- tool starts and closes: read `.userdata/tg5040/logs/BRKCHRD.txt`;
- settings do not persist: verify the card is writable and `.userdata/tg5040/brkchrd/` can be created;
- no audio: check the system output route and volume.
