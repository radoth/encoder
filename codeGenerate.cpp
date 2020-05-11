/* 变长编码程序*/



#include <cstdio>
#include "timeSettings.h"
#include "commonData.h"
#include "presetTables.h"

//为DC系数产生可变长的编码
static bool dcGenerate(sVLCtable *tab,int val)
{
  int absval, size;

  absval = (val<0) ? -val : val; /* 生成绝对值 */

  if (absval>2047 || (mpeg1Flag && absval>255))     //遇到不合法的直流系数，报错
  {
    sprintf(errortext,"DC value out of range (%d)\n",val);
    {error(errortext);return false;}
  }

  /* 计算dct_dc_size */
  size = 0;

  while (absval)
  {
    absval >>= 1;
    size++;
  }

  /*根据表B-12 或B-13为dct_dc_size 产生VLC */
  writeData(tab[size].code,tab[size].len);

  /* 附加固定长度的代码 (dc_dct_differential) */
  if (size!=0)
  {
    if (val>=0)
      absval = val;
    else
      absval = val + (1<<size) - 1; /* val + (2 ^ size) - 1 */
    writeData(absval,size);
  }
  return true;
}

/* 为亮度DC系数产生可变长编码 */
bool dcYGenerate(int val)
{
  return dcGenerate(DClumtab,val);
}

/* 为色度DC系数产生可变长编码 */
bool dcUVGenerate(int val)
{
  return dcGenerate(DCchromtab,val);
}



/*为非帧内方式的块的第一个AC系数进行VLC编码*/
bool acGenerateBegin(int run,int val)
{
  if (run==0 && (val==1 || val==-1)) /* 对这两个条件下的值要区别对待*/
    writeData(2|(val<0),2);
  else
    if(acGenerateElse(run,val,0)==false)
        return false;
  return true;
}

/*对其他的DCT系数进行编码。*/
bool acGenerateElse(int run,int signed_level,int vlcformat)
{
  int level, len;
  VLCtable *ptab = NULL;

  level = (signed_level<0) ? -signed_level : signed_level; /* 对 signed_level 取绝对值 */

  /* 要保证游程和级别（level）有效 */
  if (run<0 || run>63 || level==0 || level>2047 || (mpeg1Flag && level>255))     //在越界时报错
  {
    sprintf(errortext,"AC value out of range (run=%d, signed_level=%d)\n",
      run,signed_level);
    {error(errortext);return false;}
  }

  len = 0;     //初始化长度为0

  if (run<2 && level<41)     //两种可以查表的情况
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
	  if (vlcformat)
      ptab = &dctCodeTab1a[run][level-1];
    else
      ptab = &dctCodeTab1[run][level-1];

    len = ptab->len;
  }
  else if (run<32 && level<6)
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
	  if (vlcformat)
      ptab = &dctCodeTab2a[run-2][level-1];
    else
      ptab = &dctCodeTab2[run-2][level-1];

    len = ptab->len;
  }

  if (len!=0) /* 说明存在一个VLC 编码 */
  {
    writeData(ptab->code,len);
    writeData(signed_level<0,1); /* 设标识 */
  }
  else
  {
        /* 说明对这个中间格式 (run, level) 没有VLC编码:：使用escape编码 */
    writeData(1l,6); /* Escape */
    writeData(run,6); /* 用6 bit 表示游程（run ）*/
    if (mpeg1Flag)
    {
      /* ISO/IEC 11172-2 规定使用 8 或 16 bit 的代码 */
		if (signed_level>127)
        writeData(0,8);
      if (signed_level<-127)
        writeData(128,8);
      writeData(signed_level,8);
    }
    else
    {
      /* ISO/IEC 13818-2 规定使用12 bit 代码根据表B-16 */
        writeData(signed_level,12);
    }
  }
  return true;
}

/* 为macroblock_address_increment 进行VLC编码 */
void addressCodeGenerate(int addrinc)
{
  while (addrinc>33)     //大于33时没有霍夫曼编码
  {
    writeData(0x08,11); /* 宏块转义 */
    addrinc-= 33;
  }

  writeData(addrinctab[addrinc-1].code,addrinctab[addrinc-1].len);     //输出对应的比特流
}

/* 为macroblock_type 进行VLC */
void macroTypeCodeGene(int pict_type,int mb_type)
{
  writeData(mbtypetab[pict_type-1][mb_type].code,     //输出宏块类型对应的比特流
          mbtypetab[pict_type-1][mb_type].len);
}

/* 为motion_code 进行VLC */
void motionCodeGenerate(int motion_code)
{
  int abscode;

  abscode = (motion_code>=0) ? motion_code : -motion_code; /* 对运动编码取绝对值 */
  writeData(motionvectab[abscode].code,motionvectab[abscode].len);
  if (motion_code!=0)
    writeData(motion_code<0,1);  /* 标识, 0为正, 1为负 */
}

/* 为dmvector[t]进行VLC根据表 B-11 */
void DMVectorCodeGene(int dmv)
{
  if (dmv==0)
    writeData(0,1);
  else if (dmv>0)
    writeData(2,2);
  else
    writeData(3,2);
}

/* 为编码块的模式coded_block_pattern 进行VLC，
对于 4:2:2, 4:4:4 图像格式的块不进行 */
void codedBlockPatternCodeGene(int cbp)
{
  writeData(cbptable[cbp].code,cbptable[cbp].len);
}
