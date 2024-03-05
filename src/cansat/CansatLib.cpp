#include "File.h"
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

  if (!_createLogFile()) {
    errorFlag = 1;
  }

  if (!_gnssInit()) {
    errorFlag = 1;
  }

  if (!_imuInit()) {
    errorFlag = 1;
  }

  if (errorFlag == 1) {
    Led_isError(true);
    spError();
    exit(0);
  }
}

/**
 * @brief ログファイルに追記する
 */
bool CansatLib::appendLog() {
  APP_PRINT_I("called appendLog()\n");

  String message = _createMessage();

  myFile = SD.open("log/log.csv", FILE_WRITE);

  if (!myFile) {
    APP_PRINT_E("error opening log.csv");
    return false;
  }

  APP_PRINT_I("writing to log.csv...");
  myFile.println(message);
  myFile.close();
  APP_PRINT_I("done.\n");

  return true;
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

void CansatLib::updateSensorValue() {
  APP_PRINT_I("called updateSensorValue()\n");
  _updateGnss();
  _updateImuValue();
}

void CansatLib::printSensorValue() {
  APP_PRINT_I("called printSensorValue()\n");
  // Serial.print("roll: "); Serial.print(roll);
  // Serial.print("pitch: "); Serial.print(pitch);
  // Serial.print("heading: "); Serial.print(heading);
  Serial.print(accX); Serial.print(",");
  Serial.print(accY); Serial.print(",");
  Serial.print(accZ);
  Serial.println();
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

  return true;
}

bool CansatLib::_createLogFile() {
  APP_PRINT_I("called _createLogFile()\n");

#ifdef DELETE_EXIST_LOG_FILE
  APP_PRINT_I("remove log.csv...");
  SD.remove("log/log.csv");
  APP_PRINT_I("done.\n");
#endif

  SD.mkdir("log/");
  myFile = SD.open("log/log.csv", FILE_WRITE);

  if (!myFile) {
    APP_PRINT_E("error opening log.csv\n");
    return false;
  }

  APP_PRINT_I("writing to log.csv...");
  myFile.println(CSV_HEADER);
  myFile.close();
  APP_PRINT_I("done.\n");

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

/**
 * @brief 時刻・緯度・経度・高度・ゴールとの距離・方位の値を更新する
 */
void CansatLib::_updateGnss() {
  APP_PRINT_I("called _updateGnss()\n");

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
      currentDate = _createDate(NavData.time);
      currentLat = NavData.latitude;
      currentLng = NavData.longitude;
      currentAlt = NavData.altitude;
      distance2goal = _haversineDistance(currentLat, currentLng, userConfig.goalLat, userConfig.goalLng);
      direction2goal = _haversineBearing(currentLat, currentLng, userConfig.goalLat, userConfig.goalLng);

      APP_PRINT_I(currentDate);               APP_PRINT_I(",");
      APP_PRINT_I(String(currentLat, 6));     APP_PRINT_I(",");
      APP_PRINT_I(String(currentLng, 6));     APP_PRINT_I(",");
      APP_PRINT_I(String(currentAlt, 2));     APP_PRINT_I(",");
      APP_PRINT_I(String(distance2goal, 2));  APP_PRINT_I(",");
      APP_PRINT_I(String(direction2goal, 2)); APP_PRINT_I(",");
    }

    APP_PRINT_I("\n");
  } else {
    APP_PRINT_I("data not update\n");
  }
}

bool CansatLib::_imuInit() {
  APP_PRINT_I("called _imuInit()\n");

  if (!bno.begin()) {
    APP_PRINT_E("No BNO055 detected\n");
    return false;
  }
  return true;
}

void CansatLib::_updateImuValue() {
  APP_PRINT_I("called _updateImuValue()\n");

  sensors_event_t event, accelerometerData;
  bno.getEvent(&event);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);

  heading = event.orientation.x;
  pitch = event.orientation.y;
  roll = event.orientation.z;
  accX = accelerometerData.acceleration.x;
  accY = accelerometerData.acceleration.y;
  accZ = accelerometerData.acceleration.z;
  gyroX = event.gyro.x;
  gyroY = event.gyro.y;
  gyroZ = event.gyro.z;
  magX = event.magnetic.x;
  magY = event.magnetic.y;
  magZ = event.magnetic.z;
}

