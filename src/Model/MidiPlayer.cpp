#include "MidiPlayer.h"
#include <map>
#include <QKeyEvent>
#include "MidiMaster.h"

int buttonsNotes[20]{
    Qt::Key_A, 
    Qt::Key_W, 
    Qt::Key_S, 
    Qt::Key_E, 
    Qt::Key_D, 
    Qt::Key_F, 
    Qt::Key_T, 
    Qt::Key_G, 
    Qt::Key_Y, 
    Qt::Key_H, 
    Qt::Key_U, 
    Qt::Key_J, 
    Qt::Key_K,
    Qt::Key_O, 
    Qt::Key_L, 
    Qt::Key_P,
    Qt::Key_Semicolon,
    Qt::Key_Apostrophe,
    Qt::Key_BracketRight,
    Qt::Key_Backslash
};


bool MidiPlayer::keyPressed(int key)
{
    
    for (int i = 0; i < 20; i++)
    {
        if (buttonsNotes[i] == key) {
            MidiMaster::setPress(i+60);
            return true;
        }
    }
    return false;
}

bool MidiPlayer::keyReleased(int key)
{
    for (int i = 0; i < 20; i++)
    {
    if (buttonsNotes[i] == key) {
        MidiMaster::setRelease(i+60);
        return true;
    }
}
    return false;
}
