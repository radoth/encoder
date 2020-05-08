/* motion.c, 运动估计函数*/



#include <cstdio>
#include "timeSettings.h"
#include "commonData.h"

/*
 * 求两个块之间绝对差值
 * blk1,blk2：为两个块左上角像素坐标
 *lx：垂直相邻的像素之间的距离（以字节为单位）
 *hx，hy：水平和垂直插值的标志
 *h：块的高度（通常为8或16）
 *distlim：极值，如果结果超过该值则放弃之。
 */
static int absoluteDistance(unsigned char *blk1,unsigned char *blk2,int lx,int hx,int hy,int h,int distlim)
{
  unsigned char *p1,*p1a,*p2;
  int i,j;
  int s,v;

  s = 0;
  p1 = blk1;
  p2 = blk2;

  if (!hx && !hy)
    for (j=0; j<h; j++)
    {
      if ((v = p1[0]  - p2[0])<0)  v = -v; s+= v;
      if ((v = p1[1]  - p2[1])<0)  v = -v; s+= v;
      if ((v = p1[2]  - p2[2])<0)  v = -v; s+= v;
      if ((v = p1[3]  - p2[3])<0)  v = -v; s+= v;
      if ((v = p1[4]  - p2[4])<0)  v = -v; s+= v;
      if ((v = p1[5]  - p2[5])<0)  v = -v; s+= v;
      if ((v = p1[6]  - p2[6])<0)  v = -v; s+= v;
      if ((v = p1[7]  - p2[7])<0)  v = -v; s+= v;
      if ((v = p1[8]  - p2[8])<0)  v = -v; s+= v;
      if ((v = p1[9]  - p2[9])<0)  v = -v; s+= v;
      if ((v = p1[10] - p2[10])<0) v = -v; s+= v;
      if ((v = p1[11] - p2[11])<0) v = -v; s+= v;
      if ((v = p1[12] - p2[12])<0) v = -v; s+= v;
      if ((v = p1[13] - p2[13])<0) v = -v; s+= v;
      if ((v = p1[14] - p2[14])<0) v = -v; s+= v;
      if ((v = p1[15] - p2[15])<0) v = -v; s+= v;

      if (s >= distlim)
        break;

      p1+= lx;
      p2+= lx;
    }
  else if (hx && !hy)
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1[i+1]+1)>>1) - p2[i];
        if (v>=0)
          s+= v;
        else
          s-= v;
      }
      p1+= lx;
      p2+= lx;
    }
  else if (!hx && hy)
  {
    p1a = p1 + lx;
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1a[i]+1)>>1) - p2[i];
        if (v>=0)
          s+= v;
        else
          s-= v;
      }
      p1 = p1a;
      p1a+= lx;
      p2+= lx;
    }
  }
  else /* if (hx && hy) */
  {
    p1a = p1 + lx;
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1[i+1]+p1a[i]+p1a[i+1]+2)>>2) - p2[i];
        if (v>=0)
          s+= v;
        else
          s-= v;
      }
      p1 = p1a;
      p1a+= lx;
      p2+= lx;
    }
  }

  return s;
}

/*
 * 两个块之间均方误差

 */
static int meanDiffBlock(unsigned char *blk1,unsigned char *blk2,int lx,int hx,int hy,int h)
{
  unsigned char *p1,*p1a,*p2;
  int i,j;
  int s,v;

  s = 0;
  p1 = blk1;
  p2 = blk2;
  if (!hx && !hy)
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = p1[i] - p2[i];
        s+= v*v;
      }
      p1+= lx;
      p2+= lx;
    }
  else if (hx && !hy)
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1[i+1]+1)>>1) - p2[i];
        s+= v*v;
      }
      p1+= lx;
      p2+= lx;
    }
  else if (!hx && hy)
  {
    p1a = p1 + lx;
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1a[i]+1)>>1) - p2[i];
        s+= v*v;
      }
      p1 = p1a;
      p1a+= lx;
      p2+= lx;
    }
  }
  else /* if (hx && hy) */
  {
    p1a = p1 + lx;
    for (j=0; j<h; j++)
    {
      for (i=0; i<16; i++)
      {
        v = ((unsigned int)(p1[i]+p1[i+1]+p1a[i]+p1a[i+1]+2)>>2) - p2[i];
        s+= v*v;
      }
      p1 = p1a;
      p1a+= lx;
      p2+= lx;
    }
  }

  return s;
}

/*
 *(16*h) 块和双向预测之间的绝对误差
 *

 */
static int dualAbsDiff(unsigned char *pf,unsigned char *pb,unsigned char *p2,int lx,int hxf,int hyf,int hxb,int hyb,int h)
{
  unsigned char *pfa,*pfb,*pfc,*pba,*pbb,*pbc;
  int i,j;
  int s,v;

  pfa = pf + hxf;
  pfb = pf + lx*hyf;
  pfc = pfb + hxf;

  pba = pb + hxb;
  pbb = pb + lx*hyb;
  pbc = pbb + hxb;

  s = 0;

  for (j=0; j<h; j++)
  {
    for (i=0; i<16; i++)
    {
      v = ((((unsigned int)(*pf++ + *pfa++ + *pfb++ + *pfc++ + 2)>>2) +
            ((unsigned int)(*pb++ + *pba++ + *pbb++ + *pbc++ + 2)>>2) + 1)>>1)
           - *p2++;
      if (v>=0)
        s+= v;
      else
        s-= v;
    }
    p2+= lx-16;
    pf+= lx-16;
    pfa+= lx-16;
    pfb+= lx-16;
    pfc+= lx-16;
    pb+= lx-16;
    pba+= lx-16;
    pbb+= lx-16;
    pbc+= lx-16;
  }

  return s;
}

/*
 * (16*h) 块和双向预测之间的均方误差
 */
static int blockDualMeanDiff(unsigned char *pf,unsigned char *pb,unsigned char *p2,int lx,int hxf,int hyf,int hxb,int hyb,int h)
{
  unsigned char *pfa,*pfb,*pfc,*pba,*pbb,*pbc;
  int i,j;
  int s,v;

  pfa = pf + hxf;
  pfb = pf + lx*hyf;
  pfc = pfb + hxf;

  pba = pb + hxb;
  pbb = pb + lx*hyb;
  pbc = pbb + hxb;

  s = 0;

  for (j=0; j<h; j++)
  {
    for (i=0; i<16; i++)
    {
      v = ((((unsigned int)(*pf++ + *pfa++ + *pfb++ + *pfc++ + 2)>>2) +
            ((unsigned int)(*pb++ + *pba++ + *pbb++ + *pbc++ + 2)>>2) + 1)>>1)
          - *p2++;
      s+=v*v;
    }
    p2+= lx-16;
    pf+= lx-16;
    pfa+= lx-16;
    pfb+= lx-16;
    pfc+= lx-16;
    pb+= lx-16;
    pba+= lx-16;
    pbb+= lx-16;
    pbc+= lx-16;
  }

  return s;
}



