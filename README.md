<p align="center">
  <img src="assets/branding/brkchrd-banner.jpg" width="900" alt="BRKCHRD — chord-performance instrument for TrimUI Brick">
</p>

# BRKCHRD

<p align="center">
  <strong>A playable harmony instrument for TrimUI Brick.</strong><br>
  Functional chords, instrument-aware voicings, sixteen synthesis presets and live performance FX without a laptop.
</p>

<p align="center">
  <img alt="Version" src="https://img.shields.io/badge/version-0.5.1-f1e3c5?labelColor=111514">
  <img alt="Platform" src="https://img.shields.io/badge/platform-TrimUI%20Brick-8f8699?labelColor=111514">
  <img alt="Architectures" src="https://img.shields.io/badge/arch-aarch64-8f8699?labelColor=111514">
  <img alt="License" src="https://img.shields.io/badge/license-GPL--3.0--or--later-8f8699?labelColor=111514">
</p>

**English:** [Manual](docs/manual.en.md) · [Controls](docs/controls.en.md) · [Sound design](docs/sound-design.en.md) · [Installation](docs/install.en.md) · [NextUI](docs/install.nextui.en.md) · [Architecture](docs/architecture.en.md) · [Development](docs/development.en.md) · [Troubleshooting](docs/troubleshooting.en.md) · [Licensing](docs/licensing.en.md)

**Русский:** [Руководство](docs/manual.ru.md) · [Управление](docs/controls.ru.md) · [Звуковой движок](docs/sound-design.ru.md) · [Установка](docs/install.ru.md) · [NextUI](docs/install.nextui.ru.md) · [Архитектура](docs/architecture.ru.md) · [Разработка](docs/development.ru.md) · [Диагностика](docs/troubleshooting.ru.md) · [Лицензирование](docs/licensing.ru.md)

---

## English

BRKCHRD turns the four face buttons into a coherent chord instrument. The default bank provides the core progression functions; R1 and R2 temporarily expose alternative banks, while the D-pad selects harmonic colour, edits the current sound or performs momentary effects.

### What makes it different

- **Playable harmony rather than a chord encyclopedia.** The banks are arranged for performance and songwriting.
- **Deterministic or expressive voicing.** `VOICE LEAD = OFF` always returns the same notes for the same button; `ON` enables route-sensitive nearby inversions without ignoring the selected octave.
- **Instrument-aware orchestration.** Pads can use wide five-note voicings, while bass and heavy presets automatically avoid dense low extensions.
- **Sixteen factory sounds.** Keys, pads, organ, strings, choir, plucks, reeds, glass, heavy and sub-oriented patches use distinct synthesis paths.
- **Two installation formats.** PortMaster/Knulli and a native NextUI Pak for `tg5040` devices.

### Main controls

| Control | Action |
| --- | --- |
| Front illuminated left/right | Octave down/up |
| L1 press | Cycle `CHORD → SOUND → PERF FX` |
| L2 hold | Alternate colour, sound or PERF FX layer |
| R1 / R2 hold | Temporary alternate chord banks |
| D-pad | Harmonic colour, sound editing or momentary FX |
| ABXY | Play the active functional chord set |
| Select | Settings |
| Start tap | PAD / STRUM / ARP / PULSE |
| Start hold | All notes off |
| Start + Select | Save and exit |

`PERF FX` can be disabled in Settings. When disabled, L1 cycles only between CHORD and SOUND.

### 0.5.1 interface polish

- preset names use a dedicated larger two-line-height card and remain inside the SOUND panel;
- percentages and other values are right-aligned with safe padding from the frame;
- the selected parameter keeps a clearly visible white value bar;
- Settings displays `START+SELECT: SAVE AND EXIT` correctly;
- official artwork is included in the repository, PortMaster package and NextUI card.

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
./build/brkchrd-sdl
```

See [Development](docs/development.en.md) for architecture, packaging, tests and contribution rules.

---

## Русский

BRKCHRD превращает четыре лицевые кнопки в цельный гармонический инструмент. Основной банк содержит базовые функции тональности; R1 и R2 временно открывают дополнительные банки, а крестовина выбирает окраску аккорда, редактирует звук или управляет моментальными сценическими эффектами.

### В чём идея

- **Не справочник аккордов, а инструмент для игры.** Банки организованы вокруг музыкальных функций и удобных переходов.
- **Стабильные или живые раскладки.** При `VOICE LEAD = OFF` одна кнопка всегда выдаёт одинаковые ноты. При `ON` порядок переходов влияет на инверсии, но выбранная октава сохраняет смысл.
- **Раскладка зависит от тембра.** Пэды получают широкие аккорды, а басовые и перегруженные пресеты не забиваются плотными низкими расширениями.
- **Шестнадцать заводских звуков.** Клавишные, пэды, орган, струнные, хор, щипковые, язычковые, стеклянные, тяжёлые и сабовые тембры используют разные методы синтеза.
- **Два варианта установки.** PortMaster/Knulli и нативный NextUI Pak для платформы `tg5040`.

### Главное управление

| Кнопка | Действие |
| --- | --- |
| Передние светящиеся слева/справа | Октава вниз/вверх |
| L1 нажатием | `CHORD → SOUND → PERF FX` |
| L2 удержанием | Альтернативный слой текущего режима |
| R1 / R2 удержанием | Временные дополнительные банки аккордов |
| Крестовина | Окраска, редактирование звука или моментальные FX |
| ABXY | Игра аккордов активного банка |
| Select | Настройки |
| Start коротко | PAD / STRUM / ARP / PULSE |
| Start долго | Снять все ноты |
| Start + Select | Сохранить и выйти |

Режим `PERF FX` можно отключить. Тогда L1 переключает только CHORD и SOUND.

### Что исправлено в 0.5.1

- название пресета вынесено в отдельную крупную карточку и больше не вылезает за рамку;
- проценты и остальные значения выровнены справа с нормальным отступом;
- шкала выбранного параметра больше не исчезает;
- внизу Settings корректно написано `START+SELECT: SAVE AND EXIT`;
- фирменный логотип добавлен в репозиторий, PortMaster и карточку NextUI.

## License / Лицензия

BRKCHRD is licensed under **GNU GPL-3.0-or-later**. Modified distributions must preserve the origin notice described in [`NOTICE.md`](NOTICE.md):

> Based on BRKCHRD by Myldy design — https://github.com/myldy20/BRKCHRD

BRKCHRD распространяется по лицензии **GNU GPL-3.0-or-later**. При распространении модифицированной версии необходимо сохранить уведомление об исходном проекте из [`NOTICE.md`](NOTICE.md). Форки должны использовать собственное оформление и не выдавать себя за официальный релиз BRKCHRD.

Developed by **Myldy design** — [@myldy20](https://github.com/myldy20).
