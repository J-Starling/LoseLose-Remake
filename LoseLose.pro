QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoseLose
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    enemy.cpp \
    particle.cpp

HEADERS += \
    enemy.h \
    mainwindow.h \
    particle.h

RESOURCES += \
    resources.qrc

DISTFILES +=

win32: LIBS += -ladvapi32 -luser32
