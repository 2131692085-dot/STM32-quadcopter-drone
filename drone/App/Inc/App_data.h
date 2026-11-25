#ifndef __APP_DATA_H__
#define __APP_DATA_H__
#include "Common_Types.h"
#include "Int_SI24R1.h"
#include "FreeRTOS.h"
#include "task.h"


#define DRONE_ADDR0   0x67
#define DRONE_ADDR1   0x76
#define DRONE_ADDR2   0x22

Data_Status_e App_data_Receive(RcData_t *rc_data);

THR_Status_e App_data_Unlock(RcData_t *rc_data);

void App_data_ProcessDroneState(RcData_t *rc_data,Drone_Status_e *drone_state,RC_Status_e rc_state);

#endif /* __APP_DATA_H__ */
