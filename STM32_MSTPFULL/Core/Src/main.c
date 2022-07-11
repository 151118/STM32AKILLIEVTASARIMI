/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#define ADXL345_I2C_SLAVE_ADDRESS 0xA6 //0x53 << 1
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char rx_buffer[50];
int tx_buffer[50];
int tem=0;
int hum=0;
uint32_t echo1;
uint32_t delayy;
float dis1 ;
int limit=10;
int count=0;
uint16_t adc_value;
float Vadc;
uint16_t adcc_value;
float Vadcc;
//char *rx[50];
//char rxx_buffer[50],txx_buffer[50];
bool led_state = false;

uint8_t i;
uint8_t myDatas[6];
int16_t x , y , z;
float xG , yG, zG;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM11_Init(void);
static void MX_TIM8_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void Scan_I2C_Address();
void ADX345_Init();
void ADXL345_Write(uint8_t wRegister,uint8_t value);
void ADXL345_Read(uint8_t rRegister, uint8_t numberOfBytes);
void Sistem_ON();
void Sistem_OFF();
char rx[50];
void delay(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim6,0);
	while(__HAL_TIM_GET_COUNTER(&htim6)< time);
}

void Set_Pin_Output(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef DHT11_DATA={0};

	DHT11_DATA.Pin=GPIO_Pin;
	DHT11_DATA.Mode=GPIO_MODE_OUTPUT_PP;
	DHT11_DATA.Pull=GPIO_NOPULL;
	DHT11_DATA.Speed=GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOx, &DHT11_DATA);
}
void Set_Pin_Input(GPIO_TypeDef *GPIOx,uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef DHT11_DATA={0};

	DHT11_DATA.Pin=GPIO_Pin;
	DHT11_DATA.Mode=GPIO_MODE_INPUT;
	DHT11_DATA.Pull=GPIO_NOPULL;
	DHT11_DATA.Speed=GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOx, &DHT11_DATA);
}
#define DHT11_PORT GPIOA
#define DHT11_PIN GPIO_PIN_4

uint8_t durum=0,Humidity=0,Temperature=0;
uint16_t tempVal=0,humVal=0;
uint8_t dhtval[2];
char sendData[128];
uint8_t mData[40];
uint16_t mTime1 = 0,mTime2 = 0;
uint16_t mbit=0;
uint8_t parityVal=0, genParity=0;
int a = 15;
uint8_t DHT11_Read(void)
{
	for(int a=0;a<40;a++) mData[a]=0;

		mTime1=0, mTime2=0, durum=0, tempVal=0, humVal=0, parityVal=0, genParity=0, mbit=0;
		Set_Pin_Output(DHT11_PORT, DHT11_PIN);
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
		delay(18000);
		Set_Pin_Input(DHT11_PORT, DHT11_PIN);

		__HAL_TIM_SET_COUNTER(&htim6,0);
		while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)==GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim6)>500) return 0;

		__HAL_TIM_SET_COUNTER(&htim6,0);
	    while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)==GPIO_PIN_RESET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim6)>500) return 0;
	    mTime1 =(uint16_t)__HAL_TIM_GET_COUNTER(&htim6);

	    __HAL_TIM_SET_COUNTER(&htim6,0);
	    while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)==GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim6)>500) return 0;
	    mTime2 =(uint16_t)__HAL_TIM_GET_COUNTER(&htim6);

	    if(mTime1 < 75 && mTime1 > 85 && mTime2 < 75 && mTime2 > 85)
	    {

	    	return 0;
	    }
	    for(int j=0; j<40; j++)
	    {
	    	__HAL_TIM_SET_COUNTER(&htim6,0);
	    	while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)==GPIO_PIN_RESET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim6)>500) return 0;
	    	__HAL_TIM_SET_COUNTER(&htim6,0);
	    	while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)==GPIO_PIN_SET) if((uint16_t)__HAL_TIM_GET_COUNTER(&htim6)>500) return 0;
	    	mTime1=(uint16_t)__HAL_TIM_GET_COUNTER(&htim6);
	    	if(mTime1 >20 && mTime1<30)
	    	{
	    		mbit=0;
	    	}
	    	else if(mTime1>60 && mTime1<80)
	    	{
	    		mbit=1;
	    	}
	    	mData[j]=mbit;
	    }
	    for(int i=0;i <8;i++)
	    {
	    	humVal +=mData[i];
	    	humVal=humVal<<1;
	    }
	    for(int i=16;i <24;i++)
	   	    {
	   	    	tempVal +=mData[i];
	   	    	tempVal=tempVal<<1;
	   	    }
	    for(int i=32;i <40;i++)
	   	    {
	   	    	parityVal +=mData[i];
	   	    	parityVal=parityVal<<1;
	   	    }
	    parityVal=parityVal >>1;
	    humVal=humVal >>1;
	    tempVal=tempVal >>1;
	    genParity=humVal+tempVal;
	    dhtval[0]=tempVal;
	    dhtval[1]=humVal;
	    return 1;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Read_ADC()
{
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 100000)==HAL_OK)
	adc_value=HAL_ADC_GetValue(&hadc1);
	Vadc=5.5*adc_value/4095;
}
void Readd_ADC()
{
	HAL_ADC_Start(&hadc2);
	if(HAL_ADC_PollForConversion(&hadc2, 100000)==HAL_OK)
	adcc_value=HAL_ADC_GetValue(&hadc2);
	Vadcc=5.5*adcc_value/4095;
}

