#加载pri文件
#include(deployment.pri)
#qtcAddDeployment()

#模板变量指定生成makefile(app:应用程序/lib:库)
TEMPLATE = app

#用来告诉qmake关于应用程序的配置信息
CONFIG += console
CONFIG -= app_bundle qt

#使用到的Qt定义的类(core/gui/widgets...)	QT += widgtes

#指定生成的应用程序放置的目录
DESTDIR += .
#指定生成的应用程序名 TARGET = hello
TARGET = ProcDataServer

#头文件包含路径
INCLUDEPATH += $$PWD/include  \
    ../Share/Tool
#工程中包含的头文件	HEADERS += hello.h
HEADERS += \
    include/DBCommSockEvent.h \
    include/ProcDataServ.h \
    include/LogMacro.h \
    tooltime.h \
    ../Tool/tooltime.h \
    ../Share/Tool/tooltime.h \
    include/FlieProc.h \
    ../Share/Tool/TypeDef.h \
    ../Share/Tool/Tool.h \
    include/Res.h

#工程中包含的源文件
SOURCES += \
    DBCommSockEvent.cpp \
    ProcDataServ.cpp \
    FlieProc.cpp \
    tooltime.cpp \
    ../Tool/tooltime.cpp \
    ../Share/Tool/tooltime.cpp

#工程中包含的.ui设计文件	FORMS += hello.ui

#指定.ui文件转化成ui_*.h文件的存放目录	UI_DIR += forms

#工程中包含的资源文件 RESOURCES += qrc/hello.qrc
#指定将.qrc文件转换成qrc_*.h文件的存放目录	RCC_DIR += ../tmp
#指定将含Q_OBJECT的头文件转换成标准.h文件的存放目录	MOC_DIR += ../tmp

#引入的lib文件的路径 -L：引入路径	LIBS += -L.

#指定目标文件(obj)的存放目录 OBJECTS_DIR += ../tmp

#程序编译时依赖的相关路径 DEPENDPATH += . forms include qrc sources

#增加预处理器宏(gcc的-D选项)
DEFINES += \
    _cpu32

#设置c编译器flag参数	QMAKE_CFLAGS += -g
#设置c++编译器flag参数	QMAKE_CXXFLAGS += -g
#设置链接器flag参数	QMAKE_LFLAGS += -rdynamic

DISTFILES += \
    ProcDataServer.xml