/*
 * 计算一个(16*16) 块的方差，并乘以256
 * p:  块左上角像素的地址
 * lx: 相邻像素的垂直距离（以字节为单位）
 */
static int variCalc(unsigned char *p,int lx)
{
  int i,j;
  unsigned int v,s,s2;

  s = s2 = 0;

  for (j=0; j<16; j++)
  {
    for (i=0; i<16; i++)
    {
      v = *p++;
      s+= v;
      s2+= v*v;
    }
    p+= lx-16;
  }
  return s2 - (s*s)/256;
}


/*
 全搜索块匹配

 blk：块的左上角像素坐标
h：块的高度
lx：在参考块中，垂直相邻的像素之间的距离（以字节为单位）
org：源参考图像的左上角像素的坐标
ref：重建参考图像的左上角像素的坐标
i0，j0：搜索窗口的中心点
sx，sy：搜索窗口的半长和半宽
xmax，ymax：搜索区域的右边界和下边界
iminp，jminp：指向存储结果的指针
*/

static int blockSearchMatch(unsigned char *org,unsigned char *ref,unsigned char *blk,int lx,int i0,int j0,int sx,int sy,int h,int xmax,int ymax,int *iminp,int *jminp)
{
  int i,j,imin,jmin,ilow,ihigh,jlow,jhigh;
  int d,dmin;
  int k,l,sxy;

  ilow = i0 - sx;
  ihigh = i0 + sx;

  if (ilow<0)
    ilow = 0;

  if (ihigh>xmax-16)
    ihigh = xmax-16;

  jlow = j0 - sy;
  jhigh = j0 + sy;

  if (jlow<0)
    jlow = 0;

  if (jhigh>ymax-h)
    jhigh = ymax-h;

  /* full pel search, spiraling outwards */

  imin = i0;
  jmin = j0;
  dmin = absoluteDistance(org+imin+lx*jmin,blk,lx,0,0,h,65536);

  sxy = (sx>sy) ? sx : sy;

  for (l=1; l<=sxy; l++)
  {
    i = i0 - l;
    j = j0 - l;
    for (k=0; k<8*l; k++)
    {
      if (i>=ilow && i<=ihigh && j>=jlow && j<=jhigh)
      {
        d = absoluteDistance(org+i+lx*j,blk,lx,0,0,h,dmin);

        if (d<dmin)
        {
          dmin = d;
          imin = i;
          jmin = j;
        }
      }

      if      (k<2*l) i++;
      else if (k<4*l) j++;
      else if (k<6*l) i--;
      else            j--;
    }
  }

  /* half pel */
  dmin = 65536;
  imin <<= 1;
  jmin <<= 1;
  ilow = imin - (imin>0);
  ihigh = imin + (imin<((xmax-16)<<1));
  jlow = jmin - (jmin>0);
  jhigh = jmin + (jmin<((ymax-h)<<1));

  for (j=jlow; j<=jhigh; j++)
    for (i=ilow; i<=ihigh; i++)
    {
      d = absoluteDistance(ref+(i>>1)+lx*(j>>1),blk,lx,i&1,j&1,h,dmin);

      if (d<dmin)
      {
        dmin = d;
        imin = i;
        jmin = j;
      }
    }

  *iminp = imin;
  *jminp = jmin;

  return dmin;
}


static void framePredict(unsigned char *ref,unsigned char *mb,int i,int j,int iminf[2][2],int jminf[2][2],
  int *iminp,int *jminp,int *imindmvp, int *jmindmvp,int * dmcp, int *vmcp)
{
  int pref,ppred,delta_x,delta_y;
  int is,js,it,jt,ib,jb,it0,jt0,ib0,jb0;
  int imins,jmins,imint,jmint,iminb,jminb,imindmv,jmindmv;
  int vmc,local_dist;

  /* Calculate Dual Prime distortions for 9 delta candidates
   * for each of the four minimum field vectors
   * Note: only for P pictures!
   */

  /* initialize minimum dual prime distortion to large value */
  vmc = 1 << 30;

  for (pref=0; pref<2; pref++)
  {
    for (ppred=0; ppred<2; ppred++)
    {
      /* 将笛卡儿绝对坐标系转化成相对运动向量的值
       */
      is = iminf[pref][ppred] - (i<<1);
      js = jminf[pref][ppred] - (j<<1);

      if (pref!=ppred)
      {
        /* vertical field shift adjustment */
        if (ppred==0)
          js++;
        else
          js--;

        /* mvxs and mvys scaling*/
        is<<=1;
        js<<=1;
        if (topfirst == ppred)
        {
          /* second field: scale by 1/3 */
          is = (is>=0) ? (is+1)/3 : -((-is+1)/3);
          js = (js>=0) ? (js+1)/3 : -((-js+1)/3);
        }
        else
          continue;
      }

      /* vector for prediction from field of opposite 'parity' */
      if (topfirst)
      {
        /* vector for prediction of top field from bottom field */
        it0 = ((is+(is>0))>>1);
        jt0 = ((js+(js>0))>>1) - 1;

        /* vector for prediction of bottom field from top field */
        ib0 = ((3*is+(is>0))>>1);
        jb0 = ((3*js+(js>0))>>1) + 1;
      }
      else
      {
        /* vector for prediction of top field from bottom field */
        it0 = ((3*is+(is>0))>>1);
        jt0 = ((3*js+(js>0))>>1) - 1;

        /* vector for prediction of bottom field from top field */
        ib0 = ((is+(is>0))>>1);
        jb0 = ((js+(js>0))>>1) + 1;
      }

      /* convert back to absolute half-pel field picture coordinates */
      is += i<<1;
      js += j<<1;
      it0 += i<<1;
      jt0 += j<<1;
      ib0 += i<<1;
      jb0 += j<<1;

      if (is >= 0 && is <= (width-16)<<1 &&
          js >= 0 && js <= (height-16))
      {
        for (delta_y=-1; delta_y<=1; delta_y++)
        {
          for (delta_x=-1; delta_x<=1; delta_x++)
          {
            /* opposite field coordinates */
            it = it0 + delta_x;
            jt = jt0 + delta_y;
            ib = ib0 + delta_x;
            jb = jb0 + delta_y;

            if (it >= 0 && it <= (width-16)<<1 &&
                jt >= 0 && jt <= (height-16) &&
                ib >= 0 && ib <= (width-16)<<1 &&
                jb >= 0 && jb <= (height-16))
            {
              /* compute prediction error */
              local_dist = blockDualMeanDiff(
                ref + (is>>1) + (width<<1)*(js>>1),
                ref + width + (it>>1) + (width<<1)*(jt>>1),
                mb,             /* current mb location */
                width<<1,       /* adjacent line distance */
                is&1, js&1, it&1, jt&1, /* half-pel flags */
                8);             /* block height */
              local_dist += blockDualMeanDiff(
                ref + width + (is>>1) + (width<<1)*(js>>1),
                ref + (ib>>1) + (width<<1)*(jb>>1),
                mb + width,     /* current mb location */
                width<<1,       /* adjacent line distance */
                is&1, js&1, ib&1, jb&1, /* half-pel flags */
                8);             /* block height */

              /* update delta with least distortion vector */
              if (local_dist < vmc)
              {
                imins = is;
                jmins = js;
                imint = it;
                jmint = jt;
                iminb = ib;
                jminb = jb;
                imindmv = delta_x;
                jmindmv = delta_y;
                vmc = local_dist;
              }
            }
          }  /* end delta x loop */
        } /* end delta y loop */
      }
    }
  }

  /* Compute L1 error for decision purposes */
  local_dist = dualAbsDiff(
    ref + (imins>>1) + (width<<1)*(jmins>>1),
    ref + width + (imint>>1) + (width<<1)*(jmint>>1),
    mb,
    width<<1,
    imins&1, jmins&1, imint&1, jmint&1,
    8);
  local_dist += dualAbsDiff(
    ref + width + (imins>>1) + (width<<1)*(jmins>>1),
    ref + (iminb>>1) + (width<<1)*(jminb>>1),
    mb + width,
    width<<1,
    imins&1, jmins&1, iminb&1, jminb&1,
    8);

  *dmcp = local_dist;
  *iminp = imins;
  *jminp = jmins;
  *imindmvp = imindmv;
  *jmindmvp = jmindmv;
  *vmcp = vmc;
}


