#include <Arduino.h>

// Led PWM
#include "LedPWM.h"
#define LED_PIN 6
#define PWM_RANGE 255
LedPWM ledPwm(LED_PIN, PWM_RANGE);

// Ajustar curva de luminosidad y valores pwm por pasos
int pwmValues[] = {0, 6, 11, 19, 32, 53, 87, 110, 140, 180, 215, 255};
int pwmValuesLength = 12;
int indexPwm = 0;

int mapPwmIncrease(int val) {
    indexPwm = indexPwm + val;
    if (indexPwm > pwmValuesLength - 1) {
        indexPwm = pwmValuesLength - 1;
    }
    return pwmValues[indexPwm];
}
int mapPwmDecrease(int val) {
    indexPwm = indexPwm - val;
    if (indexPwm < 0) {
        indexPwm = 0;
    }
    return pwmValues[indexPwm];
}

// Interpolacion
#define INTERPOLATE_TIME 10
int setPwmVal = 0;
int interPwmVal = 0;
int interpolateValue(int finalValue) {
    if (finalValue < interPwmVal) {
        interPwmVal--;
    } else if (finalValue > interPwmVal) {
        interPwmVal++;
    }
    return interPwmVal;
}

#include <LightChrono.h>
LightChrono interpolateTimer;

// Encoder
#include <Encoder.h>
#define ENC_PIN_A 2
#define ENC_PIN_B 3
Encoder encoder(ENC_PIN_A, ENC_PIN_B);

#include "EasyEncoder.h"
EasyEncoder enc;

// Button enconder
#include <JC_Button.h>
#define BTN_ENC_PIN A0
Button btnEnc(BTN_ENC_PIN);

// *************************
void setup() {
    Serial.begin(9600);
    btnEnc.begin();
    Serial.println("---------------------");
    Serial.println("Starting...");
    ledPwm.init();
    ledPwm.onNow();
}

void loop() {
    enc.tick(encoder.read());
    btnEnc.read();

    if (btnEnc.wasReleased()) {
        ledPwm.toggle();
    }

    if (ledPwm.getState()) {
        if (enc.fullStepDir() == CCW_R) {
            Serial.println("Left - CCW_R");
            setPwmVal = mapPwmDecrease(1);
        }
        if (enc.fullStepDir() == CW_R) {
            Serial.println("Right - CW_R");
            setPwmVal = mapPwmIncrease(1);
        }
    }

    if (interpolateTimer.hasPassed(INTERPOLATE_TIME, true)) {
        interPwmVal = interpolateValue(setPwmVal);
        ledPwm.setValue(interPwmVal);
    }

    ledPwm.update();
}