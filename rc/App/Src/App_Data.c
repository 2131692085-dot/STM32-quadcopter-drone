#include "App_Data.h"

int16_t pitch_offset = 0;
int16_t roll_offset = 0;
int16_t yaw_offset = 0;
int16_t throttle_offset = 0;

int16_t pitch_adjust = 0;
int16_t roll_adjust = 0;

#define LIMIT(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

void App_Data_ProcessJoystick(RcData_t *rc_data)
{
   taskENTER_CRITICAL();
   // 处理摇杆数据
   // 1. 读取摇杆数据
   Int_Joystick_ReadData(rc_data);

   // 2.解决反向问题
   rc_data->pitch = 4095 - rc_data->pitch;
   rc_data->roll = 4095 - rc_data->roll;
   rc_data->yaw = 4095 - rc_data->yaw;
   rc_data->throttle = 4095 - rc_data->throttle;

   // 3.值域
   rc_data->pitch = rc_data->pitch * 1000 / 4095;
   rc_data->roll = rc_data->roll * 1000 / 4095;
   rc_data->yaw = rc_data->yaw * 1000 / 4095;
   rc_data->throttle = rc_data->throttle * 1000 / 4095;

   // 4.偏移量
   rc_data->pitch -= pitch_offset;
   rc_data->roll -= roll_offset;
   rc_data->yaw -= yaw_offset;
   rc_data->throttle -= throttle_offset;

   // 4.1.微调
   rc_data->pitch += pitch_adjust; // 俯仰
   rc_data->roll += roll_adjust;   // 横滚

   // 5.值域保护
   rc_data->pitch = LIMIT(rc_data->pitch, 0, 1000);
   rc_data->roll = LIMIT(rc_data->roll, 0, 1000);
   rc_data->yaw = LIMIT(rc_data->yaw, 0, 1000);
   rc_data->throttle = LIMIT(rc_data->throttle, 0, 1000);

   taskEXIT_CRITICAL();

   // printf("throttle:%d, pitch:%d, roll:%d, yaw:%d, off:%d, hold_height:%d\n", rc_data->throttle, rc_data->pitch, rc_data->roll, rc_data->yaw, rc_data->off, rc_data->hold_height);   // [临时] 暂不打印摇杆数据，测试收发用
}

void App_Data_ProcessKey(RcData_t *rc_data)
{
   KEY_TYPE_e key = Int_Key_Scan();
   if (key == KEY_RIGHT_X_LONG)
   {
      pitch_offset = rc_data->pitch - 500;
      roll_offset = rc_data->roll - 500;
      yaw_offset = rc_data->yaw - 500;
      throttle_offset = rc_data->throttle - 0;
   }
   else if (key == KEY_UP)
   {
      pitch_adjust += 5;
   }
   else if (key == KEY_DOWN)
   {
      pitch_adjust -= 5;
   }
   else if (key == KEY_LEFT)
   {
      roll_adjust -= 5;
   }
   else if (key == KEY_RIGHT)
   {
      roll_adjust += 5;
   }
   else if (key == KEY_LEFT_X)
   {
      rc_data->off = 1;
   }
   else if (key == KEY_RIGHT_X)
   {
      rc_data->hold_height  ^= 1;
   }
}
 
static uint8_t txbuf[17] = {0};
void App_Data_SendData(RcData_t *rc_data)
{

   txbuf[0] = (rc_data->throttle >> 8) & 0xFF;
   txbuf[1] = rc_data->throttle & 0xFF;
   txbuf[2] = (rc_data->pitch >> 8) & 0xFF;
   txbuf[3] = rc_data->pitch & 0xFF;
   txbuf[4] = (rc_data->roll >> 8) & 0xFF;
   txbuf[5] = rc_data->roll & 0xFF;
   txbuf[6] = (rc_data->yaw >> 8) & 0xFF;
   txbuf[7] = rc_data->yaw & 0xFF;
   txbuf[8] = rc_data->off;
   txbuf[9] = rc_data->hold_height;
   txbuf[10] = DRONE_ADDR0;
   txbuf[11] = DRONE_ADDR1;
   txbuf[12] = DRONE_ADDR2;
   // 校验和
   uint32_t checksum = 0;
   for (int i = 0; i < 13; i++)
   {
      checksum += txbuf[i];
   }
   txbuf[13] = checksum >> 24;
   txbuf[14] = checksum >> 16;
   txbuf[15] = checksum >> 8;
   txbuf[16] = checksum & 0xFF;
   uint8_t tx_res = Int_SI24R1_TxPacket(txbuf);
   if (tx_res == TX_DS)
   {
      rc_data->off = 0;
   }
}
