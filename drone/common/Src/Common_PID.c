#include "Common_PID.h"

void Common_Single_PID(Single_PID_Handle_t *pid_handle)
{
    float current_error = pid_handle->target - pid_handle->measure;
    float p_part = pid_handle->kp * current_error;
    pid_handle->sum += current_error * pid_handle->dt;
    float i_part = pid_handle->ki * pid_handle->sum; 
    float d_part = pid_handle->kd * (current_error - pid_handle->last_error) / pid_handle->dt;
    pid_handle->last_error = current_error;
    pid_handle->out = p_part + i_part + d_part;
}

void Common_Serial_PID(Single_PID_Handle_t *outter_pid, Single_PID_Handle_t *inner_pid)
{
    //先算外环PID 产生输出值
    Common_Single_PID(outter_pid);
    inner_pid->target = outter_pid->out;
    //再算内环PID 产生输出值
    Common_Single_PID(inner_pid);
}

