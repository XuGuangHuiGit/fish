#include "cmd.h"
#include "stm32f10x_usart.h"
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "misc.h"
#include "USART.h"
#include "main.h"
#include "TIM.h"
#include "gpio.h"
#include "stepMotor.h"

#define DEBUG

extern u8 Run_Flag;
u8 doingWhat = 0;

extern float SALT_MAX , SALT_MIN;
extern float TEMP_MAX , TEMP_MIN;


static cmd_struct cmd_tbl[] = {
  CMD_ADD("help","Print all command and usage",cmd_help_func),

	CMD_ADD("reboot","reboot",cmd_reboot),
	
	CMD_ADD("run" , "feed fish" , cmd_run),
	CMD_ADD("stop" , "feed fish" , cmd_stop),
	CMD_ADD("home" , "go home" , cmd_home),
	CMD_ADD("fish" , "feed fish" , cmd_fish),
	CMD_ADD("mot" , "test" , cmd_motor),
	CMD_ADD("setSalt" , "set Salt" , cmd_setSalt),
	CMD_ADD("setTemp" , "set Temp" , cmd_setTemp),
	CMD_ADD("get" , "get param" , cmd_get),
};

void cmd_get(int argc,char *argv[]){
	uprintf(USART1 , "param %f %f %f %f\n" , SALT_MAX , SALT_MIN , TEMP_MAX , TEMP_MIN);
#ifdef DEBUG
	uprintf(USART1 , "grt done\r\n");
#endif
}


void cmd_run(int argc,char *argv[]){
	Run_Flag = 1;
	uprintf(USART1 , "param %f %f %f %f\n" , SALT_MAX , SALT_MIN , TEMP_MAX , TEMP_MIN);
#ifdef DEBUG
	uprintf(USART1 , "set run done\r\n");
#endif
}

void cmd_stop(int argc,char *argv[]){
	Run_Flag = 0;
	
#ifdef DEBUG
	uprintf(USART1 , "set stop done\n");
#endif
}

void cmd_home(int argc,char *argv[]){
	Run_Flag = 2;
	/*a special value for Run_Flag in order to gohome*/

	
#ifdef DEBUG
	uprintf(USART1 , "set go home done\n");
#endif
}

void cmd_fish(int argc,char *argv[]){
	fishMotor(RUN);
	fishMotorRun_Flag = 2000;
#ifdef DEBUG
	uprintf(USART1 , "recieve cmd successful\n");
#endif
}

void cmd_motor(int argc,char *argv[]){
	int Y;
	int X;
	if(argc != 3){
		uprintf(USART1, "wrong param\n");
		return;
	}
	X = atoi(argv[1]);
	Y = atoi(argv[2]);
	
	setPos(X ,Y);
	
#ifdef DEBUG
	uprintf(USART1 , "set done\n");
#endif
}


void cmd_setTemp(int argc,char *argv[]){

	FLASH_Status FLASHStatus;
	u32 *c_write;
	
	if(argc != 3){
		uprintf(USART1, "wrong param\n");
		return;
	}
	TEMP_MIN = atof(argv[1]);
	TEMP_MAX = atof(argv[2]);
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	FLASHStatus= FLASH_ErasePage(0x08015000);
	if(FLASHStatus != FLASH_COMPLETE){ 
			FLASH_Lock();
	}

	c_write = (u32 *)(&(TEMP_MIN));
	FLASH_ProgramWord(0x08015000,*c_write);

	c_write = (u32 *)(&(TEMP_MAX));
	FLASH_ProgramWord(0x08015004,*c_write);
	
	c_write = (u32 *)(&(SALT_MIN));
	FLASH_ProgramWord(0x08015008,*c_write);

	c_write = (u32 *)(&(SALT_MAX));
	FLASH_ProgramWord(0x0801500C,*c_write);

	FLASH_Lock();
	//uprintf(USART1 , "param %f %f %f %f\n" , SALT_MAX , SALT_MIN , TEMP_MAX , TEMP_MIN);
}

void cmd_setSalt(int argc,char *argv[]){

	FLASH_Status FLASHStatus;
	u32 *c_write;
	
	if(argc != 3){
		uprintf(USART1, "wrong param\n");
		return;
	}
	SALT_MIN = atof(argv[1]);
	SALT_MAX = atof(argv[2]);
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
	FLASHStatus= FLASH_ErasePage(0x08015000);
	if(FLASHStatus != FLASH_COMPLETE){ 
			FLASH_Lock();
	}

	c_write = (u32 *)(&(TEMP_MIN));
	FLASH_ProgramWord(0x08015000,*c_write);

	c_write = (u32 *)(&(TEMP_MAX));
	FLASH_ProgramWord(0x08015004,*c_write);
	
	c_write = (u32 *)(&(SALT_MIN));
	FLASH_ProgramWord(0x08015008,*c_write);

	c_write = (u32 *)(&(SALT_MAX));
	FLASH_ProgramWord(0x0801500C,*c_write);

	FLASH_Lock();
	
}


static char cmd_line[MAX_CMD_LINE_LENGTH + 1];
static char *cmd_argv[MAX_ARGC];

