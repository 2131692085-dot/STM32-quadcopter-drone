#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum
{
    KEY_LEFT_X,
    KEY_RIGHT_X,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_RIGHT_X_LONG,
    KEY_LEFT_X_LONG,
    KEY_NONE,
}   KEY_TYPE_e;

KEY_TYPE_e Int_Key_Scan(void);

/* 枚举值转可读字符串，无法识别时返回 "KEY_UNKNOWN" */
const char *Int_Key_ToStr(KEY_TYPE_e key);

#endif /* __INT_KEY_H__ */

