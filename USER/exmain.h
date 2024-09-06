#include "FreeRTOS.h"
#include "task.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO		2
//�����ջ��С	
#define KEY_STK_SIZE 		50  
//������
TaskHandle_t KEYTask_Handler;
//������
void KEY_task(void *pvParameters);

//�������ȼ�
#define usart_TASK_PRIO		3
//�����ջ��С	
#define usart_STK_SIZE 		50  
//������
TaskHandle_t usartTask_Handler;
//������
void usart_task(void *pvParameters);

//�������ȼ�
#define DAC_TASK_PRIO		4
//�����ջ��С	
#define DAC_STK_SIZE 		128
//������
TaskHandle_t DACTask_Handler;
//������
void DAC_task(void *pvParameters);

//�������ȼ�
#define ADC_TASK_PRIO		5
//�����ջ��С	
#define ADC_STK_SIZE 		128
//������
TaskHandle_t ADCTask_Handler;
//������
void ADC_task(void *pvParameters);
