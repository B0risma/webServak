QT -= gui
QT+=network serialport

CONFIG += c++14 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        interfaces.cpp \
        logger.cpp \
        main.cpp \
        requesti.cpp \
        resource.cpp \
        serialmanager.cpp \
        server.cpp \
        webmanager.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    interfaces.h \
    logger.h \
    requesti.h \
    resource.h \
    serialmanager.h \
    server.h \
    webmanager.h
