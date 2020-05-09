#-------------------------------------------------
#
# Project created by QtCreator 2019-05-13T16:01:43
#
#-------------------------------------------------
include($$PWD/libs/qredis/qredisclient.pri)

QT       += core gui network sql concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestQT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR += $$PWD/bin/build/objects

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
	data/Channel.cpp \
	data/Controller.cpp \
	data/Transform.cpp \
	main.cpp \
	mainwindow.cpp \
	misc/LogUtils.cpp \
	misc/LoggingEvent.cpp \
	misc/Settings.cpp \
	models/ControllerModel.cpp \
	task/DatabaseManager.cpp \
	task/DatabaseTask.cpp \
	task/NetworkManager.cpp \
	task/RedisTask.cpp \
	task/StationTask.cpp \
	views/AddControllerDialog.cpp \
	views/ChannelWidget.cpp \
	views/ConfigurationDialog.cpp \
	views/ControllerDetails.cpp \
	views/ControllerList.cpp \
	views/ControllerWidget.cpp \
	views/DatabasePage.cpp \
	views/ModifyChannelDialog.cpp \
	views/ModifyControllerDialog.cpp \
	views/ModifyRegistersDialog.cpp \
	views/TestGraph.cpp \
    views/ThresholdWidget.cpp

HEADERS += \
	data/Channel.h \
	data/Constants.h \
	data/Controller.h \
	data/Transform.h \
	mainwindow.h \
	misc/LogUtils.h \
	misc/LoggingEvent.h \
	misc/Settings.h \
	models/ControllerModel.h \
	task/DatabaseManager.h \
	task/DatabaseTask.h \
	task/NetworkManager.h \
	task/RedisTask.h \
	task/StationTask.h \
	views/AddControllerDialog.h \
	views/ChannelWidget.h \
	views/ConfigurationDialog.h \
	views/ControllerDetails.h \
	views/ControllerList.h \
	views/ControllerWidget.h \
	views/DatabasePage.h \
	views/ModifyChannelDialog.h \
	views/ModifyControllerDialog.h \
	views/ModifyRegistersDialog.h \
	views/TestGraph.h \
    views/ThresholdWidget.h

#LIBS += \
#        $$PWD/libs/pcapplusplus-mingw/Pcap++.lib \
#        $$PWD/libs/pcapplusplus-mingw/Packet++.lib \
#        $$PWD/libs/pcapplusplus-mingw/Common++.lib \
#        $$PWD/libs/npcap-win32/Lib/Packet.lib \
#        $$PWD/libs/npcap-win32/Lib/wpcap.lib
#        $$PWD/libs/qredis/lib/libqredisclient.a
#	 -L$$PWD/libs/pcapplusplus-linux -lPcap++ -lPacket++ -lCommon++

LIBS +=  -lPcap++ -lPacket++ -lCommon++ -lpcap -lpthread

INCLUDEPATH +=  /usr/include/netinet
#INCLUDEPATH +=  $$PWD/libs/pcapplusplus-linux/header
#INCLUDEPATH +=  $$PWD/libs/npcap-win32/Include
#INCLUDEPATH +=  $$PWD/libs/pcapplusplus-mingw/header
INCLUDEPATH +=  $$PWD/libs/qredis/header

#LIBS += -lws2_32
#LIBS += -liphlpapi
#LIBS += -lpthread
LIBS += -lpcap -lpthread

#DEFINES +=  WPCAP

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    mainwindow.qrc

CONFIG += debug_and_release
CONFIG(debug, debug|release){
    DESTDIR = $$PWD/bin/debug
}else{
    DESTDIR = $$PWD/bin/release
}
