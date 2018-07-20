/*******************************************************************************
PIC16LF18854 BM1682 CORE BOARD
POWER SEQUENCE CODE 
Author: Patrick Chen(CPF) 
BITMAIN 
DATE: 05/05/2018
/****************************************************************/
#include"User_define.h"
#include"xc.h"
int status = STATUS_POWERDOWN;

void Reset(void)
{
	SYS_RST = 0;
	delayms(20);
	SYS_RST = 1;
}
//3.3V EN��Ҫ��30ms������EN ISL68127
void Power_Up(void)
{
    SYS_RST = 0;
    EN_B_3V3 = 1;
    delayms(40);//May 15 update,fulfill the 68127's spec
    EN0_ISL68127 = 0;
    delayms(5);
    EN1_ISL68127 = 0;
    delayms(5);
    EN_VDD_IO_1V8 = 1;
    delayms(5);
    EN_RGMII_3V3 = 1;
    delayms(5);
    SYS_RST = 1;
	delayms(5);
	Reset();
    LED0 = 1;
	status = STATUS_POWERUP;
}

void Power_Down(void)
{
	status = STATUS_POWERDOWN;
    LED0 = 0;
    SYS_RST = 0;
    delayms(5);    
    EN_RGMII_3V3 = 0;
    delayms(5);
    EN_VDD_IO_1V8 = 0;
    delayms(5);
    EN1_ISL68127 = 1;
    delayms(5);
    EN0_ISL68127 = 1;
    delayms(5);
    //EN_B_3V3 = 0;

}