/*  量化和逆量化函数*/

#include "timeSettings.h"
#include <cstdio>
#include "commonData.h"


static void innerSubQuan(short *dst,short *src,int dc_prec,unsigned char *quant_mat,int mquant)
{
  int i, val;

  dst[0] = src[0] << (3-dc_prec);
  for (i=1; i<64; i++)
  {
    val = (int)(src[i]*quant_mat[i]*mquant)/16;

    if ((val&1)==0 && val!=0)
      val+= (val>0) ? -1 : 1;

    dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
  }
}

static void crossSubQuan(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
  int i, val;

  for (i=0; i<64; i++)
  {
    val = src[i];
    if (val!=0)
    {
      val = (int)((2*val+(val>0 ? 1 : -1))*quant_mat[i]*mquant)/32;

      if ((val&1)==0 && val!=0)
        val+= (val>0) ? -1 : 1;
    }
    dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
  }
}

int innerQuan(short *source,short *destination,int dcPrec,unsigned char *quantizationMatrix,int MQuantFractor)
{
  int x = source[0];
  int d = 8>>dcPrec;
  destination[0] = (x>=0) ? (x+(d>>1))/d : -((-x+(d>>1))/d);
  int y;
  for (int i=1; i<64; i++)
  {
    x = source[i];
    d = quantizationMatrix[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d;
    d = (3*MQuantFractor+2)>>2;
    y = (y+d)/(2*MQuantFractor);


    if (y > 255)
    {
      if (mpeg1Flag)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    destination[i] = (x>=0) ? y : -y;
  }

  return 1;
}


int crossQuan(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
  int i;
  int x, y, d;
  int nzflag;

  nzflag = 0;

  for (i=0; i<64; i++)
  {
    x = src[i];
    d = quant_mat[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d;
    y /= (2*mquant);

    if (y > 255)
    {
      if (mpeg1Flag)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    if ((dst[i] = (x>=0 ? y : -y)) != 0)
      nzflag=1;
  }

  return nzflag;
}

void innerIQuan(short *current,short *destiantion,int dcPrec,unsigned char *quantizaitonMatrix,int MQuantFractor)
{
  if (mpeg1Flag)
    innerSubQuan(current,destiantion,dcPrec,quantizaitonMatrix,MQuantFractor);
  else
  {
    int temp = destiantion[0] = current[0] << (3-dcPrec);
    for (int i=1; i<64; i++)
    {
      int val = (int)(current[i]*quantizaitonMatrix[i]*MQuantFractor)/16;
      temp+= destiantion[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
    }
    if ((temp&1)==0)
      destiantion[63]^= 1;
  }
}

/* 非帧内图像的逆量化 */
void outerIQuan(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
    //帧间反量化
  int i, val, sum;

  if (mpeg1Flag)//如果是mpeg1,调用mpeg1的帧间反量化函数
    crossSubQuan(src,dst,quant_mat,mquant);
  else
      //mpeg2的反量化
  {
    sum = 0;
    for (i=0; i<64; i++)
    {
      val = src[i];//反量化操作
      if (val!=0)
        val = (int)((2*val+(val>0 ? 1 : -1))*quant_mat[i]*mquant)/32;
      sum+= dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
      //值的大小限制
    }

   
    if ((sum&1)==0)
      dst[63]^= 1;//错误控制，mpeg1和mpeg2的错误控制不同
  }
}


