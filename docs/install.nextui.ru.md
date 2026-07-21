# Установка BRKCHRD 0.6.0 в NextUI

BRKCHRD выпускается как нативный Pak для платформы `tg5040`:

```text
brkchrd-v0.6.0-nextui-tg5040.zip
```

## Структура пакета

```text
Tools/tg5040/BRKCHRD.pak/
Tools/tg5040/.media/BRKCHRD.png
```

В Pak находятся launcher, AArch64-бинарник, документация RU/EN и лицензии. Соседний файл `.media/BRKCHRD.png` отображается в карточке инструмента NextUI.

## Установка

1. Выключите устройство и подключите карту к компьютеру.
2. Распакуйте архив в корень карты, сохранив путь `Tools/tg5040/`.
3. Верните карту и откройте раздел Tools.

После распаковки должны существовать:

```text
SD/Tools/tg5040/BRKCHRD.pak/launch.sh
SD/Tools/tg5040/BRKCHRD.pak/brkchrd-sdl.aarch64
SD/Tools/tg5040/.media/BRKCHRD.png
```

## Постоянные данные

Launcher хранит пользовательские данные вне Pak:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
SD/.userdata/tg5040/logs/BRKCHRD.txt
```

Поэтому выбранный язык, режим DPAD АККОРД и остальные настройки сохраняются при замене Pak.

## Обновление

Распакуйте архив 0.6.0 поверх существующего дерева `Tools/tg5040/`. Не удаляйте `.userdata`, если не нужен сброс к заводским значениям.

Для сброса только BRKCHRD переименуйте:

```text
SD/.userdata/tg5040/brkchrd/brkchrd.cfg
```

## Удаление

Удалите:

```text
SD/Tools/tg5040/BRKCHRD.pak/
SD/Tools/tg5040/.media/BRKCHRD.png
```

После резервного копирования настроек можно также удалить `SD/.userdata/tg5040/brkchrd/`.

## Диагностика

- нет картинки в карточке — проверьте регистр букв и точный путь `.media/BRKCHRD.png`;
- инструмент сразу закрывается — прочитайте `.userdata/tg5040/logs/BRKCHRD.txt`;
- настройки не сохраняются — проверьте доступность записи и создание `.userdata/tg5040/brkchrd/`;
- нет звука — проверьте системный выход и громкость.
