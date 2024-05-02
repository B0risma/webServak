минимальный набор для сборки(неточно):
qtbase5-dev
libqt5network*
libqt5serialport*

Сборка:
qmake Servac.pro
make
./Servac -D {tty device}

Реализована обработка запросов GET и PATCH
Ответы в JSON
Аутентификация по Basic логин=user пароль=password
Логирование запросов/ответов ведется в файл httpLog в директории запуска
Примеры запросов:
curl -XGET --user user:password localhost:8001
curl -XPATCH --user user:password localhost:8001/date -H "Content-type : application/json" -d '{"date" : "2024-04-29T15:10:00"}'

Сервер не запустится без указания serial устройства: -D {device}
Serial: baudrate:115200 8N1
Обрабатывает Return и Backspace (Обработка остальных клавиш не гарантирована)
Пример команд:
? - Запрос всей информации
? date - запрос текущей даты
set date 2024-04-29T15:10:00 - установка даты
set interfaces inet enp0s3 192.168.0.20/255.255.255.0 - установка ipv4(inet6 для ipv6) и маски интерфейса
