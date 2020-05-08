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

  /* prevent division by zero in calcSNR() */
  if(e2==0.0)
    e2 = 0.00001;

  *pv = s2 - s1*s1; /* variance */
  *pe = e2;         /* MSE */
}


void calcRatio(unsigned char *org[3],unsigned char *rec[3])
{
  int w,h,offs;
  double v,e;

  w = horizontal_size;
  h = (pict_struct==FRAME_PICTURE) ? vertical_size : (vertical_size>>1);
  offs = (pict_struct==BOTTOM_FIELD) ? width : 0;

  subRatio(org[0]+offs,rec[0]+offs,width2,w,h,&v,&e);
  //fprintf(statfile,"Y: variance=%4.4g, MSE=%3.3g (%3.3g dB), SNR=%3.3g dB\n",
    //v, e, 10.0*log10(255.0*255.0/e), 10.0*log10(v/e));
  tmpPicture.YV=v;
  tmpPicture.YMSE=e;
  tmpPicture.YMSEdb=10.0*log10(255.0*255.0/e);
  tmpPicture.YSNR=10.0*log10(v/e);

  if (chroma_format!=CHROMA444)
  {
    w >>= 1;
    offs >>= 1;
  }

  if (chroma_format==CHROMA420)
    h >>= 1;

  subRatio(org[1]+offs,rec[1]+offs,chrom_width2,w,h,&v,&e);
  //fprintf(statfile,"U: variance=%4.4g, MSE=%3.3g (%3.3g dB), SNR=%3.3g dB\n",
    //v, e, 10.0*log10(255.0*255.0/e), 10.0*log10(v/e));
   tmpPicture.UV=v;
   tmpPicture.UMSE=e;
   tmpPicture.UMSEdb=10.0*log10(255.0*255.0/e);
   tmpPicture.USNR=10.0*log10(v/e);

  subRatio(org[2]+offs,rec[2]+offs,chrom_width2,w,h,&v,&e);
  //fprintf(statfile,"V: variance=%4.4g, MSE=%3.3g (%3.3g dB), SNR=%3.3g dB\n",
    //v, e, 10.0*log10(255.0*255.0/e), 10.0*log10(v/e));
  tmpPicture.VV=v;
  tmpPicture.VMSE=e;
  tmpPicture.VMSEdb=10.0*log10(255.0*255.0/e);
  tmpPicture.VSNR=10.0*log10(v/e);
}



