# Stack Attack

Stack Attack — аркадная игра, симулятор грузчика на складе. Присутствовала на мобильных телефонах Siemens
* ME45 https://www.mobile-review.com/review/siemens-s45.shtml
* A50 https://www.gsmarena.com/siemens_a50-335.php
* A51 https://www.gsmarena.com/siemens_a51-982.php
* A52 https://www.youtube.com/watch?v=SD24b4RzqTs
* A55 https://www.gsmarena.com/siemens_a55-394.php
* C45 https://mobileheart.com/cell-phone-reviews/siemens-13/siemens-C45-473.aspx

и, возможно, на каких-то еще.

## Правила игры

Правила установлены путём анализа поведения оригинальной игры и могут упускать некоторые моменты.

Игра ведется на очки. Для зарабатывания очков игроку необходимо перемещать ящики, чтобы заполнить ими целый ряд. Ящики периодически сбрасывают краны, которые циркулируют наверху. Начальное количество кранов зависит от уровня сложности и может принимать значения от 1 до 3. Когда нижний ряд ящиков становится заполненным, игрок получает 10 * С очков, где C - количество кранов в игре на текущий момент. После заполнения нижнего ряда он уничтожается, лежавшие на нём ящики опускаются вниз. Также после этого в игру добавляется ещё один кран, если не достигнуто максимальное количество кранов равное 5. Игроку также начисляется по 2 очка всякий раз когда кран сбрасывает ящик.

Видео
* Siemens C45, Stack Attack https://www.youtube.com/watch?v=vyp7V7llTQY
* Stack Attack Game On Siemens A50 Old Phone https://www.youtube.com/watch?v=uzw_UGVBVr0
* Stack Attack Game On Siemens C45 Old Phone https://www.youtube.com/watch?v=cjh7mMdTulk
* Siemens A50 games: Stack Attack & Balloon Shooter - by Old Phones World https://www.youtube.com/watch?v=6PaLFAZN518
* Siemens A55 VS Siemens A52 https://www.youtube.com/watch?v=oFKKd_a7sPI
* Siemens Stack Attack https://rutube.ru/video/1845cedaf8b56ca77863d2d026f7de9a/
* Siemens Stack Attack https://www.youtube.com/watch?v=CsijE3Y-z8s
* https://vk.com/wall-198131592_252615

## Зависимости

* C++11
* [SFML](https://www.sfml-dev.org/) 2.6.2

    Debian
    ```sh
    sudo apt-get install libsfml-dev
    ```

    RHEL
    ```sh
    sudo dnf install SFML-devel
    ```
* [CMake](https://cmake.org)

## Похожие проекты

Stack Project: https://masterpiet98.itch.io/stack-project

Версия для Android: https://4pda.to/forum/index.php?showtopic=605784
