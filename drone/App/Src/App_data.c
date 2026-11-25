#include "App_data.h"


static uint8_t rxbuf[17];
Data_Status_e App_data_Receive(RcData_t *rc_data)
{
    // 从SI24R1接收数据
    uint8_t res = Int_SI24R1_RxPacket(rxbuf);
    if(res == 0)
    {
       if(rxbuf[10] == DRONE_ADDR0&&rxbuf[11] == DRONE_ADDR1&&rxbuf[12] == DRONE_ADDR2)
       {
         uint32_t checksum = 0;
         for(int i = 0; i < 13; i++)    
         {
            checksum += rxbuf[i];
         }
         if(checksum == (((uint32_t)rxbuf[13] << 24) | ((uint32_t)rxbuf[14] << 16) | ((uint32_t)rxbuf[15] << 8) | rxbuf[16]))
         {
            rc_data->throttle = (rxbuf[0]<<8) | rxbuf[1];
            rc_data->pitch = (rxbuf[2]<<8) | rxbuf[3];
            rc_data->roll = (rxbuf[4]<<8) | rxbuf[5];
            rc_data->yaw = (rxbuf[6]<<8) | rxbuf[7];
            rc_data->off = rxbuf[8];
            rc_data->hold_height = rxbuf[9];
            return eData_VALID;
         }
       }
    }
    return eData_INVALID;
}

THR_Status_e curr_thr_status = eFREE;
uint32_t enter_eMAX_tick = 0;
uint32_t enter_eMIN_tick = 0;
THR_Status_e App_data_Unlock(RcData_t *rc_data) 
{
   switch(curr_thr_status)
   {
      case eFREE:
         if(rc_data->throttle >= 900)
         {
            curr_thr_status = eMAX;
            enter_eMAX_tick = xTaskGetTickCount();
         }
         //printf("eFREE\n");
         break;
      case eMAX:
         if(rc_data->throttle < 900)
         {
            if(xTaskGetTickCount() - enter_eMAX_tick >= 1000)
            {
               curr_thr_status = eLEAVE_MAX;
            }
            else
            {
               curr_thr_status = eFREE; 
            }
         }
         //printf("eMAX\n");
         break;
      case eLEAVE_MAX:
         if(rc_data->throttle <= 100)
         {
            curr_thr_status = eMIN;
            enter_eMIN_tick = xTaskGetTickCount();
         }
         //printf("eLEAVE_MAX\n");
         break;
      case eMIN:
         if(rc_data->throttle > 100)
         {
            if(xTaskGetTickCount() - enter_eMIN_tick < 1000)
            {
               curr_thr_status = eFREE;
            }
         else
            if(xTaskGetTickCount() - enter_eMIN_tick >= 1000)
            { 
               curr_thr_status = eUNLOCK;
            }
         }
         //printf("eMIN\n");
         break;
      case eUNLOCK:
         //printf("eUNLOCK\n");
         break;
   }

   return curr_thr_status;
}

void App_data_ProcessDroneState(RcData_t *rc_data,Drone_Status_e *drone_state,RC_Status_e rc_state)
{
   switch(*drone_state)  
   {
      case eDrone_IDLE:
         if(App_data_Unlock(rc_data) == eUNLOCK)
         {
            *drone_state = eDrone_NORMAL;
         }
         break;
      case eDrone_NORMAL:
         if(rc_state == eRC_UNCONNECTED)
         {
            *drone_state = eDrone_FAULT;
         }
         else
         {
            if(rc_data->hold_height == 1)
            {
               *drone_state = eDrone_HOLD_HEIGHT;
            }
         }
         break;
      case eDrone_HOLD_HEIGHT:
         if(rc_state == eRC_UNCONNECTED)
         {
            *drone_state = eDrone_FAULT;
         }
         else
         {
            if(rc_data->hold_height == 0)
            {
               *drone_state = eDrone_NORMAL;
            }
         }
         break;
      case eDrone_FAULT:
         if (rc_state == eRC_CONNECTED)
         {
               *drone_state = eDrone_NORMAL;
         }
         break;
   }
}
