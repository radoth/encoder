/* 变长编码程序*/

#include <cstdio>
#include "timeSettings.h"
#include "commonData.h"
#include "presetTables.h"

//为DC系数产生可变长的编码
static bool dcGenerate(currentVLCt *tab,int inputValue)
{
  int absoluteValue = (inputValue<0) ? -inputValue : inputValue; /* 生成绝对值 */

  if (absoluteValue>2047 || (mpeg1Flag && absoluteValue>255))     //遇到不合法的直流系数，报错
  {
    sprintf(errortext,"DC value out of range (%d)\n",inputValue);
    {error(errortext);return false;}
  }

  /* 计算dct_dc_size */
  int dctDCSize = 0;

  while (absoluteValue)
  {
    absoluteValue >>= 1;
    dctDCSize++;
  }

  /*根据表B-12 或B-13为dct_dc_size 产生VLC */
  writeData(tab[dctDCSize].code,tab[dctDCSize].len);

  /* 附加固定长度的代码 (dc_dct_differential) */
  if (dctDCSize!=0)
  {
    if (inputValue>=0)
      absoluteValue = inputValue;
    else
      absoluteValue = inputValue + (1<<dctDCSize) - 1; /* val + (2 ^ size) - 1 */
    writeData(absoluteValue,dctDCSize);
  }
  return true;
}

/* 为亮度DC系数产生可变长编码 */
bool dcYGenerate(int inputValue)
{
  return dcGenerate(DClumtab,inputValue);
}

/* 为色度DC系数产生可变长编码 */
bool dcUVGenerate(int inputValue)
{
  return dcGenerate(DCchromtab,inputValue);
}



/*为非帧内方式的块的第一个AC系数进行VLC编码*/
bool acGenerateBegin(int runCode,int currentValue)
{
  if (runCode==0 && (currentValue==1 || currentValue==-1)) /* 对这两个条件下的值要区别对待*/
    writeData(2|(currentValue<0),2);
  else
    if(acGenerateElse(runCode,currentValue,0)==false)
        return false;
  return true;
}

/*对其他的DCT系数进行编码。*/
bool acGenerateElse(int runCode,int checkLevel,int VLCFlag)
{
  int level, len;
  VLCtable *ptab = NULL;

  level = (checkLevel<0) ? -checkLevel : checkLevel; /* 对 signed_level 取绝对值 */

  /* 要保证游程和级别（level）有效 */
  if (runCode<0 || runCode>63 || level==0 || level>2047 || (mpeg1Flag && level>255))     //在越界时报错
  {
    sprintf(errortext,"AC value out of range (run=%d, signed_level=%d)\n",
      runCode,checkLevel);
    {error(errortext);return false;}
  }

  len = 0;     //初始化长度为0

  if (runCode<2 && level<41)     //两种可以查表的情况
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
      if (VLCFlag)
      ptab = &dctCodeTab1a[runCode][level-1];
    else
      ptab = &dctCodeTab1[runCode][level-1];

    len = ptab->len;
  }
  else if (runCode<32 && level<6)
  {
    /* 根据vlcformat 选择采用表B-14 还是 B-15 */
      if (VLCFlag)
      ptab = &dctCodeTab2a[runCode-2][level-1];
    else
      ptab = &dctCodeTab2[runCode-2][level-1];

    len = ptab->len;
  }

  if (len!=0) /* 说明存在一个VLC 编码 */
  {
    writeData(ptab->code,len);
    writeData(checkLevel<0,1); /* 设标识 */
  }
  else
  {
        /* 说明对这个中间格式 (run, level) 没有VLC编码:：使用escape编码 */
    writeData(1l,6); /* Escape */
    writeData(runCode,6); /* 用6 bit 表示游程（run ）*/
    if (mpeg1Flag)
    {
      /* ISO/IEC 11172-2 规定使用 8 或 16 bit 的代码 */
        if (checkLevel>127)
        writeData(0,8);
      if (checkLevel<-127)
        writeData(128,8);
      writeData(checkLevel,8);
    }
    else
    {
      /* ISO/IEC 13818-2 规定使用12 bit 代码根据表B-16 */
        writeData(checkLevel,12);
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
