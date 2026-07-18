# Диагностика — Русский

## BRKCHRD не появился в Ports

Проверь пути и права:

```bash
ssh trimui 'ls -l "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64'
```

Исправить права:

```bash
ssh trimui 'chmod +x "/userdata/roms/ports/BRKCHRD.sh" /userdata/roms/ports/brkchrd/*.aarch64 && sync'
```

После этого обнови список игр или перезагрузи консоль.

## Программа сразу закрывается

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Типовые причины: отсутствует бинарник, неправильная архитектура, ошибка инициализации SDL audio/video или неполное окружение PortMaster.

## Не работают FX1/FX2

Программа принимает как L3/R3, так и trigger axes. Если не работает ни один вариант, нужен сырой лог SDL-событий или проверка Controller Mapping в Knulli. Не переназначай весь контроллер наугад — можно сломать Start/Select и раскладку A/B/X/Y во всех портах.

## Перепутаны X и Y

Knulli может использовать позиционную Xbox-разметку, а не надписи на корпусе. BRKCHRD следует семантике SDL GameController. Сначала проверь Controller Mapping. Отдельный режим «по физическим надписям» стоит добавлять только после аппаратного лога.

## Треск или высокая нагрузка

- снизь Amount эффектов, особенно двух Delay/Reverb подряд;
- на тесте не сочетай максимальный Release с плотными шестинотными аккордами;
- отключи лишнюю сетевую активность;
- приложи к баг-репорту пресет, окраску, режим, FX и фактическую нагрузку.

## Сброс настроек

```bash
ssh trimui 'mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.bak'
```

Следующий запуск создаст заводское состояние.

## Нормальный баг-репорт

Нужны: версия прошивки, имя архива, `brkchrd.log`, реакция каждой физической кнопки, наушники/динамик, задержка, треск и фото любого наложения текста.
