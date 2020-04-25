/* predict.c, �˶�����Ԥ��*/



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


/* Ϊ������ͼ���γ�Ԥ��
 *
    reff: ǰ��ο�ͼ��֡
    refb: ����ο�ͼ��֡
	cur: Ŀ��֡������ǰҪԤ���֡��
	secondfield: Ԥ��֡�ĵڶ�����ͼ��
	mbi: ָ������Ϣ�ṹ���ָ��

 */

void predict(reff,refb,cur,secondfield,mbi)
unsigned char *reff[],*refb[],*cur[3];
int secondfield;
struct mbinfo *mbi;
{
  int i, j, k;

  k = 0;

  /* �����еĺ�����ѭ�� */
  for (j=0; j<height2; j+=16)
    for (i=0; i<width; i+=16)
    {
      predict_mb(reff,refb,cur,width,i,j,pict_type,pict_struct,
                 mbi[k].mb_type,mbi[k].motion_type,secondfield,
                 mbi[k].MV,mbi[k].mv_field_sel,mbi[k].dmvector);

      k++;
    }
}

/* Ϊһ������γ�Ԥ��
 *
 	oldref: ��������ǰ��Ԥ��Ĳο�֡
	newref: �������к���Ԥ��Ĳο�֡
	cur: Ŀ��֡������ǰҪԤ���֡��
	lx: ֡�Ŀ�ȣ���ȫ�ֱ���"width"һ�£�
	bx��by: Ҫ��Ԥ��ĺ���ͼ�����꣨�����ǳ���Ҳ������֡��
	pict_type: ͼ�����ͣ�����I��P��B
	pict_struct: ͼ��Ľṹ������֡ͼ��FRAME_PICTURE�����ϲ��ֵĳ�ͼ��
	��TOP_FIELD�����²��ֵĳ�ͼ��BOTTOM_FIELD��
	
	mb_type: ������ͣ�������ǰ��MB_FORWARD��,Ҳ�����Ǻ���MB_BACKWARD��
	����֡�ڣ�MB_INTRA�����
	
	  motion_type: �˶����ͣ���Ҫ��ֵMC_FRAME, MC_FIELD��MC_16X8��MC_DMV��ʾ
	secondfield: Ԥ��֡�ĵڶ�����ͼ��
	PMV[2][2][2]: �˶��������԰�����ͼ������Ϊ��λ��
	mv_field_sel[2][2]: ��ֱ�˶���ѡ����Գ�Ԥ�⣩
	dmvector: ����˶�����

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

  addflag = 0;  /* ��һ��Ԥ��ֵҪ�洢������Ȼ����ϵڶ�����ȡƽ�� */

  if ((mb_type & MB_FORWARD) || (pict_type==P_TYPE))
  {
    /* ǰ��Ԥ�⣬����Pͼ�������0�˶����� */
    if (pict_struct==FRAME_PICTURE)
    {
      /* ֡ͼ�� */


      if ((motion_type==MC_FRAME) || !(mb_type & MB_FORWARD))
      {
  /* ֡ͼ���У�����֡��Ԥ�� */
		  pred(oldref,0,cur,0,
          lx,16,16,bx,by,PMV[0][0][0],PMV[0][0][1],0);
      }
      else if (motion_type==MC_FIELD)
      {
         /* ֡ͼ���У����ڳ���Ԥ��-based prediction in frame picture
         *���ǰ��մ�ֱ����ı���
         */


        /* �ϲ��ֳ�ͼ���Ԥ�� */
		  pred(oldref,mv_field_sel[0][0],cur,0,
          lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);

        /* �²��ֳ�ͼ���Ԥ�� */
		  pred(oldref,mv_field_sel[1][0],cur,1,
          lx<<1,16,8,bx,by>>1,PMV[1][0][0],PMV[1][0][1]>>1,0);
      }
      else if (motion_type==MC_DMV)
      {
               /* DMVԤ�� */
        /* �����������˶����� */

        calc_DMV(DMV,dmvector,PMV[0][0][0],PMV[0][0][1]>>1);

         /* ���ϲ��ֳ�ͼ��Ԥ���ϲ��ֳ�ͼ�� */
        pred(oldref,0,cur,0,lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);
        /* ���²��ֳ�ͼ��Ԥ���²��ֳ�ͼ�� */
        pred(oldref,1,cur,1,lx<<1,16,8,bx,by>>1,PMV[0][0][0],PMV[0][0][1]>>1,0);
        /*���²��ֳ�ͼ��Ԥ���²��ֳ�ͼ�񣬲��ӵ�ԭ���ĳ�ͼ����*/
        pred(oldref,1,cur,0,lx<<1,16,8,bx,by>>1,DMV[0][0],DMV[0][1],1);
        /*���ϲ��ֳ�ͼ��Ԥ���²��ֳ�ͼ�񣬲��ӵ�ԭ���ĳ�ͼ����*/
        pred(oldref,0,cur,1,lx<<1,16,8,bx,by>>1,DMV[1][0],DMV[1][1],1);

      }
      else
      {
         /* ��Ч���˶����� */
        if (!quiet)
          fprintf(stderr,"invalid motion_type\n");
      }
    }
    else /* TOP_FIELD or BOTTOM_FIELD */
    {
       /* ��ͼ�� */

      currentfield = (pict_struct==BOTTOM_FIELD);

   /* �ж�����һ֡������Ԥ��*/
      if ((pict_type==P_TYPE) && secondfield
          && (currentfield!=mv_field_sel[0][0]))
        predframe = newref; /* ͬһ֡ */
      else
        predframe = oldref;  /* ǰһ֡ */

      if ((motion_type==MC_FIELD) || !(mb_type & MB_FORWARD))
      {
        /* ��ͼ���л��ڳ���Ԥ��*/
		  pred(predframe,mv_field_sel[0][0],cur,currentfield,
          lx<<1,16,16,bx,by,PMV[0][0][0],PMV[0][0][1],0);
      }
      else if (motion_type==MC_16X8)
      {
           /* �ڳ�ͼ���е�16��8 �˶�����Ԥ��*/
        /* �ϰ벿�� */
        
        pred(predframe,mv_field_sel[0][0],cur,currentfield,
          lx<<1,16,8,bx,by,PMV[0][0][0],PMV[0][0][1],0);

        /* �ж�����һ֡ͼ����Ԥ���°벿�� */
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
         /* DMVԤ�� */

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
    addflag = 1; /* ��һ��Ԥ��ֵ�����ֵȡƽ�� */
  }

  if (mb_type & MB_BACKWARD)
  {
        /* ����Ԥ�� */

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

/* Ԥ����ο�
 *
 * src:     Դ֡(Y,U,V)
 * sfield:  Դ��ѡ��(0: ֡���ϰ볡, 1: �°볡)
 * dst:     Ŀ��֡(Y,U,V)
 * dfield:  Ŀ�곡ѡ��
 *
 * �������ֵ������ͼ��Ĳ���
 * lx:      �������صĴ�ֱ����
 * w,h:     ��Ŀ�Ⱥ͸߶�
 * x,y:     Ŀ��������
 * dx,dy:   �������˶�����
 * addflag: �洢���Ԥ��
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

/* �͵ȼ�Ԥ��
 *
 * src:     Դ֡(Y,U,V)
 * dst:     Ŀ��֡(Y,U,V)
 *
 * lx:      �������صĴ�ֱ����
 * w,h:     ��Ŀ�Ⱥ͸߶�
 * x,y:     Ŀ��������
 * dx,dy:   �������˶�����
 * addflag: �洢���Ԥ��
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
