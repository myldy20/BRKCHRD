# BRKCHRD 0.5.1 — PortMaster / Knulli

Extract the release archive into the PortMaster ports directory. The package includes `screenshot.jpg`, `cover.jpg`, the native AArch64 executable, complete GPLv3 and attribution notices, and full English/Russian documentation under `brkchrd/docs/`.

## Controls

- front illuminated left/right: octave down/up;
- L1 press: cycle CHORD → SOUND → PERF FX;
- L2 hold: alternate layer;
- R1/R2 hold: temporary chord banks;
- D-pad: chord colour, sound edit or momentary FX;
- ABXY: play functional chords;
- Select: Settings;
- Start tap: PAD / STRUM / ARP / PULSE;
- Start hold: all notes off;
- Start + Select: save and exit.

`VOICE LEAD = OFF` gives deterministic notes and exact octave changes. `ON` enables nearby route-sensitive inversions. `PERF FX` can be removed from the L1 cycle without disabling the ordinary FX slots.

## Runtime files

```text
brkchrd/conf/brkchrd.cfg
brkchrd/conf/brkchrd.log
```

---

# BRKCHRD 0.5.1 — PortMaster / Knulli

Распакуйте релизный архив в каталог портов PortMaster. В пакет входят `screenshot.jpg`, `cover.jpg`, нативный AArch64-бинарник, полный текст GPLv3, уведомление об исходном проекте и подробная документация на русском и английском в `brkchrd/docs/`.

## Управление

- передние светящиеся слева/справа: октава вниз/вверх;
- L1 нажатием: CHORD → SOUND → PERF FX;
- L2 удержанием: альтернативный слой;
- R1/R2 удержанием: временные банки аккордов;
- крестовина: окраска, редактирование звука или моментальные FX;
- ABXY: игра аккордов;
- Select: настройки;
- Start коротко: PAD / STRUM / ARP / PULSE;
- Start долго: снять все ноты;
- Start + Select: сохранить и выйти.

При `VOICE LEAD = OFF` одна комбинация всегда выдаёт одинаковые ноты. При `ON` переходы влияют на инверсии. `PERF FX` можно отключить независимо от обычных FX1/FX2.
