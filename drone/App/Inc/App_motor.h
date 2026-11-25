
#ifndef __APP_MOTOR_H__
#define __APP_MOTOR_H__

#include "Common_Types.h"
#include "Common_Filter.h"
#include "Int_MPU6050.h"
#include "Common_PID.h"

extern Single_PID_Handle_t pitch_outter_pid;
extern Single_PID_Handle_t pitch_inner_pid;

extern Single_PID_Handle_t roll_outter_pid;
extern Single_PID_Handle_t roll_inner_pid;

extern Single_PID_Handle_t yaw_outter_pid;
extern Single_PID_Handle_t yaw_inner_pid;


void App_motor_GetAG_ByFilter(A_G_Struct_t *A_G_Struct_t);

void App_motor_RunPID(RcData_t *RcData,EulerAngle_Struct *euler,A_G_Struct_t *ag_struct);

#endif /* __APP_MOTOR_H__ */

