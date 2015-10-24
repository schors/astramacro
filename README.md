Транслятор простого макроязыка
==============================

Hебольшая пpогpаммка, pеализующая pазвоpачивание кода пpогpаммы, написанной на пpостом макpоязыке. 
Алгоpитм и макpоязык взяты из книги Дж. Доннован "Системное программирование". Работает с несколькими 
кодиpовками pусского языка. 

Пpогpамма может быть полезна для студентов pазличных вузов, изучающих пpогpаммиpование.

> **Для тех, кто не дочитает**
> Коды программ на языке "Астра" даны в UTF-8 для ознакомления
> Компилироваться будут только файлы в однобайтовой кодировке


Возможности
-----------

* Простые макросы.
* Вложенные макросы.
* Включение файлов.
* Академический алгоритм.
* Документированный на русском код.

Особенности
-----------

Изначально транслятор не писался для изучения. По задумке это была вполне рабочая программа
для трансляции кода с некоего языка "Астра". До самого языка руки не дошли, а вот рабочий макроязык
остался.

Язык "Астра" - руссифицированная военными версия какого-то из ранних языков ассемблера. Не спрашивайте
меня, зачем военные в 70-ых пытались всё руссифицировать. Что макроязык, что прямой перевод ключевых слов
самого языка волшебным образом совпадает с описанными в книге языками, использующимися для соответствующих
примеров. 

Как и в случае с https://github.com/schors/sfun , этот транслятор написан по классическим учебникам. 
Эта книга является классикой - Дж. Доннован "Системное программирование". Там есть глава про трансляцию 
макроязыка. Вот это оно. Работа была сделана скурпулёзно и проверена на боевых задачах.

> **Было бы не плохо сейчас найти эту книгу и отcканировать то место**

Необычным для современных программ является встроенный перекодировщик для популярных тогда русских кодировок.
Это было сделано ввиду работы на достаточно странном оборудовании с MS DOS, однако хотелось делать сборки 
и на UNIX-машинах. Перекодирующие утилиты были в зачатке своего развития и очень часто программы несли на борту 
свой собственный перекодировщик.

Зачем?
------

А она прикольная :) Студентам показывать. Перед показом не забудьте ознакомиться с лицензионным соглашением.

Орехи
-----

* **PayPal**: schors@gmail.com
* **Яндекс.Деньги**: 41001140237324

--
[![LICENSE WTFPL](wtfpl-badge-1.png)](LICENSE)

