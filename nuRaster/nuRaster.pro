QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 
LIBS += -fopenmp

QMAKE_CXXFLAGS -= -O
QMAKE_CXXFLAGS -= -O1
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS -= -O3
QMAKE_CXXFLAGS += -O3

QMAKE_CXXFLAGS += -fopenmp

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
    camera.cpp \
    glwidget.cpp \
    loader.cpp \
    main.cpp \
    mat4.cpp \
    matblinnphong.cpp \
    pipeline.cpp \
    renderer.cpp \
    shaderblinnphong.cpp \
    shaderlambert.cpp \
    shadernormal.cpp \
    texture.cpp \
    triangle.cpp \
    vec3.cpp \
    vec4.cpp \
    widget.cpp

HEADERS += \
    camera.h \
    glwidget.h \
    loader.h \
    mat4.h \
    matblinnphong.h \
    material.h \
    pipeline.h \
    renderer.h \
    shader.h \
    shaderblinnphong.h \
    shaderlambert.h \
    shadernormal.h \
    texture.h \
    triangle.h \
    vec3.h \
    vec4.h \
    widget.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
