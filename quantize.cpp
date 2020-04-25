/* quantize.c, 量化和逆量化函数*/


#include <cstdio>
#include "config.h"
#include "global.h"

static void iquant1_intra (short *src, short *dst,
  int dc_prec, unsigned char *quant_mat, int mquant);
static void iquant1_non_intra (short *src, short *dst,
  unsigned char *quant_mat, int mquant);

/* 在趋近于0时该量化器的补偿为1/8stepsize
（但DC系数例外）
 */

int quant_intra(short *src,short *dst,int dc_prec,unsigned char *quant_mat,int mquant)
{
  int i;
  int x, y, d;

  x = src[0];
  d = 8>>dc_prec; /* intra_dc_mult */
  dst[0] = (x>=0) ? (x+(d>>1))/d : -((-x+(d>>1))/d); /* round(x/d) */

  for (i=1; i<64; i++)
  {
    x = src[i];
    d = quant_mat[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d; /* round(32*x/quant_mat) */
    d = (3*mquant+2)>>2;
    y = (y+d)/(2*mquant); /* (y+0.75*mquant) / (2*mquant) */

/*若超过上限则进行削波处理*/
    if (y > 255)
    {
      if (mpeg1)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    dst[i] = (x>=0) ? y : -y;

#if 0
    if (x<0)
      x = -x;
    d = mquant*quant_mat[i];
    y = (16*x + ((3*d)>>3)) / d;
    dst[i] = (src[i]<0) ? -y : y;
#endif
  }

  return 1;
}

/*接下来的quant_non_intra是对非帧内方式的量化*/

int quant_non_intra(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
  int i;
  int x, y, d;
  int nzflag;

  nzflag = 0;

  for (i=0; i<64; i++)
  {
    x = src[i];
    d = quant_mat[i];
    y = (32*(x>=0 ? x : -x) + (d>>1))/d; /* round(32*x/quant_mat) */
    y /= (2*mquant);

    /* clip to syntax limits */
    if (y > 255)
    {
      if (mpeg1)
        y = 255;
      else if (y > 2047)
        y = 2047;
    }

    if ((dst[i] = (x>=0 ? y : -y)) != 0)
      nzflag=1;
  }

  return nzflag;
}

/* 帧内图像的逆量化 */
void iquant_intra(short *src,short *dst,int dc_prec,unsigned char *quant_mat,int mquant)
{
  int i, val, sum;

/*由于要求MPEG-2向下兼容，因此对于MPEG-1量化方式，采取相应的算法*/
  if (mpeg1)
    iquant1_intra(src,dst,dc_prec,quant_mat,mquant);
  else
  {
    sum = dst[0] = src[0] << (3-dc_prec);
    for (i=1; i<64; i++)
    {
      val = (int)(src[i]*quant_mat[i]*mquant)/16;
      sum+= dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
    }

     /*不匹配控制 */
    if ((sum&1)==0)
      dst[63]^= 1;
  }
}

/* 非帧内图像的逆量化 */
void iquant_non_intra(short *src,short *dst,unsigned char *quant_mat,int mquant)
{
  int i, val, sum;

  if (mpeg1)
    iquant1_non_intra(src,dst,quant_mat,mquant);
  else
  {
    sum = 0;
    for (i=0; i<64; i++)
    {
      val = src[i];
      if (val!=0)
        val = (int)((2*val+(val>0 ? 1 : -1))*quant_mat[i]*mquant)/32;
      sum+= dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
    }

   
    if ((sum&1)==0)
      dst[63]^= 1;
  }
}

/* 对MPEG-1图像量化的子函数 */
static void iquant1_intra(short *src,short *dst,int dc_prec,unsigned char *quant_mat,int mquant)
{
  int i, val;

  dst[0] = src[0] << (3-dc_prec);
  for (i=1; i<64; i++)
  {
    val = (int)(src[i]*quant_mat[i]*mquant)/16;

    if ((val&1)==0 && val!=0)
      val+= (val>0) ? -1 : 1;

    /* saturation */
    dst[i] = (val>2047) ? 2047 : ((val<-2048) ? -2048 : val);
  }
}

/* 非帧内图像的逆量化 */
static void iquant1_non_intra(short *src,short *dst,unsigned char *quant_mat,int mquant)
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
