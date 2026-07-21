# BRKCHRD 0.6.0 — NextUI Pak

Native `tg5040` package for TrimUI Brick and compatible NextUI devices.

Copy these paths from the archive to the root of the SD card:

```text
Tools/tg5040/BRKCHRD.pak/
Tools/tg5040/.media/BRKCHRD.png
```

Persistent configuration and log:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

The external data location preserves the selected language, CHORD DPAD mode and all other settings when the Pak is upgraded.

## Controls

- front illuminated left/right: octave down/up;
- L1 press: CHORD → SOUND → PERF FX;
- L2 hold: alternate layer;
- R1/R2 hold: alternate chord banks;
- Select: Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

In Settings:

- `LANGUAGE / ЯЗЫК = EN / RU` switches the interface;
- `CHORD DPAD = TOGGLE` stores a chord colour;
- `CHORD DPAD = HOLD` applies it only while the D-pad is held.

Полная документация на русском и английском находится в `docs/`. Русский интерфейс выбирается строкой `LANGUAGE / ЯЗЫК`, а режим временной окраски — строкой `DPAD АККОРД = УДЕРЖ.`.

Licensed under GPL-3.0-or-later. Redistributed modified versions must preserve `NOTICE.md`.
