#include "core/powerSave.h"
#include <interface.h>
#define JOY_X 35
#define JOY_Y 34
#define JOY_SW 33
#define JOY_LOW  1000
#define JOY_HIGH 3000

/***************************************************************************************
** Function name: _setup_gpio()
** Location: main.cpp
** Description:   initial setup for the device
***************************************************************************************/
void _setup_gpio() {
    pinMode(JOY_X, INPUT);
    pinMode(JOY_Y, INPUT);
    pinMode(JOY_SW, INPUT_PULLUP);

    bruceConfig.colorInverted = 0;
    bruceConfigPins.rotation = 0; // portrait mode for Phantom
}

/***************************************************************************************
** Function name: _post_setup_gpio()
** Location: main.cpp
** Description:   second stage gpio setup to make a few functions work
***************************************************************************************/
void _post_setup_gpio() { pinMode(TFT_BL, OUTPUT); }

/***************************************************************************************
** Function name: getBattery()
** location: display.cpp
** Description:   Delivers the battery value from 1-100
***************************************************************************************/
int getBattery() { return 0; }

/*********************************************************************
** Function: setBrightness
** location: settings.cpp
** set brightness value
**********************************************************************/
void _setBrightness(uint8_t brightval) {
    pinMode(TFT_BL, OUTPUT);
    if (brightval > 5) {
        digitalWrite(TFT_BL, LOW);
        digitalWrite(TFT_BL, HIGH);
    } else {
        digitalWrite(TFT_BL, HIGH);
        digitalWrite(TFT_BL, LOW);
    }
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
    static unsigned long tm = millis();
    static unsigned long swPressTime = 0;
    static bool swWasPressed = false;
    static bool longPressDone = false;

    if (millis() - tm < 50) return;
    tm = millis();

    int x = analogRead(JOY_X);
    int y = analogRead(JOY_Y);

    bool upPressed  = (y < JOY_LOW);
    bool dwPressed  = (y > JOY_HIGH);
    bool lPressed   = (x < JOY_LOW);
    bool rPressed   = (x > JOY_HIGH);
    bool swPressed  = (digitalRead(JOY_SW) == LOW);

    bool anyPressed = upPressed || dwPressed || lPressed || rPressed || swPressed;
    if (!anyPressed) return;

    if (!wakeUpScreen()) AnyKeyPress = true;
    else return;

    // Menu utama: kanan = opsi berikutnya
    // Submenu: bawah = pilih ke bawah
    if (rPressed || dwPressed) {
        NextPress = true;
    }

    // Menu utama: kiri = opsi sebelumnya
    // Submenu: atas = pilih ke atas
    if (lPressed || upPressed) {
        PrevPress = true;
    }

    // Switch joystick:
    // klik singkat = Enter
    // tahan 1 detik = keluar / Esc
    if (swPressed) {
        if (!swWasPressed) {
            swWasPressed = true;
            longPressDone = false;
            swPressTime = millis();
        }

        if (!longPressDone && millis() - swPressTime >= 1000) {
            longPressDone = true;
            LongPress = true;
            EscPress = true;
        }
    } else {
        if (swWasPressed && !longPressDone) {
            SelPress = true;
        }

        swWasPressed = false;
        longPressDone = false;
        LongPress = false;
    }
}
/*********************************************************************
** Function: powerOff
** location: mykeyboard.cpp
** Turns off the device (or try to)
**********************************************************************/
void powerOff() {}

/*********************************************************************
** Function: checkReboot
** location: mykeyboard.cpp
** Btn logic to turn off the device (name is odd btw)
**********************************************************************/
void checkReboot() {}
