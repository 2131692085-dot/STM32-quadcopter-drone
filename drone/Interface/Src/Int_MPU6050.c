#include "Int_MPU6050.h"

void Int_MPU6050_WriteReg(uint8_t regAddr, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, (uint16_t)regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

void Int_MPU6050_ReadRegs(uint8_t regStartAddr, uint8_t *pBuff, uint8_t len)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, (uint16_t)regStartAddr, I2C_MEMADD_SIZE_8BIT, pBuff, len, 1000);
}

void Int_MPU6050_Init(void)
{
    // 1.让6050里的所有寄存器复位
    Int_MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x80);
    HAL_Delay(150);
    //让6050退出睡眠模式
    Int_MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x00);
    // 2.让陀螺仪的量程为±2000dps
    Int_MPU6050_WriteReg(MPU6050_REG_GYRO_CONFIG, 0x18);
    // 3.让加速度计的量程为±2g
    Int_MPU6050_WriteReg(MPU6050_REG_ACCEL_CONFIG, 0x00);  //其实可以不写 因为默认值符合需求
    // 4.让6050的DLFP只开一档，如果开太高容易引入延迟
    Int_MPU6050_WriteReg(MPU6050_REG_CONFIG, 0x01);
    // 5.让6050的采样率为1KHz 不分频
    Int_MPU6050_WriteReg(MPU6050_REG_SMPRT_DIV, 0x00);  //其实可以不写 因为默认值符合需求
    // 6.关闭所有中断
    Int_MPU6050_WriteReg(MPU6050_REG_INT_ENABLE, 0x00);  //其实可以不写 因为默认值符合需求
    // 7.关闭FIFO使能 关闭主I2C
    Int_MPU6050_WriteReg(MPU6050_REG_USER_CTRL, 0x00);   //其实可以不写 因为默认值符合需求
    // 8.关闭所有FIFO的子开关
    Int_MPU6050_WriteReg(MPU6050_REG_FIFO_EN, 0x00);  //其实可以不写 因为默认值符合需求
    // 9.选择了x轴的机械时钟
    Int_MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_1, 0x01);
    // 10.6个轴都不需要待机
    Int_MPU6050_WriteReg(MPU6050_REG_PWR_MGMT_2, 0x00);  //其实可以不写 因为默认值符合需求

    Int_MPU6050_Calibration();
}

// uint8_t MPU6050_Raw_Data_Buffer[14];
// void Int_MPU6050_ReadDataRAW(void)
// {
//     Int_MPU6050_ReadRegs(MPU6050_REG_ACCEL_XOUT_H, MPU6050_Raw_Data_Buffer, 14);
//     ACCEL_t acc;
//     acc.x = MPU6050_Raw_Data_Buffer[0] << 8 | MPU6050_Raw_Data_Buffer[1];
//     acc.y = MPU6050_Raw_Data_Buffer[2] << 8 | MPU6050_Raw_Data_Buffer[3];
//     acc.z = MPU6050_Raw_Data_Buffer[4] << 8 | MPU6050_Raw_Data_Buffer[5];

//     GYRO_t gyro;
//     gyro.x = MPU6050_Raw_Data_Buffer[8] << 8 | MPU6050_Raw_Data_Buffer[9];
//     gyro.y = MPU6050_Raw_Data_Buffer[10] << 8 | MPU6050_Raw_Data_Buffer[11];
//     gyro.z = MPU6050_Raw_Data_Buffer[12] << 8 | MPU6050_Raw_Data_Buffer[13];

//     printf("acc.x = %d, acc.y = %d, acc.z = %d\n", acc.x, acc.y, acc.z);
//     printf("gyro.x = %d, gyro.y = %d, gyro.z = %d\n", gyro.x, gyro.y, gyro.z);
// }

