#include <gba_interrupt.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
#include <gba_base.h>
#include <gba_dma.h>
#include <string.h>
#include <gba_systemcalls.h>

#include "ez_define.h"
#include "NORflash_OP.h"
#include "ezkernel.h"
#include "draw.h"
#include "lang.h"
#include "Ezcard_OP.h"
#include "GBApatch.h"
//#define DEBUG

extern FM_NOR_FS pNorFS[MAX_NOR]EWRAM_BSS;
extern u8 pReadCache [MAX_pReadCache_size]EWRAM_BSS;
extern u32 gl_currentpage;
extern u32 gl_norOffset;
extern FIL gfile;
extern u32 game_total_NOR;
extern u32 iTrimSize;
//---------------------------------------------------------------
void IWRAM_CODE Chip_Reset()
{
	SetRompage_MODE(0x8000,SYSTEM_MODE_OS);//select flash1
	*((vu16 *)(FlashBase_S98)) = 0xF0 ;
	SetRompage_MODE(0x0000,SYSTEM_MODE_OS);//select flash0
	*((vu16 *)(FlashBase_S98)) = 0xF0 ;
	
}
//---------------------------------------------------------------
void IWRAM_CODE Block_Erase(u32 blockAdd) //0x20000 BYTE pre block
{
	vu16 page,v1,v2;
	u32 Address;
	u32 loop;

	page=gl_currentpage;
	Address=blockAdd;
	while(Address>=0x800000)
	{
		Address-=0x800000;
		page+=0x1000;
		
		
	}

	SetRompage_MODE(page,SYSTEM_MODE_OS);
	//u16 norid = Read_S98NOR_ID();

	v1=0;v2=1;
	if((blockAdd==0) || (blockAdd==0x3FE0000)   || (blockAdd==0x4000000) || (blockAdd==0x7FE0000) )
	{
		//Address=blockAdd;
		for(loop=0;loop<0x20000;loop+=0x8000)
		{
			*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase_S98+0x555*2)) = 0x80 ;
			*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
			*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
			*((vu16 *)(FlashBase_S98+Address+loop)) = 0x30 ;
			do
			{
				v1 = *((vu16 *)(FlashBase_S98+Address+loop)) ;
				v2 = *((vu16 *)(FlashBase_S98+Address+loop)) ;
			}while(v1!=v2);
		}
	}
	else
	{
		loop=Address;

		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0x80 ;
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+loop)) = 0x30 ;
		do
		{
			v1 = *((vu16 *)(FlashBase_S98+loop)) ;
			v2 = *((vu16 *)(FlashBase_S98+loop)) ;
		}while(v1!=v2);
	}
	SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);
}
//---------------------------------------------------------------
//---------------------------------------------------------------
void IWRAM_CODE FormatNor()
{
	char msg[128];
	sprintf(msg,"%s",gl_formatnor_info);
	DrawHZText12(msg,0,60,90+13,gl_color_text,1);
	
	u32 blocknum;
	
	while(1)
	{
		delay(500);
		scanKeys();
		u16 keys = keysDown();
		if (keys & KEY_A) {
			//Chip_Erase();
			
			for(blocknum=First_game_address;blocknum<NOR_end_address;blocknum+=0x20000)
			{		
				sprintf(msg,"%luMb/%luMb",(blocknum)/0x20000,NOR_end_address/0x20000);
				//Clear(70+54,160-15,100,15,gl_color_cheat_black,1);
				//DrawHZText12(msg,0,70+54,160-15,gl_color_text,1);
				DrawPic((u16*)(gImage_MENU+78*128*2), 56, 90+13, 128, 13, 0, 0, 1);//show menu pic
				DrawHZText12(msg,0,60,90+13,gl_color_text,1);
				Block_Erase(blocknum);
			}		
			memset(pNorFS,00,sizeof(FM_NOR_FS)*MAX_NOR);
			return;
		}
		else if(keys & KEY_B) {
			return;
		}
	}
}
//---------------------------------------------------------------
// --------------------------------------------------------------------
void IWRAM_CODE WriteFlash_with64word(u32 address,u8 *buffer,u32 size)
{
	vu16 page,v1,v2;
	register u32 loopwrite ;
	vu16* buf = (vu16*)buffer ;
	u32 i;

	Chip_Reset();
	page=gl_currentpage;
	while(address>=0x800000)
	{
		address-=0x800000;
		page+=0x1000;
	}
	SetRompage_MODE(page,SYSTEM_MODE_OS);

	//
	v1=0;v2=1;
	for(loopwrite=0;loopwrite<(size/64);loopwrite++)
	{
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+address+loopwrite*64)) = 0x25;
		*((vu16 *)(FlashBase_S98+address+loopwrite*64)) = 31;
		for(i=0;i<=31;i++)
		{
			*((vu16 *)(FlashBase_S98+address+loopwrite*64 +2*i )) = buf[loopwrite*32+i];
		}	
		*((vu16 *)(FlashBase_S98+address+loopwrite*64)) = 0x29;
		do
		{
			v1 = *((vu16 *)(FlashBase_S98+address+loopwrite*64)) ;
			v2 = *((vu16 *)(FlashBase_S98+address+loopwrite*64)) ;
		}while(v1!=v2);
		
		//DelayMicroseconds(50);
		
	}
	SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);
}
//---------------------------------------------------------------
void IWRAM_CODE WriteFlash_with32word(u32 address,u8 *buffer,u32 size)
{
	vu16 page,v1,v2;
	register u32 loopwrite ;
	vu16* buf = (vu16*)buffer ;
	u32 i;

	page=gl_currentpage;
	while(address>=0x800000)
	{
		address-=0x800000;
		page+=0x1000;
	}
	SetRompage_MODE(page,SYSTEM_MODE_OS);

	//Chip_Reset();
	v1=0;v2=1;
	for(loopwrite=0;loopwrite<(size/32);loopwrite++)
	{
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+address+loopwrite*32)) = 0x25;
		*((vu16 *)(FlashBase_S98+address+loopwrite*32)) = 15;
		for(i=0;i<=15;i++)
		{
			*((vu16 *)(FlashBase_S98+address+loopwrite*32 +2*i )) = buf[loopwrite*16+i];
		}	
		*((vu16 *)(FlashBase_S98+address+loopwrite*32)) = 0x29;
		
		do
		{
			v1 = *((vu16 *)(FlashBase_S98+address+loopwrite*32+0xF*2)) ;
			v2 = *((vu16 *)(FlashBase_S98+address+loopwrite*32+0xF*2)) ;
		}while(v1!=v2);
	}
	SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);
}
//---------------------------------------------------------------
void IWRAM_CODE WriteFlash(u32 address,u8 *buffer,u32 size)
{
	vu16 page,v1,v2;
	register u32 loopwrite ;
	vu16* buf = (vu16*)buffer ;

	page=gl_currentpage;
	while(address>=0x800000)
	{
		address-=0x800000;
		page+=0x1000;
	}
	SetRompage_MODE(page,SYSTEM_MODE_OS);

	//Chip_Reset();
	v1=0;v2=1;
	for(loopwrite=0;loopwrite<(size/2);loopwrite++)
	{
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xA0 ;
		*((vu16 *)(FlashBase_S98+address+loopwrite*2)) = buf[loopwrite];
		do
		{
			v1 = *((vu16 *)(FlashBase_S98+address+loopwrite*2)) ;
			v2 = *((vu16 *)(FlashBase_S98+address+loopwrite*2)) ;
		}while(v1!=v2);
	}
	SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);
}
//-----------------------------------------------------------
u32 IWRAM_CODE Loadfile2NOR(TCHAR *filename, u32 NORaddress,u16 have_patch,u8 SAVEMODE)
{
	u32 res;
	u32 ret;
	u32 filesize;
	u32 fileneedsize;
	
	u32 blocknum;
	char msg[128];	
	FM_NOR_FS tmpNorFS ;
	char temp[50];
	//char flag[4];
	u16 flag_64MBrom=0;
	
	//u16 readdata;
	u32 add_patch = 0;
	
	u16 norid = Read_S98NOR_ID();
	if(norid == NOR_ID)//S98
	{
		res = f_open(&gfile, filename, FA_READ);
		if(res != FR_OK)
				return 0;
		
		//DEBUG_printf("11");
		filesize = f_size(&gfile);		
		f_lseek(&gfile, 0xa0);		
		f_read(&gfile, temp, 0x10, (UINT *)&ret);//read game name

		memcpy(tmpNorFS.gamename,temp,0x10);
		tmpNorFS.rompage = NORaddress >> 17;
		
		fileneedsize = ((((filesize+0x3FFFF)/0x40000)*0x40000));
		if(have_patch)
		{			
			if(iTrimSize>=fileneedsize)
			{
				fileneedsize = fileneedsize+0x40000;
				add_patch = 1;
			}
		}
			
		if(	fileneedsize > (0x8000000-NORaddress)){ //1Gbit flash
			return 2; //Not enough NOR space 
		}
		
		//DEBUG_printf("have_patch=%x add_patch =%x",have_patch,add_patch);
		//wait_btn();
		//DEBUG_printf("33");
		/*
		////////////////// erase all BBP
		*((vu16 *)(FlashBase_S98)) = 0xF0 ;	
		
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xAA ;
		*((vu16 *)(FlashBase_S98+0x2AA*2)) = 0x55 ;
		*((vu16 *)(FlashBase_S98+0x555*2)) = 0xC0 ;		
		*((vu16 *)(FlashBase_S98+0x000*2)) = 0x80 ;
		*((vu16 *)(FlashBase_S98+0x000*2)) = 0x30 ;
		{
			int polling_counter = 0x15000;
			u16 v1;
			do {
				v1 = *((vu16 *)(FlashBase_S98+ 0x5C0000));
				polling_counter--;

			} while (polling_counter);
		}	
		*((vu16 *)(FlashBase_S98+0x000*2)) = 0x90 ;
		*((vu16 *)(FlashBase_S98+0x000*2)) = 0x00 ;			
		/////////////////		
		*/
			
		tmpNorFS.filesize = fileneedsize;
		tmpNorFS.have_patch = have_patch;
		//tmpNorFS.have_RTS = gl_rts_on;
		tmpNorFS.is_64MBrom = flag_64MBrom;
		tmpNorFS.savemode = SAVEMODE;
		tmpNorFS.reserved1 = 0;//is_EMU;
		
		sprintf(tmpNorFS.filename,"%s",filename);
		dmaCopy(&tmpNorFS,&pNorFS[game_total_NOR], sizeof(FM_NOR_FS));
 		//DEBUG_printf("44");
		Clear(60,160-15,120,15,gl_color_cheat_black,1);	
		DrawHZText12(gl_writing,0,70,160-15,gl_color_text,1);	


		Chip_Reset();		
	
		f_lseek(&gfile, 00);	
		for(blocknum=0;blocknum<filesize;blocknum+=0x20000)
		{		
			//DEBUG_printf("55");
		
			sprintf(msg,"%luMb/%luMb",(blocknum)/0x20000,filesize/0x20000);
			Clear(70+54,160-15,100,15,gl_color_cheat_black,1);
			DrawHZText12(msg,0,70+54,160-15,gl_color_text,1);
			
			
			f_read(&gfile, pReadCache, 0x20000, (UINT *)&ret);//pReadCache max 0x20000 Byte
			
			
			if(have_patch){
				if((gl_reset_on==1) || (gl_cheat_on==1))		    
				{
					PatchInternal((u32*)pReadCache,0x20000,blocknum);	
					GBApatch_NOR((u32*)pReadCache,0x20000,blocknum);//some nes need check				
				}
			}
			else{
				GBApatch_Cleanrom_NOR((u32*)pReadCache,blocknum);
			}
			Block_Erase(blocknum+NORaddress);	
			WriteFlash_with64word(blocknum+NORaddress,pReadCache,0x20000);
	
			

		}
		f_close(&gfile);
		
		if(have_patch)
		{
			if(add_patch)
			{
				//wait_btn();
				Block_Erase(blocknum+NORaddress);
				GBApatch_NOR((u32*)pReadCache,0x20000,blocknum);
				WriteFlash_with64word(blocknum+NORaddress,pReadCache,0x20000);
			}
		}
		Save_NORgame_info((u16*)pNorFS,sizeof(FM_NOR_FS)*0x40);
		return 0;
	}		
	else
	{
		#ifdef DEBUG
			//DEBUG_printf("Bad NOR ID");
		#endif
		return 1;
	}
}
//-----------------------------------------------------------
void* IWRAM_CODE my_memcpy(void *destination, const void *source, u32 num)
{
    u8 *dest = destination;
    u8 *src  = (u8*)source;
    while (num) {
        *dest++ = *src++;
        num--;
    }
    return destination;
}

