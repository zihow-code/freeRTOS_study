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

int t=100;         //��ƽ����ʱ����
float WAVE_DATA[1024];

int main(void)
{
    HAL_Init();                     //��ʼ��HAL��   
    Stm32_Clock_Init(360,25,2,8);   //����ʱ��,180Mhz
	delay_init(180);                //��ʼ����ʱ����
    LED_Init();                     //��ʼ��LED 
	KEY_Init();
    uart_init(115200);              //��ʼ������
	DAC1_Init();
    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //���������������
    xTaskCreate((TaskFunction_t )KEY_task,     	
                (const char*    )"KEY_task",   	
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEY_TASK_PRIO,	
                (TaskHandle_t*  )&KEYTask_Handler);   
    //������������
    xTaskCreate((TaskFunction_t )usart_task,     
                (const char*    )"usart_task",   
                (uint16_t       )usart_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )usart_TASK_PRIO,
                (TaskHandle_t*  )&usartTask_Handler);        
    //����DAC������������
    xTaskCreate((TaskFunction_t )DAC_task,     
                (const char*    )"float_task",   
                (uint16_t       )DAC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )DAC_TASK_PRIO,
                (TaskHandle_t*  )&DACTask_Handler);  
	//����ADC��Ⲩ������
    xTaskCreate((TaskFunction_t )ADC_task,     
                (const char*    )"float_task",   
                (uint16_t       )ADC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )ADC_TASK_PRIO,
                (TaskHandle_t*  )&ADCTask_Handler); 
	vTaskSuspend(ADCTask_Handler);  //������ADC�ɼ������ֱ�ӹ��𣬷�ֹ�����ڴ�����
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//������������� 
void KEY_task(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key=KEY_Scan(0);
		switch(key)
		{				 
			case  WKUP_PRES:	//����LED0,LED1�������
						LED1=!LED1;
						LED0=!LED1;
						break;
			case  KEY2_PRES:	//����LED0��ת
						LED0=!LED0;
						break;
			case  KEY1_PRES:	
						printf("����1�Ѱ���\n");
						vTaskResume(ADCTask_Handler);	//�ָ�ADC�ɼ�����
						break;
			case  KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
						LED0=!LED0;
						LED1=!LED1;
						break;
		}
    }
}   

//����������
void usart_task(void *pvParameters)
{
	int len, i;
    while(1)
    {
        if(USART_RX_STA&0x8000)//���ڽ��յ�����
		{
			t = 0;
			len=USART_RX_STA&0x3fff;
			t = USART_RX_BUF[0]-'0';
			for(i=1; i<len; i++)//�����ڽ��յ����ַ�תΪ10������
			{
				t *= 10;
				t += USART_RX_BUF[i]-'0';
			}
		}
		while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
		USART_RX_STA=0;
		vTaskDelay(100);
    }
}

//DAC������������
void DAC_task(void *pvParameters)
{
	u16 dacval = 0;//dac��ѹֵ
	
	HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);//��ʼֵΪ0
	while(1)
	{
		
		LCD_ShowNum(100, 100, t, 5, 16);
		dacval = 0;
		HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//����DACֵΪ0
        vTaskDelay(t);
		dacval = 3000;
		HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dacval);//����DACֵΪ3000
		vTaskDelay(t);
	}
}

//ADC�ɼ�����
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
			temp=(float)adcx*(3.3/4096);        //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
			WAVE_DATA[i]=temp;					//���ɼ���ɵĵ�ѹֵ��������WAVE_DATA
		}
		vTaskSuspend(ADCTask_Handler);			//���ɼ���ɺ�ֱ�ӹ���
	}
}

