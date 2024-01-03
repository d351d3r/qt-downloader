# Qt file downloader

## Техническое задание

Написать приложение на Qt. Главное окно - lineedit для ввода URL и кнопка "Скачать". По нажатию на кнопку lineedit становится неактивным, кнопка "Скачать" становится кнопкой "Отмена", под этими элементами появляется progress bar отображающий ход скачивания файла. Скачивание файла должно происходить в отдельном потоке. После скачивания открыть штатный файловый менеджер с местом куда скачан файл. Файл должен скачиваться на рабочий стол пользователя. По кнопке отмена прекращать скачивание, возвратить интерфейс к исходному виду. При ошибке скачивания отобразить сообщение об ошибке, вернуть интерфейс к исходному виду
