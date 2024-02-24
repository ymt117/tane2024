#include <SDHCI.h>

SDClass mySD;

boolean BeginSDCard(void) {
  if (!mySD.begin()) {
    return false;
  }
  return true;
}

int WriteChar(const char* pBuff, const char* pName, int flag) {}

int ReadChar(char* pBuff, int BufferSize, const char* pName, int flag);

boolean IsFileExist(const char* pName);