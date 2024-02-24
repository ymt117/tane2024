#include "GNSS.h"
#include "CansatLib.h"
#include "PrintLib.h"

void CansatLib::begin() {
  int errorFlag = 0;

  Serial.begin(115200);
  while(!Serial) {}
  APP_PRINT_I("called begin()\n");

  TweliteBegin(115200);

  for (int i = 0; i < 3; i++) {
    pinMode(_motorA[i], OUTPUT);
    pinMode(_motorB[i], OUTPUT);
  }
  pinMode(_speaker, OUTPUT);
  pinMode(_heat, OUTPUT);
  digitalWrite(_heat, LOW); // 電熱線は LOW にしておく

  if (!_sdInit()) {
    errorFlag = 1;
  }

  if (!_gnssInit()) {
    errorFlag = 1;
  }

  // if (!_imuInit()) {
  //   errorFlag = 1;
  // }

  if (errorFlag == 1) {
    Led_isError(true);
    spError();
    exit(0);
  }
}

/**
 * @brief 緯度・経度の値を更新する
 */
void CansatLib::posUpdate() {
  APP_PRINT_I("called posUpdate()\n");

  static bool PosFixflag = false;
  bool LedSat;

  if (Gnss.waitUpdate(-1)) {
    SpNavData NavData;
    Gnss.getNavData(&NavData);

    LedSat = ((NavData.posDataExist) && (NavData.posFixMode != FixInvalid));
    if (PosFixflag != LedSat) {
      Led_isPosfix(LedSat);
      PosFixflag = LedSat;
    }

    char StringBuffer[STRING_BUFFER_SIZE];
    if (NavData.posDataExist == 0) {
      APP_PRINT_I("numSat: ");
      APP_PRINT_I(NavData.numSatellites);
      APP_PRINT_I("\n");
      APP_PRINT_I("No Position");
    } else {
      currentLat = NavData.latitude;
      currentLng = NavData.longitude;
      currentAlt = NavData.altitude;
      APP_PRINT_I(String(currentLat, 6));
      APP_PRINT_I(",");
      APP_PRINT_I(String(currentLng, 6));
      APP_PRINT_I(",");
      APP_PRINT_I(String(currentAlt, 2));
    }

    APP_PRINT_I("\n");
  } else {
    APP_PRINT_I("data not update\n");
  }
}

/**
 * @brief LED0のオン/オフを切り替える。プログラムが実行中であることを示す。
 */
void CansatLib::Led_isActive(void) {
  APP_PRINT_I("called Led_isActive()\n");

  static int state = 1;
  if (state == 1) {
    ledOn(PIN_LED0);
    state = 0;
  } else {
    ledOff(PIN_LED0);
    state = 1;
  }
}

/**
 * @brief LED1のオン/オフを切り替える。衛星を捕捉したら点灯する。
 * @param [in] state 補足ステータス 
 */
void CansatLib::Led_isPosfix(bool state) {
  APP_PRINT_I("called Led_isPosfix()\n");

  if (state == 1) {
    ledOn(PIN_LED1);
  } else {
    ledOff(PIN_LED1);
  }
}

/**
 * @brief LED3のオン/オフを切り替える。エラーのとき点灯する。
 * @param [in] state エラーステータス
 */
void CansatLib::Led_isError(bool state) {
  APP_PRINT_I("called Led_isError\n");

  if (state == 1) {
    ledOn(PIN_LED3);
  } else {
    ledOff(PIN_LED3);
  }
}

void CansatLib::mForward(int pwm) {
  APP_PRINT_I("called mForward()");
  APP_PRINT_I(" pwm: ");
  APP_PRINT_I(String(pwm));
  APP_PRINT_I("\n");

  digitalWrite(_motorA[0], HIGH);
  digitalWrite(_motorA[1], LOW);
  analogWrite(_motorA[2], pwm);

  digitalWrite(_motorB[0], HIGH);
  digitalWrite(_motorB[1], LOW);
  analogWrite(_motorB[2], pwm);
}

void CansatLib::mTurnRight(int pwm) {
  APP_PRINT_I("called mTurnRight()");
  APP_PRINT_I(" pwm: ");
  APP_PRINT_I(String(pwm));
  APP_PRINT_I("\n");

  digitalWrite(_motorA[0], HIGH);
  digitalWrite(_motorA[1], LOW);
  analogWrite(_motorA[2], pwm);

  digitalWrite(_motorB[0], HIGH);
  digitalWrite(_motorB[1], LOW);
  analogWrite(_motorB[2], pwm);
}

void CansatLib::mTurnLeft(int pwm) {
  APP_PRINT_I("called mTurnLeft()");
  APP_PRINT_I(" pwm: ");
  APP_PRINT_I(String(pwm));
  APP_PRINT_I("\n");

  digitalWrite(_motorA[0], LOW);
  digitalWrite(_motorA[1], HIGH);
  analogWrite(_motorA[2], pwm);

  digitalWrite(_motorB[0], LOW);
  digitalWrite(_motorB[1], HIGH);
  analogWrite(_motorB[2], pwm);
}

void CansatLib::mStop() {
  APP_PRINT_I("called mStop()\n");

  digitalWrite(_motorA[0], LOW);
  digitalWrite(_motorA[1], LOW);
  analogWrite(_motorA[2], 0);

  digitalWrite(_motorB[0], LOW);
  digitalWrite(_motorB[1], LOW);
  analogWrite(_motorB[2], 0);
}

void CansatLib::spStart() {
  APP_PRINT_I("called spStart()\n");
  _beep(_start, sizeof(_start)/sizeof(float), 150);
}

void CansatLib::spError() {
  APP_PRINT_I("called spError()\n");
  _beep(_error, sizeof(_error)/sizeof(float), 300);
}

void CansatLib::spBeep() {
  APP_PRINT_I("called spBeep()\n");
  _beep(_bell, sizeof(_bell)/sizeof(float), 200);
}

int CansatLib::readCds() {
  APP_PRINT_I("called readCds()\n");

  static int cdsVal = 0;
  cdsVal = analogRead(_cds);
  return cdsVal;
}

/***********************
 *** プライベート関数 ***
 ***********************/
bool CansatLib::_sdInit() {
  APP_PRINT_I("called _sdInit()\n");

  if (!SD.begin()) {
    APP_PRINT_E("No SD detected\n");
    return false;
  }
  SD.mkdir("log/");
  return true;
}

bool CansatLib::_gnssInit() {
  APP_PRINT_I("called _gnssInit()\n");

  Gnss.setDebugMode(PrintInfo);

  if (Gnss.begin() != 0) {
    APP_PRINT_E("Gnss begin error\n");
    return false;
  } else {
    Gnss.select(GPS);
    Gnss.select(QZ_L1CA);
    Gnss.select(QZ_L1S);
  }

  if (Gnss.start(COLD_START) != 0) {
    APP_PRINT_E("Gnss start error\n");
    return false;
  } else {
    APP_PRINT_I("Gnss setup OK\n");
  }
  return true;
}

bool CansatLib::_imuInit() {
  APP_PRINT_I("called _imuInit()\n");

  if (!bno.begin()) {
    APP_PRINT_E("No BNO055 detected\n");
    return false;
  }
  return true;
}

void CansatLib::_beep(float *mm, int m_size, int b_time) {
  APP_PRINT_I("called _beep()");

  for (int i = 0; i < m_size; i++) {
    tone(_speaker, mm[i], b_time);
    delay(b_time);
  }
  noTone(_speaker);
}
