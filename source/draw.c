#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gba_base.h>
#include <gba_dma.h>
#include <string.h>

#include "ez_define.h"
#include "font_kor.h"
#include "font_cjk.h"
#include "asc126.h"
#include "ezkernel.h"
#include "draw.h"

int current_y = 1;
extern u8 pReadCache [MAX_pReadCache_size]EWRAM_BSS;
//******************************************************************************
void IWRAM_CODE Clear(u16 x, u16 y, u16 w, u16 h, u16 c, u8 isDrawDirect)
{
	u16 *p;
	u16 yi,ww,hh;
    
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;

    hh = (y+h>160)?160:(y+h);
    ww  = (x+w>240)?(240-x):w;

	//u16 tmp[240];
	for(u32 i=0;i<240;i++)
		((u16*)pReadCache)[i] = c;

	for(yi=y; yi < hh; yi++)
		dmaCopy(pReadCache,p+yi*240+x,ww*2);         
}
//******************************************************************************
void IWRAM_CODE ClearWithBG(u16* pbg,u16 x, u16 y, u16 w, u16 h, u8 isDrawDirect)
{
	u16 *p;
	u16 yi,ww,hh;
    
	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;

    hh = (y+h>160)?160:(y+h);
    ww  = (x+w>240)?(240-x):w;

	for(yi=y; yi < hh; yi++)
		dmaCopy(pbg+yi*240+x,p+yi*240+x,ww*2);       
}
//******************************************************************************
void IWRAM_CODE DrawPic(u16 *GFX, u16 x, u16 y, u16 w, u16 h, u8 isTrans, u16 tcolor, u8 isDrawDirect)
{
	u16 *p,c;
	u16 xi,yi,ww,hh;

	if(isDrawDirect)
		p = VideoBuffer;
	else
		p = Vcache;
		
  hh = (y+h>160)?160:(y+h);
  ww  = (x+w>240)?(240-x):w;	
	
	if(isTrans)
	{
		for(yi=y; yi < hh; yi++)
			for(xi=x;xi<x+ww;xi++)
			{
				c = GFX[(yi-y)*w+(xi-x)];
				if(c!=tcolor)
					p[yi*240+xi] = c;
			}
	}
	else
	{
		for(yi=y; yi < hh; yi++)
			dmaCopy(GFX+(yi-y)*w,p+yi*240+x,w*2); 
	}
}
//---------------------------------------------------------------------------------
/* 12x12 font block lookup -- to add a language: add a generated font block here.
   Dense block (index==0): glyph at glyphs[(cp-first)*24].
   Sparse block: binary search 'index' (sorted Unicode codepoints), glyph at glyphs[i*24]. */
typedef struct {
	unsigned int first, last;
	const unsigned char *glyphs;
	const unsigned short *index;
	unsigned short count;
} FontBlock;

static const FontBlock font_blocks[] = {
	{ KOR_FIRST, KOR_LAST, KOR_GLYPHS, 0, KOR_COUNT },          /* dense Hangul */
	{ 0, 0xFFFF, CJK_GLYPHS, CJK_INDEX, CJK_COUNT },            /* sparse, bsearch (real bounds from CJK_INDEX at runtime) */
};