int IWRAM_CODE my_memcmp(void *ptr, const void *ptr2, u32 size)
{
  u8 *p1 = (u8 *)ptr;
  u8 *p2 = (u8 *)ptr2;
  u8 *p3 = (u8 *)ptr + size;

  while(p1 != p3) {
    if (*p1 != *p2) return 1;
    p1++;
    p2++;
  }
    return 0;
}

//-----------------------------------------------------------
u32 IWRAM_CODE GetFileListFromNor(void)
{
	REG_IME = 0 ;
	u32 page=0x1000 ,count=0;
	u32 StartAddress = FlashBase_S98;
	//FM_NOR_FS tmpNorFS ;
	char temp[50];
	vu16  Value;

	SetRompage_MODE(gl_currentpage+page,SYSTEM_MODE_OS); //from 8M start

	Value = *(vu16 *)(StartAddress + 0xbe);//5F
	u16 x24 = *(vu16 *)(StartAddress + 0x6);//3

	
	while( ((Value&0xFF)==0xCE) || ((Value&0xFF)==0xCF)|| ((Value&0xFF)==0x00)|| (x24==0x51ae))
	{
		//DEBUG_printf("StartAddress %x %x %x",StartAddress,Value,x24);
		if(*(vu8 *)(StartAddress+0xb2) == 0x96) //59
		{
			my_memcpy(temp,(char*)(StartAddress+0xa0),0x10);//50
			//temp[12] = 0 ;
		
			if(my_memcmp(temp,pNorFS[count].gamename,9) ==0) //if(!strcasecmp(temp, pNorFS[count].gamename))
			{
				gl_norOffset += pNorFS[count].filesize;
				StartAddress += pNorFS[count].filesize;
				count ++ ;
			}
			else 
			{
				break;
			}
		}
		else
		{
			break;
		}		

		while(StartAddress >= FlashBase_S98_end)
		{
			page += 0x1000 ;
			if(page>0xF000)   // 
			{
				SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);
				return count;
			}
			SetRompage_MODE(gl_currentpage+page,SYSTEM_MODE_OS);
			StartAddress -= 0x800000 ;
		}
		if(count > MAX_NOR) 
			break; //max
			
		Value = *(vu16 *)(StartAddress + 0xbe);
		x24 = *(vu16 *)(StartAddress + 0x6);
	}
	SetRompage_MODE(gl_currentpage,SYSTEM_MODE_OS);

	REG_IME   = 1 ;
	return count ;
}
//-----------------------------------------------------------
