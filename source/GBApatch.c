#include <stdio.h>
#include <stdlib.h>
#include <gba_base.h>
#include <string.h>
#include <gba_dma.h>

#include "ez_define.h"
#include "draw.h"
#include "GBApatch.h"
#include "gba_nes_patch.h"
#include "ezkernel.h"
#include "reset_table.h"
#include "lang.h"
#include "showcht.h"
#include "Ezcard_OP.h"



u32 windows_offset;
u32 is_NORpatch;
u32 g_Offset;
u32 is_Nes;
u32 Nes_index;
u32 Nes_index_17_patch;
u32 iTrimSize;
u32 EA_offset;

u32 w_reset_on;
u32 w_rts_on;
u32 w_sleep_on;
u32 w_cheat_on;

SPatchInfo2 iPatchInfo2[EMax];
u32 iCount2;
extern ST_entry pCHEAT[256];

#define sizeofa(array) (sizeof(array)/sizeof(array[0]))

u32 spend_address;
//------------------------------------------------------------------
void Write(u32 romaddress, const u8* buffer, u32 size)
{
	u32 x;
	if(is_NORpatch)
	{
		if((romaddress >= windows_offset) && (romaddress < windows_offset+0x20000))
		{
			for(x=0;x<size/2;x++){
				((vu16*)(pReadCache+romaddress-windows_offset))[x] = ((vu16*)buffer)[x];
			}					
			//DEBUG_printf("NORaddress{%x}:%x %x", romaddress,size ,((vu32*)buffer)[0]);			
		}
	}

}
//------------------------------------------------------------------
bool IWRAM_CODE PatchDragonBallZ(u32 *Data)
{
  bool res=false;
	const u32 game_codes_DragonBallZ[]=
	{
	  0x45464c42, //2 Games in 1 - Dragon Ball Z - The Legacy of Goku I & II (USA).gba
	  0x50474c41, //Dragon Ball Z - The Legacy of Goku (Europe) (En,Fr,De,Es,It).gba
	  0x45474c41, //Dragon Ball Z - The Legacy of Goku (USA).gba
	  0x50464c41, //Dragon Ball Z - The Legacy of Goku II (Europe) (En,Fr,De,Es,It).gba
	  0x45464c41, //Dragon Ball Z - The Legacy of Goku II (USA).gba
	  0x4a464c41,  //Dragon Ball Z - The Legacy of Goku II International (Japan).gba

	  0x45593241   //1928 - Top Gun - Combat Zones(US).gba
	  
	};
  const u32 patch_1_offsets[]=
  {
    0x033C,
    0x0340,
    0x0356,
    0x035A,
    0x035E,
    0x0384,
    0x0388,
    0x494C,
    0x4950,
    0x4978,
    0x497C,
    0x998E,
    0x9992
  };
  const u32 patch_2_offsets[]=
  {
    0x356,
    0x35e,
    0x37e,
    0x382
  };
  s32 index_DragonBallZ=-1;
  for(u32 ii=0;ii<sizeofa(game_codes_DragonBallZ);ii++)
  {
    if( *(u32*)GAMECODE==game_codes_DragonBallZ[ii])
    {
      index_DragonBallZ=ii;
      break;
    }
  }
  if( index_DragonBallZ!=-1)
  {
    u16 patch=0;
    res=true;
    switch( index_DragonBallZ)
    {
      case 0: //2in1 us
        for(u32 ii=0;ii<sizeofa(patch_2_offsets);ii++)
        {
          Write(patch_2_offsets[ii]+0x40000,(u8*)&patch,sizeof(patch));
        }
        patch=0x1001;
        Write(0xbb9016,(u8*)&patch,sizeof(patch));
        break;
      case 1: //I eu
        patch=0x46c0;
        for(u32 ii=0;ii<sizeofa(patch_1_offsets);ii++)
        {
          Write(patch_1_offsets[ii],(u8*)&patch,sizeof(patch));
        }
        break;
      case 2: //I us
        for(u32 ii=0;ii<sizeofa(patch_2_offsets);ii++)
        {
          Write(patch_2_offsets[ii],(u8*)&patch,sizeof(patch));
        }
        break;
      case 3: //II eu
        patch=0x1001;
        Write(0x6f42b2,(u8*)&patch,sizeof(patch));
        break;
      case 4: //II us
        patch=0x1001;
        Write(0x3b8e9e,(u8*)&patch,sizeof(patch));
        break;
      case 5: //II jp
        patch=0x1001;
        Write(0x3fc8f6,(u8*)&patch,sizeof(patch));
        break;
      case 6: //1928 - Top Gun - Combat Zones(US).gba
    	patch=0x3401;
	    Write(0x088816,(u8*)&patch,sizeof(patch));
    	patch=0x46c0; //only need one
	    Write(0x088814,(u8*)&patch,sizeof(patch));
	    Write(0x088932,(u8*)&patch,sizeof(patch));
	    Write(0x088938,(u8*)&patch,sizeof(patch));
	    Write(0x08893C,(u8*)&patch,sizeof(patch));
	    Write(0x08897C,(u8*)&patch,sizeof(patch));
	    Write(0x088982,(u8*)&patch,sizeof(patch));
	    Write(0x088986,(u8*)&patch,sizeof(patch));
	    Write(0x088988,(u8*)&patch,sizeof(patch));
	    break;
      default:
        res=false;
        break;
    }
  }
  return res;
}
//------------------------------------------------------------------
void IWRAM_CODE CheckNes(u32 *Data)
{
  u32 jump=Data[0];
  if((jump&0xff000000)==0xea000000)
  {
		Nes_index=(jump&0xffffff)+2;
		if((Data[Nes_index]&0xffffff00)==0xe28f5000&& \
		   Data[Nes_index+1]==0xe8b503d3&&\
		   Data[Nes_index+2]==0xe129f007&&\
		   Data[Nes_index+3]==0xe281deba&&\
		   Data[Nes_index+4]==0xe129f008&&\
		   Data[Nes_index+5]==0xe281debe&&\
		   Data[Nes_index+6]==0xe129f009&&\
		   Data[Nes_index+7]==0xe281dc0b&&\
		   Data[Nes_index+8]==0xe92d0003&&\
		   Data[Nes_index+9]==0xef110000&&\
		   Data[Nes_index+10]==0xe8bd8001)
		{
			if(Data[Nes_index]==0xe28f503c)
				is_Nes = 1;
			else if (Data[Nes_index]==0xe28f5040)
				is_Nes = 2;    		
		}
		else
			is_Nes = 0;
  }
}
//------------------------------------------------------------------
bool PatchNes(u32 *Data)
{
  bool res=false;
  if(is_Nes)
  {
      res=true;
      u32 patch=0xea000000|(0x3fdf5-Nes_index);
      Write(36+Nes_index*4,(u8*)&patch,sizeof(patch));
      Write(0xff800,gba_nes_patch_bin,0x340);
      if(windows_offset == 0)
    	  Nes_index_17_patch=Data[Nes_index+17+is_Nes-1];
      Write(0xff840,(u8*)&Nes_index_17_patch,sizeof(patch));
      u32 index2=(Nes_index_17_patch-0x08000000)/4; 
      if(Data[index2-1-windows_offset/4]==0x3032)
      {
        patch=0x060000f8;
        Write(0xff86c,(u8*)&patch,sizeof(patch));
      }
  }
  return res;
}
//---------------------------------------------------------------------------------
void Add2(u32 anOffset, u32 aValue)
{
	if (iCount2<(EMax))
	{
		anOffset = anOffset+ g_Offset ;
		SPatchInfo2 info = { anOffset,aValue };
		iPatchInfo2[iCount2++] = info;
	} 
}
//------------------------------------------------------------------
void IWRAM_CODE PatchInternal(u32* Data,int iSize,u32 offset)
{
  u32 search_size=iSize/4;
  g_Offset = offset/4;
  if(offset==0)
  {
	  EA_offset = Data[0] & 0xFFFFFF;
	  
	  iCount2 = 0;
	  memset(iPatchInfo2, 0x00, sizeof(iPatchInfo2));//2025
  }
  
  for(u32 ii=0;ii<search_size;ii++)
  {
    switch(Data[ii])
    {
      case 0x3007FFC: // IRQ handler
        {
          Add2(ii, 0x3007FF4);//0x3007FFC的位置
        }
        break;
      case 0x3FFFFFC: // IRQ handler
        {
          Add2(ii, 0x3007FF4);
        }
        break;
    }
  }
}
//------------------------------------------------------------------
void SetTrimSize(u8* buffer,u32 romsize,u32 iSize,u32 mode,BYTE saveMODE)
{
  u8 byte;
  u32 bottom;
  u32 top;
  u32 alignedSize;

	u32  PATCH_LENGTH;

	//if((gl_rts_on==1)/* || (gl_cheat_on==1)*/ )		
	//{
		//PATCH_LENGTH = 0x1000;
	//}
	//else 
	//{
		//PATCH_LENGTH = 0x300;	
	//}
	
	//if(gl_cheat_on==1){
		PATCH_LENGTH = 0x2000;
	//}
	
  if(0)
  {
	  iTrimSize = 0x1fff000;
  }
  else
  {
  	
	  byte=buffer[(romsize-1)%iSize-1];
	  bottom=(romsize-1)%iSize-1;
	  if(bottom-16 > PATCH_LENGTH){
			top=bottom-PATCH_LENGTH-16;
		}
		else{
			top = 0;
		}
	  alignedSize=romsize+(16-(romsize&15));
	  
	  for(u32 ii=bottom;ii>=top;ii--)
	  {
			if(buffer[ii]!=byte||ii==top)
			{
			  iTrimSize=ii+4;
			  iTrimSize=iTrimSize+ ((romsize-1)&0xFFFE0000);
			  iTrimSize=iTrimSize+(16-(iTrimSize&15));
			  if(iTrimSize>alignedSize) iTrimSize=alignedSize;	
			  break;
			}
	  }
  }

	if(mode ==1)//nor
	{
		if( ((iTrimSize&0x1FFFF)+PATCH_LENGTH)  > 0x20000)//Greater than one flash sector
		{		
			iTrimSize = ((romsize+0x1FFFF)/0x20000)*0x20000;
		}
		if(romsize <=0x1000000)
	  {
  		if((iTrimSize + PATCH_LENGTH) > 16 * 1024 * 1024)
  		{
  			if((saveMODE==0x21 )||(saveMODE==0x22 ) )//eeprom game
  			{
  				iTrimSize = 0x1000000-PATCH_LENGTH;//can not greater 16MB
  			}
  			else{//sram and flash
  				iTrimSize = 0x1000000; //page
  			}
  		}
	  }		
	}
	else
	{
	  if(romsize <=0x800000)
	  {
	  		if((iTrimSize + PATCH_LENGTH) > 8 * 1024 * 1024)
	  			iTrimSize = 0x800000; //page
	  }
	  else if(romsize <=0x1000000)
	  {
  		if((iTrimSize + PATCH_LENGTH) > 16 * 1024 * 1024)
  		{
  			if((saveMODE==0x21 )||(saveMODE==0x22 ) )//eeprom game
  			{
  				iTrimSize = 0x1000000-PATCH_LENGTH;//can not greater 16MB
  			}
  			else{//sram and flash
  				iTrimSize = 0x1000000; //page
  			}
  		}
	  }
	  else
	  {
	  		if((iTrimSize + PATCH_LENGTH) > 32 * 1024 * 1024)
	  			iTrimSize = 0x2000000-PATCH_LENGTH; //page
	  }
	}
  Patch_SpecialROM_TrimSize();
  //DEBUG_printf("iTrimSize5 %08X ", iTrimSize);
}
//------------------------------------------------------------------
void Patch_B_address(void)
{
	if (!iCount2) return;
	//u32 B_install_handler;

	for (u32 ii = 0; ii<iCount2; ii++)
	{
		Write(iPatchInfo2[ii].iOffset*4, (u8*)&(iPatchInfo2[ii].iValue), sizeof(iPatchInfo2[ii].iValue));
	}	
}
//------------------------------------------------------------------
void Patch_Reset_Cheat(u32 *Data)
{	    
	Patch_B_address();
	u32 Return_address = 0x8000000+ EA_offset*4 + 8;
		
  u8 * p_patch_start  = (u8*)CHEAT_ReplaceIRQ_start;
  u8 * p_patch_end 		= (u8*)CHEAT_ReplaceIRQ_end;
  u8 * p_patch_Return_address_L  = (u8*)CHEAT_Return_address_L;

  u8* patchbuffer = (u8*)_UnusedVram ;
  u32 Return_address_offset = p_patch_Return_address_L-p_patch_start;

  dmaCopy((void*)p_patch_start,patchbuffer, p_patch_end-p_patch_start);
  *(vu32*)(patchbuffer+Return_address_offset) = Return_address;//修改gba_sleep_patch_bin里面的返回地址

  if(spend_address != 0x0){
  	*(vu32*)(patchbuffer+Return_address_offset+4) = spend_address;
	}	

	//cheat
	u8*p_no_cheat_end =  (u8*)no_CHEAT_end;
	
	u32 cheat_count_offset = (u8*)Cheat_count - p_patch_start;		
	*(vu32*)(patchbuffer+cheat_count_offset) = gl_cheat_count;	
	
	u32 cheat_offset = (u8*)CHEAT - p_patch_start;

	for (u32 ii = 0; ii<gl_cheat_count; ii++)
	{
		if(pCHEAT[ii].address >= 0x40000)
		{
			*(vu32*)(patchbuffer+cheat_offset+8*ii) = ((pCHEAT[ii].address)&0x7FFF) + 0x3000000;	
		}
		else
		{
			*(vu32*)(patchbuffer+cheat_offset+8*ii) = ((pCHEAT[ii].address)&0x3FFFF) + 0x2000000;	
		}
		*(vu32*)(patchbuffer+cheat_offset+8*ii+4) = pCHEAT[ii].VAL;	
		//DEBUG_printf("%x=%x", *(vu32*)(patchbuffer+cheat_offset+8*ii),*(vu32*)(patchbuffer+cheat_offset+8*ii+4));
	}

	u32 copysize = p_no_cheat_end-p_patch_start ;
	copysize = copysize +  gl_cheat_count*8;
	
	if(	iTrimSize+copysize > 0x2000000){
		copysize = 0x2000000 - iTrimSize;
	}
	//DEBUG_printf("iTrimSize =%x %x", iTrimSize,copysize);
		
	Write(iTrimSize, patchbuffer,copysize);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u32 Get_spend_address(u32* Data)
{
	u32 ii;	
	u32 offset;
	u32 updown;
	
  u32 search_size=0x5000/4; 
  for(ii=0;ii<search_size;ii++)
  {
		u32 word1 = Data[ii] & 0xFFFF001F;
		u32 word2 = Data[ii+1];
		if (word1 == 0xE3A0001F)
		{
			if ((word2 == 0xE129F000) || (word2 == 0xE121F000) )
			{
				offset= Data[ii + 2] & 0xFFF;
				updown = Data[ii + 2] & 0x00F00000;
				break;
			}
		}
  }
  if(ii == search_size) 
  {
  	return 0;//Find_spend_address_SpecialROM(Data); 
  }
  
  u32 address;
  if(updown == 0x00900000)
		address = ii*4 + offset+16;
	else if (updown == 0x00100000)
		address = ii*4 - offset+16;
	else
		address = 0;
  
  if(	(Data[address/4] > 0x03007E80) /*|| (Data[address/4] == 0x03007E00)*/ || (Data[address/4] == 0x0203FFFC) )
  {
  	Data[address/4] = Data[address/4] - 0x80; 
  	return (Data[address/4]);
  }
  else 
  	return 0;	
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GBApatch_Cleanrom_NOR(u32* address,u32 offset)
{
	windows_offset = offset;
	is_NORpatch = 1;
	if(offset==0)
  {
		CheckNes(address);
	}
	PatchNes(address);
	PatchDragonBallZ(address);
	//Check_Fire_Emblem();
}
//------------------------------------------------------------------
void GBApatch_NOR(u32* address,int filesize,u32 offset)
{
	windows_offset = offset;
	is_NORpatch = 1;
  if(offset==0)
  {
	  CheckNes(address);
	  EA_offset = address[0] & 0xFFFFFF;
	  
		u32 B_install_handler;
		B_install_handler = 0xEA000000|((iTrimSize-8)/4);
		Write(0,(u8*)&B_install_handler , 4); //B
		spend_address = Get_spend_address(address);
		
		Patch_somegame(address);
  }
	PatchNes(address);
	PatchDragonBallZ(address);

	if(  (gl_cheat_on == 1) || (gl_reset_on == 1)   ) 
	{
		Patch_Reset_Cheat(address);
	}
}
//------------------------------------------------------------------
void make_pat_name(TCHAR*patnamebuf,TCHAR* gamefilename)
{
	memcpy(patnamebuf,gamefilename,100);
	u32 len=strlen(patnamebuf);
	patnamebuf[len-3] = 'p';
	patnamebuf[len-2] = 'a';
	patnamebuf[len-1] = 't';	
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void Make_pat_file(TCHAR* gamefilename)
{
	u32 res;
	u32 written;
	u32 w_buffer[16];
	
	res = f_mkdir("/PATCH");
	res=f_chdir("/PATCH");
	
	memset(w_buffer, 0x00, sizeof(w_buffer));

	if(res == FR_OK){
		TCHAR patnamebuf[100];	
		make_pat_name(patnamebuf,gamefilename);

		res = f_open(&gfile,patnamebuf, FA_WRITE | FA_OPEN_ALWAYS);
		if(res == FR_OK)
		{	
			f_lseek(&gfile, 0x0000);
			res=f_write(&gfile, (void*)iPatchInfo2, sizeof(iPatchInfo2), (UINT*)&written);
			w_buffer[0] = is_NORpatch;
			w_buffer[1] = windows_offset;
			w_buffer[2] = is_Nes;
			w_buffer[3] = Nes_index;
			w_buffer[4] = g_Offset;
			w_buffer[5] = iCount2;
			w_buffer[6] = iTrimSize;
			w_buffer[7] = EA_offset;

			w_buffer[8] = gl_reset_on;
			//w_buffer[9] = gl_rts_on;
			//w_buffer[10] = gl_sleep_on;
			w_buffer[11] = gl_cheat_on;
					
			res=f_write(&gfile, (void*)w_buffer, sizeof(w_buffer), (UINT*)&written);
			f_close(&gfile);
		}
	}
	res=f_chdir("/");
}
//------------------------------------------------------------------
void make_mde_name(TCHAR*mdenamebuf,TCHAR* gamefilename)
{
	memcpy(mdenamebuf,gamefilename,100);
	u32 len=strlen(mdenamebuf);
	mdenamebuf[len-3] = 'm';
	mdenamebuf[len-2] = 'd';
	mdenamebuf[len-1] = 'e';	
}
//------------------------------------------------------------------
u8 Check_mde_file(TCHAR* gamefilename)
{
	UINT  ret;
	u32 find_the_mdefile;
	u32 mdefilesize;
	u32 res;
	
	TCHAR mdenamebuf[100];	
	make_mde_name(mdenamebuf,gamefilename);
	
	res=f_chdir("/SAVER");
	if(res == FR_OK)
	{
		res = f_open(&gfile,mdenamebuf, FA_OPEN_EXISTING);
		
		if(res == FR_OK)//have a old file
		{
			f_open(&gfile,mdenamebuf, FA_READ);
			mdefilesize = f_size(&gfile);
			f_read(&gfile, pReadCache, mdefilesize, &ret);
			f_close(&gfile);
			find_the_mdefile = 1;
		}					
		else
		{
			find_the_mdefile = 0;
		}
	}
	else//cant fine folder
	{
		find_the_mdefile = 0;
	}

	if(find_the_mdefile)
	{
		//read
		return (pReadCache[0]);
	}
	else
	{
		return 0;
	}	
}
//------------------------------------------------------------------
u8 Make_mde_file(TCHAR* gamefilename,u8 Save_num)
{
	u32 res;
	u32 written;
	u8 w_buffer[16];

	TCHAR currentpath[256];
	memset(currentpath,00,256);
	res = f_getcwd(currentpath, sizeof currentpath / sizeof *currentpath);
	
	//res = f_mkdir(SAVER_FOLDER);
	res=f_chdir(SAVER_FOLDER);
	if(res != FR_OK){
			return 2;
	}
	
	memset(w_buffer, 0x00, sizeof(w_buffer));

	if(res == FR_OK){
		TCHAR mdenamebuf[100];	
		make_mde_name(mdenamebuf,gamefilename);

		res = f_open(&gfile,mdenamebuf, FA_WRITE | FA_OPEN_ALWAYS);
		if(res == FR_OK)
		{	
			f_lseek(&gfile, 0x0000);
			
			w_buffer[0] = Save_num;
			res=f_write(&gfile, (void*)w_buffer, sizeof(w_buffer), (UINT*)&written);

			f_close(&gfile);
		}
	}
	res=f_chdir(currentpath);
	return res;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
u32 use_internal_engine(u8 gamecode[])
{
	u32 i;
	u32 count0x3007FFC=0;
	u32 result=0;
	
	g_Offset = 0;
	
	dmaCopy((void*)reset_table, (void*)pReadCache, sizeof(reset_table));
	for(i=0;i<sizeof(reset_table)/4;i++)
	{
		count0x3007FFC = ((vu32*)pReadCache)[i+1];				

		if( ((vu32*)pReadCache)[i] == *(vu32*)gamecode )
		{	
			result =1;			
			break;
		}
		i += (count0x3007FFC+1);
	}	
	if(result==0)	return 0;
		
	#ifdef DEBUG
		//DEBUG_printf("%d: %X VS %X %x",i,((vu32*)pReadCache)[i],*(vu32*)gamecode,count0x3007FFC);
		//wait_btn();	
	#endif
	i += 2;
	iCount2 = 0;
  for(u32 ii=0;ii<count0x3007FFC;ii++)
  {
      Add2(((vu32*)pReadCache)[i+ii], 0x3007FF4);//0x3007FFC offset
  }
	return result;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void Patch_SpecialROM_TrimSize(void)
{
	switch(*(u32*)GAMECODE)
	{
	case 0x4A4D4F41://1058 - Disney Sports - Motocross(JP)
	case 0x504D4F41://1068 - Disney Sports - Motocross(EU)
		iTrimSize = 0xFE3000;
		break;
	case 0x4A494442://1176 - Koinu to Issho - Aijou Monogatari(JP)
	case 0x4A324942://1741 - Koinu to Issho 2(JP)	
	case 0x454A4142://1864 - Banjo Pilot(US)
	case 0x504A4142://1899 - Banjo Pilot(EU)		
	case 0x4A324D42://2045 - Momotarou Densetsu G - Gold Deck wo Tsukure!(JP)
	case 0x4A464842://2071 - Twin Series 4 - Hamu Hamu Monster EX + F Puzzle Hamusuta(JP)		
	case 0x50514442://2214 - Donkey Kong Country 3(EU)
	case 0x45514442://2220 - Donkey Kong Country 3(US)	
	case 0x4A514442://2270 - Super Donkey Kong Country 3(JP)	
	case 0x4A564642://2286 - Twin Series 1 - Mezase Debut! Fashion Designer Monogatari + Kawaii Pet Game Gallery 2(JP)
	case 0x4A575A42://2335 - Akagi(JP)
	case 0x50385442://2342 - 2 Games in 1 - Teenage Mutant Ninja Turtles + Teenage Mutant Ninja Turtles 2 - Battle Nexus(EU)
	case 0x50384E41://2351 - Tales of Phantasia(EU)
	case 0x4A534842://2457 - Hamster Monogatari 3EX + 4 Special(JP)
	case 0x45385442://2526 - 2 Games in 1 - Teenage Mutant Ninja Turtles + Teenage Mutant Ninja Turtles 2 - Battle Nexus(US)			
		iTrimSize = 0xFE4000;
		break;
	case 0x45505342://1553 - Spider-Man 2(UE)
	case 0x58505342://1565 - Spider-Man 2(EU)		
	case 0x49505342://1657 - Spider-Man 2(IT).zip
	case 0x45585142://2618 - Superman Returns - Fortress of Solitude(UE)
		iTrimSize = 0xFE1000;
		break;
	case 0x44504C42://1815 - 2 Games in 1 - Disneys Konig der Lowen + Disneys Prinzessinnen(DE)
		iTrimSize = 0x3A9FD0;
		break;
	case 0x46504C42://1827 - 2 Games in 1 - Roi Lion, Le + Disney Princesse(FR)
		iTrimSize = 0x5ABF00;
		break;
	case 0x58424C42://1867 - 2 Games in 1 - Brother Bear + Lion King, The(EU)
		iTrimSize = 0xFFB510;
		break;
	case 0x50413542://2053 - 2 Games in 1 - Spyro - Season of Ice + Crash Bandicoot 2 - N-Tranced(EU)
		iTrimSize = 0x5E3CC0;
		break;	
	case 0x45533842://2394 - 2 Games in 1 - Spyro - Season of Ice + Spyro 2 - Season of Flame(US)
		iTrimSize = 0x7CFF00;
		break;		
	case 0x46425742://2503 - 2 Games in 1 - Disney Princesse + Frere des Ours(FR) cant work
		iTrimSize = 0x7FF000;
		break;			
	case 0x49425742://2703 - 2 Games in 1 - Disney Principesse + Koda, Fratello Orso(IT) cant work
		iTrimSize = 0x7FEC30;
		break;	
	case 0x44425742://2009 - 2 Games in 1 - Disneys Prinzessinnen + Baren Bruder(DE)
	case 0x53425742://2012 - 2 Games in 1 - Disney Princesas + Hermano Oso(ES)
		iTrimSize = 0x731000;
		break;
	case 0x53434B42://2361 - Shin-chan - Aventuras en Cineland(ES)
		iTrimSize = 0xAFF000;
		break;
	case 0x50504C42://2745 - 2 Games in 1 - Lion King, The + Disney Princess(EU)
	case 0x50425742://2780 - 2 Games in 1 - Disney Princess + Brother Bear(EU)		
		iTrimSize = 0x738000;
		break;
	case 0x4A553341://A3UJ
			iTrimSize = 0x1F90000;
		break;
	case 0x45343242://B24E 2485 - Pokemon Mystery Dungeon - Red Rescue Team(US)
			iTrimSize = 0x1FFD750;
		break;
	case 0x45555942://BYUE 2598 - Yggdra Union - We'll Never Fight Alone(US)
			iTrimSize = 0x1FFF000;
		break;
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void Patch_somegame(u32 *Data)
{
	u32 size = 0x7FF0;
	if( *(u32*)GAMECODE == 0x50424732 )
	{
  	for(u32 ii=0;ii<0x100;ii++)
  	{
   	 if(0x3000000==Data[ii])
    	{
    		if(0x8000 ==Data[ii+1] )
    		{
    			Write((ii+1)*4,(u8*)&size,4 );
    		}
    	}
		}
	}
}