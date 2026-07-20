# Установка BRKCHRD 0.5.1 на TrimUI Brick / Knulli

## Что нужно

- TrimUI Brick или другая совместимая AArch64-консоль;
- Knulli с поддержкой PortMaster;
- релизный файл `brkchrd-v0.5.1-portmaster.zip`;
- около 5 МБ свободного места;
- SSH необязателен, но удобнее всего для тестовых обновлений.

## Способ A: через SD-карту

1. Корректно выключите консоль и извлеките карту.
2. Откройте каталог портов PortMaster.
3. Распакуйте **содержимое** `brkchrd-v0.5.1-portmaster.zip` в:

```text
/userdata/roms/ports/
```

4. Проверьте наличие файлов:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/cover.jpg
```

5. Верните карту, загрузите Knulli и обновите список игр, если BRKCHRD не появился сразу.

В архив также входят верхнеуровневые `screenshot.jpg` и `cover.jpg` для метаданных PortMaster и полная документация в `brkchrd/docs/`.

## Способ B: через SSH

Если архив лежит в `~/Downloads`, а адрес консоли — `10.53.219.134`:

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

После установки обновите список Ports или перезагрузите консоль.

## Обновление с 0.5.0

Бинарник и документацию можно перезаписать поверх. Формат настроек совместим.

Перед тестом заводских значений сохраните копию:

```bash
ssh root@10.53.219.134 '
  cp /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.backup \
     2>/dev/null || true
'
```

Для чистого запуска переименуйте конфиг:

```bash
ssh root@10.53.219.134 '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v051 \
     2>/dev/null || true
'
```

## Постоянные данные

Launcher направляет HOME и XDG в сохраняемый каталог пакета:

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

Не храните личные файлы рядом с бинарником вне `conf/`: следующая распаковка релиза может их перезаписать.

## Удаление

При необходимости сохраните `brkchrd.cfg`, затем удалите:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/
```

## Первая проверка

1. Убедитесь, что заставка показывается две секунды.
2. Откройте SOUND и проверьте, что название пресета помещается в крупную карточку.
3. Выберите Motion и убедитесь, что шкала не исчезает.
4. Откройте Settings: значения должны иметь отступ справа, а внизу должно быть `SAVE AND EXIT`.
5. Выйдите через Start+Select и проверьте восстановление настроек при следующем запуске.

При проблемах смотрите [диагностику](troubleshooting.ru.md).
