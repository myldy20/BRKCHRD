# Installing BRKCHRD 0.5.1 on NextUI

BRKCHRD ships as a native Pak for the `tg5040` platform. The release archive is named:

```text
brkchrd-v0.5.1-nextui-tg5040.zip
```

## Package layout

The archive contains:

```text
Tools/tg5040/BRKCHRD.pak/
Tools/tg5040/.media/BRKCHRD.png
```

The `.pak` directory contains the launcher, AArch64 executable, documentation and license files. The sibling `.media/BRKCHRD.png` is the image displayed on the NextUI tool card.

## Install from an SD card

1. Shut down the device and mount its SD card.
2. Extract the archive to the root of the card, preserving the `Tools/tg5040/` path.
3. Confirm:

```text
SD/Tools/tg5040/BRKCHRD.pak/launch.sh
SD/Tools/tg5040/BRKCHRD.pak/brkchrd-sdl.aarch64
SD/Tools/tg5040/.media/BRKCHRD.png
```

4. Return the card and open the Tools section. BRKCHRD should appear with the project artwork.

## Persistent data

The launcher uses NextUI user-data conventions rather than writing into the Pak:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

This means the Pak can be replaced during an upgrade without deleting user settings.

## Upgrade

Extract the new archive over the existing `Tools/tg5040/` tree. Do not delete `.userdata` unless factory defaults are required.

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

- card image missing: verify exact case and path `.media/BRKCHRD.png`;
- tool starts and closes: read `.userdata/tg5040/logs/BRKCHRD.txt`;
- settings do not persist: verify the SD card is writable and `.userdata/tg5040/brkchrd/` can be created;
- no audio: check system output routing and volume before changing BRKCHRD parameters.