void insertStatistics()
{
  int i, j, k, nmb, mb_type;
  int n_skipped, n_intra, n_ncoded, n_blocks, n_interp, n_forward, n_backward;
  struct mbinfo *mbi;

  nmb = mb_width*mb_height2;

  n_skipped=n_intra=n_ncoded=n_blocks=n_interp=n_forward=n_backward=0;

  for (k=0; k<nmb; k++)
  {
    mbi = mbinfo+k;
    if (mbi->skipped)
      n_skipped++;
    else if (mbi->mb_type & MB_INTRA)
      n_intra++;
    else if (!(mbi->mb_type & MB_PATTERN))
      n_ncoded++;

    for (i=0; i<block_count; i++)
      if (mbi->cbp & (1<<i))
        n_blocks++;

    if (mbi->mb_type & MB_FORWARD)
    {
      if (mbi->mb_type & MB_BACKWARD)
        n_interp++;
      else
        n_forward++;
    }
    else if (mbi->mb_type & MB_BACKWARD)
      n_backward++;
  }

  //fprintf(statfile,"\npicture statistics:\n");
  //fprintf(statfile," # of intra coded macroblocks:  %4d (%.1f%%)\n",
   // n_intra,100.0*(double)n_intra/nmb);
  //fprintf(statfile," # of coded blocks:             %4d (%.1f%%)\n",
  //  n_blocks,100.0*(double)n_blocks/(block_count*nmb));
  //fprintf(statfile," # of not coded macroblocks:    %4d (%.1f%%)\n",
  //  n_ncoded,100.0*(double)n_ncoded/nmb);
  //fprintf(statfile," # of skipped macroblocks:      %4d (%.1f%%)\n",
  //  n_skipped,100.0*(double)n_skipped/nmb);
  //fprintf(statfile," # of forw. pred. macroblocks:  %4d (%.1f%%)\n",
  //  n_forward,100.0*(double)n_forward/nmb);
  //fprintf(statfile," # of backw. pred. macroblocks: %4d (%.1f%%)\n",
   // n_backward,100.0*(double)n_backward/nmb);
  //fprintf(statfile," # of interpolated macroblocks: %4d (%.1f%%)\n",
   // n_interp,100.0*(double)n_interp/nmb);
  tmpPicture.hash1=n_intra;
  tmpPicture.hash2=n_blocks;
  tmpPicture.hash3=n_ncoded;
  tmpPicture.hash4=n_skipped;
  tmpPicture.hash5=n_forward;
  tmpPicture.hash6=n_backward;
  tmpPicture.hash7=n_interp;
  tmpPicture.hashp1=100.0*(double)n_intra/nmb;
  tmpPicture.hashp2=100.0*(double)n_blocks/(block_count*nmb);
  tmpPicture.hashp3=100.0*(double)n_ncoded/nmb;
  tmpPicture.hashp4=100.0*(double)n_skipped/nmb;
  tmpPicture.hashp5=100.0*(double)n_forward/nmb;
  tmpPicture.hashp6=100.0*(double)n_backward/nmb;
  tmpPicture.hashp7=100.0*(double)n_interp/nmb;

  //fprintf(statfile,"\nmacroblock_type map:\n");

  k = 0;

  tmpPicture.rountineOut=mb_height2;
  tmpPicture.rountineIn=mb_width;
  tmpPicture.mmap1.resize(tmpPicture.rountineOut);
  tmpPicture.mmap2.resize(tmpPicture.rountineOut);
  tmpPicture.quantmap.resize(tmpPicture.rountineOut);
  for(int i=0;i<tmpPicture.rountineOut;i++)
  {
      (tmpPicture.mmap1)[i].resize(tmpPicture.rountineIn);
      (tmpPicture.mmap2)[i].resize(tmpPicture.rountineIn);
      (tmpPicture.quantmap)[i].resize(tmpPicture.rountineIn);
  }

  for (j=0; j<mb_height2; j++)
  {
    for (i=0; i<mb_width; i++)
    {
      mbi = mbinfo + k;
      mb_type = mbi->mb_type;
      if (mbi->skipped)
        {
          //putc('S',statfile);
          (tmpPicture.mmap1)[j][i]='S';
      }
      else if (mb_type & MB_INTRA)
        {
          //putc('I',statfile);
          (tmpPicture.mmap1)[j][i]='I';
      }
      else switch (mb_type & (MB_FORWARD|MB_BACKWARD))
      {
      case MB_FORWARD:
        //putc(mbi->motion_type==MC_FIELD ? 'f' :
        //     mbi->motion_type==MC_DMV   ? 'p' :
         //                                 'F',statfile);
       (tmpPicture.mmap1)[j][i]= mbi->motion_type==MC_FIELD ? 'f' :
                                                              mbi->motion_type==MC_DMV   ? 'p' :
                                                                                           'F';
          break;
      case MB_BACKWARD:
        //putc(mbi->motion_type==MC_FIELD ? 'b' :
         //                                 'B',statfile);
          (tmpPicture.mmap1)[j][i]=mbi->motion_type==MC_FIELD ? 'b' :
                                                                'B';
          break;
      case MB_FORWARD|MB_BACKWARD:
        //putc(mbi->motion_type==MC_FIELD ? 'd' :
         //                                 'D',statfile);
          (tmpPicture.mmap1)[j][i]=mbi->motion_type==MC_FIELD ? 'd' :
                                                                'D';
          break;
      default:
        //putc('0',statfile);
        (tmpPicture.mmap1)[j][i]='0';
          break;
      }

      if (mb_type & MB_QUANT)
        {
          //putc('Q',statfile);
          (tmpPicture.mmap2)[j][i]='Q';
      }
      else if (mb_type & (MB_PATTERN|MB_INTRA))
        {
          //putc(' ',statfile);
          (tmpPicture.mmap2)[j][i]=' ';
      }
      else
        {
          //putc('N',statfile);
          (tmpPicture.mmap2)[j][i]='N';
      }

      //putc(' ',statfile);

      k++;
    }
    //putc('\n',statfile);
  }

  //fprintf(statfile,"\nmquant map:\n");

  k=0;
  for (j=0; j<mb_height2; j++)
  {
    for (i=0; i<mb_width; i++)
    {
      (tmpPicture.quantmap)[j][i]=mbinfo[k].mquant;
      k++;
    }
    //putc('\n',statfile);
  }
    pictureDATA.push_back(tmpPicture);

}
