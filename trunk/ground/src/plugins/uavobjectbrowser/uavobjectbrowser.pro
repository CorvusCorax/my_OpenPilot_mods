TEMPLATE = lib
TARGET = UAVObjectBrowser
include(../../openpilotgcsplugin.pri)
include(uavobjectbrowser_dependencies.pri)
HEADERS += browserplugin.h \
    uavobjectbrowserconfiguration.h \
    uavobjectbrowser.h \
    uavobjectbrowserwidget.h \
    uavobjectbrowserfactory.h \
    uavobjectbrowseroptionspage.h \
    uavobjecttreemodel.h \
    treeitem.h \
    browseritemdelegate.h
SOURCES += browserplugin.cpp \
    uavobjectbrowserconfiguration.cpp \
    uavobjectbrowser.cpp \
    uavobjectbrowserfactory.cpp \
    uavobjectbrowserwidget.cpp \
    uavobjectbrowseroptionspage.cpp \
    uavobjecttreemodel.cpp \
    treeitem.cpp \
    browseritemdelegate.cpp
OTHER_FILES += UAVObjectBrowser.pluginspec
FORMS += uavobjectbrowser.ui