/*
 * 帧图像运动估计
 *

 */
static void frameEstimate(unsigned char *org,unsigned char *ref,unsigned char *mb,int i,int j,int sx,int sy,
  int *iminp,int *jminp,int *imintp,int *jmintp,int *iminbp,int *jminbp,int *dframep,int *dfieldp,int *tselp,int *bselp,
  int imins[2][2],int jmins[2][2])
{
  int dt,db,dmint,dminb;
  int imint,iminb,jmint,jminb;

  /* frame prediction */
  *dframep = blockSearchMatch(org,ref,mb,width,i,j,sx,sy,16,width,height,
                        iminp,jminp);

  /* predict top field from top field */
  dt = blockSearchMatch(org,ref,mb,width<<1,i,j>>1,sx,sy>>1,8,width,height>>1,
                  &imint,&jmint);

  /* predict top field from bottom field */
  db = blockSearchMatch(org+width,ref+width,mb,width<<1,i,j>>1,sx,sy>>1,8,width,height>>1,
                  &iminb,&jminb);

  imins[0][0] = imint;
  jmins[0][0] = jmint;
  imins[1][0] = iminb;
  jmins[1][0] = jminb;

  /* select prediction for top field */
  if (dt<=db)
  {
    dmint=dt; *imintp=imint; *jmintp=jmint; *tselp=0;
  }
  else
  {
    dmint=db; *imintp=iminb; *jmintp=jminb; *tselp=1;
  }

  /* predict bottom field from top field */
  dt = blockSearchMatch(org,ref,mb+width,width<<1,i,j>>1,sx,sy>>1,8,width,height>>1,
                  &imint,&jmint);

  /* predict bottom field from bottom field */
  db = blockSearchMatch(org+width,ref+width,mb+width,width<<1,i,j>>1,sx,sy>>1,8,width,height>>1,
                  &iminb,&jminb);

  imins[0][1] = imint;
  jmins[0][1] = jmint;
  imins[1][1] = iminb;
  jmins[1][1] = jminb;

  /* select prediction for bottom field */
  if (db<=dt)
  {
    dminb=db; *iminbp=iminb; *jminbp=jminb; *bselp=1;
  }
  else
  {
    dminb=dt; *iminbp=imint; *jminbp=jmint; *bselp=0;
  }

  *dfieldp=dmint+dminb;
}

/*
 * 场图片运动预测程序
 *
 * toporg：原始上半部分场地址
 *topref：重建的上半部分场地址
 *botorg：原始下半部分参考场地址
 *botref：重建的下半部分参考场的地址
 *mb：待匹配的宏块
 *i，j：宏块的位置（即搜索窗口的中心）
 *iminp，jminp，selp，dfieldp：最佳预测场的位置和距离
 *imin8up，jmin8up，sel8up：上半部分宏块的最佳预测的位置
 *imin8lp，jmin8lp，sel8lp：下半部分宏块的最佳预测的位置
 *d8p：最佳16×8的预测块的距离
 *iminsp，jminsp，dsp：最佳同奇偶性预测场的位置和距离
 */

static void fieldEstimate(unsigned char *toporg,unsigned char *topref,unsigned char *botorg,unsigned char *botref,unsigned char *mb,int i,int j,int sx,int sy,int ipflag,
  int *iminp,int *jminp,int *imin8up,int *jmin8up,int *imin8lp,int *jmin8lp,int *dfieldp,int *d8p,int *selp,int *sel8up,int *sel8lp,
  int *iminsp,int *jminsp,int *dsp)
{
  int dt, db, imint, jmint, iminb, jminb, notop, nobot;

  /* if ipflag is set, predict from field of opposite parity only */
  notop = ipflag && (pict_struct==TOP_FIELD);
  nobot = ipflag && (pict_struct==BOTTOM_FIELD);

  /* field prediction */

  /* predict current field from top field */
  if (notop)
    dt = 65536; /* infinity */
  else
    dt = blockSearchMatch(toporg,topref,mb,width<<1,
                    i,j,sx,sy>>1,16,width,height>>1,
                    &imint,&jmint);

  /* predict current field from bottom field */
  if (nobot)
    db = 65536; /* infinity */
  else
    db = blockSearchMatch(botorg,botref,mb,width<<1,
                    i,j,sx,sy>>1,16,width,height>>1,
                    &iminb,&jminb);

  /* same parity prediction (only valid if ipflag==0) */
  if (pict_struct==TOP_FIELD)
  {
    *iminsp = imint; *jminsp = jmint; *dsp = dt;
  }
  else
  {
    *iminsp = iminb; *jminsp = jminb; *dsp = db;
  }

  /* select field prediction */
  if (dt<=db)
  {
    *dfieldp = dt; *iminp = imint; *jminp = jmint; *selp = 0;
  }
  else
  {
    *dfieldp = db; *iminp = iminb; *jminp = jminb; *selp = 1;
  }


  /* 16x8 motion compensation */

  /* predict upper half field from top field */
  if (notop)
    dt = 65536;
  else
    dt = blockSearchMatch(toporg,topref,mb,width<<1,
                    i,j,sx,sy>>1,8,width,height>>1,
                    &imint,&jmint);

  /* predict upper half field from bottom field */
  if (nobot)
    db = 65536;
  else
    db = blockSearchMatch(botorg,botref,mb,width<<1,
                    i,j,sx,sy>>1,8,width,height>>1,
                    &iminb,&jminb);

  /* select prediction for upper half field */
  if (dt<=db)
  {
    *d8p = dt; *imin8up = imint; *jmin8up = jmint; *sel8up = 0;
  }
  else
  {
    *d8p = db; *imin8up = iminb; *jmin8up = jminb; *sel8up = 1;
  }

  /* predict lower half field from top field */
  if (notop)
    dt = 65536;
  else
    dt = blockSearchMatch(toporg,topref,mb+(width<<4),width<<1,
                    i,j+8,sx,sy>>1,8,width,height>>1,
                    &imint,&jmint);

  /* predict lower half field from bottom field */
  if (nobot)
    db = 65536;
  else
    db = blockSearchMatch(botorg,botref,mb+(width<<4),width<<1,
                    i,j+8,sx,sy>>1,8,width,height>>1,
                    &iminb,&jminb);

  /* select prediction for lower half field */
  if (dt<=db)
  {
    *d8p += dt; *imin8lp = imint; *jmin8lp = jmint; *sel8lp = 0;
  }
  else
  {
    *d8p += db; *imin8lp = iminb; *jmin8lp = jminb; *sel8lp = 1;
  }
}




