TEMPLATE = app
CONFIG += c++17 qt plugin
QT += core gui widgets charts
CONFIG += qmake_qt_autogen



# 设置构建类型
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
     message("Build type: Debug")
} else {
     message("Build type: Release")
}


# OSG 路径
OSG_RELEASE = /data/SDK/osg3.4
OSG_DEBUG = /data/SDK/osg3.4d

# 根据构建类型设置 OSG 的库和包含路径
CONFIG(debug, debug|release) {
    INCLUDEPATH += $$OSG_DEBUG/include
    LIBS += -L$$OSG_DEBUG/lib -losgd -losgViewerd -losgDBd -losgGAd -losgTextd -lOpenThreadsd -losgUtild -losgParticled
} else {
    INCLUDEPATH += $$OSG_RELEASE/include
    LIBS += -L$$OSG_RELEASE/lib -losg -losgViewer -losgDB -losgGA -losgText -lOpenThreads -losgUtil -losgParticle
}

# 添加头文件、源文件和资源文件
HEADERS += $$files($$PWD/src/*.h, true)
HEADERS += $$files($$PWD/src/*.hpp, true)
SOURCES += $$files($$PWD/src/*.cpp, true)
RESOURCES += 

# 可执行文件的输出目录
DESTDIR = $$PWD/bin


