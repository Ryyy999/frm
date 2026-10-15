#include "core/powerSave.h"
#include <interface.h>
#include "core/display.h"

#define JOY_X 35
#define JOY_Y 34
#define JOY_SW 33
#define JOY_LOW  1000
#define JOY_HIGH 3000
#define JOY_INV_H 0
#define JOY_INV_V 0

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
    static unsigned long tm = 0;
    static unsigned long swStart = 0;
    static bool swHeld = false;
    static bool swLongDone = false;
    unsigned long now = millis();

    // SW: tekan pendek = enter, tahan 2 detik = back
    bool sw = (digitalRead(JOY_SW) == LOW);
    if (sw && !swHeld) {
        swHeld = true;
        swStart = now;
        swLongDone = false;
        return;
    }
    if (sw && swHeld) {
        if (!swLongDone && now - swStart >= 2000) {
            swLongDone = true;
            if (!wakeUpScreen()) { AnyKeyPress = true; EscPress = true; }
        }
        return;
    }
    if (!sw && swHeld) {
        swHeld = false;
        if (!swLongDone && now - swStart >= 40) {
            if (!wakeUpScreen()) { AnyKeyPress = true; SelPress = true; }
        }
        return;
    }

    // Joystick
    if (now - tm < 200) return;

    int x = analogRead(JOY_X);
    int y = analogRead(JOY_Y);

    bool left  = JOY_INV_H ? (x > JOY_HIGH) : (x < JOY_LOW);
    bool right = JOY_INV_H ? (x < JOY_LOW)  : (x > JOY_HIGH);
    bool up    = JOY_INV_V ? (y > JOY_HIGH) : (y < JOY_LOW);
    bool down  = JOY_INV_V ? (y < JOY_LOW)  : (y > JOY_HIGH);

    bool inMainMenu = (menuOptionType == MENU_TYPE_MAIN);
    bool prev = inMainMenu ? left  : up;
    bool next = inMainMenu ? right : down;
    if (!prev && !next) return;

    tm = now;
    if (!wakeUpScreen()) {
        AnyKeyPress = true;
        if (prev) PrevPress = true;
        if (next) NextPress = true;
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
