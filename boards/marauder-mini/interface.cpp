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
    if (brightval > 100) brightval = 100;

    // 0 = backlight mati total (buat sleep / screen off)
    if (brightval == 0) {
        analogWrite(TFT_BL, 0);
        return;
    }

    // 1..100% dipetakan ke duty 25..255 supaya 1% tetap kelihatan
    const uint8_t MIN_DUTY = 25;
    uint8_t duty = MIN_DUTY + ((255 - MIN_DUTY) * brightval) / 100;
    analogWrite(TFT_BL, duty);
}

/*********************************************************************
** Function: InputHandler
** Handles the variables PrevPress, NextPress, SelPress, AnyKeyPress and EscPress
**********************************************************************/
void InputHandler(void) {
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

    // Joystick: satu dorongan = satu gerakan, tidak berulang selama ditahan
    static bool hArmed = true;
    static bool vArmed = true;
    const int NEUTRAL_LOW  = 1500;
    const int NEUTRAL_HIGH = 2500;

    int x = analogRead(JOY_X);
    int y = analogRead(JOY_Y);

    if (x > NEUTRAL_LOW && x < NEUTRAL_HIGH) hArmed = true;
    if (y > NEUTRAL_LOW && y < NEUTRAL_HIGH) vArmed = true;

    bool left = false, right = false, up = false, down = false;
    bool horizontalWins = (abs(x - 2048) >= abs(y - 2048));

    if (horizontalWins && hArmed) {
        bool l = JOY_INV_H ? (x > JOY_HIGH) : (x < JOY_LOW);
        bool r = JOY_INV_H ? (x < JOY_LOW)  : (x > JOY_HIGH);
        if (l) { left = true; hArmed = false; }
        if (r) { right = true; hArmed = false; }
    } else if (!horizontalWins && vArmed) {
        bool u = JOY_INV_V ? (y > JOY_HIGH) : (y < JOY_LOW);
        bool d = JOY_INV_V ? (y < JOY_LOW)  : (y > JOY_HIGH);
        if (u) { up = true; vArmed = false; }
        if (d) { down = true; vArmed = false; }
    }

    bool inMainMenu = (menuOptionType == MENU_TYPE_MAIN);
    if (!inMainMenu) { bool t = left; left = right; right = t; }
    if (inMainMenu) { up = false; down = false; }

    if (!left && !right && !up && !down) return;

    if (!wakeUpScreen()) {
        AnyKeyPress = true;
        if (left)  PrevPress = true;
        if (right) NextPress = true;
        if (up)    UpPress   = true;
        if (down)  DownPress = true;
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
