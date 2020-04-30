/* mpeg2enc.c, main() 和读取参数文件函数*/



#include <cstdio>
#include <cstdlib>

#define GLOBAL /* used by global.h */
#include "config.h"
#include "global.h"
#include <QString>

/* private prototypes */

bool init()
{
  int i, size;
  static int block_count_tab[3] = {6,8,12};

  initbits();
  init_fdct();
  init_idct();

  /* round picture dimensions to nearest multiple of 16 or 32 */
  mb_width = (horizontal_size+15)/16;
  mb_height = prog_seq ? (vertical_size+15)/16 : 2*((vertical_size+31)/32);
  mb_height2 = fieldpic ? mb_height>>1 : mb_height; /* for field pictures */
  width = 16*mb_width;
  height = 16*mb_height;

  chrom_width = (chroma_format==CHROMA444) ? width : width>>1;
  chrom_height = (chroma_format!=CHROMA420) ? height : height>>1;

  height2 = fieldpic ? height>>1 : height;
  width2 = fieldpic ? width<<1 : width;
  chrom_width2 = fieldpic ? chrom_width<<1 : chrom_width;
  
  block_count = block_count_tab[chroma_format-1];

  /* clip table */
  if (!(clp = (unsigned char *)malloc(1024)))
    {error("malloc failed\n");return false;}
  clp+= 384;
  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  for (i=0; i<3; i++)
  {
    size = (i==0) ? width*height : chrom_width*chrom_height;

    if (!(newrefframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(oldrefframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(auxframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(neworgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(oldorgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(auxorgframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
    if (!(predframe[i] = (unsigned char *)malloc(size)))
      {error("malloc failed\n");return false;}
  }

  mbinfo = (struct mbinfo *)malloc(mb_width*mb_height2*sizeof(struct mbinfo));

  if (!mbinfo)
    {error("malloc failed\n");return false;}

  blocks =
    (short (*)[64])malloc(mb_width*mb_height2*block_count*sizeof(short [64]));

  if (!blocks)
    {error("malloc failed\n");return false;}


  return true;
}

void error(QString text)
{
  //fprintf(stderr,text.toStdString().c_str());
  //putc('\n',stderr);
  hasError=1;
}

