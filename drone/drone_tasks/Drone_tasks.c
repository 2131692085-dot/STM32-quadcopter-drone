#include "Drone_tasks.h"
#include <stdio.h> /* printf 原型声明 */

// 全局变量区 Start
RC_Status_e RC_Status = eRC_UNCONNECTED;
Drone_Status_e Drone_Status = eDrone_IDLE;

RcData_t RcData;

// 全局变量区 End

void Power_task(void *pvParameters);
#define POWER_TASK_NAME "Power_task"
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4
TaskHandle_t Power_task_handle;

// LED任务
void LED_task(void *pvParameters);
#define LED_TASK_NAME "LED_task"
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIORITY 1
TaskHandle_t LED_task_handle;

// 电机任务
void Motor_task(void *pvParameters);
#define MOTOR_TASK_NAME "Motor_task"
#define MOTOR_TASK_STACK_SIZE 128
#define MOTOR_TASK_PRIORITY 2
TaskHandle_t Motor_task_handle;

// 通讯任务
void Comm_task(void *pvParameters);
#define COMM_TASK_NAME "Comm_task"
#define COMM_TASK_STACK_SIZE 128
#define COMM_TASK_PRIORITY 3
TaskHandle_t Comm_task_handle;

#define LIMIT(x, min, max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

void Start_all_tasks(void)
{
    // 初始化电机
    Int_Motor_Init();
    HAL_Delay(400);
    // 初始化SI24R1
    Int_SI24R1_Init(RX_MODE);
    // 初始化MPU6050
    Int_MPU6050_Init();
    // 电源任务
    xTaskCreate(Power_task, POWER_TASK_NAME, POWER_TASK_STACK_SIZE, NULL, POWER_TASK_PRIORITY, &Power_task_handle);
    // LED任务
    xTaskCreate(LED_task, LED_TASK_NAME, LED_TASK_STACK_SIZE, NULL, LED_TASK_PRIORITY, &LED_task_handle);
    // 电机任务
    xTaskCreate(Motor_task, MOTOR_TASK_NAME, MOTOR_TASK_STACK_SIZE, NULL, MOTOR_TASK_PRIORITY, &Motor_task_handle);
    // 通讯任务
    xTaskCreate(Comm_task, COMM_TASK_NAME, COMM_TASK_STACK_SIZE, NULL, COMM_TASK_PRIORITY, &Comm_task_handle);
    // 启动任务
    vTaskStartScheduler();
}

void Power_task(void *pvParameters)
{
    printf("Power_task start\n");
    while (1)
    {
        // 再来一个10000ms的延时 意思是每隔10秒拉低一次电源
        uint32_t notify_count = ulTaskNotifyTake(pdTRUE, 10000);
        if (notify_count == 0)
        {
            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // 保证短按的时间
            vTaskDelay(100);
            // 再断开
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
        }
        else if (notify_count > 0)
        {
            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // 保证短按的时间
            HAL_Delay(500);
            // 再断开
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);

            HAL_Delay(100);

            // 拉低
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_RESET);
            // 保证短按的时间
            HAL_Delay(500);
            // 再断开
            HAL_GPIO_WritePin(IP5305T_KEY_GPIO_Port, IP5305T_KEY_Pin, GPIO_PIN_SET);
            
        }
    }
}

void LED_task(void *pvParameters)
{
    //printf("LED_task start\n");
    uint32_t bottom_led_last_toggle_tick = 0;
    while (1)
    {
        // 根据遥控状态切换LED状态
        switch (RC_Status)
        {
        case eRC_UNCONNECTED:
            // 未连接时，关闭前两个灯
            Int_LED_Off(&led_left_top);
            Int_LED_Off(&led_right_top);
            break;
        case eRC_CONNECTED:
            // 已连接时，常亮前两个灯
            Int_LED_On(&led_left_top);
            Int_LED_On(&led_right_top);
            break;
        }

        // 根据无人机状态切换LED状态
        switch (Drone_Status)
        {
        case eDrone_HOLD_HEIGHT:
            // 定高时，常亮后两个灯
            Int_LED_On(&led_left_bottom);
            Int_LED_On(&led_right_bottom);
            break;
        case eDrone_FAULT:
            // 故障时，关闭后两个灯
            Int_LED_Off(&led_left_bottom);
            Int_LED_Off(&led_right_bottom);
            break;
        case eDrone_IDLE:
            // 空闲时，后两个灯每隔1秒切换一次（慢闪烁）
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 1000)
            {
                bottom_led_last_toggle_tick = xTaskGetTickCount();
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
            }
            break;
        case eDrone_NORMAL:
            // 正常状态，后两个灯快闪烁（每0.5秒切换一次）
            if (xTaskGetTickCount() - bottom_led_last_toggle_tick >= 100)
            {
                bottom_led_last_toggle_tick = xTaskGetTickCount();
                Int_LED_Toggle(&led_left_bottom);
                Int_LED_Toggle(&led_right_bottom);
            }
            break;
        }
        vTaskDelay(50);
    }
}