static void frame_ME(unsigned char *oldorg,unsigned char *neworg,unsigned char *oldref,unsigned char *newref,unsigned char *cur,int i,int j,int sxf,int syf,int sxb,int syb,struct mbinfo *mbi)
{
  int imin,jmin,iminf,jminf,iminr,jminr;
  int imint,jmint,iminb,jminb;
  int imintf,jmintf,iminbf,jminbf;
  int imintr,jmintr,iminbr,jminbr;
  int var,v0;
  int dmc,dmcf,dmcr,dmci,vmc,vmcf,vmcr,vmci;
  int dmcfield,dmcfieldf,dmcfieldr,dmcfieldi;
  int tsel,bsel,tself,bself,tselr,bselr;
  unsigned char *mb;
  int imins[2][2],jmins[2][2];
  int imindp,jmindp,imindmv,jmindmv,dmc_dp,vmc_dp;

  mb = cur + i + width*j;

  var = variCalc(mb,width);

  if (pict_type==I_TYPE)
    mbi->mb_type = MB_INTRA;
  else if (pict_type==P_TYPE)
  {
    if (frame_pred_dct)
    {
      dmc = blockSearchMatch(oldorg,oldref,mb,
                       width,i,j,sxf,syf,16,width,height,&imin,&jmin);
      vmc = meanDiffBlock(oldref+(imin>>1)+width*(jmin>>1),mb,
                  width,imin&1,jmin&1,16);
      mbi->motion_type = MC_FRAME;
    }
    else
    {
      frameEstimate(oldorg,oldref,mb,i,j,sxf,syf,
        &imin,&jmin,&imint,&jmint,&iminb,&jminb,
        &dmc,&dmcfield,&tsel,&bsel,imins,jmins);

      if (M==1)
        framePredict(oldref,mb,i,j>>1,imins,jmins,
          &imindp,&jmindp,&imindmv,&jmindmv,&dmc_dp,&vmc_dp);

      /* select between dual prime, frame and field prediction */
      if (M==1 && dmc_dp<dmc && dmc_dp<dmcfield)
      {
        mbi->motion_type = MC_DMV;
        dmc = dmc_dp;
        vmc = vmc_dp;
      }
      else if (dmc<=dmcfield)
      {
        mbi->motion_type = MC_FRAME;
        vmc = meanDiffBlock(oldref+(imin>>1)+width*(jmin>>1),mb,
                    width,imin&1,jmin&1,16);
      }
      else
      {
        mbi->motion_type = MC_FIELD;
        dmc = dmcfield;
        vmc = meanDiffBlock(oldref+(tsel?width:0)+(imint>>1)+(width<<1)*(jmint>>1),
                    mb,width<<1,imint&1,jmint&1,8);
        vmc+= meanDiffBlock(oldref+(bsel?width:0)+(iminb>>1)+(width<<1)*(jminb>>1),
                    mb+width,width<<1,iminb&1,jminb&1,8);
      }
    }

    /* select between intra or non-intra coding:
     *
     * selection is based on intra block variance (var) vs.
     * prediction error variance (vmc)
     *
     * blocks with small prediction error are always coded non-intra
     * even if variance is smaller (is this reasonable?)
     */
    if (vmc>var && vmc>=9*256)
      mbi->mb_type = MB_INTRA;
    else
    {
      /* select between MC / No-MC
       *
       * use No-MC if var(No-MC) <= 1.25*var(MC)
       * (i.e slightly biased towards No-MC)
       *
       * blocks with small prediction error are always coded as No-MC
       * (requires no motion vectors, allows skipping)
       */
      v0 = meanDiffBlock(oldref+i+width*j,mb,width,0,0,16);
      if (4*v0>5*vmc && v0>=9*256)
      {
        /* use MC */
        var = vmc;
        mbi->mb_type = MB_FORWARD;
        if (mbi->motion_type==MC_FRAME)
        {
          mbi->MV[0][0][0] = imin - (i<<1);
          mbi->MV[0][0][1] = jmin - (j<<1);
        }
        else if (mbi->motion_type==MC_DMV)
        {
          /* these are FRAME vectors */
          /* same parity vector */
          mbi->MV[0][0][0] = imindp - (i<<1);
          mbi->MV[0][0][1] = (jmindp<<1) - (j<<1);

          /* opposite parity vector */
          mbi->dmvector[0] = imindmv;
          mbi->dmvector[1] = jmindmv;
        }
        else
        {
          /* these are FRAME vectors */
          mbi->MV[0][0][0] = imint - (i<<1);
          mbi->MV[0][0][1] = (jmint<<1) - (j<<1);
          mbi->MV[1][0][0] = iminb - (i<<1);
          mbi->MV[1][0][1] = (jminb<<1) - (j<<1);
          mbi->mv_field_sel[0][0] = tsel;
          mbi->mv_field_sel[1][0] = bsel;
        }
      }
      else
      {
        /* No-MC */
        var = v0;
        mbi->mb_type = 0;
        mbi->motion_type = MC_FRAME;
        mbi->MV[0][0][0] = 0;
        mbi->MV[0][0][1] = 0;
      }
    }
  }
  else /* if (pict_type==B_TYPE) */
  {
    if (frame_pred_dct)
    {
      /* forward */
      dmcf = blockSearchMatch(oldorg,oldref,mb,
                        width,i,j,sxf,syf,16,width,height,&iminf,&jminf);
      vmcf = meanDiffBlock(oldref+(iminf>>1)+width*(jminf>>1),mb,
                   width,iminf&1,jminf&1,16);

      /* backward */
      dmcr = blockSearchMatch(neworg,newref,mb,
                        width,i,j,sxb,syb,16,width,height,&iminr,&jminr);
      vmcr = meanDiffBlock(newref+(iminr>>1)+width*(jminr>>1),mb,
                   width,iminr&1,jminr&1,16);

      /* interpolated (bidirectional) */
      vmci = blockDualMeanDiff(oldref+(iminf>>1)+width*(jminf>>1),
                    newref+(iminr>>1)+width*(jminr>>1),
                    mb,width,iminf&1,jminf&1,iminr&1,jminr&1,16);

      /* decisions */

      /* select between forward/backward/interpolated prediction:
       * use the one with smallest mean sqaured prediction error
       */
      if (vmcf<=vmcr && vmcf<=vmci)
      {
        vmc = vmcf;
        mbi->mb_type = MB_FORWARD;
      }
      else if (vmcr<=vmci)
      {
        vmc = vmcr;
        mbi->mb_type = MB_BACKWARD;
      }
      else
      {
        vmc = vmci;
        mbi->mb_type = MB_FORWARD|MB_BACKWARD;
      }

      mbi->motion_type = MC_FRAME;
    }
    else
    {
      /* forward prediction */
      frameEstimate(oldorg,oldref,mb,i,j,sxf,syf,
        &iminf,&jminf,&imintf,&jmintf,&iminbf,&jminbf,
        &dmcf,&dmcfieldf,&tself,&bself,imins,jmins);

      /* backward prediction */
      frameEstimate(neworg,newref,mb,i,j,sxb,syb,
        &iminr,&jminr,&imintr,&jmintr,&iminbr,&jminbr,
        &dmcr,&dmcfieldr,&tselr,&bselr,imins,jmins);

      /* calculate interpolated distance */
      /* frame */
      dmci = dualAbsDiff(oldref+(iminf>>1)+width*(jminf>>1),
                    newref+(iminr>>1)+width*(jminr>>1),
                    mb,width,iminf&1,jminf&1,iminr&1,jminr&1,16);

      /* top field */
      dmcfieldi = dualAbsDiff(
                    oldref+(imintf>>1)+(tself?width:0)+(width<<1)*(jmintf>>1),
                    newref+(imintr>>1)+(tselr?width:0)+(width<<1)*(jmintr>>1),
                    mb,width<<1,imintf&1,jmintf&1,imintr&1,jmintr&1,8);

      /* bottom field */
      dmcfieldi+= dualAbsDiff(
                    oldref+(iminbf>>1)+(bself?width:0)+(width<<1)*(jminbf>>1),
                    newref+(iminbr>>1)+(bselr?width:0)+(width<<1)*(jminbr>>1),
                    mb+width,width<<1,iminbf&1,jminbf&1,iminbr&1,jminbr&1,8);

      /* select prediction type of minimum distance from the
       * six candidates (field/frame * forward/backward/interpolated)
       */
      if (dmci<dmcfieldi && dmci<dmcf && dmci<dmcfieldf
          && dmci<dmcr && dmci<dmcfieldr)
      {
        /* frame, interpolated */
        mbi->mb_type = MB_FORWARD|MB_BACKWARD;
        mbi->motion_type = MC_FRAME;
        vmc = blockDualMeanDiff(oldref+(iminf>>1)+width*(jminf>>1),
                     newref+(iminr>>1)+width*(jminr>>1),
                     mb,width,iminf&1,jminf&1,iminr&1,jminr&1,16);
      }
      else if (dmcfieldi<dmcf && dmcfieldi<dmcfieldf
               && dmcfieldi<dmcr && dmcfieldi<dmcfieldr)
      {
        /* field, interpolated */
        mbi->mb_type = MB_FORWARD|MB_BACKWARD;
        mbi->motion_type = MC_FIELD;
        vmc = blockDualMeanDiff(oldref+(imintf>>1)+(tself?width:0)+(width<<1)*(jmintf>>1),
                     newref+(imintr>>1)+(tselr?width:0)+(width<<1)*(jmintr>>1),
                     mb,width<<1,imintf&1,jmintf&1,imintr&1,jmintr&1,8);
        vmc+= blockDualMeanDiff(oldref+(iminbf>>1)+(bself?width:0)+(width<<1)*(jminbf>>1),
                     newref+(iminbr>>1)+(bselr?width:0)+(width<<1)*(jminbr>>1),
                     mb+width,width<<1,iminbf&1,jminbf&1,iminbr&1,jminbr&1,8);
      }
      else if (dmcf<dmcfieldf && dmcf<dmcr && dmcf<dmcfieldr)
      {
        /* frame, forward */
        mbi->mb_type = MB_FORWARD;
        mbi->motion_type = MC_FRAME;
        vmc = meanDiffBlock(oldref+(iminf>>1)+width*(jminf>>1),mb,
                    width,iminf&1,jminf&1,16);
      }
      else if (dmcfieldf<dmcr && dmcfieldf<dmcfieldr)
      {
        /* field, forward */
        mbi->mb_type = MB_FORWARD;
        mbi->motion_type = MC_FIELD;
        vmc = meanDiffBlock(oldref+(tself?width:0)+(imintf>>1)+(width<<1)*(jmintf>>1),
                    mb,width<<1,imintf&1,jmintf&1,8);
        vmc+= meanDiffBlock(oldref+(bself?width:0)+(iminbf>>1)+(width<<1)*(jminbf>>1),
                    mb+width,width<<1,iminbf&1,jminbf&1,8);
      }
      else if (dmcr<dmcfieldr)
      {
        /* frame, backward */
        mbi->mb_type = MB_BACKWARD;
        mbi->motion_type = MC_FRAME;
        vmc = meanDiffBlock(newref+(iminr>>1)+width*(jminr>>1),mb,
                    width,iminr&1,jminr&1,16);
      }
      else
      {
        /* field, backward */
        mbi->mb_type = MB_BACKWARD;
        mbi->motion_type = MC_FIELD;
        vmc = meanDiffBlock(newref+(tselr?width:0)+(imintr>>1)+(width<<1)*(jmintr>>1),
                    mb,width<<1,imintr&1,jmintr&1,8);
        vmc+= meanDiffBlock(newref+(bselr?width:0)+(iminbr>>1)+(width<<1)*(jminbr>>1),
                    mb+width,width<<1,iminbr&1,jminbr&1,8);
      }
    }

    /* select between intra or non-intra coding:
     *
     * selection is based on intra block variance (var) vs.
     * prediction error variance (vmc)
     *
     * blocks with small prediction error are always coded non-intra
     * even if variance is smaller (is this reasonable?)
     */
    if (vmc>var && vmc>=9*256)
      mbi->mb_type = MB_INTRA;
    else
    {
      var = vmc;
      if (mbi->motion_type==MC_FRAME)
      {
        /* forward */
        mbi->MV[0][0][0] = iminf - (i<<1);
        mbi->MV[0][0][1] = jminf - (j<<1);
        /* backward */
        mbi->MV[0][1][0] = iminr - (i<<1);
        mbi->MV[0][1][1] = jminr - (j<<1);
      }
      else
      {
        /* these are FRAME vectors */
        /* forward */
        mbi->MV[0][0][0] = imintf - (i<<1);
        mbi->MV[0][0][1] = (jmintf<<1) - (j<<1);
        mbi->MV[1][0][0] = iminbf - (i<<1);
        mbi->MV[1][0][1] = (jminbf<<1) - (j<<1);
        mbi->mv_field_sel[0][0] = tself;
        mbi->mv_field_sel[1][0] = bself;
        /* backward */
        mbi->MV[0][1][0] = imintr - (i<<1);
        mbi->MV[0][1][1] = (jmintr<<1) - (j<<1);
        mbi->MV[1][1][0] = iminbr - (i<<1);
        mbi->MV[1][1][1] = (jminbr<<1) - (j<<1);
        mbi->mv_field_sel[0][1] = tselr;
        mbi->mv_field_sel[1][1] = bselr;
      }
    }
  }

  mbi->var = var;
}


