#include "stm32f10x.h"
/****************************以下是使用代码之前，必须配置的宏*******************************/
#define CMD_USARTn 1    //接收命令的串口号
//自动初始化外设使能,如果使能了，则调用cmd_init()函数之后，即可初始化好串口和中断
#define CMD_PERIPH_INIT_EN 0

//如果没有使能自动初始化，则无需配置下面的宏了
#if CMD_PERIPH_INIT_EN == 1
    #define CMD_NVIC_GROUP NVIC_PriorityGroup_1               //中断组别 
    #define CMD_NVIC_IRQPP 0              //中断抢断优先级
    #define CMD_NVIC_IRQSP 0              //中断子优先级
    #define CMD_USART_BAUD 115200         //串口波特率
#endif



#if CMD_USARTn == 1
    #define CMD_USARTx USART1 
#elif CMD_USARTn == 2
    #define CMD_USARTx USART2 
#endif


#define MAX_CMD_ARG_LENGTH 9
#define MAX_CMD_INFO_LENGTH 32
#define MAX_CMD_LINE_LENGTH 32
#define MAX_CMD_LINE_CNT 5
#define MAX_ARGC 3
#define STR_BUFFER_LEN 32


typedef struct {
    char cmd_name[MAX_CMD_ARG_LENGTH];   //命令的名字
    char cmd_usage[MAX_CMD_INFO_LENGTH];   //命令的信息
    void (*cmd_func)(int acgc,char *argv[]); //命令执行函数
}cmd_struct;


#define CMD_ADD(cmd_name,cmd_usage,cmd_func) \
    {\
        cmd_name,\
        cmd_usage,\
        cmd_func\
    }


extern u8 doingWhat;
	
void cmd_init(void);
int cmd_parse(char *cmd_line,int *argc,char *argv[]);  //命令行解析
int cmd_exec(int argc,char *argv[]);
void cmd_help_func(int argc,char *argv[]);
void cmd_reboot(int argc,char *argv[]);
		
void cmd_run(int argc,char *argv[]);
void cmd_stop(int argc,char *argv[]);
void cmd_home(int argc,char *argv[]);
void cmd_fish(int argc,char *argv[]);
void cmd_motor(int argc,char *argv[]);
void cmd_setSalt(int argc,char *argv[]);
void cmd_setTemp(int argc,char *argv[]);		
void cmd_get(int argc,char *argv[]);