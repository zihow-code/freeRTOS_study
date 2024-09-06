#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "dac.h"
#include "adc.h"
#include "lcd.h"

#include "FreeRTOS.h"
#include "task.h"
#include "exmain.h"
#include "math.h"

int t=100;         //电平跳变时间间隔
float WAVE_DATA[1024];

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED 
	KEY_Init();
    uart_init(115200);              //初始化串口
	DAC1_Init();
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建按键检测任务
    xTaskCreate((TaskFunction_t )KEY_task,     	
                (const char*    )"KEY_task",   	
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEY_TASK_PRIO,	
                (TaskHandle_t*  )&KEYTask_Handler);   
    //创建串口任务
    xTaskCreate((TaskFunction_t )usart_task,     
                (const char*    )"usart_task",   
                (uint16_t       )usart_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )usart_TASK_PRIO,
                (TaskHandle_t*  )&usartTask_Handler);        
    //创建DAC产生波形任务
    xTaskCreate((TaskFunction_t )DAC_task,     
                (const char*    )"float_task",   
                (uint16_t       )DAC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DAC_TASK_PRIO,
                (TaskHandle_t*  )&DACTask_Handler);  
	//创建ADC检测波形任务
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"float_task",   
                (uint16_t       )ADC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ADC_TASK_PRIO,
                (TaskHandle_t*  )&ADCTask_Handler); 
	vTaskSuspend(ADCTask_Handler);  //创建完ADC采集任务后直接挂起，防止程序卡在此任务
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//按键检测任务函数 
void KEY_task(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key=KEY_Scan(0);
		switch(key)
		{				 
			case  WKUP_PRES:	//控制LED0,LED1互斥点亮
						LED1=!LED1;
						LED0=!LED1;
						break;
			case  KEY2_PRES:	//控制LED0翻转
						LED0=!LED0;
						break;
			case  KEY1_PRES:	
						printf("按键1已按下\n");
						vTaskResume(ADCTask_Handler);	//恢复ADC采集任务
						break;
			case  KEY0_PRES:	//同时控制LED0,LED1翻转 
						LED0=!LED0;
						LED1=!LED1;
						break;
		}
    }
}   

//串口任务函数
void usart_task(void *pvParameters)
{
	int len, i;
    while(1)
    {
        if(USART_RX_STA&0x8000)//串口接收到数据
		{
			t = 0;
			len=USART_RX_STA&0x3fff;
			t = USART_RX_BUF[0]-'0';
			for(i=1; i<len; i++)//将串口接收到的字符转为10进制数
			{
				t *= 10;
				t += USART_RX_BUF[i]-'0';
			}
		}
		while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
		USART_RX_STA=0;
		vTaskDelay(100);
    }
}

//DAC产生波形任务
void DAC_task(void *pvParameters)
{
	u16 dacval = 0;//dac电压值
	
	HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);//初始值为0
	while(1)
	{
		
		LCD_ShowNum(100, 100, t, 5, 16);
		dacval = 0;
		HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//设置DAC值为0
        vTaskDelay(t);
		dacval = 3000;
		HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//设置DAC值为3000
		vTaskDelay(t);
	}
}

//ADC采集任务
void ADC_task(void *pvParameters)
{
	u16 adcx;
	float temp;
	int i;
	while(1)
	{
		for(i=0; i<1024; i++)
		{
			adcx = Get_Adc(5);
			temp=(float)adcx*(3.3/4096);        //获取计算后的带小数的实际电压值，比如3.1111
			WAVE_DATA[i]=temp;					//将采集完成的电压值存入数组WAVE_DATA
		}
		vTaskSuspend(ADCTask_Handler);			//当采集完成后直接挂起
	}
}

