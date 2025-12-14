#include "lang.h"

char* gl_init_error;
char* gl_power_off;
char* gl_init_ok;
char* gl_Loading;
char* gl_file_overflow;

char* gl_menu_btn;
char* gl_lastest_game;

char* gl_writing;

char* gl_time;
char* gl_Mon;
char* gl_Tues;
char* gl_Wed;
char* gl_Thur;
char* gl_Fri;
char* gl_Sat;
char* gl_Sun;

char* gl_addon;
char* gl_reset;

char* gl_cheat;


char* gl_language;
char* gl_en_lang;
char* gl_zh_lang;
char* gl_set_btn;
char* gl_ok_btn;

char* gl_formatnor_info;

char* gl_check_sav;
char* gl_make_sav;


char* gl_loading_game;



char* gl_LRSTART_help;
char* gl_LRSELECT_help;
char* gl_SELECT_help;
char* gl_L_A_help;
char* gl_LSTART_help;
char* gl_online_manual;


char* gl_error_0;
char* gl_error_1;
char* gl_error_2;
char* gl_error_3;
char* gl_error_4;
char* gl_error_5;
char* gl_error_6;

char* gl_save_sav;
char* gl_save_ing;

char* gl_save;
char* gl_auto_save;

char* gl_modeB_INITstr;
char* gl_modeB_RUMBLE;

char* gl_modeB_LINK;



char* gl_NOR_full;
char* gl_save_loaded;
char* gl_save_saved;
char* gl_file_exist;
char* gl_file_noexist;

char* gl_free;
//--
char**  gl_rom_menu;
char**  gl_nor_op;




//中文
const char zh_init_error[]="TF卡初始化失败";
const char zh_power_off[]="关机";
const char zh_init_ok[]="TF卡初始化成功";
const char zh_Loading[]="加载中...";
const char zh_file_overflow[]="文件太大,请加载到NOR";

const char zh_menu_btn[]=" [B]取消    [A]确定";
const char zh_writing[]="正在写...";
const char zh_lastest_game[]="请选择最后一个游戏";

const char zh_time[] ="     时间";
const char zh_Mon[]="一";
const char zh_Tues[]="二";
const char zh_Wed[]="三";
const char zh_Thur[]="四";
const char zh_Fri[]="五";
const char zh_Sat[]="六";
const char zh_Sun[]="日";

const char zh_addon[]="     功能";
const char zh_reset[]="软复位";

const char zh_cheat[]="金手指";

const char zh_hot_key[]=" 睡眠热键";
const char zh_hot_key2[]=" 菜单热键";

const char zh_language[]=" LANGUAGE";
const char zh_lang[]="中文";

const char zh_set_btn[]="设置";
const char zh_ok_btn[]="保存";
const char zh_formatnor_info[]="确定?大约4分钟";

const char zh_check_sav[]="检查SAV文件";
const char zh_make_sav[]="创建SAV文件";


const char zh_loading_game[]="加载游戏";


const char zh_LRSTART_help[]="软复位热键";
const char zh_LRSELECT_help[]="金手指开关";
const char zh_SELECT_help[]="缩略图开关";
const char zh_L_A_help[]="冷启动";
const char zh_LSTART_help[]="删除文件";
const char zh_online_manual[]="  在线说明书";



const char zh_error_0[]="文件夹错误";
const char zh_error_1[]="文件错误";
const char zh_error_2[]="SAVER错误";
const char zh_error_3[]="存档错误";
const char zh_error_4[]="读取存档错误";
const char zh_error_5[]="创建存档错误";


const char zh_save_sav[]="保存存档?";
const char zh_save_ing[]="保存...";
const char zh_save[]="     存档";
const char zh_auto_save[]="开机自动备份存档";

const char zh_modeB_INITstr[]="模式B状态";
const char zh_modeB_RUMBLE[]="震动";
const char zh_modeB_LINK[]="联动";


const char zh_NOR_full[]="NOR空间不足";
const char zh_save_loaded[]="存档已加载到RAM";
const char zh_save_saved[]="存档已保存到SD";
const char zh_file_exist[]="文件存在,覆盖吗?";
const char zh_file_noexist[]="找不到存档文件";

const char zh_free[]="空闲:";

const char *zh_rom_menu[]={
	"烧录到NOR",
	"烧录到NOR带辅助",
	"存档类型",
	"金手指",
};
const char *zh_nor_op[5]={
	"直接运行",
	"删除",
	"全部格式化",
	"加载存档到RAM",
	"保存RAM存档",
};



