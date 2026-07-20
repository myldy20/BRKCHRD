# Установка BRKCHRD 0.4.0 на TrimUI Brick / Knulli

## Что нужно

- TrimUI Brick с Knulli/PortMaster;
- артефакт `brkchrd-v040-portmaster-aarch64` из успешной сборки GitHub Actions;
- около 2 МБ свободного места плюс настройки и лог;
- SSH необязателен, но удобнее всего для тестовых обновлений.

GitHub упаковывает артефакт во внешний ZIP. Внутри находится установочный архив:

```text
brkchrd-v0.4.0-portmaster.zip
```

## Установка по SSH

При настроенном alias `trimui`:

```bash
scp ~/Downloads/brkchrd-v0.4.0-portmaster.zip trimui:/userdata/system/
ssh trimui '
  unzip -o /userdata/system/brkchrd-v0.4.0-portmaster.zip -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Без alias:

```bash
scp ~/Downloads/brkchrd-v0.4.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/
```

Затем подключись и установи:

```bash
ssh root@10.53.219.134
unzip -o /userdata/system/brkchrd-v0.4.0-portmaster.zip \
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

Обновление через `unzip -o` сохраняет папку `conf/` и существующий текстовый конфиг.

## Обновление с 0.3

Версия 0.4 читает полезные значения 0.3: тональность, октаву, параметры движка и обычные эффекты. Старые режимы панелей и Latch игнорируются или заменяются новой схемой D-pad. Latch всегда выключен.

Перед тестом сделай резервную копию:

```bash
ssh trimui '
  rm -rf /userdata/system/brkchrd-conf-backup &&
  cp -a /userdata/roms/ports/brkchrd/conf \
    /userdata/system/brkchrd-conf-backup
'
```

Чтобы проверить полностью заводские настройки 0.4 и не удалять старый файл:

```bash
ssh trimui '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v040
'
```

## Порядок задних кнопок

Подтверждённая раскладка отдаёт четыре задние кнопки раздельно. Если L1 работает как L2 или R1 как R2, открой Settings и переключи:

- `SWAP L1/L2`
- `SWAP R1/R2`

Не переназначай глобально весь контроллер Knulli, пока не попробовал эти пункты.

## Лог

Посмотреть на консоли:

```bash
ssh trimui 'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Скачать на Mac:

```bash
scp trimui:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v040.log
```

Лог содержит SDL mapping, raw-события кнопок и осей и ожидаемые роли передних октавных кнопок, L1/L2 и R1/R2.

## Удаление

```bash
ssh trimui '
  rm -f "/userdata/roms/ports/BRKCHRD.sh" &&
  rm -rf /userdata/roms/ports/brkchrd &&
  sync
'
```
