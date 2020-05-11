/*  量化和逆量化函数*/

#include "timeSettings.h"
#include <cstdio>
#include "commonData.h"

/* 对MPEG-1图像量化的子函数 */
static void innerSubQuan(short *dst,short *src,int dc_prec,unsigned char *quant_mat,int mquant)
{
    //mpeg1的帧内反量化
  int i, val;

  dst[0] = src[0] << (3-dc_prec);//直流系数反量化
  for (i=1; i<64; i++)//交流系数反量化
  {
    val = (int)(src[i]*quant_mat[i]*mquant)/16;

    if ((val&1)==0 && val!=0)//错误控制
      val+= (val>0) ? -1 : 1;

    //大小限制，饱和时削波
    dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
  }
}

/* 非帧内图像的逆量化 */
static void crossSubQuan(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
 //mpeg1的帧间反量化
  int i, val;

  for (i=0; i<64; i++)
  {
    val = src[i];
    if (val!=0)
    {
      val = (int)((2*val+(val>0 ? 1 : -1))*quant_mat[i]*mquant)/32;

      if ((val&1)==0 && val!=0)//同前，差错控制
        val+= (val>0) ? -1 : 1;
    }
    //同前，饱和时削波
    dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
  }
}

/* 在趋近于0时该量化器的补偿为1/8stepsize
（但DC系数例外）
 */

//帧内量化
int innerQuan(short *src,short *dst,int dc_prec,unsigned char *quant_mat,int mquant)
//输入参数分别是输入和输出数组的指针，直流系数的量化精度控制参数，量化数组和码率的控制参数
{
  int i;
  int x, y, d;

  x = src[0];
  d = 8>>dc_prec; /* intra_dc_mult */
  dst[0] = (x>=0) ? (x+(d>>1))/d : -((-x+(d>>1))/d); /* round(x/d)，即直流系数的量化 */

  for (i=1; i<64; i++)//交流系数的量化
  {
    x = src[i];
    d = quant_mat[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d; /* round(32*x/quant_mat)，量化 */
    d = (3*mquant+2)>>2;
    y = (y+d)/(2*mquant); /* (y+0.75*mquant) / (2*mquant)，调整 */

/*若超过上限则进行削波处理*/
    if (y > 255)//对量化结果进行限制，以节约bit，mpeg1和mpeg2有不同的量化限制
    {
      if (mpeg1Flag)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    dst[i] = (x>=0) ? y : -y;//量化结果取绝对值
  }

  return 1;
}

/*接下来的quant_non_intra是对非帧内方式的量化*/
//帧间量化，没有直流系数的量化，所以参数比帧内量化少dc_prec
int crossQuan(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
  int i;
  int x, y, d;
  int nzflag;

  nzflag = 0;

  for (i=0; i<64; i++)//对所有的帧间残差的dct系数进行量化
  {
    x = src[i];
    d = quant_mat[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d; /* round(32*x/quant_mat) */
    y /= (2*mquant);//量化过程

    /* clip to syntax limits */
    if (y > 255)
    {
      if (mpeg1Flag)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    if ((dst[i] = (x>=0 ? y : -y)) != 0)//如果量化结果不为，nzflag置1
      nzflag=1;
  }

  return nzflag;//返回nzflag，如果它为0，整个量化结果是数组0
}

/* 帧内图像的逆量化 */
void innerIQuan(short *src,short *dst,int dc_prec,unsigned char *quant_mat,int mquant)
{
    //帧内反量化
  int i, val, sum;

/*由于要求MPEG-2向下兼容，因此对于MPEG-1量化方式，采取相应的算法*/
  if (mpeg1Flag)//如果是mpeg1，调用mpeg1的反量化函数
    innerSubQuan(src,dst,dc_prec,quant_mat,mquant);
  else
      //mpeg2的量化
  {
    sum = dst[0] = src[0] << (3-dc_prec);
    //直流系数的反量化，量化误差的范围(-(8>>dc_prec+1),(8>>dc_prec+1)]并且一般均匀分布
    for (i=1; i<64; i++)
    {
      val = (int)(src[i]*quant_mat[i]*mquant)/16;
      //反量化
      sum+= dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
      //对反量化的结果进行限制[-2048,2047]，来节约bit
    }

     /*不匹配控制 */
    if ((sum&1)==0)//如果sum的最后一位是0
      dst[63]^= 1;//此时dst[63]的最后一位取反
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


