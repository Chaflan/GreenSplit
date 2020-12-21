QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    comboboxitemdelegate.cpp \
    main.cpp \
    mainwindow.cpp \
    nameeditdialog.cpp \
    peoplewidget.cpp \
    resultswidget.cpp \
    transactioneditdialog.cpp \
    transactionswidget.cpp

HEADERS += \
    comboboxitemdelegate.h \
    mainwindow.h \
    nameeditdialog.h \
    peoplewidget.h \
    resultswidget.h \
    transactioneditdialog.h \
    transactionswidget.h

FORMS += \
    mainwindow.ui \
    nameeditdialog.ui \
    peoplewidget.ui \
    resultswidget.ui \
    transactioneditdialog.ui \
    transactionswidget.ui

OTHER_FILES += \
    icons/appicon.ico

RC_ICONS = icons/appicon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

windows {
    message(WWWWWWWWWWWWWW-WINDOWS DESKTOP-DDDDDDDDDDDDDD)
    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/release/ -lBillSplit-Core
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/debug/ -lBillSplit-Core
    else:unix: LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core
}

android {
    message(AAAAAAAAAAAAAA-ANDROID DESKTOP-DDDDDDDDDDDDDD)
    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7a
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7ad
    else:unix: LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7a
}

INCLUDEPATH += $$PWD/../BillSplit-Core
DEPENDPATH += $$PWD/../BillSplit-Core

#ANDROID_ABIS = armeabi-v7a
