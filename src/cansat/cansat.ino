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
  sat.updateSensorValue();
  sat.appendLog();
  sat.printSensorValue();
  // delay(2000);
  // playDango();
  // while(1){}
}

/**
 * @brief 打ち上げ処理
 */
void launch() {
  // 放出を検知したら DROP モードに遷移する
  // 照度センサの変化
  // 高度の変化
}

/**
 * @brief 投下処理
 */
void drop() {
  // 着地を検知したら LANDING モードに遷移する
}

/**
 * @brief 着地処理
 */
void landing() {
  // パラシュートの切り離し
  // 着地時の姿勢判定・修正 
  // NAVIGATION モードに遷移する
}

/**
 * @brief 誘導処理
 */
void navigation() {
  // 目標地点に到達したら GOAL モードに遷移する
}

/**
 * @brief ゴール処理
 */
void goal() {
  // だんご大家族を演奏する
  // LED2を点灯する
}