void CansatLib::_beep(float *mm, int m_size, int b_time) {
  APP_PRINT_I("called _beep()");

  for (int i = 0; i < m_size; i++) {
    tone(_speaker, mm[i], b_time);
    delay(b_time);
  }
  noTone(_speaker);
}

/** 
 * @brief GNSS受信した値から現在時刻の文字列を作成する
 * @param [in] time SpGnssTime SpNavData::time
 * @return 現在時刻
 */
String CansatLib::_createDate(SpGnssTime time) {
  APP_PRINT_I("called _createDate()\n");

  // YYYY/MM/DD hh:mm:ssZ
  String date = "";
  date += String(time.year);   date += "/";
  date += String(time.month);  date += "/";
  date += String(time.day);    date += " ";
  date += String(time.hour);   date += ":";
  date += String(time.minute); date += ":";
  date += String(time.sec);    date += "Z";

  return date;
}

/**
 * @brief ハーバインの公式による距離計算
 * @param [in] lat1 出発点の緯度
 * @param [in] lng1 出発点の経度
 * @param [in] lat2 到着点の緯度
 * @param [in] lng2 到着点の経度
 * @return 出発点から到着点ヘの距離
 */
double CansatLib::_haversineDistance(double lat1, double lng1, double lat2, double lng2) {
  APP_PRINT_I("called _haversineDistance()\n");

  double dLat = TO_RADIANS(lat2 - lat1);
  double dLon = TO_RADIANS(lng2 - lng1);
  double a = pow(sin(dLat / 2), 2) + cos(TO_RADIANS(lat1)) * cos(TO_RADIANS(lat2)) * pow(sin(dLon / 2), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return EARTH_RADIUS * c;
}

/**
 * @brief ハーバインの公式による方位計算
 * @param [in] lat1 出発点の緯度
 * @param [in] lng1 出発点の経度
 * @param [in] lat2 到着点の緯度
 * @param [in] lng2 到着点の経度
 * @return 出発点から到着点ヘの方位
 */
double CansatLib::_haversineBearing(double lat1, double lng1, double lat2, double lng2) {
  APP_PRINT_I("called _haversineBearing()\n");

  double dLon = TO_RADIANS(lng2 - lng1);
  double y = sin(dLon) * cos(TO_RADIANS(lat2));
  double x = cos(TO_RADIANS(lat1)) * sin(TO_RADIANS(lat2)) -
          sin(TO_RADIANS(lat1)) * cos(TO_RADIANS(lat2)) * cos(dLon);
  double bearing = atan2(y, x);
  return TO_DEGREES(bearing);
}

/**
 * @brief ログへ記録、または地上局へ送信するメッセージを一行分作成する
 * @return ログへ記録する値
 */
String CansatLib::_createMessage() {
  APP_PRINT_I("called _createMessage()\n");

  unsigned long currentTime = millis();

  String message = "";
  message += String(currentTime);    message += ","; // time
  message += String(currentDate);    message += ","; // date
  message += String((int)state);     message += ","; // mode
  message += String(currentLat);     message += ","; // lat
  message += String(currentLng);     message += ","; // lng
  message += String(currentAlt);     message += ","; // alt
  message += String(distance2goal);  message += ","; // distance
  message += String(direction2goal); message += ","; // direction
  message += ","; // mr_pwm
  message += ","; // ml_pwm
  message += ","; // cds
  message += String(accX);           message += ","; // ax
  message += String(accY);           message += ","; // ay
  message += String(accZ);           message += ","; // az
  message += String(gyroX);          message += ","; // gx
  message += String(gyroY);          message += ","; // gy
  message += String(gyroZ);          message += ","; // gz
  message += String(magX);           message += ","; // mx
  message += String(magY);           message += ","; // my
  message += String(magZ);           message += ","; // mz
  message += String(roll);           message += ","; // roll
  message += String(pitch);          message += ","; // pitch
  message += String(heading);        message += ","; // heading

  return message;
}
