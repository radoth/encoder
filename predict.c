/* predict.c, 运动补偿预测*/



#include <stdio.h>
#include "config.h"
#include "global.h"

/* private prototypes */
static void predict_mb _ANSI_ARGS_((
  unsigned char *oldref[], unsigned char *newref[], unsigned char *cur[],
  int lx, int bx, int by, int pict_type, int pict_struct, int mb_type,
  int motion_type, int secondfield,
  int PMV[2][2][2], int mv_field_sel[2][2], int dmvector[2]));

static void pred _ANSI_ARGS_((unsigned char *src[], int sfield,
  unsigned char *dst[], int dfield,
  int lx, int w, int h, int x, int y, int dx, int dy, int addflag));

static void pred_comp _ANSI_ARGS_((unsigned char *src, unsigned char *dst,
  int lx, int w, int h, int x, int y, int dx, int dy, int addflag));

static void calc_DMV _ANSI_ARGS_((int DMV[][2], int *dmvector, int mvx,
  int mvy));

static void clearblock _ANSI_ARGS_((unsigned char *cur[], int i0, int j0));


/* 为完整的图像形成预测
 *
    reff: 前向参考图像帧
    refb: 后向参考图像帧
	cur: 目标帧（即当前要预测的帧）
	secondfield: 预测帧的第二个场图像
	mbi: 指向宏块信息结构体的指针

 */

void predict(reff,refb,cur,secondfield,mbi)
unsigned char *reff[],*refb[],*cur[3];
int secondfield;
struct mbinfo *mbi;
{
  int i, j, k;

  k = 0;

  /* 对所有的宏块进行循环 */
  for (j=0; j<height2; j+=16)
    for (i=0; i<width; i+=16)
    {
      predict_mb(reff,refb,cur,width,i,j,pict_type,pict_struct,
                 mbi[k].mb_type,mbi[k].motion_type,secondfield,
                 mbi[k].MV,mbi[k].mv_field_sel,mbi[k].dmvector);

      k++;
    }
}

/* 为一个宏块形成预测
 *
 	oldref: 用来进行前向预测的参考帧
	newref: 用来进行后向预测的参考帧
	cur: 目标帧（即当前要预测的帧）
	lx: 帧的宽度（与全局变量"width"一致）
	bx、by: 要被预测的宏块的图像坐标（可以是场，也可以是帧）
	pict_type: 图像类型，就是I、P或B
	pict_struct: 图像的结构，就是帧图像（FRAME_PICTURE），上部分的场图像
	（TOP_FIELD）和下部分的场图像（BOTTOM_FIELD）
	
	mb_type: 宏块类型，可以是前向（MB_FORWARD）,也可以是后向（MB_BACKWARD）
	或是帧内（MB_INTRA）宏块
	
	  motion_type: 运动类型，主要用值MC_FRAME, MC_FIELD、MC_16X8、MC_DMV表示
	secondfield: 预测帧的第二个场图像
	PMV[2][2][2]: 运动向量（以半像素图像坐标为单位）
	mv_field_sel[2][2]: 垂直运动场选择（针对场预测）
	dmvector: 差分运动向量

 */

static void predict_mb(oldref,newref,cur,lx,bx,by,pict_type,pict_struct,
  mb_type,motion_type,secondfield,PMV,mv_field_sel,dmvector)
