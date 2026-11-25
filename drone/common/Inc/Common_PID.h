#ifndef __COMMON_PID_H__
#define __COMMON_PID_H__
#include "stdint.h"

typedef struct
{
    float kp;
    float ki;
    float kd;
    float target;
    float measure;
    float dt;
    float sum;
    float last_error;
    float out;
   } Single_PID_Handle_t;


void Common_Single_PID(Single_PID_Handle_t *pid_handle);

void Common_Serial_PID(Single_PID_Handle_t *pid_outter, Single_PID_Handle_t *pid_inner);

#endif /* __COMMON_PID_H__ */

