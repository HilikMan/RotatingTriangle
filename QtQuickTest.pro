QT += qml quick

HEADERS += triangle.h common.h
SOURCES += triangle.cpp main.cpp
RESOURCES += qml.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/quick/scenegraph/openglunderqml
INSTALLS += target
