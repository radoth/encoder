#include <cstdio>
#include "timeSettings.h"
#include "commonData.h"
#include "presetTables.h"
static bool dcGenerate(currentVLCt *current,int inputValue)
{
  int absoluteValue = (inputValue<0) ? -inputValue : inputValue;

  if (absoluteValue>2047 || (mpeg1Flag && absoluteValue>255))
  {
    sprintf(errortext,"DC value out of range (%d)\n",inputValue);
    {error(errortext);return false;}
  }
  int dctDCSize = 0;

  while (absoluteValue)
  {
    absoluteValue >>= 1;
    dctDCSize++;
  }
  writeData(current[dctDCSize].code,current[dctDCSize].len);
  if (dctDCSize!=0)
  {
    if (inputValue>=0)
      absoluteValue = inputValue;
    else
      absoluteValue = inputValue + (1<<dctDCSize) - 1;
    writeData(absoluteValue,dctDCSize);
  }
  return true;
}
bool dcYGenerate(int inputValue)
{
  return dcGenerate(DClumtab,inputValue);
}
bool dcUVGenerate(int inputValue)
{
  return dcGenerate(DCchromtab,inputValue);
}
bool acGenerateBegin(int runCode,int currentValue)
{
  if (runCode==0 && (currentValue==1 || currentValue==-1))
    writeData(2|(currentValue<0),2);
  else
    if(acGenerateElse(runCode,currentValue,0)==false)
        return false;
  return true;
}
bool acGenerateElse(int runCode,int checkLevel,int VLCFlag)
{

  int AClevel = (checkLevel<0) ? -checkLevel : checkLevel;
  if (runCode<0 || runCode>63 || AClevel==0 || AClevel>2047 || (mpeg1Flag && AClevel>255))
  {
    sprintf(errortext,"AC value out of range (run=%d, signed_level=%d)\n",
      runCode,checkLevel);
    {error(errortext);return false;}
  }
  int runCodelength = 0;
  VLCtable *ptabselectFractor;
  if (runCode<2 && AClevel<41)
  {
      if (VLCFlag)
      ptabselectFractor = &dctCodeTab1a[runCode][AClevel-1];
    else
      ptabselectFractor = &dctCodeTab1[runCode][AClevel-1];

    runCodelength = ptabselectFractor->len;
  }
  else if (runCode<32 && AClevel<6)
  {
      if (VLCFlag)
      ptabselectFractor = &dctCodeTab2a[runCode-2][AClevel-1];
    else
      ptabselectFractor = &dctCodeTab2[runCode-2][AClevel-1];

    runCodelength = ptabselectFractor->len;
  }

  if (runCodelength!=0)
  {
    writeData(ptabselectFractor->code,runCodelength);
    writeData(checkLevel<0,1);
  }
  else
  {
    writeData(1l,6);
    writeData(runCode,6);
    if (mpeg1Flag)
    {
        if (checkLevel>127)
        writeData(0,8);
      if (checkLevel<-127)
        writeData(128,8);
      writeData(checkLevel,8);
    }
    else
    {
        writeData(checkLevel,12);
    }
  }
  return true;
}
void addressCodeGenerate(int addrinc)
{
  while (addrinc>33)
  {
    writeData(0x08,11);
    addrinc-= 33;
  }

  writeData(addrinctab[addrinc-1].code,addrinctab[addrinc-1].len);
}
void macroTypeCodeGene(int pict_type,int mb_type)
{
  writeData(mbtypetab[pict_type-1][mb_type].code,
          mbtypetab[pict_type-1][mb_type].len);
}
void motionCodeGenerate(int motion_code)
{
  int abscode;

  abscode = (motion_code>=0) ? motion_code : -motion_code;
  writeData(motionvectab[abscode].code,motionvectab[abscode].len);
  if (motion_code!=0)
    writeData(motion_code<0,1);
}
void DMVectorCodeGene(int dmv)
{
  if (dmv==0)
    writeData(0,1);
  else if (dmv>0)
    writeData(2,2);
  else
    writeData(3,2);
}
void codedBlockPatternCodeGene(int cbp)
{
  writeData(cbptable[cbp].code,cbptable[cbp].len);
}