static void dpfield_estimate(unsigned char *topref,unsigned char *botref,unsigned char *mb,int i,int j,int imins,int jmins,
  int *imindmvp, int *jmindmvp, int *dmcp, int *vmcp)
{
  unsigned char *sameref, *oppref;
  int io0,jo0,io,jo,delta_x,delta_y,mvxs,mvys,mvxo0,mvyo0;
  int imino,jmino,imindmv,jmindmv,vmc_dp,local_dist;


  /* Assign opposite and same reference pointer */
  if (pict_struct==TOP_FIELD)
  {
    sameref = topref;
    oppref = botref;
  }
  else
  {
    sameref = botref;
    oppref = topref;
  }


  mvxs = imins - (i<<1);
  mvys = jmins - (j<<1);

  /* vector for prediction from field of opposite 'parity' */
  mvxo0 = (mvxs+(mvxs>0)) >> 1;  /* mvxs // 2 */
  mvyo0 = (mvys+(mvys>0)) >> 1;  /* mvys // 2 */

  /* vertical field shift correction */
  if (pict_struct==TOP_FIELD)
    mvyo0--;
  else
    mvyo0++;

  /* convert back to absolute coordinates */
  io0 = mvxo0 + (i<<1);
  jo0 = mvyo0 + (j<<1);

  /* initialize minimum dual prime distortion to large value */
  vmc_dp = 1 << 30;

  for (delta_y = -1; delta_y <= 1; delta_y++)
  {
    for (delta_x = -1; delta_x <=1; delta_x++)
    {
      /* opposite field coordinates */
      io = io0 + delta_x;
      jo = jo0 + delta_y;

      if (io >= 0 && io <= (width-16)<<1 &&
          jo >= 0 && jo <= (height2-16)<<1)
      {
        /* compute prediction error */
        local_dist = blockDualMeanDiff(
          sameref + (imins>>1) + width2*(jmins>>1),
          oppref  + (io>>1)    + width2*(jo>>1),
          mb,             /* current mb location */
          width2,         /* adjacent line distance */
          imins&1, jmins&1, io&1, jo&1, /* half-pel flags */
          16);            /* block height */

        /* update delta with least distortion vector */
        if (local_dist < vmc_dp)
        {
          imino = io;
          jmino = jo;
          imindmv = delta_x;
          jmindmv = delta_y;
          vmc_dp = local_dist;
        }
      }
    }  /* end delta x loop */
  } /* end delta y loop */

  /* Compute L1 error for decision purposes */
  *dmcp = dualAbsDiff(
    sameref + (imins>>1) + width2*(jmins>>1),
    oppref  + (imino>>1) + width2*(jmino>>1),
    mb,             /* current mb location */
    width2,         /* adjacent line distance */
    imins&1, jmins&1, imino&1, jmino&1, /* half-pel flags */
    16);            /* block height */

  *imindmvp = imindmv;
  *jmindmvp = jmindmv;
  *vmcp = vmc_dp;
}




