#pragma once
#include <Arduino.h>
#include <GNSS.h>
#include <SDHCI.h>
#include <File.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "PrintLib.h"

#define CREATE_NEW_LOG_FILE

#define TweliteBegin(baud) Serial2.begin(baud); while(!Serial2) {}
#define TweliteSend(c)     Serial2.print(c)

enum class CansatState {
  STAND_BY,   // 待機
  LAUNCH,     // 打ち上げ
  DROP,       // 投下
  LANDING,    // 着地
  NAVIGATION, // 誘導
  GOAL,       // ゴール
};

enum class PrintLevel {
  PrintNone = 0,
  PrintError,
  PrintWarning,
  PrintInfo,
};

class CansatLib {
  public:
    CansatState state = CansatState::STAND_BY;
    PrintLevel printLevel = PrintLevel::PrintNone;

    void begin();

    /** SDカード */
    SDClass SD;
    File myFile;
    bool appendLog();

    /** GNSS受信機 */
    SpGnss Gnss;
    const int STRING_BUFFER_SIZE = 128;
    double currentLat = 0.0;
    double currentLng = 0.0;
    double currentAlt = 0.0;
    void posUpdate();

    // IMUセンサ
    Adafruit_BNO055 bno = Adafruit_BNO055();

    /** LED */
    void Led_isActive();
    void Led_isPosfix(bool state);
    void Led_isError(bool state);

    /** モータ */
    void mForward(int pwm);
    void mTurnRight(int pwm);
    void mTurnLeft(int pwm);
    void mStop();

    /** スピーカ */
    void spStart();
    void spError();
    void spBeep();

    /** 照度センサ */
    int readCds();

  private:
    /** GPIO */
    const uint8_t _motorA[3] = {8, 4, 5};
    const uint8_t _motorB[3] = {7, 2, 3};
    const uint8_t _speaker = 9;
    const uint8_t _heat = 6;
    const uint8_t _cds = A0;

    /** SDカード */
    bool _sdInit();
    bool _createLogFile();

    /** GNSS受信機 */
    bool _gnssInit();

    /** IMUセンサ */
    bool _imuInit();

    /** スピーカ */
    float mC = 261.626;
    float mD = 293.665;
    float mE = 329.628;
    float mF = 349.228;
    float mG = 391.995;
    float mA = 440.000;
    float mB = 493.883;
    float nn = 0.0;
    float _start[3] = {mG*4, mF*4, mG*4};
    float _error[5] = {mG*8, nn, mG*8, nn, mG*8};
    float _bell[2] = {mD*4, mF*4};
    void _beep(float *mm, int m_size, int b_time);
};
