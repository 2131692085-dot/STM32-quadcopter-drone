#include "App_motor.h"

int16_t last_gyro_x_out = 0;
int16_t last_gyro_y_out = 0;
int16_t last_gyro_z_out = 0;

int16_t last_acc_x_out = 0;

void App_motor_GetAG_ByFilter(A_G_Struct_t *ag_struct)
{
    Int_MPU6050_ReadDateByCalibration(&ag_struct->acc, &ag_struct->gyro);
    // 一阶低通滤波
    ag_struct->gyro.x = Common_LPF(ag_struct->gyro.x, &last_gyro_x_out, 0.15);
    ag_struct->gyro.y = Common_LPF(ag_struct->gyro.y, &last_gyro_y_out, 0.15);
    ag_struct->gyro.z = Common_LPF(ag_struct->gyro.z, &last_gyro_z_out, 0.15);
    // 更新上一次输出
    last_gyro_x_out = ag_struct->gyro.x;
    last_gyro_y_out = ag_struct->gyro.y;
    last_gyro_z_out = ag_struct->gyro.z;

    // 针对加速度噪音更多 我们使用卡尔曼滤波器进行滤波
    //卡尔曼滤波
    ag_struct->acc.x = Common_Filter_KalmanFilter(&kfs[0], ag_struct->acc.x);
    ag_struct->acc.y = Common_Filter_KalmanFilter(&kfs[1], ag_struct->acc.y);
    ag_struct->acc.z = Common_Filter_KalmanFilter(&kfs[2], ag_struct->acc.z);
}

Single_PID_Handle_t pitch_outter_pid={.kp=5.0f, .ki=0.0f, .kd=0.0f,.dt=0.006f};
Single_PID_Handle_t pitch_inner_pid={.kp=-2.0f, .ki=0.0f, .kd=-0.09f,.dt=0.006f};

Single_PID_Handle_t roll_outter_pid={.kp=5.0f, .ki=0.0f, .kd=0.0f,.dt=0.006f};
Single_PID_Handle_t roll_inner_pid={.kp=2.0f, .ki=0.0f, .kd=0.09f,.dt=0.006f};

Single_PID_Handle_t yaw_outter_pid={.kp=2.0f, .ki=0.0f, .kd=0.0f,.dt=0.006f};
Single_PID_Handle_t yaw_inner_pid={.kp=4.0f, .ki=0.0f, .kd=0.0f,.dt=0.006f};

void App_motor_RunPID(RcData_t *RcData,EulerAngle_Struct *euler,A_G_Struct_t *ag_struct)
{
    //运行串级PID
    pitch_outter_pid.target = (RcData->pitch-500)*0.03;
    pitch_outter_pid.measure=euler->pitch;
    pitch_inner_pid.measure=ag_struct->gyro.y*4000.0/65535;   // 转换为角度

    roll_outter_pid.target = (RcData->roll-500)*0.03;
    roll_outter_pid.measure=euler->roll;
    roll_inner_pid.measure=ag_struct->gyro.x*4000.0/65535;   // 转换为角度

    yaw_outter_pid.target = (RcData->yaw-500)*0.03;
    yaw_outter_pid.measure=euler->yaw;
    yaw_inner_pid.measure=ag_struct->gyro.z*4000.0/65535;   // 转换为角度

    Common_Serial_PID(&pitch_outter_pid, &pitch_inner_pid);
    Common_Serial_PID(&roll_outter_pid, &roll_inner_pid);
    Common_Serial_PID(&yaw_outter_pid, &yaw_inner_pid);
}
