TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    DBCommServ.cpp \
    EDUDBComm.cpp \
    EDUDBMgr.cpp \
    MMTDBModuleTrace.cpp \
    MyDB.cpp \
    MgrTimerTasker.cpp \
    MgrBreathAlarm.cpp

DISTFILES += \
    BreathDBCommServ.xml

#头文件包含路径
INCLUDEPATH += $$PWD/include
HEADERS += \
    include/BaseArray.h \
    include/ByteStream.h \
    include/Classify.h \
    include/DArrStack.h \
    include/DBCommServ.h \
    include/DBConfigClass.h \
    include/DBSubPDU.h \
    include/EDUDBComm.h \
    include/EDUDBMgr.h \
    include/EDUDBRes.h \
    include/MMTDBModuleTrace.h \
    include/MyDB.h \
    include/TypeDef.h

