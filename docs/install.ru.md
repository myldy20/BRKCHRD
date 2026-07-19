# Установка BRKCHRD 0.3 на TrimUI Brick / Knulli

## Что нужно

- TrimUI Brick с Knulli/PortMaster;
- артефакт `brkchrd-v030-portmaster-aarch64` из успешной сборки GitHub Actions;
- около 2 МБ свободного места плюс настройки и лог;
- SSH необязателен, но удобнее всего для тестовых обновлений.

GitHub упаковывает артефакт во внешний ZIP. Внутри него находится установочный архив:

```text
brkchrd-v0.3.0-portmaster.zip
```

## Установка по SSH

При настроенном alias `trimui`:

```bash
scp ~/Downloads/brkchrd-v0.3.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.3.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Без alias:

```bash
scp ~/Downloads/brkchrd-v0.3.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/
```

Затем подключись:

```bash
ssh root@10.53.219.134
```

И выполни:

```bash
unzip -o /userdata/system/brkchrd-v0.3.0-portmaster.zip \
  -d /userdata/roms/ports/
chmod +x "/userdata/roms/ports/BRKCHRD.sh"
chmod +x /userdata/roms/ports/brkchrd/*.aarch64
sync
reboot
```

## Итоговая структура

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
/userdata/roms/ports/brkchrd/docs/
/userdata/roms/ports/brkchrd/licenses/
/userdata/roms/ports/brkchrd/conf/
```

Папка `conf/` создаётся или переиспользуется при запуске. Обновление через `unzip -o` не удаляет настройки.

## Резервная копия настроек

```bash
ssh trimui '
  rm -rf /userdata/system/brkchrd-conf-backup &&
  cp -a /userdata/roms/ports/brkchrd/conf \
    /userdata/system/brkchrd-conf-backup
'
```

## Лог управления и запуска

Посмотреть на консоли:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Скачать на Mac:

```bash
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v030.log
```

В 0.3 лог содержит SDL mapping и первые raw button/axis events. Он особенно важен при проверке другой версии Knulli.

## Удаление

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