static const unsigned char* Font_lookup12(unsigned int cp)
{
	u32 b;
	for(b=0; b<sizeof(font_blocks)/sizeof(font_blocks[0]); b++)
	{
		const FontBlock *fb = &font_blocks[b];
		if(fb->index == 0)	/* dense */
		{
			if(cp >= fb->first && cp <= fb->last)
				return &fb->glyphs[(cp - fb->first)*24];
		}
		else			/* sparse */
		{
			u32 lo, hi2;
			if(fb->count == 0 || cp < fb->index[0] || cp > fb->index[fb->count-1])
				continue;
			lo = 0;
			hi2 = fb->count - 1;
			while(lo <= hi2)
			{
				u32 mid = (lo + hi2) >> 1;
				if(fb->index[mid] == cp)
					return &fb->glyphs[mid*24];
				if(fb->index[mid] < cp)
					lo = mid + 1;
				else
				{
					if(mid == 0)
						break;
					hi2 = mid - 1;
				}
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------
void DrawHZText12(char *str, u16 len, u16 x, u16 y, u16 c, u8 isDrawDirect)
{
  u32 i,l,hi=0;
  u32 location;
  u32 cp;
  const unsigned char *g;
	u8 cc,c1,c2,c3;
	u16 *v;
	u16 *p1 = Vcache;
	u16 *p2 = VideoBuffer;
	u16 yy;
	

	if(isDrawDirect)
		v = p2;
	else
		v = p1;

	if(len==0)
		l=strlen(str);
	else
		if(len>strlen(str))
			l=strlen(str);
		else
			l=len;

	if((u16)(len*6)>(u16)(240-x))
		len=(240-x)/6;
    while(hi<l)
    {
		c1 = str[hi];
    	hi++;
    	if(c1<0x80)  //ASCII
    	{
			yy = 240*y;
    		location = c1*12;
    		for(i=0;i<12;i++)
			{
				cc = ASC_DATA[location+i];
				if(cc & 0x01)
					v[x+7+yy]=c;
				if(cc & 0x02)
					v[x+6+yy]=c;
				if(cc & 0x04)
					v[x+5+yy]=c;
				if(cc & 0x08)
					v[x+4+yy]=c;
				if(cc & 0x10)
					v[x+3+yy]=c;
				if(cc & 0x20)
					v[x+2+yy]=c;
				if(cc & 0x40)
					v[x+1+yy]=c;
				if(cc & 0x80)
					v[x+yy]=c;
				yy+=240;
			}		
    		x+=6;
    		continue;
    	}
		else	//UTF-8 multi-byte
		{
    		if((c1 & 0xE0) == 0xC0)			// 2-byte sequence
    		{
    			if(hi >= l)
    				break;					// truncated at end of clip
    			c2 = str[hi];
    			hi++;
    			cp = ((u32)(c1 & 0x1F) << 6) | (c2 & 0x3F);
    		}
    		else if((c1 & 0xF0) == 0xE0)	// 3-byte sequence
    		{
    			if(hi+1 >= l)
    				break;					// truncated at end of clip
    			c2 = str[hi];
    			hi++;
    			c3 = str[hi];
    			hi++;
    			cp = ((u32)(c1 & 0x0F) << 12) | ((u32)(c2 & 0x3F) << 6) | (c3 & 0x3F);
    		}
    		else							// stray continuation / 4-byte lead: skip byte
    		{
    			continue;
    		}

			g = Font_lookup12(cp);
			if(g)
			{
				yy = 240*y;
				for(i=0;i<12;i++)
				{
					cc = g[i*2];
					if(cc & 0x01)
						v[x+7+yy]=c;
					if(cc & 0x02)
						v[x+6+yy]=c;
					if(cc & 0x04)
						v[x+5+yy]=c;
					if(cc & 0x08)
						v[x+4+yy]=c;
					if(cc & 0x10)
						v[x+3+yy]=c;
					if(cc & 0x20)
						v[x+2+yy]=c;
					if(cc & 0x40)
						v[x+1+yy]=c;
					if(cc & 0x80)
						v[x+yy]=c;

					cc = g[i*2+1];
					if(cc & 0x01)
						v[x+15+yy]=c;
					if(cc & 0x02)
						v[x+14+yy]=c;
					if(cc & 0x04)
						v[x+13+yy]=c;
					if(cc & 0x08)
						v[x+12+yy]=c;
					if(cc & 0x10)
						v[x+11+yy]=c;
					if(cc & 0x20)
						v[x+10+yy]=c;
					if(cc & 0x40)
						v[x+9+yy]=c;
					if(cc & 0x80)
						v[x+8+yy]=c;
					yy+=240;
				}
			}
			else	// tofu: hollow rect, cols 1..10, rows 1..10
			{
				yy = 240*(y+1);
				for(i=1;i<=10;i++)
				{
					if(i==1 || i==10)
					{
						u32 xi;
						for(xi=1;xi<=10;xi++)
							v[x+xi+yy]=c;
					}
					else
					{
						v[x+1+yy]=c;
						v[x+10+yy]=c;
					}
					yy+=240;
				}
			}
			x+=12;
		}
	}
}
//---------------------------------------------------------------------------------
void DEBUG_printf(const char *format, ...)
{
    char str[128];
    va_list va;
    va_start(va, format);
    //vasprintf(str, format, va);
    vsprintf(str, format, va);
    va_end(va);

		if(current_y==1)
			{
				
				Clear(0, 0, 240, 160, 0x0000, 1);
			}

    DrawHZText12(str,0,0,current_y, RGB(31,31,31),1);
    
    //free(str);

    current_y += 12;
    if(current_y>150) 
    {
    	wait_btn();
    	current_y=1;
    }
}
//---------------------------------------------------------------------------------
void ShowbootProgress(char *str)
{
	u8 str_len = strlen(str); 	
	Clear(60,160-15,120,15,gl_color_cheat_black,1);	
	DrawHZText12(str,0,(240-str_len*6)/2,160-15,gl_color_text,1);	
}