# Установка на TrimUI Brick / Knulli

## Что нужно

- TrimUI Brick с Knulli/PortMaster;
- AArch64-артефакт из успешной сборки GitHub Actions;
- около 2 МБ свободного места плюс настройки и лог;
- SSH необязателен, но сильно ускоряет тестирование.

## Установка по SSH

Предположим, внутренний архив называется `brkchrd-v0.2.0-portmaster.zip`, а для консоли уже настроен alias `trimui`.

```bash
scp ~/Downloads/brkchrd-v0.2.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.2.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Без alias:

```bash
scp ~/Downloads/brkchrd-v0.2.0-portmaster.zip root@10.53.219.134:/userdata/system/
```

## Итоговая структура

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/docs/
/userdata/roms/ports/brkchrd/licenses/
/userdata/roms/ports/brkchrd/conf/
```

Папка `conf/` создаётся или переиспользуется при запуске. Обычное обновление через `unzip -o` не удаляет настройки намеренно.

## Обновление

При желании сделай резервную копию:

```bash
ssh trimui 'cp -a /userdata/roms/ports/brkchrd/conf /userdata/system/brkchrd-conf-backup'
```

После этого распакуй новый архив поверх старой версии.

## Удаление

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
