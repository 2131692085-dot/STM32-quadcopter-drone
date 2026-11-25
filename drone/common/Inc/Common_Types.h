#ifndef __COMMONON_TYPES_H__
#define __COMMONON_TYPES_H__

#include <stdint.h>

typedef enum
{
    eRC_UNCONNECTED,
    eRC_CONNECTED 
} RC_Status_e;

typedef enum
{
    eDrone_IDLE,
    eDrone_NORMAL,
    eDrone_HOLD_HEIGHT,
    eDrone_FAULT,
} Drone_Status_e;

typedef enum
{
    eData_INVALID = 0,
    eData_VALID = 1,
} Data_Status_e;



typedef struct
{
    int16_t throttle; //油门
    int16_t yaw;      //偏航
    int16_t pitch;    //俯仰
    int16_t roll;     //横滚
    uint8_t off;      //关机信号位
    uint8_t hold_height;      //保持高度状态位
} RcData_t; 


typedef enum
{
    eFREE,
    eMAX,
    eLEAVE_MAX,
    eMIN,
    eUNLOCK
} THR_Status_e;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
}  ACCEL_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
}  GYRO_t;


typedef struct 
{
    ACCEL_t acc;
    GYRO_t gyro;
}A_G_Struct_t;

/* 封装表征飞行器姿态的欧拉角 */
typedef struct
{
    float pitch; /* 俯仰角 */
    float roll;  /* 横滚角 */
    float yaw;   /* 偏航角 */
} EulerAngle_Struct;



#endif 
