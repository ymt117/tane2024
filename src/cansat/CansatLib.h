#pragma once
#include <Arduino.h>
#include <GNSS.h>
#include <SDHCI.h>
#include <File.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
// #include <Math.h>
#include "PrintLib.h"

#define DELETE_EXIST_LOG_FILE

#define TweliteBegin(baud) Serial2.begin(baud); while(!Serial2) {}
#define TweliteSend(c)     Serial2.print(c)

enum class CansatState {
  CALIBRATION, // キャリブレーション
  STAND_BY,    // 待機
  LAUNCH,      // 打ち上げ
  DROP,        // 投下
  LANDING,     // 着地
  NAVIGATION,  // 誘導
  GOAL,        // ゴール
};

enum class PrintLevel {
  PrintNone = 0,
  PrintError,
  PrintWarning,
  PrintInfo,
};

/** ユーザ設定 */
struct UserConfig {
  double goalLat;
  double goalLng;
  int altThreshold;
  long timeThreshold;
  int cdsThreshold;
  double accThreshold;
  int distanceThreshold;
};

class CansatLib {
  public:
    CansatState state = CansatState::STAND_BY;
    PrintLevel printLevel = PrintLevel::PrintNone;

    void begin();

    /** ユーザ設定 */
    UserConfig userConfig = {35.7100152, 139.8107594}; // 初期値：スカイツリー

    /** SDカード */
    SDClass SD;
    File myFile;
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading";
    bool appendLog();

    /** GNSS受信機 */
    SpGnss Gnss;
    const int STRING_BUFFER_SIZE = 128;
    String currentDate = "";
    double currentLat = 0.0;
    double currentLng = 0.0;
    double currentAlt = 0.0;
    double distanceToGoal = 10000.0;
    double directionToGoal = 0.0;

    // IMUセンサ
    Adafruit_BNO055 bno = Adafruit_BNO055();
    float heading, pitch, roll = -1000000;
    float accX,  accY,  accZ  = -1000000;
    float gyroX, gyroY, gyroZ = -1000000;
    float magX,  magY,  magZ  = -1000000; 

    /** LED */
    void Led_isActive();
    void Led_isPosfix(bool state);
    void Led_isGoal();
    void Led_isError(bool state);

    /** モータ */
    int mr_pwm = 0;
    int ml_pwm = 0;
    int mOutputTime = 0;
    void mForward(int pwm);
    void mTurnRight(int pwm);
    void mTurnLeft(int pwm);
    void mStop();

    /** スピーカ */
    void spStart();
    void spError();
    void spBeep();

    /** 照度センサ */
    int cdsValue = -1000;
    int readCds();

    /** 電熱線回路 */
    void heatWire(int pwm, int time);

    /** Utils */
    /** センサーの情報を更新する */
    void updateSensorValue();
    /** センサの情報をシリアル出力する */
    void printSensorValue();

  private:
    /** GPIO */
    const uint8_t _motorR[3] = {8, 4, 5};
    const uint8_t _motorL[3] = {7, 2, 3};
    const uint8_t _speaker = 9;
    const uint8_t _heat = 6;
    const uint8_t _cds = A0;

    /** SDカード */
    bool _sdInit();
    bool _createLogFile();

    /** GNSS受信機 */
    bool _gnssInit();
    void _updateGnss();

    /** IMUセンサ */
    bool _imuInit();
    void _updateImuValue();

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

    /** 照度センサ */
    void _updateCdsValue();

    /** Utils */
    String _createDate(SpGnssTime time);

    #define EARTH_RADIUS 6371000 // 地球の半径 (単位: メートル)
    #define TO_DEGREES(rad) ((rad) * 180.0 / M_PI) // ラジアンから度への変換
    #define TO_RADIANS(deg) ((deg) * M_PI / 180.0) // 度からラジアンへの変換
    double _haversineDistance(double lat1, double lng1, double lat2, double lng2);
    double _haversineBearing(double lat1, double lng1, double lat2, double lng2);

    String _createMessage();
};
