#-------------------------------------------------
#
# Project created by QtCreator 2017-10-03T21:42:36
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Cible
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    storedData/student.cpp \
    storedData/group.cpp \
    storedData/competence.cpp \
    managers/groupsstudentsmanager.cpp \
    storedData/domain.cpp \
    aboutdialog.cpp \
    utilities/kscopemanager.cpp \
    utilities/preferences.cpp \
    utilities/radar.cpp \
    utilities/competencegrid/competencegrid.cpp \
    utilities/competencegrid/custombutton.cpp \
    utilities/competencegrid/simpleword.cpp \
    managers/competencemanager/competencemanager.cpp \
    managers/competencemanager/competenceaction.cpp \
    managers/competencemanager/competencecell.cpp \
    managers/studentsimportmanager.cpp \
    utilities/pdfcreator.cpp

HEADERS  += mainwindow.h \
    storedData/student.h \
    storedData/group.h \
    storedData/competence.h \
    managers/groupsstudentsmanager.h \
    storedData/domain.h \
    aboutdialog.h \
    utilities/kscopemanager.h \
    utilities/preferences.h \
    utilities/radar.h \
    utilities/competencegrid/competencegrid.h \
    utilities/competencegrid/custombutton.h \
    utilities/competencegrid/simpleword.h \
    managers/competencemanager/competencemanager.h \
    managers/competencemanager/competenceaction.h \
    managers/competencemanager/competencecell.h \
    managers/studentsimportmanager.h \
    utilities/pdfcreator.h

FORMS    += mainwindow.ui \
    managers/groupsstudentsmanager.ui \
    aboutdialog.ui \
    utilities/competencegrid/competencegrid.ui \
    utilities/competencegrid/simpleword.ui \
    managers/competencemanager/competencemanager.ui \
    managers/competencemanager/competencecell.ui

DISTFILES +=

RESOURCES += \
    cible.qrc

VERSION = 1.0
DEFINES += "APP_VERSION='$$VERSION'"
QMAKE_TARGET_PRODUCT = "CIBLE\0"

win32:RC_ICONS += "images/app_icon.ico"
