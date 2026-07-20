# Установка BRKCHRD 0.5.0 на TrimUI Brick / Knulli

Скачай артефакт `brkchrd-v050-portmaster-aarch64` из успешной сборки GitHub Actions. Во внешнем ZIP от GitHub находится установочный архив:

```text
brkchrd-v0.5.0-portmaster.zip
```

## Установка по SSH

```bash
scp ~/Downloads/brkchrd-v0.5.0-portmaster.zip \
  root@10.53.219.134:/userdata/system/

ssh root@10.53.219.134 '
  unzip -o /userdata/system/brkchrd-v0.5.0-portmaster.zip \
    -d /userdata/roms/ports/ &&
  chmod +x "/userdata/roms/ports/BRKCHRD.sh" &&
  chmod +x /userdata/roms/ports/brkchrd/*.aarch64 &&
  sync && reboot
'
```

Ожидаемые файлы:

```text
/userdata/roms/ports/BRKCHRD.sh
/userdata/roms/ports/brkchrd/brkchrd-sdl.aarch64
```

## Обновление с 0.4

Версия 0.5 сохраняет совместимые key, octave, palette, bank, sound и обычные FX. Новые значения по умолчанию:

```text
VOICE LEAD = OFF
PERF FX = ON
```

Функции L1/L2 поменялись: L1 переключает режим D-pad, L2 удержанием открывает альтернативный слой. `SWAP L1/L2` по-прежнему нужен только для различий порядка кнопок в прошивке.

Для полностью чистого теста:

```bash
ssh root@10.53.219.134 '
  mv /userdata/roms/ports/brkchrd/conf/brkchrd.cfg \
     /userdata/roms/ports/brkchrd/conf/brkchrd.cfg.pre-v050 2>/dev/null || true
'
```

## Лог

```bash
ssh root@10.53.219.134 \
  'cat /userdata/roms/ports/brkchrd/conf/brkchrd.log'
```

Скачать на Mac:

```bash
scp root@10.53.219.134:/userdata/roms/ports/brkchrd/conf/brkchrd.log \
  ~/Downloads/brkchrd-v050.log
```
