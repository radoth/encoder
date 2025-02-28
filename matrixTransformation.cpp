/*前向/逆变换*/

#include "timeSettings.h"

#include <cstdio>
#include <cmath>
#include "commonData.h"
static void processPrediction(unsigned char *pred,unsigned char *cur,int lx,short *blk)
{
  int i, j;

  for (j=0; j<8; j++)
  {
    for (i=0; i<8; i++)
      cur[i] = clp[blk[i] + pred[i]];
    blk+= 8;
    cur+= lx;
    pred+= lx;
  }
}

static void restorePrediction(unsigned char *pred,unsigned char *cur,int lx,short *blk)
{
  int i, j;

  for (j=0; j<8; j++)
  {
    for (i=0; i<8; i++)
      blk[i] = cur[i] - pred[i];
    blk+= 8;
    cur+= lx;
    pred+= lx;
  }
}

void matrixTransform(unsigned char *pred[],unsigned char *cur[],struct MacroBlockInfo *mbi,short blocks[][64])
{
  int i, j, i1, j1, k, n, cc, offs, lx;

  k = 0;

  for (j=0; j<pictureHeight; j+=16)
    for (i=0; i<width; i+=16)
    {
      for (n=0; n<blockCount; n++)
      {
        cc = (n<4) ? 0 : (n&1)+1;
        if (cc==0)
        {
          if ((pictStruct==FRAME_PICTURE) && mbi[k].DCTType)
          {
            offs = i + ((n&1)<<3) + width*(j+((n&2)>>1));
            lx = width<<1;
          }
          else
          {
            offs = i + ((n&1)<<3) + pictureWidth*(j+((n&2)<<2));
            lx = pictureWidth;
          }

          if (pictStruct==BOTTOM_FIELD)
            offs += width;
        }
        else
        {
          i1 = (chromaFormat==CHROMA444) ? i : i>>1;
          j1 = (chromaFormat!=CHROMA420) ? j : j>>1;

          if ((pictStruct==FRAME_PICTURE) && mbi[k].DCTType
              && (chromaFormat!=CHROMA420))
          {
            offs = i1 + (n&8) + chromWidth*(j1+((n&2)>>1));
            lx = chromWidth<<1;
          }
          else
          {
            offs = i1 + (n&8) + chromWidth2*(j1+((n&2)<<2));
            lx = chromWidth2;
          }

          if (pictStruct==BOTTOM_FIELD)
            offs += chromWidth;
        }

        restorePrediction(pred[cc]+offs,cur[cc]+offs,lx,blocks[k*blockCount+n]);
        fDCTCalc(blocks[k*blockCount+n]);
      }

      k++;
    }
}

void matrixInverseTransform(unsigned char *pred[],unsigned char *cur[],struct MacroBlockInfo *mbi,short blocks[][64])
{
  int i, j, i1, j1, k, n, cc, offs, lx;

  k = 0;

  for (j=0; j<pictureHeight; j+=16)
    for (i=0; i<width; i+=16)
    {
      for (n=0; n<blockCount; n++)
      {
        cc = (n<4) ? 0 : (n&1)+1;

        if (cc==0)
        {
          if ((pictStruct==FRAME_PICTURE) && mbi[k].DCTType)
          {
            offs = i + ((n&1)<<3) + width*(j+((n&2)>>1));
            lx = width<<1;
          }
          else
          {
            offs = i + ((n&1)<<3) + pictureWidth*(j+((n&2)<<2));
            lx = pictureWidth;
          }

          if (pictStruct==BOTTOM_FIELD)
            offs += width;
        }
        else
        {
          i1 = (chromaFormat==CHROMA444) ? i : i>>1;
          j1 = (chromaFormat!=CHROMA420) ? j : j>>1;

          if ((pictStruct==FRAME_PICTURE) && mbi[k].DCTType
              && (chromaFormat!=CHROMA420))
          {
            offs = i1 + (n&8) + chromWidth*(j1+((n&2)>>1));
            lx = chromWidth<<1;
          }
          else
          {
            offs = i1 + (n&8) + chromWidth2*(j1+((n&2)<<2));
            lx = chromWidth2;
          }

          if (pictStruct==BOTTOM_FIELD)
            offs += chromWidth;
        }

        idct(blocks[k*blockCount+n]);
        processPrediction(pred[cc]+offs,cur[cc]+offs,lx,blocks[k*blockCount+n]);
      }

      k++;
    }
}

void chooseDCT(unsigned char *pred,unsigned char *cur,struct MacroBlockInfo *mbi)
{
  short blk0[128], blk1[128];
  int i, j, i0, j0, k, offs, s0, s1, sq0, sq1, s01;
  double d, r;

  k = 0;

  for (j0=0; j0<pictureHeight; j0+=16)
    for (i0=0; i0<width; i0+=16)
    {
      if (framePredDct || pictStruct!=FRAME_PICTURE)
        mbi[k].DCTType = 0;
      else
      {
        for (j=0; j<8; j++)
        {
          offs = width*((j<<1)+j0) + i0;
          for (i=0; i<16; i++)
          {
            blk0[16*j+i] = cur[offs] - pred[offs];
            blk1[16*j+i] = cur[offs+width] - pred[offs+width];
            offs++;
          }
        }
        s0=s1=sq0=sq1=s01=0;

        for (i=0; i<128; i++)
        {
          s0+= blk0[i];
          sq0+= blk0[i]*blk0[i];
          s1+= blk1[i];
          sq1+= blk1[i]*blk1[i];
          s01+= blk0[i]*blk1[i];
        }

        d = (sq0-(s0*s0)/128.0)*(sq1-(s1*s1)/128.0);

        if (d>0.0)
        {
          r = (s01-(s0*s1)/128.0)/sqrt(d);
          if (r>0.5)
            mbi[k].DCTType = 0;
          else
            mbi[k].DCTType = 1;
        }
        else
          mbi[k].DCTType = 1;
      }
      k++;
    }
}
