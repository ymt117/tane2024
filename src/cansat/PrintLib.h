#pragma once

enum AppPrintLevel {
  AppPrintNone = 0,
  AppPrintError,
  AppPrintWarning,
  AppPrintInfo,
};

extern AppPrintLevel AppDebugPrintLevel;

#define APP_DEBUG

#define APP_PRINT(c) Serial.print(c)
#ifdef APP_DEBUG
  #define APP_PRINT_E(c) if (AppPrintError   <= AppDebugPrintLevel) { Serial.print(c); }
  #define APP_PRINT_W(c) if (AppPrintWarning <= AppDebugPrintLevel) { Serial.print(c); }
  #define APP_PRINT_I(c) if (AppPrintInfo    <= AppDebugPrintLevel) { Serial.print(c); }
#else
  #define APP_PRINT_E(c)
  #define APP_PRINT_W(c)
  #define APP_PRINT_I(c)
#endif