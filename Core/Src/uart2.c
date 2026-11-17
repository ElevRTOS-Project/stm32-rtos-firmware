#include "uart2.h"
#include <string.h>   // strcpy strncmp
#define  COMMAND_NUMER  20
#define  COMMAND_LENGTH 40
#define  MESSAGE_NUMBER 2

extern void led_all_on(void);
extern void led_all_off(void);
extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;
extern void (*funcp[])();
extern void set_rtc(char *date_time);

uint8_t pdht11=1;  // default print dht11
uint8_t pdatetime=1;  // default print date&time

volatile uint8_t rx_buffer[COMMAND_NUMER][COMMAND_LENGTH];
volatile int rear=0;
volatile int front=0;

void pc_command_processing();
void print_status(void);

const char help_cmd[][50] =
{
		"pdht11: print dht11 debug message",
		"pdatetime: print date&time message"
};

// move Drviers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c to here
// ex) led_all_on\n
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	volatile static int i=0;  // static으로하면 전역변수 처럼 동작

	if (huart == &huart2)
	{
		uint8_t data;

		data = rx_data;

		if (data == '\n')
		{
			rx_buffer[rear][i++] = '\0';   // 문장의끝을 가리키는 NULL
			i=0;
			rear++;
			rear %= COMMAND_NUMER;
			// queue full 체크 하는 logic이 들어 가야 한다.
		}
		else
		{
			rx_buffer[rear][i++] = data;
		}
		HAL_UART_Receive_IT(&huart2, &rx_data, 1); // 반드시 집어넣어야 한다.
	}
}

void pc_command_processing()
{
	static int func_index=-1;

	if (front != rear)
	{
		printf("%s\n", rx_buffer[front]);  // &rx_buffer[front][0]
		if (strncmp(rx_buffer[front], "led_all_on", strlen("led_all_on"))==0)
		{
			func_index=0;
		}
		else if (strncmp(rx_buffer[front], "led_all_off", strlen("led_all_off"))==0)
		{
			func_index=1;
		}
		else if (strncmp(rx_buffer[front], "led_up_on", strlen("led_up_on"))==0)
		{
			func_index=2;
		}
		else if (strncmp(rx_buffer[front], "led_down_on", strlen("led_down_on"))==0)
		{
			func_index=3;
		}
		else if (strncmp(rx_buffer[front], "led_flower_on", strlen("led_flower_on"))==0)
		{
			func_index=4;
		}
		else if (strncmp(rx_buffer[front], "led_flower_off", strlen("led_flower_off"))==0)
		{
			func_index=5;  // led_flower_off
		}
		else if (strncmp(rx_buffer[front], "led_keepon_up", strlen("led_keepon_up"))==0)
		{
			func_index=6;
		}
		else if (strncmp(rx_buffer[front], "led_keepon_down", strlen("led_keepon_down"))==0)
		{
			func_index=7;
		}
		else if (strncmp(rx_buffer[front], "setrtc", strlen("setrtc"))==0)
		{
			set_rtc(&rx_buffer[front][6]);  // setrtc241008154500
		}
		else if (strncmp(rx_buffer[front], "pdatetime", strlen("pdatetime"))==0)
		{
			pdatetime = !pdatetime;
		}
		else if (strncmp(rx_buffer[front], "pdht11", strlen("pdht11"))==0)
		{
			pdht11 = !pdht11;
		}
		else if (strncmp(rx_buffer[front], "help", strlen("help"))==0)
		{
			print_help();
		}
		else if (strncmp(rx_buffer[front], "status", strlen("status"))==0)
		{
			print_status();
		}
		// pdatetime
		front++;
		front %= COMMAND_NUMER;
		// queue full check !!!!
//		if (func_index != -1)
//			funcp[func_index]();
	}
}

void print_help(void)
{
	for (int i=0; i < MESSAGE_NUMBER; i++)
		printf("%s\n", help_cmd[i]);  // help_cmd[i] ==> &help_cmd[i][0]
}

void print_status(void)
{
	if (pdht11)
		printf("pdht11 : ON\n");
	else printf("pdht11 : OFF\n");

	if (pdatetime)
		printf("pdatetime: ON\n");
	else printf("pdatetime: OFF\n");
}