uint8_t MPU6050_Raw_Data_Buffer[14];
void Int_MPU6050_ReadDataRAW(ACCEL_t *acc, GYRO_t *gyro)
{
    Int_MPU6050_ReadRegs(MPU6050_REG_ACCEL_XOUT_H, MPU6050_Raw_Data_Buffer, 14);
    acc->x = MPU6050_Raw_Data_Buffer[0] << 8 | MPU6050_Raw_Data_Buffer[1];
    acc->y = MPU6050_Raw_Data_Buffer[2] << 8 | MPU6050_Raw_Data_Buffer[3];
    acc->z = MPU6050_Raw_Data_Buffer[4] << 8 | MPU6050_Raw_Data_Buffer[5];

    gyro->x = MPU6050_Raw_Data_Buffer[8] << 8 | MPU6050_Raw_Data_Buffer[9];
    gyro->y = MPU6050_Raw_Data_Buffer[10] << 8 | MPU6050_Raw_Data_Buffer[11];
    gyro->z = MPU6050_Raw_Data_Buffer[12] << 8 | MPU6050_Raw_Data_Buffer[13];
}

int16_t acc_x_avg=0;
int16_t acc_y_avg=0;
int16_t acc_z_avg=0;
int16_t gyro_x_avg=0;
int16_t gyro_y_avg=0;
int16_t gyro_z_avg=0;

void Int_MPU6050_Calibration(void)
{
    //1.先判断是否稳定 （以角速度为标准判定是否稳定）
    //当前的角速度 减去上次的角速度 小于50 计数器就++1一次 达到400次就稳定了
    GYRO_t last_gyro={0};
    uint16_t count = 0;
    int32_t acc_x_sum=0;
    int32_t acc_y_sum=0;
    int32_t acc_z_sum=0;
    int32_t gyro_x_sum=0;
    int32_t gyro_y_sum=0;
    int32_t gyro_z_sum=0;
    while(1)
    {
        ACCEL_t current_acc={0};
        GYRO_t current_gyro={0};
        Int_MPU6050_ReadDataRAW(&current_acc, &current_gyro);
        if((abs(current_gyro.x - last_gyro.x) < 50) && (abs(current_gyro.y - last_gyro.y) < 50) && (abs(current_gyro.z - last_gyro.z) < 50))
        {
            //稳定了
            count++;
            acc_x_sum += current_acc.x;
            acc_y_sum += current_acc.y;
            acc_z_sum += current_acc.z;
            gyro_x_sum += current_gyro.x;
            gyro_y_sum += current_gyro.y;
            gyro_z_sum += current_gyro.z; 
        }
        else
        {
            count = 0;
            acc_x_sum = 0;
            acc_y_sum = 0;
            acc_z_sum = 0;
            gyro_x_sum = 0;
            gyro_y_sum = 0;
            gyro_z_sum = 0;
        }
        last_gyro = current_gyro;
        //2.稳定之后去算均值（6轴）
        if(count == 400)
        {
            acc_x_avg = acc_x_sum / count;
            acc_y_avg = acc_y_sum / count;
            acc_z_avg = acc_z_sum / count;
            gyro_x_avg = gyro_x_sum / count;
            gyro_y_avg = gyro_y_sum / count;
            gyro_z_avg = gyro_z_sum / count;
            break;
        }
        HAL_Delay(1);
    }
}

void Int_MPU6050_ReadDateByCalibration(ACCEL_t *acc, GYRO_t *gyro)
{
    Int_MPU6050_ReadDataRAW(acc, gyro);
    acc->x -= acc_x_avg;
    acc->y -= acc_y_avg;
    acc->z -= acc_z_avg;
    acc->z +=16384;
    gyro->x -= gyro_x_avg;
    gyro->y -= gyro_y_avg;
    gyro->z -= gyro_z_avg;
    //printf("%d, %d, %d\n", acc->x, acc->y, acc->z);
    //printf("%d, %d, %d\n", gyro->x, gyro->y, gyro->z);
}

