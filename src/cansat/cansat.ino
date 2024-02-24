#include "CansatLib.h"
#include "PrintLib.h"

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
      case CansatState::STAND_BY:
        APP_PRINT("*** STAND BY ***\n");
        standBy();
        break;

      case CansatState::LAUNCH:
        APP_PRINT("*** LAUNCH ***\n");
        launch();
        break;

      case CansatState::DROP:
        APP_PRINT("*** DROP ***\n");
        drop();
        break;

      case CansatState::LANDING:
        APP_PRINT("*** LANDING ***\n");
        landing();
        break;

      case CansatState::NAVIGATION:
        APP_PRINT("*** NAVIGATION ***\n");
        navigation();
        break;

      case CansatState::GOAL:
        APP_PRINT("*** GOAL ***\n");
        goal();
        break;

      default:
        break;
    }
  }

  delay(1000);
}

/**
 * @brief 待機処理
 */
void standBy() {
  // 一定の高度を超えたら LAUNCH モードに遷移する
  sat.posUpdate();
  sat.appendLog();
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
  // パラシュートを切り離したら NAVIGATION モードに遷移する
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
}