unsigned char *oldref[],*newref[],*cur[];
int lx;
int bx,by;
int pict_type;
int pict_struct;
int mb_type;
int motion_type;
int secondfield;
int PMV[2][2][2], mv_field_sel[2][2], dmvector[2];
{
  int addflag, currentfield;
  unsigned char **predframe;
  int DMV[2][2];

  if (mb_type&MB_INTRA)
  {
    clearblock(cur,bx,by);
    return;
  }

  addflag = 0;  /* 第一个预测值要存储起来，然后加上第二个并取平均 */

  if ((mb_type & MB_FORWARD) || (pict_type==P_TYPE))
  {
    /* 前向预测，包括P图像里面的0运动向量 */
    if (pict_struct==FRAME_PICTURE)
    {
      /* 帧图像 */


      if ((motion_type==MC_FRAME) || !(mb_type & MB_FORWARD))
      {
  /* 帧图像中，基于帧的预测 */
		  pred(oldref,0,cur,0,
          lx,16,16,bx,by,PMV[0][0][0],PMV[0][0][1],0);
      }
      else if (motion_type==MC_FIELD)
      {
         /* 帧图像中，基于场的预测-based prediction in frame picture
         *并非按照垂直坐标的比例
         */


        /* 上部分场图像的预测 */
		  pred(oldref,mv_field_sel[0][0],cur,0,
          lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);

        /* 下部分场图像的预测 */
		  pred(oldref,mv_field_sel[1][0],cur,1,
          lx<<1,16,8,bx,by>>1,PMV[1][0][0],PMV[1][0][1]>>1,0);
      }
      else if (motion_type==MC_DMV)
      {
               /* DMV预测 */
        /* 计算衍生的运动向量 */

        calc_DMV(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);

         /* 用上部分场图像预测上部分场图像 */
        pred(oldref,0,cur,0,lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);
        /* 用下部分场图像预测下部分场图像 */
        pred(oldref,1,cur,1,lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);
        /*用下部分场图像预测下部分场图像，并加到原来的场图像上*/
        pred(oldref,1,cur,0,lx<<1,16,8,bx,by>>1,DMV[0][0],DMV[0][1],1);
        /*用上部分场图像预测下部分场图像，并加到原来的场图像上*/
        pred(oldref,0,cur,1,lx<<1,16,8,bx,by>>1,DMV[1][0],DMV[1][1],1);

      }
      else
      {
         /* 无效的运动类型 */
        if (!quiet)
          fprintf(stderr,"invalid motion_type\n");
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
       /* 场图像 */

      currentfield = (pict_struct==BOTTOM_FIELD);

   /* 判断用哪一帧来进行预测*/
      if ((pict_type==P_TYPE) && secondfield
          && (currentfield!=mv_field_sel[0][0]))
        predframe = newref; /* 同一帧 */
      else
        predframe = oldref;  /* 前一帧 */

      if ((motion_type==MC_FIELD) || !(mb_type & MB_FORWARD))
      {
        /* 场图像中基于场的预测*/
		  pred(predframe,mv_field_sel[0][0],cur,currentfield,
          lx<<1,16,16,bx,by,PMV[0][0][0],PMV[0][0][1],0);
      }
      else if (motion_type==MC_16X8)
      {
           /* 在场图像中的16×8 运动补偿预测*/
        /* 上半部分 */
        
        pred(predframe,mv_field_sel[0][0],cur,currentfield,
          lx<<1,16,8,bx,by,PMV[0][0][0],PMV[0][0][1],0);

        /* 判断用哪一帧图像来预测下半部分 */
        if ((pict_type==P_TYPE) && secondfield
            && (currentfield!=mv_field_sel[1][0]))
          predframe = newref; /* same frame */
        else
          predframe = oldref; /* previous frame */

        /* lower half */
        pred(predframe,mv_field_sel[1][0],cur,currentfield,
          lx<<1,16,8,bx,by+8,PMV[1][0][0],PMV[1][0][1],0);
      }
      else if (motion_type==MC_DMV)
      {
         /* DMV预测 */

        if (secondfield)
          predframe = newref; /* same frame */
        else
          predframe = oldref; /* previous frame */

        calc_DMV(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]);

        pred(oldref,currentfield,cur,currentfield,
          lx<<1,16,16,bx,by,PMV[0][0][0],PMV[0][0][1],0);

        pred(predframe,!currentfield,cur,currentfield,
          lx<<1,16,16,bx,by,DMV[0][0],DMV[0][1],1);
      }
      else
      {
        if (!quiet)
          fprintf(stderr,"invalid motion_type\n");
      }
    }
    addflag = 1; /* 下一个预测值将与该值取平均 */
  }

  if (mb_type & MB_BACKWARD)
  {
        /* 后向预测 */

    if (pict_struct==FRAME_PICTURE)
    {
      /* frame picture */

      if (motion_type==MC_FRAME)
      {
        pred(newref,0,cur,0,
          lx,16,16,bx,by,PMV[0][1][0],PMV[0][1][1],addflag);
      }
      else
      {
       
        /* top field prediction */
        pred(newref,mv_field_sel[0][1],cur,0,
          lx<<1,16,8,bx,by>>1,PMV[0][1][0],PMV[0][1][1]>>1,addflag);

        /* bottom field prediction */
        pred(newref,mv_field_sel[1][1],cur,1,
          lx<<1,16,8,bx,by>>1,PMV[1][1][0],PMV[1][1][1]>>1,addflag);
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
      /* field picture */

      currentfield = (pict_struct==BOTTOM_FIELD);

      if (motion_type==MC_FIELD)
      {
        pred(newref,mv_field_sel[0][1],cur,currentfield,
          lx<<1,16,16,bx,by,PMV[0][1][0],PMV[0][1][1],addflag);
      }
      else if (motion_type==MC_16X8)
      {
        pred(newref,mv_field_sel[0][1],cur,currentfield,
          lx<<1,16,8,bx,by,PMV[0][1][0],PMV[0][1][1],addflag);

        /* lower half */
        pred(newref,mv_field_sel[1][1],cur,currentfield,
          lx<<1,16,8,bx,by+8,PMV[1][1][0],PMV[1][1][1],addflag);
      }
      else
      {
        /* invalid motion_type in field picture */
        if (!quiet)
          fprintf(stderr,"invalid motion_type\n");
      }
    }
  }
}

/* 预测矩形块
 *
 * src:     源帧(Y,U,V)
 * sfield:  源场选择(0: 帧或上半场, 1: 下半场)
 * dst:     目标帧(Y,U,V)
 * dfield:  目标场选择
 *
 * 下面的数值是亮度图像的参数
 * lx:      相邻像素的垂直距离
 * w,h:     块的宽度和高度
 * x,y:     目标块的坐标
 * dx,dy:   半像素运动向量
 * addflag: 存储或加预测
 */
static void pred(src,sfield,dst,dfield,lx,w,h,x,y,dx,dy,addflag)
unsigned char *src[];
int sfield;
unsigned char *dst[];
int dfield;
int lx;
int w, h;
int x, y;
int dx, dy;
int addflag;
{
  int cc;

  for (cc=0; cc<3; cc++)
  {
    if (cc==1)
    {
      /* scale for color components */
      if (chroma_format==CHROMA420)
      {
        /* vertical */
        h >>= 1; y >>= 1; dy /= 2;
      }
      if (chroma_format!=CHROMA444)
      {
        /* horizontal */
        w >>= 1; x >>= 1; dx /= 2;
        lx >>= 1;
      }
    }
    pred_comp(src[cc]+(sfield?lx>>1:0),dst[cc]+(dfield?lx>>1:0),
      lx,w,h,x,y,dx,dy,addflag);
  }
}

/* 低等级预测
 *
 * src:     源帧(Y,U,V)
 * dst:     目标帧(Y,U,V)
 *
 * lx:      相邻像素的垂直距离
 * w,h:     块的宽度和高度
 * x,y:     目标块的坐标
 * dx,dy:   半像素运动向量
 * addflag: 存储或加预测
 */


static void pred_comp(src,dst,lx,w,h,x,y,dx,dy,addflag)
unsigned char *src;
unsigned char *dst;
int lx;
int w, h;
int x, y;
int dx, dy;
int addflag;
{
  int xint, xh, yint, yh;
  int i, j;
  unsigned char *s, *d;

  /* half pel scaling */
  xint = dx>>1; /* integer part */
  xh = dx & 1;  /* half pel flag */
  yint = dy>>1;
  yh = dy & 1;

  /* origins */
  s = src + lx*(y+yint) + (x+xint); /* motion vector */
  d = dst + lx*y + x;

  if (!xh && !yh)
    if (addflag)
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (unsigned int)(d[i]+s[i]+1)>>1;
        s+= lx;
        d+= lx;
      }
    else
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = s[i];
        s+= lx;
        d+= lx;
      }
  else if (!xh && yh)
    if (addflag)
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (d[i] + ((unsigned int)(s[i]+s[i+lx]+1)>>1)+1)>>1;
        s+= lx;
        d+= lx;
      }
    else
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (unsigned int)(s[i]+s[i+lx]+1)>>1;
        s+= lx;
        d+= lx;
      }
  else if (xh && !yh)
    if (addflag)
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (d[i] + ((unsigned int)(s[i]+s[i+1]+1)>>1)+1)>>1;
        s+= lx;
        d+= lx;
      }
    else
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (unsigned int)(s[i]+s[i+1]+1)>>1;
        s+= lx;
        d+= lx;
      }
  else /* if (xh && yh) */
    if (addflag)
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (d[i] + ((unsigned int)(s[i]+s[i+1]+s[i+lx]+s[i+lx+1]+2)>>2)+1)>>1;
        s+= lx;
        d+= lx;
      }
    else
      for (j=0; j<h; j++)
      {
        for (i=0; i<w; i++)
          d[i] = (unsigned int)(s[i]+s[i+1]+s[i+lx]+s[i+lx+1]+2)>>2;
        s+= lx;
        d+= lx;
      }
}



