# Создание звуков для игры.

Записать на видео процесс игры на реальном телефоне или скачать видео "Stack Attack Game On Siemens C45 Old Phone" https://www.youtube.com/watch?v=cjh7mMdTulk

Вырезать фрагменты, содержащие звуки игры и фоновый шум. Пример для видео c YouTube:
```sh
ffmpeg -ss 00:02:45 -i "Stack Attack Game On Siemens C45 Old Phone [cjh7mMdTulk].mp4" -t 2 -vn -y sound_blow_land.aac
ffmpeg -ss 00:02:54 -i "Stack Attack Game On Siemens C45 Old Phone [cjh7mMdTulk].mp4" -t 2 -vn -y sound_score_land.aac
ffmpeg -ss 00:02:50.8 -i "Stack Attack Game On Siemens C45 Old Phone [cjh7mMdTulk].mp4" -t 3 -vn -y sound_push.aac
ffmpeg -ss 00:04:41.2 -i "Stack Attack Game On Siemens C45 Old Phone [cjh7mMdTulk].mp4" -t 2 -vn -y sound_game_over.aac
ffmpeg -ss 00:02:22 -i "Stack Attack Game On Siemens C45 Old Phone [cjh7mMdTulk].mp4" -t 1.5 -vn -y sound_jump_blow.aac
```

Объединить фрагменты в один файл.
```sh
ffmpeg -f concat -safe 0 -i <(find . -regextype sed -regex './sound_.*\.aac' -printf "file '$PWD/%p'\n" | sort) -c copy -y sounds.aac
```

Обработать полученный файл с помощью Audacity.

Выделить участок, содержащий только шум. С помощью инструмента Подавление шума получить профиль шума. Удалить шум этим же инструментом, используя следующие настройки: подавление шума (дБ): 10, чувствительность: 24, сглаживание высоты (в полосах): 6.

Опционально: применить усиливающий фильтр для повышения громкости, указав усиление (дБ): 25.

Применить фильтр высоких частот со следующими настройками: частота (Гц): 800, крутизна (дБ/октаву): 48 дБ.

Заполнить паузы в пределах одного звука тишиной (Ctrl+L).

Вырезать фрагменты, содержащие отдельные звуки (прыжок, приземление, толкание ящика, сбивание ящика, удаление ряда, конец игры).

Экспортировать фрагменты в wav или ogg файлы со следующими настройками: каналы: Моно, частота: 22050 Гц, кодирование: Signed 16-bit PCM.