void Deprem_ON()
{
    if (zG<= 1.000)
	 {
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		     HAL_Delay(100);
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
		     HAL_Delay(360);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
		     HAL_Delay(720);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
		     HAL_Delay(360);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);

	 }
    else if(zG> 1.00)
     {
		     HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12, GPIO_PIN_RESET);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
     }
}
void Deprem_OFF()
{
	 if (zG<= 1.000)
     {
             HAL_GPIO_WritePin(GPIOD,   GPIO_PIN_12, GPIO_PIN_RESET);
             HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
     }
     else
     {
             HAL_GPIO_WritePin(GPIOD,   GPIO_PIN_12, GPIO_PIN_RESET);
             HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
     }
}
void Ldr_ON()
{
	 if(Vadc<2.7)
     {
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);


	 }
    if(Vadc>2.7)
    {
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
		     HAL_Delay(20);
    }
}
void Ldr_OFF()
{
    if(Vadc<2.7)
   	{
   		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
   	         HAL_Delay(20);
   }
 	  if(Vadc>2.7)
      {
 	         HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
		     HAL_Delay(20);
     }
}
void Hareket_ON()
{
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET)
	 {
		   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
		   HAL_Delay(360);
	       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
		   HAL_Delay(720);
		   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
		   HAL_Delay(360);
		   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
	}
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_RESET)
    {
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}
void Hareket_OFF()
{
	 if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_RESET)
     {
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
     }
	 else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_8) == GPIO_PIN_SET);
	{
		    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
	}
}
void Gaz_ON()
{
	if(adcc_value>2000)
   {
		     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
			 HAL_Delay(20);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
		     HAL_Delay(360);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
		     HAL_Delay(720);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
		     HAL_Delay(360);
		     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
	}
	else
   {

		    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
    }
}
void Gaz_OFF()
{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
}
void Fire_ON()
{
	//yangın sensörü
if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)==GPIO_PIN_RESET)
	     {
		    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
		    HAL_Delay(360);
		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
		    HAL_Delay(720);
		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_SET);
	        HAL_Delay(360);
		    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);

		 }
