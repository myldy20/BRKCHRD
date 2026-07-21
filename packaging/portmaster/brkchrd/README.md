# BRKCHRD 0.6.0 — PortMaster / Knulli

Extract the archive into the PortMaster ports directory. The package includes the native AArch64 executable, project artwork, GPLv3 and attribution notices, and complete English/Russian documentation under `brkchrd/docs/`.

## Controls

- front illuminated left/right: octave down/up;
- L1 press: CHORD → SOUND → PERF FX;
- L2 hold: alternate layer;
- R1/R2 hold: temporary chord banks;
- D-pad: chord colour, sound editing or momentary FX;
- ABXY: play functional chords;
- Select: Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

New 0.6.0 settings:

- `LANGUAGE / ЯЗЫК = EN / RU` switches the interface immediately;
- `CHORD DPAD = TOGGLE` stores a colour;
- `CHORD DPAD = HOLD` applies a colour only while the direction is held and returns to BASE on release.

`VOICE LEAD = OFF` gives deterministic notes and exact octave changes. `ON` enables nearby route-sensitive inversions. `PERF FX` can be removed from the L1 cycle without disabling FX1/FX2.

Runtime files:

```text
brkchrd/conf/brkchrd.cfg
brkchrd/conf/brkchrd.log
```

---

# BRKCHRD 0.6.0 — PortMaster / Knulli

Распакуйте архив в каталог портов PortMaster. В пакет входят нативный AArch64-бинарник, оформление проекта, полный текст GPLv3, уведомление об исходном проекте и документация на русском и английском в `brkchrd/docs/`.

## Управление

- передние светящиеся слева/справа: октава вниз/вверх;
- L1 нажатием: АККОРД → ЗВУК → ПЕРФ FX;
- L2 удержанием: альтернативный слой;
- R1/R2 удержанием: временные банки;
- крестовина: окраска, редактирование звука или моментальные FX;
- ABXY: игра аккордов;
- Select: настройки;
- Start коротко: ПЭД / БОЙ / АРП / ПУЛЬС;
- Start долго: снять все ноты;
- Start + Select: сохранить и выйти.

Новые настройки 0.6.0:

- `LANGUAGE / ЯЗЫК = EN / RU` сразу переключает интерфейс;
- `DPAD АККОРД = НАЖАТИЕ` сохраняет окраску;
- `DPAD АККОРД = УДЕРЖ.` применяет её только во время удержания и возвращает ОСНОВУ после отпускания.

При выключенном ВЕДЕНИИ одна комбинация всегда выдаёт одинаковые ноты. При включённом переходы влияют на инверсии. ПЕРФ FX можно отключить независимо от FX1/FX2.
