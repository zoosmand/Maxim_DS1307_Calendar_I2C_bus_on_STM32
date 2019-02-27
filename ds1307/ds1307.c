#include "ds1307.h"

const uint8_t WordSize = 8;

bool Init_I2C_DS1307(void)
{
  GPIO_InitTypeDef i2c_ec;
  // I2C1 SCL
  i2c_ec.GPIO_Pin = GPIO_Pin_6;
  i2c_ec.GPIO_Mode = GPIO_Mode_AF_OD;
  i2c_ec.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &i2c_ec);
  // I2C1 SDA
  i2c_ec.GPIO_Pin = GPIO_Pin_7;
  i2c_ec.GPIO_Mode = GPIO_Mode_AF_OD;
  i2c_ec.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &i2c_ec);
  // I2C Transport
  I2C_InitTypeDef i2c_tr;
  i2c_tr.I2C_ClockSpeed = 100000;
  i2c_tr.I2C_Mode = I2C_Mode_I2C;
  i2c_tr.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_tr.I2C_OwnAddress1 = 0;
  i2c_tr.I2C_Ack = I2C_Ack_Disable;
  i2c_tr.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C1, &i2c_tr);

  return true;
}


void DS1307_Init(uint8_t* __params)
{
  uint8_t ch = 0;
  // To confugure clock set _CH_ bit on address 0x00
  DS1307_Read(0x00, 1, &ch);
  if(ch & _CH_)
  {
    DS1307_Write(0x00, 8, __params);
  }
  // Store init parameters into backup registers (just in case)
  DS1307_Write(0x08, 8, __params);
}





void DS1307_Read(uint8_t __address, uint8_t __words, uint8_t* __buffer)
{
  if ((__address + __words) < _CAP_)
  {
    I2C_Cmd(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    // ---------------------------------------------
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
   
    I2C_Send7bitAddress(I2C1, _ADDR_, I2C_Direction_Transmitter);
    while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
    if ((I2C_ReadRegister(I2C1, I2C_Register_SR2) & I2C_SR2_MSL))
    {
      I2C_SendData(I2C1, __address & 0xff);
      while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_TXE));
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    
    Delay(10);  
    
    // ---------------------------------------------
    if (__words)
    {
      I2C_GenerateSTART(I2C1, ENABLE);
      while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
     
      I2C_Send7bitAddress(I2C1, _ADDR_, I2C_Direction_Receiver);
      while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
      
      if ((I2C_ReadRegister(I2C1, I2C_Register_SR2) & I2C_SR2_MSL))
      {
        for (int i = 0; i < __words; i++)
        {
          while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_RXNE));
          if (i == (__words - 1))
          {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
          }
          __buffer[i] = I2C_ReceiveData(I2C1);
        }
      }
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    
    I2C_Cmd(I2C1, DISABLE);
  }
}





void DS1307_Write(uint8_t __address, uint8_t __words, uint8_t* __buffer)
{
  if ((__address + __words) < _CAP_)
  {
    I2C_Cmd(I2C1, ENABLE);

    // ---------------------------------------------
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
   
    I2C_Send7bitAddress(I2C1, _ADDR_, I2C_Direction_Transmitter);
    while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
    
    if ((I2C_ReadRegister(I2C1, I2C_Register_SR2) & I2C_SR2_MSL))
    {
      I2C_SendData(I2C1, __address & 0xff);
      while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_TXE));

      if (__words)
      {
        for (int i = 0; i < __words; i++)
        {
          I2C_SendData(I2C1, __buffer[i]);
          while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_TXE));
        }
      }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);

    Delay(10);

    I2C_Cmd(I2C1, DISABLE);
  }
}
