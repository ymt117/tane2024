#include "CansatLib.h"
#include "PrintLib.h"
#include "Dango.h"

CansatLib sat;
AppPrintLevel AppDebugPrintLevel;

/**
 * @enum LoopState
 * @brief ループ処理のステータス
 */
enum LoopState {
  eStateSleep,
  eStateActive,
};

/**
 * @brief 初期化処理。最初に一度だけ実行される関数。
 */
void setup() {
  // シリアルコンソールに出力する情報のレベルを設定
  sat.printLevel = PrintLevel::PrintInfo;
  AppDebugPrintLevel = (AppPrintLevel)sat.printLevel;

  sat.begin();
  // ゴール座標を設定する
  // TODO: SDカードから設定ファイルを読み込めるようにする
  sat.userConfig.goalLat = 35.658582;
  sat.userConfig.goalLng = 139.7454544;
  sat.userConfig.altThreshold = 20; // 打ち上げ判定高度のしきい値を設定する
  sat.userConfig.cdsThreshold = 400; // 放出判定（照度センサ）のしきい値を設定する
  sat.userConfig.accThreshold = 0; // 着地判定（加速度センサ）のしきい値を設定する
  sat.userConfig.distanceThreshold = 5; // ゴール判定の距離のしきい値を設定する

  APP_PRINT("Hello 100kinSAT!!!\n");
  TweliteSend("Hello 100kinSAT!!!\n");

  delay(2000);
  sat.spStart();
}

/**
 * @brief ループ処理。setup()関数が実行されたあと、繰り返し実行される関数
 */
void loop() {
  static int state = eStateActive;

  if (state == eStateSleep) {
    // 何もしない
  } else {
    sat.Led_isActive();

    // switch-case文でCanSatの状態遷移を行う
    switch (sat.state) {
      case CansatState::CALIBRATION:
        APP_PRINT_I("*** CALIBRATION ***\n");
        break;

      case CansatState::STAND_BY:
        APP_PRINT_I("*** STAND BY ***\n");
        standBy();
        break;

      case CansatState::LAUNCH:
        APP_PRINT_I("*** LAUNCH ***\n");
        launch();
        break;

      case CansatState::DROP:
        APP_PRINT_I("*** DROP ***\n");
        drop();
        break;

      case CansatState::LANDING:
        APP_PRINT_I("*** LANDING ***\n");
        landing();
        break;

      case CansatState::NAVIGATION:
        APP_PRINT_I("*** NAVIGATION ***\n");
        navigation();
        break;

      case CansatState::GOAL:
        APP_PRINT_I("*** GOAL ***\n");
        goal();
        break;

      default:
        break;
    }
  }

  delay(50);
}

/**
 * @brief キャリブレーション処理
 */
void calibration() {
  // IMUセンサのキャリブレーション
  // 高度のキャリブレーション（地表をゼロメートルに合わせる）
}

/**
 * @brief 待機処理
 */
void standBy() {
  // 一定の高度を超えたら LAUNCH モードに遷移する
  static bool altFlag = false;
  static bool timeFlag = false;
  static long currentTime = millis();

  sat.updateSensorValue();
  sat.appendLog();
  // sat.printSensorValue();

  if (sat.currentAlt > sat.userConfig.altThreshold) {
    altFlag = true;
  }

  long elapsedTime = millis() - currentTime;
  if (elapsedTime > sat.userConfig.timeThreshold) {
    timeFlag = true;
  }

  // 高度または時間の条件を満たしたらモード変更
  if (
    altFlag
    || timeFlag
  ) {
    sat.state = CansatState::LAUNCH;
  }
}

/**
 * @brief 打ち上げ処理
 */
void launch() {
  // 放出を検知したら DROP モードに遷移する
  // 照度センサの変化
  // 高度の変化
  static bool cdsFlag = false;

  sat.updateSensorValue();
  sat.appendLog();
  // sat.printSensorValue();

  if (sat.cdsValue < sat.userConfig.cdsThreshold) {
    cdsFlag = true;
  }

  if (cdsFlag) {
    sat.state = CansatState::DROP;
  }
}

/**
 * @brief 投下処理
 */
void drop() {
  // 着地を検知したら LANDING モードに遷移する
  static bool accFlag = false;

  sat.updateSensorValue();
  sat.appendLog();
  // sat.printSensorValue();

  // 加速度センサのxyz軸の平方和を計算
  double acc = sat.accX * sat.accX + sat.accY * sat.accY + sat.accZ * sat.accZ;

  // しきい値以下になったら着地と判断する
  if (acc < sat.userConfig.accThreshold) {
    accFlag = true;
  }

  if (accFlag) {
    sat.state = CansatState::LANDING;
  }
}

/**
 * @brief 着地処理
 */
void landing() {
  // パラシュートの切り離し
  // 着地時の姿勢判定・修正 
  // NAVIGATION モードに遷移する

  sat.updateSensorValue();
  sat.appendLog();
  // sat.printSensorValue();

  delay(5000);

  // ニクロム線を加熱してテグスを切る
  sat.heatWire(150, 10000);

  delay(5000);

  sat.state = CansatState::NAVIGATION;
}

/**
 * @brief 誘導処理
 */
void navigation() {
  // 目標地点に到達したら GOAL モードに遷移する
  static bool nearGoalFlag = false;
  static bool reachGoalFlag = false;

  sat.updateSensorValue();
  sat.appendLog();
  // sat.printSensorValue();

  // 移動前のゴールとの距離を取得
  double beforeDistance = sat.distanceToGoal;
  // ゴールとの距離がしきい値以下の場合、ゴール状態へ遷移
  if (beforeDistance < sat.userConfig.distanceThreshold) {
    sat.state = CansatState::GOAL;
    return;
  }

  // 移動前のゴールとの方位を取得
  double beforeDirection = sat.directionToGoal;
  // CanSatの向きと比較
  double courseDiff = beforeDirection - sat.heading;
  // ゴールへ方向転換
  sat.mOutputTime = (int)(14 * abs(courseDiff)); // モータへの出力時間を求める
  if (courseDiff > 0) {
    sat.mTurnRight(150);
    delay(sat.mOutputTime);
    sat.mStop();
  } else {
    sat.mTurnLeft(150);
    delay(sat.mOutputTime);
    sat.mStop();
  }

  sat.updateSensorValue();
  sat.appendLog();

  // 直進する
  sat.mOutputTime = 5000;
  sat.mForward(200);
  sat.mStop();

  sat.updateSensorValue();
  sat.appendLog();

  sat.mOutputTime = 0;
}

/**
 * @brief ゴール処理
 */
void goal() {
  // だんご大家族を演奏する
  // LED2を点灯する
  sat.updateSensorValue();
  sat.appendLog();

  delay(2000);
  playDango();
  while(1);
}
