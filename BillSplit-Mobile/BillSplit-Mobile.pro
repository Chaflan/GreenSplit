QT += quick

CONFIG += c++17

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp

RESOURCES += \
    billsplit.qrc

OTHER_FILES += \
    qml/FileLocalDialog.qml \
    qml/main.qml \
    qml/PeoplePage.qml \
    qml/PersonInputDialog.qml \
    qml/PopupDialog.qml \
    qml/ResultsPage.qml \
    qml/TransactionInputDialog.qml \
    qml/TransactionsPage.qml \
    icons/appicon.ico \
    icons/ic_dots_vertical_grey600_36dp.png \
    icons/ic_menu_grey600_36dp.png

RC_ICONS = icons/appicon.ico

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

windows {
    message(WWWWWWWWWWWWWW-WINDOWS MOBILE-MMMMMMMMMMMMMMMM)
    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/release/ -lBillSplit-Core
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/debug/ -lBillSplit-Core
    else:unix: LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core
}

android {
    message(AAAAAAAAAAAAAA-ANDROID MOBILE-MMMMMMMMMMMMMMMM)
    win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7a
    else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7ad
    else:unix: LIBS += -L$$OUT_PWD/../BillSplit-Core/ -lBillSplit-Core_armeabi-v7a
}

INCLUDEPATH += $$PWD/../BillSplit-Core
DEPENDPATH += $$PWD/../BillSplit-Core

ANDROID_ABIS = armeabi-v7a

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
