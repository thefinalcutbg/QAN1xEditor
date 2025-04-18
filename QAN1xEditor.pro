QT += core widgets gui network

CONFIG += c++17

macx{
    LIBS += -framework CoreMidi
    LIBS += -framework CoreAudio
    LIBS += -framework CoreFoundation
    ICON = $$PWD/installer/macos/icon.icns
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
}



linux{
    LIBS += -lasound
    ICON = $$PWD/installer/icon.ico
}

INCLUDEPATH += include
INCLUDEPATH += src

SOURCES += \
    include/QMidi/RtMidi.cpp \
    include/QMidi/qmidiin.cpp \
    include/QMidi/qmidimapper.cpp \
    include/QMidi/qmidimessage.cpp \
    include/QMidi/qmidiout.cpp \
    include/QMidi/qmidipianoroll.cpp \
    include/sqLite3/sqlite3.c \
    src/Database/Database.cpp \
    src/Model/An1File.cpp \
    src/Model/An1x.cpp \
    src/Model/An1xPatch.cpp \
    src/Model/ClipoboardManager.cpp \
    src/Model/DragDropManager.cpp \
    src/Model/MidiMaster.cpp \
    src/Model/PatchDatabase.cpp \
    src/Model/PatchMemory.cpp \
    src/View/ArpSeq.cpp \
    src/View/Browser.cpp \
    src/View/BrowserTableModel.cpp \
    src/View/ControlMatrixScene.cpp \
    src/View/DbTableView.cpp \
    src/View/FreeEG/EGPath.cpp \
    src/View/FreeEG/FreeEGScene.cpp \
    src/View/FreeEGWidget.cpp \
    src/View/FreeFunctions.cpp \
    src/View/FxEq.cpp \
    src/View/GlobalWidgets.cpp \
    src/View/MemoryList.cpp \
    src/View/PianoRoll/PianoKey.cpp \
    src/View/PianoRoll/PianoView.cpp \
    src/View/QAN1xEditor.cpp \
    src/View/SceneView.cpp \
    src/View/SettingsDialog.cpp \
    src/View/UiWidgets.cpp \
    src/View/VoiceNameEdit.cpp \
    src/main.cpp

HEADERS += \
    include/QMidi/RtMidi.h \
    include/QMidi/qmidiin.h \
    include/QMidi/qmidimapper.h \
    include/QMidi/qmidimessage.h \
    include/QMidi/qmidiout.h \
    include/QMidi/qmidipianoroll.h \
    include/sqLite3/sqlite3.h \
    include/sqLite3/sqlite3ext.h \
    src/Database/Database.h \
    src/Model/An1File.h \
    src/Model/An1x.h \
    src/Model/An1xPatch.h \
    src/Model/ClipoboardManager.h \
    src/Model/DragDropManager.h \
    src/Model/MidiMaster.h \
    src/Model/PatchDatabase.h \
    src/Model/PatchMemory.h \
    src/Model/PatchRow.h \
    src/Model/Settings.h \
    src/View/AbstractController.h \
    src/View/ArpSeq.h \
    src/View/Browser.h \
    src/View/BrowserTableModel.h \
    src/View/ControlMatrixScene.h \
    src/View/DbTableView.h \
    src/View/FreeEG/EGPath.h \
    src/View/FreeEG/FreeEGScene.h \
    src/View/FreeEGWidget.h \
    src/View/FreeFunctions.h \
    src/View/FxEq.h \
    src/View/GlobalWidgets.h \
    src/View/MemoryList.h \
    src/View/PianoRoll/PianoKey.h \
    src/View/PianoRoll/PianoView.h \
    src/View/QAN1xEditor.h \
    src/View/SceneView.h \
    src/View/SettingsDialog.h \
    src/View/UiWidgets.h \
    src/View/VoiceNameEdit.h

FORMS += \
    src/View/ArpSeq.ui \
    src/View/Browser.ui \
    src/View/ControlMatrixScene.ui \
    src/View/FreeEGWidget.ui \
    src/View/FxEq.ui \
    src/View/QAN1xEditor.ui \
    src/View/SceneView.ui \
    src/View/SettingsDialog.ui

RESOURCES += \
    QAN1xEditor.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
