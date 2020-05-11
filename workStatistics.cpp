/* 编码数据统计*/
#include "timeSettings.h"
#include <cstdio>
#include <cmath>
#include "commonData.h"

static void subRatio(unsigned char *org,unsigned char *rec,int lx,int w,int h,double *pv,double *pe)
{
  int i, j;
  double v1, s1, s2, e2;

  s1 = s2 = e2 = 0.0;

  for (j=0; j<h; j++)
  {
    for (i=0; i<w; i++)
    {
      v1 = org[i];
      s1+= v1;
      s2+= v1*v1;
      v1-= rec[i];
      e2+= v1*v1;
    }
    org += lx;
    rec += lx;
  }

  s1 /= w*h;
  s2 /= w*h;
  e2 /= w*h;


  if(e2==0.0)
    e2 = 0.00001;

  *pv = s2 - s1*s1;
  *pe = e2;
}


void calcRatio(unsigned char *org[3],unsigned char *rec[3])
{
  int w,h,offs;
  double v,e;

  w = horiSize;
  h = (pictStruct==FRAME_PICTURE) ? vertiSize : (vertiSize>>1);
  offs = (pictStruct==BOTTOM_FIELD) ? width : 0;

  subRatio(org[0]+offs,rec[0]+offs,pictureWidth,w,h,&v,&e);
  tmpPicture.YV=v;
  tmpPicture.YMSE=e;
  tmpPicture.YMSEdb=10.0*log10(255.0*255.0/e);
  tmpPicture.YSNR=10.0*log10(v/e);

  if (chromaFormat!=CHROMA444)
  {
    w >>= 1;
    offs >>= 1;
  }

  if (chromaFormat==CHROMA420)
    h >>= 1;

  subRatio(org[1]+offs,rec[1]+offs,chromWidth2,w,h,&v,&e);

   tmpPicture.UV=v;
   tmpPicture.UMSE=e;
   tmpPicture.UMSEdb=10.0*log10(255.0*255.0/e);
   tmpPicture.USNR=10.0*log10(v/e);

  subRatio(org[2]+offs,rec[2]+offs,chromWidth2,w,h,&v,&e);
  tmpPicture.VV=v;
  tmpPicture.VMSE=e;
  tmpPicture.VMSEdb=10.0*log10(255.0*255.0/e);
  tmpPicture.VSNR=10.0*log10(v/e);
}



void insertStatistics()
{
  int i, j, k, nmb, mb_type;
  int n_skipped, n_intra, n_ncoded, n_blocks, n_interp, n_forward, n_backward;
  struct MacroBlockInfo *mbi;

  nmb = macroBlockWidth*mbHeight2;

  n_skipped=n_intra=n_ncoded=n_blocks=n_interp=n_forward=n_backward=0;

  for (k=0; k<nmb; k++)
  {
    mbi = MacroBlockInfo+k;
    if (mbi->skipped)
      n_skipped++;
    else if (mbi->blockType & MB_INTRA)
      n_intra++;
    else if (!(mbi->blockType & MB_PATTERN))
      n_ncoded++;

    for (i=0; i<blockCount; i++)
      if (mbi->cbp & (1<<i))
        n_blocks++;

    if (mbi->blockType & MB_FORWARD)
    {
      if (mbi->blockType & MB_BACKWARD)
        n_interp++;
      else
        n_forward++;
    }
    else if (mbi->blockType & MB_BACKWARD)
      n_backward++;
  }

  tmpPicture.hash1=n_intra;
  tmpPicture.hash2=n_blocks;
  tmpPicture.hash3=n_ncoded;
  tmpPicture.hash4=n_skipped;
  tmpPicture.hash5=n_forward;
  tmpPicture.hash6=n_backward;
  tmpPicture.hash7=n_interp;
  tmpPicture.hashp1=100.0*(double)n_intra/nmb;
  tmpPicture.hashp2=100.0*(double)n_blocks/(blockCount*nmb);
  tmpPicture.hashp3=100.0*(double)n_ncoded/nmb;
  tmpPicture.hashp4=100.0*(double)n_skipped/nmb;
  tmpPicture.hashp5=100.0*(double)n_forward/nmb;
  tmpPicture.hashp6=100.0*(double)n_backward/nmb;
  tmpPicture.hashp7=100.0*(double)n_interp/nmb;


  k = 0;

  tmpPicture.rountineOut=mbHeight2;
  tmpPicture.rountineIn=macroBlockWidth;
  tmpPicture.mmap1.resize(tmpPicture.rountineOut);
  tmpPicture.mmap2.resize(tmpPicture.rountineOut);
  tmpPicture.quantmap.resize(tmpPicture.rountineOut);
  for(int i=0;i<tmpPicture.rountineOut;i++)
  {
      (tmpPicture.mmap1)[i].resize(tmpPicture.rountineIn);
      (tmpPicture.mmap2)[i].resize(tmpPicture.rountineIn);
      (tmpPicture.quantmap)[i].resize(tmpPicture.rountineIn);
  }

  for (j=0; j<mbHeight2; j++)
  {
    for (i=0; i<macroBlockWidth; i++)
    {
      mbi = MacroBlockInfo + k;
      mb_type = mbi->blockType;
      if (mbi->skipped)
        {

          (tmpPicture.mmap1)[j][i]='S';
      }
      else if (mb_type & MB_INTRA)
        {

          (tmpPicture.mmap1)[j][i]='I';
      }
      else switch (mb_type & (MB_FORWARD|MB_BACKWARD))
      {
      case MB_FORWARD:

       (tmpPicture.mmap1)[j][i]= mbi->motionType==MC_FIELD ? 'f' :
                                                              mbi->motionType==MC_DMV   ? 'p' :
                                                                                           'F';
          break;
      case MB_BACKWARD:

          (tmpPicture.mmap1)[j][i]=mbi->motionType==MC_FIELD ? 'b' :
                                                                'B';
          break;
      case MB_FORWARD|MB_BACKWARD:

          (tmpPicture.mmap1)[j][i]=mbi->motionType==MC_FIELD ? 'd' :
                                                                'D';
          break;
      default:
        (tmpPicture.mmap1)[j][i]='0';
          break;
      }

      if (mb_type & MB_QUANT)
        {
          (tmpPicture.mmap2)[j][i]='Q';
      }
      else if (mb_type & (MB_PATTERN|MB_INTRA))
        {

          (tmpPicture.mmap2)[j][i]=' ';
      }
      else
        {

          (tmpPicture.mmap2)[j][i]='N';
      }


      k++;
    }
  }

  k=0;
  for (j=0; j<mbHeight2; j++)
  {
    for (i=0; i<macroBlockWidth; i++)
    {
      (tmpPicture.quantmap)[j][i]=MacroBlockInfo[k].mquant;
      k++;
    }
  }
    pictureDATA.push_back(tmpPicture);

}