static void calc_DMV(DMV,dmvector,mvx,mvy)
int DMV[][2];
int *dmvector;
int mvx, mvy;
{
  if (pict_struct==FRAME_PICTURE)
  {
    if (topfirst)
    {
      /* vector for prediction of top field from bottom field */
      DMV[0][0] = ((mvx  +(mvx>0))>>1) + dmvector[0];
      DMV[0][1] = ((mvy  +(mvy>0))>>1) + dmvector[1] - 1;

      /* vector for prediction of bottom field from top field */
      DMV[1][0] = ((3*mvx+(mvx>0))>>1) + dmvector[0];
      DMV[1][1] = ((3*mvy+(mvy>0))>>1) + dmvector[1] + 1;
    }
    else
    {
      /* vector for prediction of top field from bottom field */
      DMV[0][0] = ((3*mvx+(mvx>0))>>1) + dmvector[0];
      DMV[0][1] = ((3*mvy+(mvy>0))>>1) + dmvector[1] - 1;

      /* vector for prediction of bottom field from top field */
      DMV[1][0] = ((mvx  +(mvx>0))>>1) + dmvector[0];
      DMV[1][1] = ((mvy  +(mvy>0))>>1) + dmvector[1] + 1;
    }
  }
  else
  {
    /* vector for prediction from field of opposite 'parity' */
    DMV[0][0] = ((mvx+(mvx>0))>>1) + dmvector[0];
    DMV[0][1] = ((mvy+(mvy>0))>>1) + dmvector[1];

    /* correct for vertical field shift */
    if (pict_struct==TOP_FIELD)
      DMV[0][1]--;
    else
      DMV[0][1]++;
  }
}

static void clearblock(cur,i0,j0)
unsigned char *cur[];
int i0, j0;
{
  int i, j, w, h;
  unsigned char *p;

  p = cur[0] + ((pict_struct==BOTTOM_FIELD) ? width : 0) + i0 + width2*j0;

  for (j=0; j<16; j++)
  {
    for (i=0; i<16; i++)
      p[i] = 128;
    p+= width2;
  }

  w = h = 16;

  if (chroma_format!=CHROMA444)
  {
    i0>>=1; w>>=1;
  }

  if (chroma_format==CHROMA420)
  {
    j0>>=1; h>>=1;
  }

  p = cur[1] + ((pict_struct==BOTTOM_FIELD) ? chrom_width : 0) + i0
             + chrom_width2*j0;

  for (j=0; j<h; j++)
  {
    for (i=0; i<w; i++)
      p[i] = 128;
    p+= chrom_width2;
  }

  p = cur[2] + ((pict_struct==BOTTOM_FIELD) ? chrom_width : 0) + i0
             + chrom_width2*j0;

  for (j=0; j<h; j++)
  {
    for (i=0; i<w; i++)
      p[i] = 128;
    p+= chrom_width2;
  }
}
