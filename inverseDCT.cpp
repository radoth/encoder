/* 反向快速离散余弦变换*/

#include "timeSettings.h"
void initIdct (void);
void idct (short *block);
static short clipTable[1024];
static short *clipPointer;
static void rowIDCT(short *blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;
  if (!((x1 = blk[4]<<11) | (x2 = blk[6]) | (x3 = blk[2]) |
        (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
  {
    blk[0]=blk[1]=blk[2]=blk[3]=blk[4]=blk[5]=blk[6]=blk[7]=blk[0]<<3;
    return;
  }
  x0 = (blk[0]<<11) + 128;
  x8 = 565*(x4+x5);
  x4 = x8 + (2841-565)*x4;
  x5 = x8 - (2841+565)*x5;
  x8 = 2408*(x6+x7);
  x6 = x8 - (2408-1609)*x6;
  x7 = x8 - (2408+1609)*x7;
  x8 = x0 + x1;
  x0 -= x1;
  x1 = 1108*(x3+x2);
  x2 = x1 - (2676+1108)*x2;
  x3 = x1 + (2676-1108)*x3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  blk[0] = (x7+x1)>>8;
  blk[1] = (x3+x2)>>8;
  blk[2] = (x0+x4)>>8;
  blk[3] = (x8+x6)>>8;
  blk[4] = (x8-x6)>>8;
  blk[5] = (x0-x4)>>8;
  blk[6] = (x3-x2)>>8;
  blk[7] = (x7-x1)>>8;
}

static void idctcol(short *blk)
{
  int x0, x1, x2, x3, x4, x5, x6, x7, x8;
  if (!((x1 = (blk[8*4]<<8)) | (x2 = blk[8*6]) | (x3 = blk[8*2]) |
        (x4 = blk[8*1]) | (x5 = blk[8*7]) | (x6 = blk[8*5]) | (x7 = blk[8*3])))
  {
    blk[8*0]=blk[8*1]=blk[8*2]=blk[8*3]=blk[8*4]=blk[8*5]=blk[8*6]=blk[8*7]=
      clipPointer[(blk[8*0]+32)>>6];
    return;
  }

  x0 = (blk[8*0]<<8) + 8192;
  x8 = 565*(x4+x5) + 4;
  x4 = (x8+(2841-565)*x4)>>3;
  x5 = (x8-(2841+565)*x5)>>3;
  x8 = 2408*(x6+x7) + 4;
  x6 = (x8-(2408-1609)*x6)>>3;
  x7 = (x8-(2408+1609)*x7)>>3;
  x8 = x0 + x1;
  x0 -= x1;
  x1 = 1108*(x3+x2) + 4;
  x2 = (x1-(2676+1108)*x2)>>3;
  x3 = (x1+(2676-1108)*x3)>>3;
  x1 = x4 + x6;
  x4 -= x6;
  x6 = x5 + x7;
  x5 -= x7;
  x7 = x8 + x3;
  x8 -= x3;
  x3 = x0 + x2;
  x0 -= x2;
  x2 = (181*(x4+x5)+128)>>8;
  x4 = (181*(x4-x5)+128)>>8;
  blk[8*0] = clipPointer[(x7+x1)>>14];
  blk[8*1] = clipPointer[(x3+x2)>>14];
  blk[8*2] = clipPointer[(x0+x4)>>14];
  blk[8*3] = clipPointer[(x8+x6)>>14];
  blk[8*4] = clipPointer[(x8-x6)>>14];
  blk[8*5] = clipPointer[(x0-x4)>>14];
  blk[8*6] = clipPointer[(x3-x2)>>14];
  blk[8*7] = clipPointer[(x7-x1)>>14];
}


void idct(short *block)
{
  int i;

  for (i=0; i<8; i++)
    rowIDCT(block+8*i);

  for (i=0; i<8; i++)
    idctcol(block+i);
}

void initIdct()
{
  int i;

  clipPointer = clipTable+512;
  for (i= -512; i<512; i++)
    clipPointer[i] = (i<-256) ? -256 : ((i>255) ? 255 : i);
}
