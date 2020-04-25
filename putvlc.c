/* putvlc.c, 变长编码程序*/



#include <stdio.h>

#include "config.h"
#include "global.h"
#include "vlc.h"

/* 函数声明 */
static void putDC _ANSI_ARGS_((sVLCtable *tab, int val));

/* 为亮度DC系数产生可变长编码 */
void putDClum(val)
int val;
{
  putDC(DClumtab,val);
}

/* 为色度DC系数产生可变长编码 */
void putDCchrom(val)
int val;
{
  putDC(DCchromtab,val);
}

//为DC系数产生可变长的编码
static void putDC(tab,val)
sVLCtable *tab;
int val;
{
  int absval, size;

  absval = (val<0) ? -val : val; /* abs(val) */

  if (absval>2047 || (mpeg1 && absval>255))
  {
    sprintf(errortext,"DC value out of range (%d)\n",val);
    error(errortext);
  }

  /* 计算dct_dc_size */
  size = 0;

  while (absval)
  {
    absval >>= 1;
    size++;
  }

  /*根据表B-12 或B-13为dct_dc_size 产生VLC */
  putbits(tab[size].code,tab[size].len);

  /* 附加固定长度的代码 (dc_dct_differential) */
  if (size!=0)
  {
    if (val>=0)
      absval = val;
    else
      absval = val + (1<<size) - 1; /* val + (2 ^ size) - 1 */
    putbits(absval,size);
  }
}

/*为非帧内方式的块的第一个AC系数进行VLC编码*/
void putACfirst(run,val)
int run,val;
{
  if (run==0 && (val==1 || val==-1)) /* 对这两个条件下的值要区别对待*/
    putbits(2|(val<0),2); 
  else
    putAC(run,val,0); 
}

/*对其他的DCT系数进行编码。*/
void putAC(run,signed_level,vlcformat)
int run,signed_level,vlcformat;
{
  int level, len;
  VLCtable *ptab = NULL;

  level = (signed_level<0) ? -signed_level : signed_level; /* abs(signed_level) */

  /* 要保证游程和级别（level）有效 */
  if (run<0 || run>63 || level==0 || level>2047 || (mpeg1 && level>255))
  {
    sprintf(errortext,"AC value out of range (run=%d, signed_level=%d)\n",
      run,signed_level);
    error(errortext);
  }

  len = 0;

  if (run<2 && level<41)
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
	  if (vlcformat)
      ptab = &dct_code_tab1a[run][level-1];
    else
      ptab = &dct_code_tab1[run][level-1];

    len = ptab->len;
  }
  else if (run<32 && level<6)
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
	  if (vlcformat)
      ptab = &dct_code_tab2a[run-2][level-1];
    else
      ptab = &dct_code_tab2[run-2][level-1];

    len = ptab->len;
  }

  if (len!=0) /* 说明存在一个VLC 编码 */
  {
    putbits(ptab->code,len);
    putbits(signed_level<0,1); /* 设标识 */
  }
  else
  {
        /* 说明对这个中间格式 (run, level) 没有VLC编码:：使用escape编码 */
    putbits(1l,6); /* Escape */
    putbits(run,6); /* 用6 bit 表示游程（run ）*/
    if (mpeg1)
    {
      /* ISO/IEC 11172-2 规定使用 8 或 16 bit 的代码 */
		if (signed_level>127)
        putbits(0,8);
      if (signed_level<-127)
        putbits(128,8);
      putbits(signed_level,8);
    }
    else
    {
      /* ISO/IEC 13818-2 规定使用12 bit 代码根据表B-16 */
		putbits(signed_level,12);
    }
  }
}

/* 为macroblock_address_increment 进行VLC编码 */
void putaddrinc(addrinc)
int addrinc;
{
  while (addrinc>33)
  {
    putbits(0x08,11); /* macroblock_escape */
    addrinc-= 33;
  }

  putbits(addrinctab[addrinc-1].code,addrinctab[addrinc-1].len);
}

/* 为macroblock_type 进行VLC */
void putmbtype(pict_type,mb_type)
int pict_type,mb_type;
{
  putbits(mbtypetab[pict_type-1][mb_type].code,
          mbtypetab[pict_type-1][mb_type].len);
}

/* 为motion_code 进行VLC */
void putmotioncode(motion_code)
int motion_code;
{
  int abscode;

  abscode = (motion_code>=0) ? motion_code : -motion_code; /* abs(motion_code) */
  putbits(motionvectab[abscode].code,motionvectab[abscode].len);
  if (motion_code!=0)
    putbits(motion_code<0,1);  /* 标识, 0为正, 1为负 */
}

/* 为dmvector[t]进行VLC根据表 B-11 */
void putdmv(dmv)
int dmv;
{
  if (dmv==0)
    putbits(0,1);
  else if (dmv>0)
    putbits(2,2);
  else
    putbits(3,2);
}

/* 为编码块的模式coded_block_pattern 进行VLC，
对于 4:2:2, 4:4:4 图像格式的块不进行 */
void putcbp(cbp)
int cbp;
{
  putbits(cbptable[cbp].code,cbptable[cbp].len);
}
