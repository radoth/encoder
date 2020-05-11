/*恢复重建图像*/

#include <cstdio>
#include <cstdlib>
#include "timeSettings.h"
#include "commonData.h"

bool reconstructPicture(char *fname,unsigned char *frame[])

{
  int chrom_hsize, chrom_vsize;
  char name[128];
  FILE *fd;

  chrom_hsize = (chromaFormat==CHROMA444) ? horiSize
                                           : horiSize>>1;

  chrom_vsize = (chromaFormat!=CHROMA420) ? vertiSize
                                           : vertiSize>>1;

  if (fname[0]=='-')
    return true;

   /* Y */
   sprintf(name,"%s.Y",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     {error(errortext);return false;}
   }
   fwrite(frame[0],1,horiSize*vertiSize,fd);
   fclose(fd);
 
   /* Cb */
   sprintf(name,"%s.U",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     {error(errortext);return false;}
   }
   fwrite(frame[1],1,chrom_hsize*chrom_vsize,fd);
   fclose(fd);
 
   /* Cr */
   sprintf(name,"%s.V",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     {error(errortext);return false;}
   }
   fwrite(frame[2],1,chrom_hsize*chrom_vsize,fd);
  fclose(fd);
  return true;
}
