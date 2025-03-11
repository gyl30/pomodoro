QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets openglwidgets


CONFIG += c++17

SOURCES += \
    main.cpp \
    message_box.cpp \
    glbackgroundwidget.cpp \
    widget.cpp

HEADERS += \
    message_box.h \
    glbackgroundwidget.h \
    widget.h
