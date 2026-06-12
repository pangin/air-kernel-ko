//#ifndef	EZKERNEL_HEADER
//#define EZKERNEL_HEADER

#include "ff.h"
#include "ez_define.h"


typedef struct FM_NOR_FILE_SECT{////save to nor
	char filename[100];	
	u16 rompage ;
	u16 have_patch ;
	u16	have_RTS;
	u16 is_64MBrom;//reserved;
	u32 filesize;
	u8 savemode;
	u8 reserved1;
	u16 reserved2 ;
	char gamename[0x10];
} FM_NOR_FS;

typedef struct FM_Folder_SECT{
	char filename[100];	
} FM_Folder_FS;

typedef struct FM_FILE_SECT{
	char filename[100];
	u32 filesize;	
} FM_FILE_FS;



typedef enum {
	NOR_list=0,	
	SD_list=1,
	SET_win=2,
	HELP=3,
}PAGE_NUM ;

//----------------------------
extern DWORD Get_NextCluster(	FFOBJID* obj,	DWORD clst);
extern DWORD ClustToSect(FATFS* fs,DWORD clst);

extern FM_NOR_FS pNorFS[MAX_NOR]EWRAM_BSS;
extern u8 pReadCache [MAX_pReadCache_size]EWRAM_BSS;
extern u8 __attribute__((aligned(4)))GAMECODE[4];


extern u16 gl_reset_on;
extern u16 gl_rts_on;
extern u16 gl_sleep_on;
extern u16 gl_cheat_on;



u32 Setting_window(void);


void delay(u32 R0);
u32 LoadRTSfile(TCHAR *filename);
void ShowTime(u32 page_num ,u32 page_mode);
u8 NOR_list_MENU(u32 show_offset,	u32 file_select);
u8 SD_list_MENU(u32 show_offset,	u32 file_select,u32 play_re);
//#endif