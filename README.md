# uSwitcher

(I don't think it's nessesary to describe in English this app. It's for Russian-speaking users anyway.)

*If you're paranoid enough not to use [Punto switcher](http://punto.yandex.ru "closed source software")* (с) [kurumpa] (https://github.com/kurumpa)

uSwitcher это попытка написать замену широко известному Punto Switcher. uSwitcher - легковесный клавиатурный переключатель, он не требует установки, написан на C++ и не имеет никаких зависимостей от внешних библиотек. uSwitcher работает ТОЛЬКО в ручном режиме - ТОЛЬКО по нажатию клавиши, никакого автопереключения в самый неподходящий момент! uSwitcher работает только с русским и английским языками, но добавление других языков не должно представлять большой трудности.

#Что умеет uSwitcher:
* изменить раскладку последнего набранного слова
* изменить раскладку выделенного текста
* переключать язык по заданной клавише (поддерживаются одноклавишные сочетания типа Right Ctrl или Caps Lock)
* искать выделенный текст в заданном поисковике через системный браузер (WIP)
* показывать текущую раскладку различными способами - рядом с текстовым курсором мыши, с помощью диода Scroll Lock и даже с помощью цвета темы Windows
* проигрывать звук при переключении раскладки

#Известные баги:
* uSwitcher не работает в окне консоли.
* uSwitcher не работает или плохо работает в modern-приложениях Windows
* если uSwitcher запущен от обычного пользователя, то он не работает в программах, запущенных от администратора

uSwitcher тестировался в Windows 8.1 и немного в Windows 7.

При написании uSwitcher я черпал вдохновение в проектах [dotSwitcher] (https://github.com/kurumpa/dotSwitcher) и [pvnswitch] (http://sourceforge.net/projects/pvnswitch/).

uSwitcher собирается с помощью MS Visual Studio 2013 или MS Visual Studio 2015.

Keywords: keyboard switcher, punto switcher alternative, keyboard ninja alternative
