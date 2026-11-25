#include "RC_tasks.h"
#include <stdio.h>  /* printf 原型声明 */

//全局变量区域start
RcData_t rc_data = {0};





//全局变量区域end

void Power_task(void *pvParameters);
#define POWER_TASK_NAME "Power_task"  
#define POWER_TASK_STACK_SIZE 256
#define POWER_TASK_PRIORITY 4
TaskHandle_t Power_task_handle = NULL;

//按键任务
void Key_task(void *pvParameters);
#define KEY_TASK_NAME "Key_task"  
#define KEY_TASK_STACK_SIZE 256
#define KEY_TASK_PRIORITY 2
TaskHandle_t Key_task_handle = NULL;

//摇杆任务
void Joystick_task(void *pvParameters);
#define JOYSTICK_TASK_NAME "Joystick_task"  
#define JOYSTICK_TASK_STACK_SIZE 256
#define JOYSTICK_TASK_PRIORITY 2
TaskHandle_t Joystick_task_handle = NULL;

//通讯任务
void Comm_task(void *pvParameters);
#define COMM_TASK_NAME "Comm_task"  
#define COMM_TASK_STACK_SIZE 256
#define COMM_TASK_PRIORITY 3
TaskHandle_t Comm_task_handle = NULL;



void Start_all_tasks(void)
{
    //初始化摇杆
    Int_Joystick_Init();
    //初始化通讯--发送模式
    Int_SI24R1_Init(TX_MODE);
    //电源任务
    xTaskCreate(Power_task, POWER_TASK_NAME, POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &Power_task_handle);
    //按键任务
    xTaskCreate(Key_task, KEY_TASK_NAME, KEY_TASK_STACK_SIZE, NULL, KEY_TASK_PRIORITY, &Key_task_handle);
    //摇杆任务
    xTaskCreate(Joystick_task, JOYSTICK_TASK_NAME, JOYSTICK_TASK_STACK_SIZE, NULL, JOYSTICK_TASK_PRIORITY, &Joystick_task_handle);
    //通讯任务
    xTaskCreate(Comm_task, COMM_TASK_NAME, COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, &Comm_task_handle);
    //启动任务
    vTaskStartScheduler();
}

void Power_task(void *pvParameters)
{
    printf("Power_task start\n");
    while (1)
    {
        //再来一个10000ms的延时 意思是每隔10秒拉低一次电源
        vTaskDelay(10000);
        //拉低
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
        //保证短按的时间 
        vTaskDelay(100);
        //再断开
        HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);

    }
}


void Key_task(void *pvParameters)
{
    printf("Key_task start\n");
    while (1)
    {
        App_Data_ProcessKey(&rc_data);
        vTaskDelay(10);
    }
}

void Joystick_task(void *pvParameters)
{
    printf("Joystick_task start\n");
    while (1)
    {
        //扫描摇杆
        App_Data_ProcessJoystick(&rc_data);
        vTaskDelay(10);
    }
}



void Comm_task(void *pvParameters)
{
    printf("Comm_task start\n");
    uint32_t pre_ticks = xTaskGetTickCount();
    while (1)
    {
        //发送数据
       App_Data_SendData(&rc_data);
       vTaskDelayUntil(&pre_ticks, 6);
    }
}