A_G_Struct_t ag_struct={0};
EulerAngle_Struct eulerAngle={0};
void Motor_task(void *pvParameters)
{
    //printf("Motor_task start\n");
    uint32_t pre_tick = xTaskGetTickCount();
    while (1)
    {
        App_motor_GetAG_ByFilter(&ag_struct);
        // ★2026-11-26 对齐day09: 控制周期 10ms→6ms，dt 必须与周期严格一致
        Common_IMU_GetEulerAngle(&ag_struct, &eulerAngle, 0.006f);
        //printf("%f,%f,%f\n", eulerAngle.pitch, eulerAngle.roll, eulerAngle.yaw);

        if(Drone_Status == eDrone_IDLE)
        {
            // 空闲时，电机速度为0
            motor_left_top.speed = 0;
            motor_left_bottom.speed = 0;
            motor_right_top.speed = 0;
            motor_right_bottom.speed = 0; 
        }
        else if(Drone_Status == eDrone_NORMAL)
        {
            //实现一个算法 根据遥控器数据 控制飞机姿态
            App_motor_RunPID(&RcData,&eulerAngle, &ag_struct);

            motor_left_top.speed = LIMIT((LIMIT(RcData.throttle,0,800) + pitch_inner_pid.out + roll_inner_pid.out + yaw_inner_pid.out), 0, 1000);
            motor_left_bottom.speed = LIMIT((LIMIT(RcData.throttle,0,800) - pitch_inner_pid.out + roll_inner_pid.out - yaw_inner_pid.out), 0, 1000);
            motor_right_top.speed = LIMIT((LIMIT(RcData.throttle,0,800) + pitch_inner_pid.out - roll_inner_pid.out - yaw_inner_pid.out), 0, 1000);
            motor_right_bottom.speed = LIMIT((LIMIT(RcData.throttle,0,800) - pitch_inner_pid.out - roll_inner_pid.out + yaw_inner_pid.out), 0, 1000);
        }
        else if(Drone_Status == eDrone_HOLD_HEIGHT) 
        {
            // 再实现一个算法 控制飞机保持高度
            motor_left_top.speed = RcData.throttle;
            motor_left_bottom.speed = RcData.throttle;
            motor_right_top.speed = RcData.throttle;
            motor_right_bottom.speed = RcData.throttle;
        }
        else if(Drone_Status == eDrone_FAULT)
        {
            // 故障时，电机速度为0
            motor_left_top.speed = 0;
            motor_left_bottom.speed = 0;
            motor_right_top.speed = 0;
            motor_right_bottom.speed = 0;
        }

        if(RcData.throttle < 50)
        {
            // 0 时，电机速度为0
            motor_left_top.speed = 0;
            motor_left_bottom.speed = 0;
            motor_right_top.speed = 0;
            motor_right_bottom.speed = 0;
        }

        Int_Motor_UpdateSpeed(&motor_right_top);
        Int_Motor_UpdateSpeed(&motor_right_bottom);
        // ★2026-11-26 对齐day09: Motor_task 周期 10ms→6ms（与上面 dt=0.006 配套，与 Comm_task 同拍）
        vTaskDelayUntil(&pre_tick, 6);
    }
}

void Comm_task(void *pvParameters)
{
    //printf("Comm_task start\n");
    uint32_t pre_tick = xTaskGetTickCount();

    uint8_t invalid_count = 0;
    while (1)
    {
        Data_Status_e res = App_data_Receive(&RcData);
        // 1.处理遥控的连接状态
        if (res == eData_VALID)
        {
            invalid_count = 0;
            RC_Status = eRC_CONNECTED;
        }
        else if (res == eData_INVALID)
        {
            invalid_count++;
            if (invalid_count == 50)
            {
                RC_Status = eRC_UNCONNECTED;
            }
        }
        // 2.判断off==1 如果是：就给电源任务一个give 让它执行关机
        if (RcData.off == 1)
        {
            xTaskNotifyGive(Power_task_handle);
        }

        App_data_ProcessDroneState(&RcData,&Drone_Status,RC_Status);
        vTaskDelayUntil(&pre_tick, 6);
    }
}
