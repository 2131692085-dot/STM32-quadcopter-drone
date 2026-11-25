#include "Int_Key.h"

/* 枚举值 -> 名字。顺序必须与 KEY_TYPE_e 的定义顺序完全一致 */
static const char *const s_key_type_names[] = {
    "KEY_LEFT_X",
    "KEY_RIGHT_X",
    "KEY_UP",
    "KEY_DOWN",
    "KEY_LEFT",
    "KEY_RIGHT",
    "KEY_RIGHT_X_LONG",
    "KEY_LEFT_X_LONG",
    "KEY_NONE",
};

/* 编译期检查：表项数必须等于枚举成员数，枚举新增成员时会在这里报错 */
typedef char key_type_name_count_check[
    (sizeof(s_key_type_names) / sizeof(s_key_type_names[0]) == (size_t)(KEY_NONE + 1)) ? 1 : -1];

const char *Int_Key_ToStr(KEY_TYPE_e key)
{
    /* 转成无符号再比较，负值或越界都安全落到默认分支 */
    if ((unsigned)key < sizeof(s_key_type_names) / sizeof(s_key_type_names[0]))
    {
        return s_key_type_names[key];
    }
    return "KEY_UNKNOWN";
}

KEY_TYPE_e Int_Key_Scan(void)
{
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_SET)
                {
                    return KEY_UP;
                }
            }
        }
    }

        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_SET)
                {
                    return KEY_DOWN;
                }
            }
        }
    }

        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_LEFT;
                }
            }
        }
    }

        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_SET)
                {
                    return KEY_RIGHT;
                }
            }
        }
    }

        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_time = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_time = xTaskGetTickCount();
                    if (end_time - start_time >= 1000)
                    {
                        return KEY_RIGHT_X_LONG;
                    }
                    else
                    {
                        return KEY_RIGHT_X;
                    }
                }
            }
        }
    }

        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
    {
        uint32_t start_time = xTaskGetTickCount();
        vTaskDelay(10);
        if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
        {
            while (1)
            {
                if (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_SET)
                {
                    uint32_t end_time = xTaskGetTickCount();
                    if (end_time - start_time >= 1000)
                    {
                        return KEY_LEFT_X_LONG;
                    }
                    else
                    {
                        return KEY_LEFT_X;
                    }
                }
            }
        }
    }

    return KEY_NONE;
}

