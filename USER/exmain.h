#include "FreeRTOS.h"
#include "task.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define KEY_TASK_PRIO		2
//任务堆栈大小	
#define KEY_STK_SIZE 		50  
//任务句柄
TaskHandle_t KEYTask_Handler;
//任务函数
void KEY_task(void *pvParameters);

//任务优先级
#define usart_TASK_PRIO		3
//任务堆栈大小	
#define usart_STK_SIZE 		50  
//任务句柄
TaskHandle_t usartTask_Handler;
//任务函数
void usart_task(void *pvParameters);

//任务优先级
#define DAC_TASK_PRIO		4
//任务堆栈大小	
#define DAC_STK_SIZE 		128
//任务句柄
TaskHandle_t DACTask_Handler;
//任务函数
void DAC_task(void *pvParameters);

//任务优先级
#define ADC_TASK_PRIO		5
//任务堆栈大小	
#define ADC_STK_SIZE 		128
//任务句柄
TaskHandle_t ADCTask_Handler;
//任务函数
void ADC_task(void *pvParameters);