/*
 * 场图像的运动估计
 *
 *oldorg：前向预测的原始参考图（供P图像和B图像使用）
 *neworg：后向预测的原始参考图（仅供B图像使用）
 *oldref：前向预测的重建图像（P图像和B图像）
 *newref：后向预测的重建图像（B图像）
 *cur：当前原始图像帧（由预测图像产生的）
 *curref：当前的重建图像帧（用来根据第一个场预测第二个场）
 *sxf，syf：前向搜索窗口
 *sxb，syb：后向搜索窗口
 *mbi：指向宏块信息结构体的指针
 *secondfield：指示一帧图像的第二个场
 *ipflag：指示当前P图像帧是由I图像帧所预测产生的

 *输出结果mbi->
 *mb_type: 0, MB_INTRA, MB_FORWARD, MB_BACKWARD, MB_FORWARD|MB_BACKWARD
 *MV[][][]: 运动向量 (场格式)
 *mv_field_sel: 顶部/底部场
 *motion_type: MC_FIELD, MC_16X8

 */
static void field_ME(unsigned char *oldorg,unsigned char *neworg,unsigned char *oldref,unsigned char *newref,unsigned char *cur,unsigned char *curref,int i,int j,
  int sxf,int syf,int sxb,int syb,struct mbinfo *mbi,int secondfield,int ipflag)
{
  int w2;
  unsigned char *mb, *toporg, *topref, *botorg, *botref;
  int var,vmc,v0,dmc,dmcfieldi,dmc8i;
  int imin,jmin,imin8u,jmin8u,imin8l,jmin8l,dmcfield,dmc8,sel,sel8u,sel8l;
  int iminf,jminf,imin8uf,jmin8uf,imin8lf,jmin8lf,dmcfieldf,dmc8f,self,sel8uf,sel8lf;
  int iminr,jminr,imin8ur,jmin8ur,imin8lr,jmin8lr,dmcfieldr,dmc8r,selr,sel8ur,sel8lr;
  int imins,jmins,ds,imindmv,jmindmv,vmc_dp,dmc_dp;

  w2 = width<<1;

  mb = cur + i + w2*j;
  if (pict_struct==BOTTOM_FIELD)
    mb += width;

  var = variCalc(mb,w2);

  if (pict_type==I_TYPE)
    mbi->mb_type = MB_INTRA;
  else if (pict_type==P_TYPE)
  {
    toporg = oldorg;
    topref = oldref;
    botorg = oldorg + width;
    botref = oldref + width;

    if (secondfield)
    {
      /* opposite parity field is in same frame */
      if (pict_struct==TOP_FIELD)
      {
        /* current is top field */
        botorg = cur + width;
        botref = curref + width;
      }
      else
      {
        /* current is bottom field */
        toporg = cur;
        topref = curref;
      }
    }

    fieldEstimate(toporg,topref,botorg,botref,mb,i,j,sxf,syf,ipflag,
                   &imin,&jmin,&imin8u,&jmin8u,&imin8l,&jmin8l,
                   &dmcfield,&dmc8,&sel,&sel8u,&sel8l,&imins,&jmins,&ds);

    if (M==1 && !ipflag)  /* generic condition which permits Dual Prime */
      dpfield_estimate(topref,botref,mb,i,j,imins,jmins,&imindmv,&jmindmv,
        &dmc_dp,&vmc_dp);

    /* select between dual prime, field and 16x8 prediction */
    if (M==1 && !ipflag && dmc_dp<dmc8 && dmc_dp<dmcfield)
    {
      /* Dual Prime prediction */
      mbi->motion_type = MC_DMV;
      dmc = dmc_dp;     /* L1 metric */
      vmc = vmc_dp;     /* we already calculated L2 error for Dual */

    }
    else if (dmc8<dmcfield)
    {
      /* 16x8 prediction */
      mbi->motion_type = MC_16X8;
      /* upper half block */
      vmc = meanDiffBlock((sel8u?botref:topref) + (imin8u>>1) + w2*(jmin8u>>1),
                  mb,w2,imin8u&1,jmin8u&1,8);
      /* lower half block */
      vmc+= meanDiffBlock((sel8l?botref:topref) + (imin8l>>1) + w2*(jmin8l>>1),
                  mb+8*w2,w2,imin8l&1,jmin8l&1,8);
    }
    else
    {
      /* field prediction */
      mbi->motion_type = MC_FIELD;
      vmc = meanDiffBlock((sel?botref:topref) + (imin>>1) + w2*(jmin>>1),
                  mb,w2,imin&1,jmin&1,16);
    }

    /* select between intra and non-intra coding */
    if (vmc>var && vmc>=9*256)
      mbi->mb_type = MB_INTRA;
    else
    {
      /* zero MV field prediction from same parity ref. field
       * (not allowed if ipflag is set)
       */
      if (!ipflag)
        v0 = meanDiffBlock(((pict_struct==BOTTOM_FIELD)?botref:topref) + i + w2*j,
                   mb,w2,0,0,16);
      if (ipflag || (4*v0>5*vmc && v0>=9*256))
      {
        var = vmc;
        mbi->mb_type = MB_FORWARD;
        if (mbi->motion_type==MC_FIELD)
        {
          mbi->MV[0][0][0] = imin - (i<<1);
          mbi->MV[0][0][1] = jmin - (j<<1);
          mbi->mv_field_sel[0][0] = sel;
        }
        else if (mbi->motion_type==MC_DMV)
        {
          /* same parity vector */
          mbi->MV[0][0][0] = imins - (i<<1);
          mbi->MV[0][0][1] = jmins - (j<<1);

          /* opposite parity vector */
          mbi->dmvector[0] = imindmv;
          mbi->dmvector[1] = jmindmv;
        }
        else
        {
          mbi->MV[0][0][0] = imin8u - (i<<1);
          mbi->MV[0][0][1] = jmin8u - (j<<1);
          mbi->MV[1][0][0] = imin8l - (i<<1);
          mbi->MV[1][0][1] = jmin8l - ((j+8)<<1);
          mbi->mv_field_sel[0][0] = sel8u;
          mbi->mv_field_sel[1][0] = sel8l;
        }
      }
      else
      {
        /* No MC */
        var = v0;
        mbi->mb_type = 0;
        mbi->motion_type = MC_FIELD;
        mbi->MV[0][0][0] = 0;
        mbi->MV[0][0][1] = 0;
        mbi->mv_field_sel[0][0] = (pict_struct==BOTTOM_FIELD);
      }
    }
  }
  else /* if (pict_type==B_TYPE) */
  {
    /* forward prediction */
    fieldEstimate(oldorg,oldref,oldorg+width,oldref+width,mb,
                   i,j,sxf,syf,0,
                   &iminf,&jminf,&imin8uf,&jmin8uf,&imin8lf,&jmin8lf,
                   &dmcfieldf,&dmc8f,&self,&sel8uf,&sel8lf,&imins,&jmins,&ds);

    /* backward prediction */
    fieldEstimate(neworg,newref,neworg+width,newref+width,mb,
                   i,j,sxb,syb,0,
                   &iminr,&jminr,&imin8ur,&jmin8ur,&imin8lr,&jmin8lr,
                   &dmcfieldr,&dmc8r,&selr,&sel8ur,&sel8lr,&imins,&jmins,&ds);

    /* calculate distances for bidirectional prediction */
    /* field */
    dmcfieldi = dualAbsDiff(oldref + (self?width:0) + (iminf>>1) + w2*(jminf>>1),
                       newref + (selr?width:0) + (iminr>>1) + w2*(jminr>>1),
                       mb,w2,iminf&1,jminf&1,iminr&1,jminr&1,16);

    /* 16x8 upper half block */
    dmc8i = dualAbsDiff(oldref + (sel8uf?width:0) + (imin8uf>>1) + w2*(jmin8uf>>1),
                   newref + (sel8ur?width:0) + (imin8ur>>1) + w2*(jmin8ur>>1),
                   mb,w2,imin8uf&1,jmin8uf&1,imin8ur&1,jmin8ur&1,8);

    /* 16x8 lower half block */
    dmc8i+= dualAbsDiff(oldref + (sel8lf?width:0) + (imin8lf>>1) + w2*(jmin8lf>>1),
                   newref + (sel8lr?width:0) + (imin8lr>>1) + w2*(jmin8lr>>1),
                   mb+8*w2,w2,imin8lf&1,jmin8lf&1,imin8lr&1,jmin8lr&1,8);

    /* select prediction type of minimum distance */
    if (dmcfieldi<dmc8i && dmcfieldi<dmcfieldf && dmcfieldi<dmc8f
        && dmcfieldi<dmcfieldr && dmcfieldi<dmc8r)
    {
      /* field, interpolated */
      mbi->mb_type = MB_FORWARD|MB_BACKWARD;
      mbi->motion_type = MC_FIELD;
      vmc = blockDualMeanDiff(oldref + (self?width:0) + (iminf>>1) + w2*(jminf>>1),
                   newref + (selr?width:0) + (iminr>>1) + w2*(jminr>>1),
                   mb,w2,iminf&1,jminf&1,iminr&1,jminr&1,16);
    }
    else if (dmc8i<dmcfieldf && dmc8i<dmc8f
             && dmc8i<dmcfieldr && dmc8i<dmc8r)
    {
      /* 16x8, interpolated */
      mbi->mb_type = MB_FORWARD|MB_BACKWARD;
      mbi->motion_type = MC_16X8;

      /* upper half block */
      vmc = blockDualMeanDiff(oldref + (sel8uf?width:0) + (imin8uf>>1) + w2*(jmin8uf>>1),
                   newref + (sel8ur?width:0) + (imin8ur>>1) + w2*(jmin8ur>>1),
                   mb,w2,imin8uf&1,jmin8uf&1,imin8ur&1,jmin8ur&1,8);

      /* lower half block */
      vmc+= blockDualMeanDiff(oldref + (sel8lf?width:0) + (imin8lf>>1) + w2*(jmin8lf>>1),
                   newref + (sel8lr?width:0) + (imin8lr>>1) + w2*(jmin8lr>>1),
                   mb+8*w2,w2,imin8lf&1,jmin8lf&1,imin8lr&1,jmin8lr&1,8);
    }
    else if (dmcfieldf<dmc8f && dmcfieldf<dmcfieldr && dmcfieldf<dmc8r)
    {
      /* field, forward */
      mbi->mb_type = MB_FORWARD;
      mbi->motion_type = MC_FIELD;
      vmc = meanDiffBlock(oldref + (self?width:0) + (iminf>>1) + w2*(jminf>>1),
                  mb,w2,iminf&1,jminf&1,16);
    }
    else if (dmc8f<dmcfieldr && dmc8f<dmc8r)
    {
      /* 16x8, forward */
      mbi->mb_type = MB_FORWARD;
      mbi->motion_type = MC_16X8;

      /* upper half block */
      vmc = meanDiffBlock(oldref + (sel8uf?width:0) + (imin8uf>>1) + w2*(jmin8uf>>1),
                  mb,w2,imin8uf&1,jmin8uf&1,8);

      /* lower half block */
      vmc+= meanDiffBlock(oldref + (sel8lf?width:0) + (imin8lf>>1) + w2*(jmin8lf>>1),
                  mb+8*w2,w2,imin8lf&1,jmin8lf&1,8);
    }
    else if (dmcfieldr<dmc8r)
    {
      /* field, backward */
      mbi->mb_type = MB_BACKWARD;
      mbi->motion_type = MC_FIELD;
      vmc = meanDiffBlock(newref + (selr?width:0) + (iminr>>1) + w2*(jminr>>1),
                  mb,w2,iminr&1,jminr&1,16);
    }
    else
    {
      /* 16x8, backward */
      mbi->mb_type = MB_BACKWARD;
      mbi->motion_type = MC_16X8;

      /* upper half block */
      vmc = meanDiffBlock(newref + (sel8ur?width:0) + (imin8ur>>1) + w2*(jmin8ur>>1),
                  mb,w2,imin8ur&1,jmin8ur&1,8);

      /* lower half block */
      vmc+= meanDiffBlock(newref + (sel8lr?width:0) + (imin8lr>>1) + w2*(jmin8lr>>1),
                  mb+8*w2,w2,imin8lr&1,jmin8lr&1,8);
    }

    /* select between intra and non-intra coding */
    if (vmc>var && vmc>=9*256)
      mbi->mb_type = MB_INTRA;
    else
    {
      var = vmc;
      if (mbi->motion_type==MC_FIELD)
      {
        /* forward */
        mbi->MV[0][0][0] = iminf - (i<<1);
        mbi->MV[0][0][1] = jminf - (j<<1);
        mbi->mv_field_sel[0][0] = self;
        /* backward */
        mbi->MV[0][1][0] = iminr - (i<<1);
        mbi->MV[0][1][1] = jminr - (j<<1);
        mbi->mv_field_sel[0][1] = selr;
      }
      else /* MC_16X8 */
      {
        /* forward */
        mbi->MV[0][0][0] = imin8uf - (i<<1);
        mbi->MV[0][0][1] = jmin8uf - (j<<1);
        mbi->mv_field_sel[0][0] = sel8uf;
        mbi->MV[1][0][0] = imin8lf - (i<<1);
        mbi->MV[1][0][1] = jmin8lf - ((j+8)<<1);
        mbi->mv_field_sel[1][0] = sel8lf;
        /* backward */
        mbi->MV[0][1][0] = imin8ur - (i<<1);
        mbi->MV[0][1][1] = jmin8ur - (j<<1);
        mbi->mv_field_sel[0][1] = sel8ur;
        mbi->MV[1][1][0] = imin8lr - (i<<1);
        mbi->MV[1][1][1] = jmin8lr - ((j+8)<<1);
        mbi->mv_field_sel[1][1] = sel8lr;
      }
    }
  }

  mbi->var = var;
}