if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_7)==GPIO_PIN_SET)
    {
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
     }
}
void Fire_OFF()
{  //YANGIN SENSÖRÜ
	       HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	       HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12, GPIO_PIN_RESET);
}
void Garaj_ON()
{
	 if(limit>=dis1)
   {
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,500);
    HAL_Delay(100);
   __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,1500); //MOTOR HCSR
   HAL_Delay(200);
   }
	 else
    {
       __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,1500);
	  HAL_Delay(100);
	  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,500);
  }
}
void Garaj_OFF()
{
	 if(limit>=dis1)
  {
   __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,1500);
   HAL_Delay(100);
  }
	 else
   {
      __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,1500);
	  HAL_Delay(100);
 }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_TIM11_Init();
  MX_TIM8_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_ADC2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  Scan_I2C_Address();
  ADX345_Init();
  HAL_TIM_Base_Start(&htim6);
  HAL_TIM_Base_Start(&htim8);
  HAL_TIM_Base_Start(&htim11);
  HAL_TIM_PWM_Start(&htim11,  TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4,  TIM_CHANNEL_3);
  HAL_TIM_IC_Start(&htim8, TIM_CHANNEL_1);

  TIM11->CCR1=3;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  Read_ADC();
	  Readd_ADC();
	  ADXL345_Read(0x32, 6);
	  x= (myDatas[1] << 8) | myDatas[0] ; // x value in myDatas[0] myDatas[1];
	  y= (myDatas[3] << 8) | myDatas[2] ; // x value in myDatas[2] myDatas[3];
	  z= (myDatas[5] << 8) | myDatas[4] ; // x value in myDatas[5] myDatas[4];
	  xG=x* .0087;
	  yG=y* .0087;
	  zG=z* .0087;
	  HAL_Delay(10);
	  HAL_UART_Receive(&huart2, (uint8_t*)rx_buffer, 50, 100);
	  durum=DHT11_Read();
			  if(durum==1)
			  {
				  Temperature=tempVal;
				  Humidity=humVal;
				  hum=humVal;
				  tem=tempVal;
			  }
			    if(durum==1)
			 {
				    uint16_t len = sprintf(sendData, "Nem Değeri: %d\n Sıcaklık: %d\n\r", dhtval[1], dhtval[0]);
				     //send data with UART
				     HAL_UART_Transmit(&huart2, (uint8_t*)sendData, len, 100);
				     //    HAL_Delay(1000);
			   }
			  if(HAL_GetTick()-delayy >= 100)//hcsr
		      {
				  delayy=HAL_GetTick();
				  echo1=HAL_TIM_ReadCapturedValue(&htim8, TIM_CHANNEL_1);
				  dis1=echo1/58.0f;
		     }
			  HAL_Delay(1000);
	       HAL_Delay(1000);
			  if(rx_buffer[0]=='w')
			  {
			 	 Sistem_ON();
			  }
			  if(rx_buffer[0]=='z')
			  {
				  Sistem_OFF();
			  }
			  if(rx_buffer[0]=='a')//ldac
			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
				   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5, GPIO_PIN_SET);
				   HAL_Delay(1000);
			  }
			  if(rx_buffer[0]=='b')//leadkapa
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
				   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5, GPIO_PIN_RESET);
				   HAL_Delay(1000);
			 			  }

			  if(rx_buffer[0]=='o')//motoracPENCERE
			 	  {
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);
							   HAL_Delay(200);
							 //  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,500);
								HAL_Delay(200);
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				  Gaz_ON();
				   Garaj_ON();

			 		  }
			  if(rx_buffer[0]=='c')//motorkapaPENCERE
			 			  {
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,500);
									HAL_Delay(200);
							//  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);
									HAL_Delay(200);
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();

			 			  }
			  if(rx_buffer[0]=='d')//ldr ac
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }

			  if(rx_buffer[0]=='e')//ldrkapa
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_OFF();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='f')//pırac
			 			  {
				   Fire_OFF();
				   Deprem_OFF();
				   Ldr_OFF();
				   Hareket_ON();
				   Gaz_OFF();
				   Garaj_OFF();
			 			  }
			  if(rx_buffer[0]=='g')//pırkapa
			 			  {
				   Fire_OFF();
				   Deprem_OFF();
				   Ldr_OFF();
				   Hareket_OFF();
				   Gaz_OFF();
				   Garaj_OFF();
			 			  }
			  if(rx_buffer[0]=='h')//gazon
			 	 		  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='j')//gazof
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_OFF();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='k')//yangınac
						   {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
						   }
			  if(rx_buffer[0]=='l')//yangın kapat
			 			  {
				   Fire_OFF();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='m')//depremac
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='n')//depremkapa
			 			  {
				   Fire_ON();
				   Deprem_OFF();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='p')//garaj on
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_ON();
			 			  }
			  if(rx_buffer[0]=='r')//garakof
			 			  {
				   Fire_ON();
				   Deprem_ON();
				   Ldr_ON();
				   Hareket_OFF();
				   Gaz_ON();
				   Garaj_OFF();
			 			  }
			  if(rx_buffer[0]=='s')//kapıac
			 			  {
				  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);
				 				   HAL_Delay(200);
				 				  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,500);
				 				   HAL_Delay(200);
				  // Fire_ON();
				//   Deprem_ON();
				 //  Ldr_ON();
				 //  Hareket_OFF();
				 //  Gaz_ON();
				 //  Garaj_ON();

			 			  }
			  if(rx_buffer[0]=='t')//kapıkapa
			 			  {
				   __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,500);
							   HAL_Delay(200);
								   __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,1500);
							   HAL_Delay(200);
				//   Fire_ON();
				//   Deprem_ON();
				 //  Ldr_ON();
				//   Hareket_OFF();
				 //  Gaz_ON();
				 //  Garaj_ON();

			 			  }
			  if(rx_buffer[0]=='y')
			  {
				  Fire_ON();
				  Gaz_ON();
				  Hareket_ON();
			  }
	                if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4))
	             		{
	                     while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4))
	                       HAL_Delay(100);
	                      count++;
	            		}
	                 if(count%2==1)
	                  {
	                 	  	 HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5, GPIO_PIN_SET);
	                   }
	                 else if(count%2==0)
	                 {
	                 	     HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5, GPIO_PIN_RESET);
	                  }

   }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 83;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 19999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 83;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 19999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 84-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xffff-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 83;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim8, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 209;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 39999;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim11, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */
  HAL_TIM_MspPostInit(&htim11);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE5 PE12 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD10 PD12 PD13 PD14
                           PD15 PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void Scan_I2C_Address()
{
	for(int i=0;i<=255;i++)
	{
		if(HAL_I2C_IsDeviceReady(&hi2c1, i, 1, 10)==HAL_OK)
		break;
	}
}
void ADXL345_Write(uint8_t wRegister,uint8_t value)
{
	uint8_t data[2] = {0};
	data[0]=wRegister;
	data[1]=value;
	HAL_I2C_Master_Transmit(&hi2c1, ADXL345_I2C_SLAVE_ADDRESS, data, 2, 100);
}
void ADXL345_Read(uint8_t rRegister,uint8_t numberOfBytes)
{
	HAL_I2C_Mem_Read(&hi2c1, ADXL345_I2C_SLAVE_ADDRESS, rRegister, 1, myDatas, numberOfBytes, 100);
}
void ADX345_Init()
{
	ADXL345_Read(0x00, 1);//if the read data 0xe5 ,device is read
	ADXL345_Write(0x2D, 0);//reset  powe controlls ALL bits
	//SetUp->
	ADXL345_Write(0x2D, 0x08);//measure bit1, wake up bit 0, autosleep 0,sleep 0,wake up frequence 8
    ADXL345_Write(0x31, 0x01);//-+ 4g range
}


void Sistem_ON()
{
   Fire_ON();
   Deprem_ON();
   Ldr_ON();
   Hareket_OFF();
   Gaz_ON();
   Garaj_ON();


}
void Sistem_OFF()
{
	Gaz_OFF();
	Hareket_OFF();
	Ldr_OFF();
	Deprem_OFF();
	Fire_OFF();
	Garaj_OFF();


}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
