#include <gba_systemcalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <gba_base.h>
#include <gba_input.h>

#include "ez_define.h"
#include "lang.h"
#include "ezkernel.h"
#include "RTC.h"
#include "draw.h"
#include "Ezcard_OP.h"
#include <string.h>
#include "version.h"



extern u16 gl_select_lang;
u16 Rev;
//---------------------------------------------------------------------------------
void Show_ver(void)
{
	char msg[24];
	char *ver="K:" KERNEL_KO_DISPLAY;
	u16 FPGAver = Read_FPGA_ver();
	sprintf(msg,"FW:%d %s",FPGAver&0xFF,ver);
	DrawHZText12(msg,0,240-strlen(msg)*6-2,3, gl_color_text,1);	
	Rev = FPGAver & 0xF000;
}
//---------------------------------------------------------------------------------
void Show_help_window()
{
	char msg[20];
	
	u32 linex= 15;
	
	Show_ver();
	//Branch inverted vs original: Chinese manual QR only for 0xE2E2; everything
	//else (English 0xE1E1 AND Korean 0xE3E3) gets the English manual QR,
	//because no Korean manual image exists.
	if(gl_select_lang == 0xE2E2)//chinese
	{
		DrawPic((u16*)gImage_Chinese_manual, 240-70, 160-70, 70, 70, 0, 0, 1);//
	}
	else{//english + korean
		DrawPic((u16*)gImage_English_manual, 240-70, 160-70, 70, 70, 0, 0, 1);//
	}
	DrawHZText12("L + R + START :",0,3,20+linex*0, gl_color_selected,1);
		DrawHZText12(gl_LRSTART_help,0,92,20+linex*0, gl_color_text,1);
		
	DrawHZText12("L + R + SELECT:",0,3,20+linex*1, gl_color_selected,1);
		DrawHZText12(gl_LRSELECT_help,0,92,20+linex*1, gl_color_text,1);		
		
	DrawHZText12("SELECT        :",0,3,20+linex*2, gl_color_selected,1);
		DrawHZText12(gl_SELECT_help,0,92,20+linex*2, gl_color_text,1);
		
	DrawHZText12("L + A         :",0,3,20+linex*3, gl_color_selected,1);
		DrawHZText12(gl_L_A_help,0,92,20+linex*3, gl_color_text,1);
		
	DrawHZText12("L+START       :",0,3,20+linex*4, gl_color_selected,1);
		DrawHZText12(gl_LSTART_help,0,92,20+linex*4, gl_color_text,1);	
	
	sprintf(msg,"Rev.%s","Air");
	DrawHZText12(msg,0,3,149, gl_color_text,1);	
				
	DrawHZText12(gl_online_manual,0,240-70-7,77, gl_color_text,1);
	while(1)
	{
		VBlankIntrWait(); 	
		scanKeys();
		u16 keys = keysUp();
		if (keys & KEY_L) {//return
			return;
		}	
	}
}