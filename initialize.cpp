/*读取参数文件函数*/



#include <cstdio>
#include <cstdlib>

#define GLOBAL /* used by global.h */

#include "commonData.h"
#include <QString>
#include "timeSettings.h"
/* private prototypes */

bool init()
{
  int i, size;
  static int block_count_tab[3] = {6,8,12};

  writeInit();
  initFdct();
  initIdct();

  /* round picture dimensions to nearest multiple of 16 or 32 */
  macroBlockWidth = (horiSize+15)/16;
  macroBlockHeight = progSeq ? (vertiSize+15)/16 : 2*((vertiSize+31)/32);
  mbHeight2 = fieldPicFlag ? macroBlockHeight>>1 : macroBlockHeight; /* for field pictures */
  width = 16*macroBlockWidth;
  height = 16*macroBlockHeight;

  chromWidth = (chromaFormat==CHROMA444) ? width : width>>1;
  chromHeight = (chromaFormat!=CHROMA420) ? height : height>>1;

  pictureHeight = fieldPicFlag ? height>>1 : height;
  pictureWidth = fieldPicFlag ? width<<1 : width;
  chromWidth2 = fieldPicFlag ? chromWidth<<1 : chromWidth;
  
  blockCount = block_count_tab[chromaFormat-1];

  /* clip table */
  if (!(clp = (unsigned char *)malloc(1024)))
    {error("malloc failed\n");return false;}
  clp+= 384;
  for (i=-384; i<640; i++)
    clp[i] = (i<0) ? 0 : ((i>255) ? 255 : i);

  for (i=0; i<3; i++)
  {
    size = (i==0) ? width*height : chromWidth*chromHeight;

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

  MacroBlockInfo = (struct MacroBlockInfo *)malloc(macroBlockWidth*mbHeight2*sizeof(struct MacroBlockInfo));

  if (!MacroBlockInfo)
    {error("malloc failed\n");return false;}

  blocks =
    (short (*)[64])malloc(macroBlockWidth*mbHeight2*blockCount*sizeof(short [64]));

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