//英文
const char en_init_error[]="Micro SD card initial error";
const char en_power_off[]="Power off";
const char en_init_ok[]="Micro SD card initial OK";
const char en_Loading[]="Loading...";
const char en_file_overflow[]="File overflow,load to NOR";

const char en_menu_btn[]="[B]CANCEL    [A]OK";
const char en_writing[]="WRITING...";
const char en_lastest_game[]="SELECT THE LASTEST";

const char en_time[]="     TIME";
const char en_Mon[]="MON";
const char en_Tues[]="TUE";
const char en_Wed[]="WED";
const char en_Thur[]="THU";
const char en_Fri[]="FRI";
const char en_Sat[]="SAT";
const char en_Sun[]="SUN";

const char en_addon[]="    ADDON";
const char en_reset[]="RESET";

const char en_cheat[]="CHEAT";


const char en_language[]=" LANGUAGE";
const char en_lang[]="ENGLISH";
const char en_set_btn[]="SET";
const char en_ok_btn[]=" OK";

const char en_formatnor_info[]="SURE?about 4 mins";

const char en_check_sav[]="CHECKING SAV FILE";
const char en_make_sav[] ="CREATING SAV FILE";


const char en_loading_game[]="LOADING GAME";

const char en_LRSTART_help[]="Reset hotkey";
const char en_LRSELECT_help[]="Cheat toggle";
	
const char en_SELECT_help[]="Thumbnail toggle";
const char en_L_A_help[]="Multiboot";
const char en_LSTART_help[]="Delete file";
const char en_online_manual[]="Online manual";


const char en_error_0[]="Folder error";
const char en_error_1[]="File error";
const char en_error_2[]="SAVER error";
const char en_error_3[]="Save error";
const char en_error_4[]="Read save error";
const char en_error_5[]="Make save error";



const char en_save_sav[]="Save sav?";
const char en_save_ing[]="Sav...";
const char en_save[]="     SAVE";
const char en_auto_save[]="AUTO SAVE";

const char en_modeB_INITstr[]="   MODE B";
const char en_modeB_RUMBLE[]="RUMBLE";

const char en_modeB_LINK[]="LINK";



const char en_NOR_full[]="NOR space not enough  ";
const char en_save_loaded[]="Sav have been loaded";
const char en_save_saved[]="Sav have been saved";
const char en_file_exist[]="File exist，cover it?";
const char en_file_noexist[]="Cnt not find sav file";

const char en_free[]="FREE:";

const char *en_rom_menu[] = {
	"WRITE TO NOR CLEAN",
	"WRITE TO NOR ADDON",
	"SAVE TYPE",
	"CHEAT",
};
const char *en_nor_op[5]={
	"DIRECT BOOT",
	"DELETE",
	"FORMAT ALL",
	"LOAD SAV FILE",
	"SAVE SAV FILE",
};	

