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

//extern const unsigned char __attribute__((aligned(4)))gImage_SET[76800];
extern u16 gl_select_lang;
//extern u16 gl_engine_sel;
extern u16 gl_show_Thumbnail;
//extern u16 gl_ingame_RTC_open_status;
extern u16 gl_auto_save_sel;
extern u16 gl_ModeB_init;


extern void CheckSwitch(void);
extern void CheckLanguage(void);
void save_setw_info(void);

u16 SET_info_buffer [0x200]EWRAM_BSS;

#define	K_A		 	 (0)
#define	K_B		 	 (1)
#define	K_SELECT (2)
#define	K_START  (3)
#define	K_RIGHT	 (4)
#define	K_LEFT	 (5)
#define	K_UP		 (6)
#define	K_DOWN	 (7)
#define	K_R		   (8)
#define	K_L		 	 (9)



u16 v_reset;

u16 v_cheat;
u16 language_sel;	


u16 auto_save_sel;
u16 ModeB_INIT;


//---------------------------------------------------------------------------------
void Draw_select_icon(u32 X,u32 Y,u32 mode)
{
	
	Clear(X+2, Y+2, 8, 8, gl_color_text, 1);
	Clear(X+3, Y+3, 6, 6, RGB(4,8,12), 1);
	if(mode)
		Clear(X+4, Y+4, 4, 4, gl_color_selected, 1);
}
//---------------------------------------------------------------------------------
u32 Setting_window(void)
{
	u16 keys;
	u32 line;
	u32 select;
	u32 Set_OK=0;
	u32 Set_OK_line=0;
	
	u8 edit_pos=0;
	//u32 i; 
	u32 currstate=0;
	
	char msg[128];
	u16 clean_color;
	u16 clean_pos;
	u16 clean_w;
	u32 re_show=1;

	u32 addon_sel=2;
	
	
	 
	u8 datetime[7];
	u8 edit_datetime[7]={0};
	

	u8 lang_pos = 1;
	u8 auto_save_pos = 1;	
	u8 ModeB_pos = 2;
		
	
	select = 0;
	u32 y_offset = 34;
	u32 set_offset = 1;
	u32 x_offset = set_offset+9*6+3;
	
	u32 line_x = 17;
	u32 line_MAX = 5;
			
	if(gl_select_lang == 0xE1E1)
	{
		language_sel = 0;
	}
	else
	{
		language_sel = 1;
	}	
	v_reset = Read_SET_info(assress_v_reset);

	v_cheat = Read_SET_info(assress_v_cheat);
	if( (v_reset != 0x0) && (v_reset != 0x1))
	{
		v_reset = 0x0;
	}
	if( (v_cheat != 0x0) && (v_cheat != 0x1))
	{
		v_cheat = 0x0;
	}
	

	//engine_sel = gl_engine_sel;
	auto_save_sel = gl_auto_save_sel;
	ModeB_INIT = gl_ModeB_init;
	
	while(1)
	{
		VBlankIntrWait(); 	
		
		if(re_show)
		{	
			//
			sprintf(msg,"%s",gl_time);
			DrawHZText12(msg,0,set_offset,y_offset,gl_color_selected,1);	

			//
			sprintf(msg,"%s",gl_addon);
			DrawHZText12(msg,0,set_offset,y_offset+line_x,gl_color_selected,1);	
			
				Draw_select_icon(x_offset,y_offset+line_x,v_reset);
				sprintf(msg,"%s",gl_reset);
				DrawHZText12(msg,0,x_offset+15,y_offset+line_x,(addon_sel==0)?gl_color_selected:gl_color_text,1);	
			
				Draw_select_icon(x_offset+12*6,y_offset+line_x,v_cheat);
				sprintf(msg,"%s",gl_cheat);
				DrawHZText12(msg,0,x_offset+12*6+15,y_offset+line_x,(addon_sel==1)?gl_color_selected:gl_color_text,1);
			
			//					
			sprintf(msg,"%s",gl_language);
			DrawHZText12(msg,0,set_offset,y_offset+line_x*2,gl_color_selected,1);			
				Draw_select_icon(x_offset,y_offset+line_x*2,(language_sel == 0x0));
				Draw_select_icon(x_offset+12*6,y_offset+line_x*2,(language_sel == 0x1));	
				sprintf(msg,"%s",gl_en_lang);
				DrawHZText12(msg,0,x_offset+15,y_offset+line_x*2,((language_sel==0)&&currstate&& (2== select))?gl_color_selected:gl_color_text,1);
				sprintf(msg,"%s",gl_zh_lang);
				DrawHZText12(msg,0,x_offset+12*6+15,y_offset+line_x*2,((language_sel==1)&&currstate&& (2== select))?gl_color_selected:gl_color_text,1);			
			
			//
			VBlankIntrWait();			
			sprintf(msg,"%s",gl_save);
			DrawHZText12(msg,0,set_offset,y_offset+line_x*3,gl_color_selected,1);
				Draw_select_icon(x_offset,y_offset+line_x*3,(auto_save_sel == 0x1));
				sprintf(msg,"%s",gl_auto_save);
				DrawHZText12(msg,0,x_offset+15,y_offset+line_x*3,(auto_save_pos==0)?gl_color_selected:gl_color_text,1);	

			//
			sprintf(msg,"%s",gl_modeB_INITstr);
			DrawHZText12(msg,0,set_offset,y_offset+line_x*4,gl_color_selected,1);
			
				Draw_select_icon(x_offset,y_offset+line_x*4,(ModeB_INIT == 0x0));
				sprintf(msg,"%s",gl_modeB_RUMBLE);
				DrawHZText12(msg,0,x_offset+15,y_offset+line_x*4,(ModeB_pos==0)?gl_color_selected:gl_color_text,1);									
				Draw_select_icon(x_offset+12*6,y_offset+line_x*4,(ModeB_INIT == 0x1));
				sprintf(msg,"%s",gl_modeB_LINK);
				DrawHZText12(msg,0,x_offset+12*6+15,y_offset+line_x*4,(ModeB_pos==1)?gl_color_selected:gl_color_text,1);	
					
					
					
			u32 offsety;
			for(line=0;line<line_MAX;line++)
			{
				if(Set_OK==1)
				{
					if((line== select) && (0== select)&& (edit_pos==7))
						clean_color = gl_color_btn_clean;					
					else if((line== select) && (1== select) && (addon_sel==2)) 
						clean_color = gl_color_btn_clean;	
					else if((line== select) && (2== select) && (lang_pos==1)) 
						clean_color = gl_color_btn_clean;	
					else if((line== select) && (3== select) && (auto_save_pos==1)) 
						clean_color = gl_color_btn_clean;	
					else if((line== select) && (4== select) && (ModeB_pos==2)) 
						clean_color = gl_color_btn_clean;							
					
					else 
						clean_color = gl_color_MENU_btn;
				}		
				else
				{
					if(line== select)
						clean_color = gl_color_btn_clean;
					else 
						clean_color = gl_color_MENU_btn;
				}	
				offsety = y_offset + line*line_x;
				//if(line>1) offsety += line_x; 
					
				Clear(202,offsety-2 ,30,14,clean_color,1);	
				
				if(Set_OK && (line == Set_OK_line)){
					sprintf(msg,"%s",gl_ok_btn);
				}
				else {
					sprintf(msg,"%s",gl_set_btn);
				}
				DrawHZText12(msg,0,200+5,offsety,gl_color_text,1);	
				VBlankIntrWait();		
			}						
		}		
			
		currstate=Set_OK;		
		switch(currstate) {
			case 0: //initial state
				//get date and time 	
				rtc_enable();
				rtc_get(datetime);
				rtc_disenable();				
				char* wkday;
				switch(UNBCD(datetime[3]&0x7))
				{
					case 0:wkday = gl_Sun;break;
					case 1:wkday = gl_Mon;break;
					case 2:wkday = gl_Tues;break;
					case 3:wkday = gl_Wed;break;
					case 4:wkday = gl_Thur;break;
					case 5:wkday = gl_Fri;break;
					case 6:wkday = gl_Sat;break;	
					default:wkday = gl_Sun;break;	
				}
				
				u8 HH = UNBCD(datetime[4]&0x3F);
				u8 MM = UNBCD(datetime[5]&0x7F);
				u8 SS = UNBCD(datetime[6]&0x7F);
				if(HH >23)HH=0;
				if(MM >59)MM=0;
				if(SS >59)SS=0;
				u8 month = UNBCD(datetime[1]&0x1F);
				u8 day = UNBCD(datetime[2]&0x3F);	
				if(month ==0)month=1;
				if(day ==0)day=1;
					
				sprintf(msg,"%u/%02u/%02u %02d:%02d:%02d %s",UNBCD(datetime[0])+2000,month,day,HH,MM,SS, wkday);
				ClearWithBG((u16*)gImage_SET,x_offset, y_offset, 22*6, 13, 1);	
				DrawHZText12(msg,0,x_offset,y_offset,gl_color_text,1);	
				VBlankIntrWait();


				re_show = 0;		
				scanKeys();
				keys = keysUp();//keysDown();
				if (keys & KEY_A) {//set
					Set_OK_line = select;
					Set_OK = 1;//!Set_OK;
					re_show=1;

					edit_datetime[_HOUR] 	= HH;//UNBCD(datetime[_HOUR]&0x3F);
					edit_datetime[_MIN]		= MM;//UNBCD(datetime[_MIN]&0x7F);
					edit_datetime[_SEC] 	= SS;//UNBCD(datetime[_SEC]&0x7F);
					edit_datetime[_DAY] 	= day;//UNBCD(datetime[_DAY]&0x3F);
					edit_datetime[_MONTH] = month;//UNBCD(datetime[_MONTH]&0x1F);
					edit_datetime[_YEAR] 	= UNBCD(datetime[_YEAR]);
					edit_datetime[_WKD] 	= UNBCD(datetime[_WKD]&0x7);	
					
					if(select==1)
					{
						addon_sel = 0;
					}	
					else if(select==3)
					{								
						auto_save_pos = 0;
					}
					else if(select==4)
					{								
						ModeB_pos = 0;
					}				

				}
				else if (keys  & KEY_DOWN){  
					if(select < (line_MAX-1)){
						select++;		
						re_show=1;
					}
				}
				else if(keys & KEY_UP){
					if(select){
						select--;
						re_show=1;
					}
				}  	
				else if(keys & KEY_L) {
					return 0;
				}
				else if(keys & KEY_R) {
					return 1;
				}
				break	;
			case 1: //edit state	
				//if(Set_OK_line==0) {														
					if(re_show)
					{																
						if(select ==0){
							ClearWithBG((u16*)gImage_SET,x_offset, y_offset, 23*6, 13, 1);	
							switch(edit_pos)
							{
								case 0:clean_pos = x_offset;
									clean_w = 24;
									break;
								case 1:clean_pos = x_offset+6*5;
									clean_w = 12;
									break;
								case 2:clean_pos = x_offset+6*8;
									clean_w = 12;
									break;
								case 3:clean_pos = x_offset+6*11;
									clean_w = 12;
									break;
								case 4:clean_pos = x_offset+6*14;
									clean_w = 12;
									break;
								case 5:clean_pos = x_offset+6*17;
									clean_w = 12;
									break;
								case 6:clean_pos = x_offset+6*20;
									clean_w = 18;
									break;	
															
							}
							if(	edit_pos < 7)	
								Clear(clean_pos,y_offset ,clean_w,13,gl_color_btn_clean,1);
								
							char* wkday;
							switch(edit_datetime[_WKD])
							{
								case 0:wkday = gl_Sun;break;
								case 1:wkday = gl_Mon;break;
								case 2:wkday = gl_Tues;break;
								case 3:wkday = gl_Wed;break;
								case 4:wkday = gl_Thur;break;
								case 5:wkday = gl_Fri;break;
								case 6:wkday = gl_Sat;break;	
								default:wkday = gl_Sun;break;	
							}						
							sprintf(msg,"20%02d/%02d/%02d %02d:%02d:%02d %s",edit_datetime[_YEAR],edit_datetime[_MONTH],edit_datetime[_DAY],edit_datetime[_HOUR],edit_datetime[_MIN],edit_datetime[_SEC] ,wkday);
							DrawHZText12(msg,0,x_offset,y_offset,gl_color_text,1);	
						}
						
						
					}
					re_show = 0;		
					scanKeys();
					keys = keysUp();//keysDown();
					u16 keysrepeat = keysDownRepeat();
					if(keysrepeat & KEY_UP) {
						if(select ==0){
							switch(edit_pos) {
								case 2:
									//day
									switch(edit_datetime[_MONTH]) {
										case 1: case 3: case 5: case 7: case 8: case 10: case 12:

											if(edit_datetime[_DAY]==31) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
										break;
										case 4: case 6: case 9: case 11:
											if(edit_datetime[_DAY]==30) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
										break;
										case 2:
											if((edit_datetime[_YEAR]%4==0 && edit_datetime[_DAY]==9) || (edit_datetime[_YEAR]%4>0 && edit_datetime[_DAY]==28)) {edit_datetime[_DAY]=1;} else {edit_datetime[_DAY]++;}
										break;
									}
								break;
								case 1:
									//month
									if(edit_datetime[_MONTH]==12) {edit_datetime[_MONTH]=1;} else {edit_datetime[_MONTH]++;}
								break;
								case 0:
									//year
									if(edit_datetime[_YEAR]==99) {edit_datetime[_YEAR]=0;} else {edit_datetime[_YEAR]++;}
								break;
								case 6:
									//week day
									if(edit_datetime[_WKD]==6) {edit_datetime[_WKD]=0;} else {edit_datetime[_WKD]++;}
								break;
								case 3:
									//hour
									if(edit_datetime[_HOUR]==23) {edit_datetime[_HOUR]=0;} else {edit_datetime[_HOUR]++;}
								break;
								case 4:
									//minute
									if(edit_datetime[_MIN]==59) {edit_datetime[_MIN]=0;} else {edit_datetime[_MIN]++;}
								break;
								case 5:
									//second
									if(edit_datetime[_SEC]==59) {edit_datetime[_SEC]=0;} else {edit_datetime[_SEC]++;}
								break;			
							}
											
						}
					
						re_show = 1;								
					} else if(keysrepeat & KEY_DOWN) {
						if(select ==0){
							switch(edit_pos) {
								case 2:
									switch(edit_datetime[_MONTH]) {
										case 1: case 3: case 5: case 7: case 8: case 10: case 12:
											if(edit_datetime[_DAY]==1) 
											{
												edit_datetime[_DAY]=31;
											} 
											else 
											{
												//if(edit_datetime[_DAY] > 1)
													edit_datetime[_DAY]--;
											}
										break;
										case 4: case 6: case 9: case 11:
											if(edit_datetime[_DAY]==1) 
											{
												edit_datetime[_DAY]=30;
											} 
											else 
											{
												//if(edit_datetime[_DAY] > 1)
													edit_datetime[_DAY]--;
											}
										break;
										case 2:
											if(edit_datetime[_DAY]==1) {
												if(edit_datetime[_YEAR]%4==0) {
													edit_datetime[_DAY]=29;
												} else {
													edit_datetime[_DAY]=28;
												}
											} else {
												edit_datetime[_DAY]--;
											}
										break;
									}
								break;
								case 1:
									if(edit_datetime[_MONTH]==1) {edit_datetime[_MONTH]=12;} else {edit_datetime[_MONTH]--;}
								break;
								case 0:
									if(edit_datetime[_YEAR]==0) {edit_datetime[_YEAR]=99;} else {edit_datetime[_YEAR]--;}
								break;
								case 6:
									//week day
									if(edit_datetime[_WKD]==0) {edit_datetime[_WKD]=6;} else {edit_datetime[_WKD]--;}
								break;
								case 3:
									//hour
									if(edit_datetime[_HOUR]==0) {edit_datetime[_HOUR]=23;} else {edit_datetime[_HOUR]--;}
								break;
								case 4:
									//minute
									if(edit_datetime[_MIN]==0) {edit_datetime[_MIN]=59;} else {edit_datetime[_MIN]--;}
								break;
								case 5:
									//second
									if(edit_datetime[_SEC]==0) {edit_datetime[_SEC]=59;} else {edit_datetime[_SEC]--;}
								break;
							
							}
						}

						re_show = 1;	
					} else if(keys & KEY_RIGHT) {
						if(select ==0){
							if(edit_pos==7) {
								edit_pos=0;
							} else {
								edit_pos++;
							}
						}
						else if(select ==1) 
						{
							if(addon_sel<2){
								addon_sel ++;
							}
						}
						else if(select ==2) //lang
						{
							language_sel = 1;								
						}
						else if(select ==3)//auto save
						{
							auto_save_pos = 1;
						}
						else if	(select ==4)
						{
							if(ModeB_pos<2){
								ModeB_pos ++;
							}
						}

						re_show = 1;	
					}
					else if(keys & KEY_LEFT) {
						if(select ==0){
							if(edit_pos==0) {
								edit_pos=7;
							} else {
								edit_pos--;
							}
						}
						else if(select ==1) 
						{
							if(addon_sel){
								addon_sel --;
							}		
						}
						else if(select ==2) //lang
						{
								language_sel = 0;
						}						
						else if(select ==3)//auto save
						{
								auto_save_pos = 0;
						}
						else if(select ==4)	
						{
							if(ModeB_pos){
								ModeB_pos--;
							}
						}	

						re_show = 1;	
					} else if(keys & KEY_A) {
						if((0== select) && (edit_pos==7)){
							rtc_enable();
							rtc_set(edit_datetime);
							rtc_disenable();
							delay(0x200);
							Set_OK = 0;//!Set_OK;
						}
						else if(select == 1) 
						{
							switch(addon_sel)
							{
								case 0:v_reset = !v_reset;break;
								case 1:v_cheat = !v_cheat;break;	
								case 2:
									{
										save_setw_info();
										CheckSwitch(); //read again  
										Set_OK = 0;	
										break;							
									}
							}	
						}
						else if(select == 2) 
						{
							save_setw_info();
							CheckLanguage(); //read again    	
							ClearWithBG((u16*)gImage_SET,0, 20, 240, 160-20, 1);
							Set_OK = 0;													
						}
						else if(select == 3) 
						{
							switch(auto_save_pos)
							{
								case 0:auto_save_sel = !auto_save_sel;break;
								case 1:
									{
										save_setw_info();
										Set_OK = 0;	
										gl_auto_save_sel = Read_SET_info(assress_auto_save_sel);
										if( (gl_auto_save_sel != 0x0) && (gl_auto_save_sel != 0x1))
										{
											gl_auto_save_sel = 0x1;
										}
										break;							
									}
							}	
						}
						else if(select == 4) 
						{
							switch(ModeB_pos)
							{
								case 0:ModeB_INIT = 0;break;
								case 1:ModeB_INIT = 1;break;
								//case 2:ModeB_INIT = 2;break;	
								case 2:
									{
										save_setw_info();
										Set_OK = 0;	
										break;
									}
							}
						}		
		
						re_show = 1;
					}
				break	;			
		}//end switch 		
	}//end while(1)
}
//---------------------------------------------------------------------------------
void save_setw_info(void)
{
	u32 address;
		
	if(language_sel == 0x0){//english						
		SET_info_buffer[assress_language] = 0xE1E1;
	}
	else{					
		SET_info_buffer[assress_language] = 0xE2E2;
	}	
	
	SET_info_buffer[assress_v_reset] = v_reset;
	SET_info_buffer[assress_v_cheat] = v_cheat;	
	
	SET_info_buffer[assress_show_Thumbnail] = gl_show_Thumbnail;		
	
	SET_info_buffer[assress_auto_save_sel] = auto_save_sel;
	SET_info_buffer[assress_ModeB_INIT] = ModeB_INIT;	
	

	for(address=60;address < assress_max;address++)
	{
		SET_info_buffer[address] = Read_SET_info(address);
	}		
	//save to nor 
	Save_SET_info(SET_info_buffer,0x200);
}