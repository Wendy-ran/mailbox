QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# INCLUDEPATH += /usr/include/openssl
# LIBS += -L/usr/lib/x86_64-linux-gnu -lssl
# LIBS += -L/usr/lib/x86_64-linux-gnu -lcrypto

#libcurl 库
# unix:!macx: LIBS += -lcurl
# INCLUDEPATH += /usr/include
# DEPENDPATH += /usr/include

TARGET = mailbox

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    newdatabase.cpp \
    newsocket.cpp \
    objectpool.cpp \
    tdialoglogin.cpp \
    tformdoc.cpp \
    utils.cpp

HEADERS += \
    mainwindow.h \
    newdatabase.h \
    newsocket.h \
    objectpool.h \
    tdialoglogin.h \
    tdialoglogin.h \
    tformdoc.h \
    utils.h

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