//---------------------------------------------------------------------------------
void LoadChinese(void)
{
	gl_init_error = (char*)zh_init_error;
	gl_power_off = (char*)zh_power_off;
	gl_init_ok = (char*)zh_init_ok;
	gl_Loading = (char*)zh_Loading;
	gl_file_overflow = (char*)zh_file_overflow;

	gl_menu_btn = (char*)zh_menu_btn;
	gl_writing = (char*)zh_writing;
	gl_lastest_game = (char*)zh_lastest_game;
	
	
	gl_time = (char*)zh_time;	
	gl_Mon = (char*)zh_Mon;
	gl_Tues = (char*)zh_Tues;
	gl_Wed = (char*)zh_Wed;
	gl_Thur = (char*)zh_Thur;
	gl_Fri = (char*)zh_Fri;
	gl_Sat = (char*)zh_Sat;
	gl_Sun = (char*)zh_Sun;

	gl_addon = (char*)zh_addon;
	gl_reset = (char*)zh_reset;

	gl_cheat = (char*)zh_cheat;	
	

	gl_language =  (char*)zh_language;
	gl_en_lang = (char*)en_lang;
	gl_zh_lang = (char*)zh_lang;;
	gl_set_btn = (char*)zh_set_btn;
	gl_ok_btn = (char*)zh_ok_btn;
	gl_formatnor_info = (char*)zh_formatnor_info;

	gl_check_sav = (char*)zh_check_sav;
	gl_make_sav = (char*)zh_make_sav;
		
	
	gl_loading_game = (char*)zh_loading_game;


	//gl_START_help = (char*)zh_START_help;
	gl_LRSTART_help = (char*)zh_LRSTART_help;
	gl_LRSELECT_help = (char*)zh_LRSELECT_help;
	gl_SELECT_help = (char*)zh_SELECT_help;	
	gl_L_A_help = (char*)zh_L_A_help;
	gl_LSTART_help = (char*)zh_LSTART_help;
	gl_online_manual = (char*)zh_online_manual;
	
	
	gl_error_0 = (char*)zh_error_0;
	gl_error_1 = (char*)zh_error_1;
	gl_error_2 = (char*)zh_error_2;
	gl_error_3 = (char*)zh_error_3;
	gl_error_4 = (char*)zh_error_4;
	gl_error_5 = (char*)zh_error_5;
	//gl_error_6 = (char*)zh_error_6;
	
	gl_save_sav = (char*)zh_save_sav;
	gl_save_ing = (char*)zh_save_ing;
	gl_save = (char*)zh_save;
	gl_auto_save = (char*)zh_auto_save;

	gl_modeB_INITstr = (char*)zh_modeB_INITstr;
	gl_modeB_RUMBLE = (char*)zh_modeB_RUMBLE;

	gl_modeB_LINK= (char*)zh_modeB_LINK;


	
	gl_NOR_full = (char*)zh_NOR_full;
	gl_save_loaded = (char*)zh_save_loaded;
	gl_save_saved = (char*)zh_save_saved;
	gl_file_exist = (char*)zh_file_exist;
	gl_file_noexist = (char*)zh_file_noexist;
	
	gl_free = (char*)zh_free;	
	//
	gl_rom_menu = (char**)zh_rom_menu;
	gl_nor_op = (char**)zh_nor_op;

}
//---------------------------------------------------------------------------------
void LoadEnglish(void)
{
	gl_init_error = (char*)en_init_error;
	gl_power_off = (char*)en_power_off;
	gl_init_ok = (char*)en_init_ok;
	gl_Loading = (char*)en_Loading;
	gl_file_overflow = (char*)en_file_overflow;

	gl_menu_btn = (char*)en_menu_btn;
	gl_writing = (char*)en_writing;
	gl_lastest_game = (char*)en_lastest_game;
	
	gl_time = (char*)en_time;	
	gl_Mon = (char*)en_Mon;
	gl_Tues = (char*)en_Tues;
	gl_Wed = (char*)en_Wed;
	gl_Thur = (char*)en_Thur;
	gl_Fri = (char*)en_Fri;
	gl_Sat = (char*)en_Sat;
	gl_Sun = (char*)en_Sun;
	gl_addon = (char*)en_addon;
	gl_reset = (char*)en_reset;

	gl_cheat = (char*)en_cheat;	
	
	
	gl_language =  (char*)en_language;
	gl_en_lang = (char*)en_lang;
	gl_zh_lang = (char*)zh_lang;;
	gl_set_btn = (char*)en_set_btn;
	gl_ok_btn = (char*)en_ok_btn;
	gl_formatnor_info = (char*)en_formatnor_info;

	gl_check_sav = (char*)en_check_sav;
	gl_make_sav = (char*)en_make_sav;
		
	
	gl_loading_game = (char*)en_loading_game;
	
	
	gl_LRSTART_help = (char*)en_LRSTART_help;
	gl_LRSELECT_help = (char*)en_LRSELECT_help;
	gl_SELECT_help = (char*)en_SELECT_help;
	gl_L_A_help = (char*)en_L_A_help;
	gl_LSTART_help = (char*)en_LSTART_help;
	gl_online_manual = (char*)en_online_manual;
	

	
	gl_error_0 = (char*)en_error_0;
	gl_error_1 = (char*)en_error_1;
	gl_error_2 = (char*)en_error_2;
	gl_error_3 = (char*)en_error_3;
	gl_error_4 = (char*)en_error_4;
	gl_error_5 = (char*)en_error_5;
	
	gl_save_sav = (char*)en_save_sav;
	gl_save_ing = (char*)en_save_ing;
	gl_save = (char*)en_save;
	gl_auto_save = (char*)en_auto_save;

	gl_modeB_INITstr = (char*)en_modeB_INITstr;
	gl_modeB_RUMBLE = (char*)en_modeB_RUMBLE;
	gl_modeB_LINK= (char*)en_modeB_LINK;
	
	
	gl_NOR_full = (char*)en_NOR_full;
	gl_save_loaded = (char*)en_save_loaded;
	gl_save_saved = (char*)en_save_saved;
	gl_file_exist = (char*)en_file_exist;
	gl_file_noexist = (char*)en_file_noexist;
	
	gl_free = (char*)en_free;	
	//
	gl_rom_menu = (char**)en_rom_menu;
	gl_nor_op = (char**)en_nor_op;
}
