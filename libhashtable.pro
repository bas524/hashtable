#-------------------------------------------------
#
# Project created by QtCreator 2013-08-24T11:08:24
#
#-------------------------------------------------

QT       -= core gui qt

TARGET = hash
TEMPLATE = lib
VERSION = 1.1.0
DESTDIR += ./lib
RPATH = ./lib


DEFINES += LIBHASH_LIBRARY

HEADERS += \ 
    fasthash.h \
    hashtable.h \
    libstdint.h
SOURCES += \ 
    fasthash.c \
    hashtable.c

macx {
    QMAKE_MAC_SDK = macosx10.9
}

unix:!symbian {
  CONFIG(debug, debug | release) {
    DEFINES +=  _GLIBCXX_DEBUG
  }
  CONFIG(release, debug | release) {
    !staticlib {
      unix:!macx {
        OUT_FILE_NAME = $$join(TARGET, OUT_FILE_1, $$TEMPLATE, .so.$$VERSION)
        QMAKE_POST_LINK =  $${QMAKE_STRIP} -s $$DESTDIR/$$OUT_FILE_NAME
      }
    }
  }

  QMAKE_CXXFLAGS_RELEASE += -s  
  INCLUDEPATH += /usr/include /usr/local/include
}