/*
 * 前向和交织帧的运动估计
 *
 *oldorg：前向预测的原始参考图（供P图像和B图像使用）
 *neworg：后向预测的原始参考图（仅供B图像使用）
 *oldref：前向预测的重建图像（P图像和B图像）
 *newref：后向预测的重建图像（B图像）
 *cur：当前原始图像帧（由预测图像产生的）
 *curref：当前的重建图像帧（用来根据第一个场预测第二个场）
 *sxf，syf：前向搜索窗口
 *sxb，syb：后向搜索窗口
 *mbi：指向宏块信息结构体的指针
 *输出结果mbi->
 *mb_type: 0, MB_INTRA, MB_FORWARD, MB_BACKWARD, MB_FORWARD|MB_BACKWARD
 *MV[][][]: 运动向量 (场格式)
 *mv_field_sel: 顶部/底部场
 *motion_type: MC_FIELD, MC_16X8
 */
void motionEstimation (unsigned char *oldorg, unsigned char *neworg,
  unsigned char *oldref, unsigned char *newref, unsigned char *cur,
  unsigned char *curref, int sxf, int syf, int sxb, int syb,
  struct mbinfo *mbi, int secondfield, int ipflag)
{
  int i, j;

    /* 对所有的宏块进行循环操作 */
  for (j=0; j<height2; j+=16)
  {
    for (i=0; i<width; i+=16)
    {
         /*根据图像结构的类型来判断是进行帧预测还是场预测*/
      if (pict_struct==FRAME_PICTURE)
        frame_ME(oldorg,neworg,oldref,newref,cur,i,j,sxf,syf,sxb,syb,mbi);
      //采用帧预测
      else
        field_ME(oldorg,neworg,oldref,newref,cur,curref,i,j,sxf,syf,sxb,syb,
          mbi,secondfield,ipflag);
      //采用场预测
      mbi++;
    }

    if (!quiet)
    {
        currentGroup++;//输出统计信息
      //putc('.',stderr);
      //fflush(stderr);
    }
  }
  if (!quiet)
    {
      //putc('\n',stderr);
      currentGroup=0;
  }
}
