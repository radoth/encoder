/* 块和运动向量编码函数*/

#include "timeSettings.h"
#include <cstdio>
#include "commonData.h"


bool innerBlockCodeCtrl(short *current,int count)
{
  int altORZScan;
  int fracNumber = current[0] - dctPredictionatDC[count];
  dctPredictionatDC[count] = current[0];
  if (!count)
    {
      if(!dcYGenerate(fracNumber))
        return false;
  }
  else
    {
        if(!dcUVGenerate(fracNumber))
        return false;
    }
  int runCodeLength = 0;
  for (int n=1; n<64; n++)
  {
    altORZScan = current[(altscan ? alternateScanTable : ZZScanTable)[n]];
    if (altORZScan!=0)
    {
      if(acGenerateElse(runCodeLength,altORZScan,intravlc)==false)
          return false;
      runCodeLength = 0;
    }
    else
      runCodeLength++;
  }
  if (intravlc)
    writeData(6,4);
  else
    writeData(2,2);
  return true;
}

bool crossBlockCodeCtrl(short *current)
{

  int runCodeLength = 0;
  int firststartPoint = 1;
  int altORScan;
  for (int n=0; n<64; n++)
  {
    altORScan = current[(altscan ? alternateScanTable : ZZScanTable)[n]];

    if (altORScan!=0)
    {
      if (firststartPoint)
      {
        if(acGenerateBegin(runCodeLength,altORScan)==false)
            return false;
        firststartPoint = 0;
      }
      else
        if(acGenerateElse(runCodeLength,altORScan,0)==false)
            return false;

      runCodeLength = 0;
    }
    else
      runCodeLength++;
  }

  writeData(2,2);

  return true;
}

void motionVectorCodeCtrl(int dmv,int f_code)
{
  int r_size, f, vmin, vmax, dv, temp, motion_code, motion_residual;

  r_size = f_code - 1;
  f = 1<<r_size;
  vmin = -16*f;
  vmax = 16*f - 1;
  dv = 32*f;
  if (dmv>vmax)
    dmv-= dv;
  else if (dmv<vmin)
    dmv+= dv;
  if (dmv<vmin || dmv>vmax)
    if (!quiet)
      {
        warningTextGlobal.append("运动类型无效");
    }

  temp = ((dmv<0) ? -dmv : dmv) + f - 1;
  motion_code = temp>>r_size;
  if (dmv<0)
    motion_code = -motion_code;
  motion_residual = temp & (f-1);

  motionCodeGenerate(motion_code);

  if (r_size!=0 && motion_code!=0)
    writeData(motion_residual,r_size);
}
