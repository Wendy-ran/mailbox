QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = mailbox

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    newdatabase.cpp \
    newsocket.cpp \
    objectpool.cpp \
    tdialoglogin.cpp \
    tformdoc.cpp

HEADERS += \
    mainwindow.h \
    newdatabase.h \
    newsocket.h \
    objectpool.h \
    tdialoglogin.h \
    tformdoc.h

FORMS += \
    mainwindow.ui \
    tdialoglogin.ui \
    tformdoc.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
   res.qrc \
   res.qrc
