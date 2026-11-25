#include "Int_Motor.h"

Motor_Handle_t motor_left_top = {&htim3, TIM_CHANNEL_1};
Motor_Handle_t motor_left_bottom = {&htim4, TIM_CHANNEL_4};
Motor_Handle_t motor_right_top = {&htim2, TIM_CHANNEL_2};
Motor_Handle_t motor_right_bottom = {&htim1, TIM_CHANNEL_3};

void Int_Motor_Init(void)
{
    // 初始化电机定时器
    HAL_TIM_PWM_Start(motor_left_top.htim, motor_left_top.channel);
    HAL_TIM_PWM_Start(motor_left_bottom.htim, motor_left_bottom.channel);
    HAL_TIM_PWM_Start(motor_right_top.htim, motor_right_top.channel);
    HAL_TIM_PWM_Start(motor_right_bottom.htim, motor_right_bottom.channel);
}

void Int_Motor_UpdateSpeed(Motor_Handle_t *motor_handle)
{
    __HAL_TIM_SET_COMPARE(motor_handle->htim, motor_handle->channel, motor_handle->speed);
}