void cmd_init(){
    int i;
	
#if CMD_PERIPH_INIT_EN == 1
    USART_InitTypeDef usart_init_stru;
    NVIC_InitTypeDef nvic_init_stru;
#endif
    for(i = 0;i < MAX_ARGC;i++){
        cmd_argv[i] = (char *)malloc(MAX_CMD_ARG_LENGTH + 1);
    }
}



int cmd_parse(char *cmd_line,int *argc,char *argv[]){
    char c_temp;
    int i = 0,arg_index = 0;
    int arg_cnt = 0;
    c_temp = cmd_line[i++];  
    while(c_temp != '\r'){
        if(c_temp == ' '){
            if(arg_index == 0){   //如果命令或者参数字符串第一个是空格，则忽略   
                c_temp = cmd_line[i++];
                continue;
            }
            //空格为参数或者命令的分隔符
            if(arg_cnt == MAX_ARGC){   //如果参数个数过多,则返回
                return -1;
            }
            argv[arg_cnt][arg_index] = 0;
            arg_cnt++;//字数
            arg_index = 0;
            c_temp = cmd_line[i++];
            continue;
        }
        if(arg_index == MAX_CMD_ARG_LENGTH){   //如果参数长度过长，则报错返回
            return -2;
        }
        argv[arg_cnt][arg_index++] = c_temp;
        c_temp = cmd_line[i++];
    }
    if(arg_cnt == 0 && arg_index == 0){  //如果命令或者参数是空的，则返回
        return -3;
    }
    //最后一个参数的结束没有在上面的while循环中解析到
    argv[arg_cnt++][arg_index] = 0;
    *argc = arg_cnt;
    return 0;
}

int cmd_exec(int argc,char *argv[]){
    int cmd_index = 0;
    u32 cmd_num;
 
    cmd_num = sizeof(cmd_tbl)/sizeof(cmd_tbl[0]);

    if(argc == 0){  //如果参数是空的，则返回
        return -1;
    }
    for(cmd_index = 0;cmd_index < cmd_num;cmd_index++){   //查找命令
        if(strcmp((char *)(cmd_tbl[cmd_index].cmd_name),(char *)argv[0]) == 0){  //如果找到了命令，则执行命令相对应的函数
            uprintf(CMD_USARTx,"\n");
            cmd_tbl[cmd_index].cmd_func(argc,argv);
            return 0;
        }
    }
    return -2;
}


#if CMD_USARTn == 1
void USART1_IRQHandler(void){
#elif CMD_USARTn == 2
void USART2_IRQHandler(void){
#endif 
    static u32 cmd_line_index = 0,cmd_line_length = 0;
    int cmd_argc,i;
    int erro_n;
    u8 c_recv;

    if(USART_GetITStatus(CMD_USARTx,USART_IT_RXNE) != RESET){      
        USART_ClearITPendingBit(CMD_USARTx,USART_IT_RXNE);
        c_recv = USART_ReceiveData(CMD_USARTx);			  //< = '0x5B'  ESC = 0x1B
        if(c_recv == '\n'){  //接受完一次指令
			//uprintf(CMD_USARTx,"done");
            if(cmd_line_index == 0){
                return;
            }
            cmd_line[cmd_line_length++] = (char)c_recv;
            erro_n = cmd_parse(cmd_line,&cmd_argc,cmd_argv);  //解析命令
            if(erro_n < 0){
                //打印函数执行错误信息
                if(erro_n == -3){
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH);
                return;
                }else if(erro_n == -2){
                    uprintf(CMD_USARTx,"\nthe param is too long\n");
                }else if(erro_n == -1){
                    uprintf(CMD_USARTx,"\ntoo many param\n");
                }
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
                return;
            }
            erro_n = cmd_exec(cmd_argc,cmd_argv);   //执行命令
            if(erro_n < 0){
                //打印函数执行错误信息
                if(erro_n == -2){
                  	  uprintf(CMD_USARTx,"\r\nnot find commmand:%s\n",cmd_argv[0]);
                }
                cmd_line_index = 0;
                cmd_line_length = 0;
                memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
                return;
            }
            cmd_line_index = 0;
            cmd_line_length = 0;
            memset(cmd_line,0,MAX_CMD_LINE_LENGTH + 1);
        }else{
            if(cmd_line_index == MAX_CMD_LINE_LENGTH){
                //打印命令行太长的信息
                cmd_line_index = 0;
                cmd_line_length = 0;
                return;
            }
            for(i = 0;i < cmd_line_length - cmd_line_index;i++){
                cmd_line[cmd_line_length - i] = cmd_line[cmd_line_length - i -1];
            }
            cmd_line[cmd_line_index] = (char)c_recv;
			cmd_line_index++;
            cmd_line_length++;
			//uprintf(CMD_USARTx,"%c",c_recv);
        }			
    }
}

void cmd_help_func(int argc,char *argv[]){
    int i;
    u32 cmd_num;
    cmd_num = sizeof(cmd_tbl)/sizeof(cmd_tbl[0]);
    if(argc > 1){
        uprintf(CMD_USARTx,"error\n");		
        return;			
    }
    for(i = 0;i < cmd_num;i++){  
        uprintf(CMD_USARTx,"cmd:%s   usage:%s\n",cmd_tbl[i].cmd_name,cmd_tbl[i].cmd_usage);
    }
}

void cmd_reboot(int argc,char *argv[]){
    __disable_fault_irq();   
    NVIC_SystemReset();
}


