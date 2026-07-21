# Установка BRKCHRD 0.6.0 на TrimUI Brick / Knulli

## Что нужно

- TrimUI Brick или другая совместимая AArch64-консоль;
- Knulli с поддержкой PortMaster;
- архив `brkchrd-v0.6.0-portmaster.zip`;
- около 5 МБ свободного места.

## Установка через SD-карту

1. Корректно выключите консоль и подключите карту к компьютеру.
2. Распакуйте **содержимое** `brkchrd-v0.6.0-portmaster.zip` в:

```text
/userdata/roms/ports/
```

После распаковки должны появиться:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/cover.jpg
/userdata/roms/ports/brkchrd/docs/
```

Верните карту, загрузите Knulli и обновите список игр, если BRKCHRD не появился сразу.

## Установка через SSH

Если архив лежит в `~/Downloads`, а адрес консоли — `10.53.219.134`:

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

После установки обновите список Ports или перезагрузите консоль.

## Обновление

Распакуйте 0.6.0 поверх существующей установки. Формат конфига совместим. Если новых строк `language` и `chorddpad` ещё нет, будут использованы английский язык и режим НАЖАТИЕ.

Перед сбросом или проверкой заводских значений сохраните копию:

```bash
ssh root@10.53.219.134 '
  cp /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.backup \
     2>/dev/null || true
'
```

Для сброса переименуйте `brkchrd.cfg`. Новый файл появится после следующего сохранения.

## Постоянные данные

```text
/userdata/roms/ports/brkchrd/conf/brkchrd.cfg
/userdata/roms/ports/brkchrd/conf/brkchrd.log
```

В конфиге хранятся язык, режим DPAD АККОРД и остальные настройки.

## Удаление

При необходимости сохраните `brkchrd.cfg`, затем удалите:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/
```

При проблемах с запуском смотрите [диагностику](troubleshooting.ru.md). Проверка физического релиза вынесена в отдельный чек-лист разработчика.
