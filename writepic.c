/* writepic.c, »Ö¸´ÖØ½¨Í¼Ïñ*/



#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "global.h"

void writeframe(fname,frame)
char *fname;
unsigned char *frame[];
{
  int chrom_hsize, chrom_vsize;
  char name[128];
  FILE *fd;

  chrom_hsize = (chroma_format==CHROMA444) ? horizontal_size
                                           : horizontal_size>>1;

  chrom_vsize = (chroma_format!=CHROMA420) ? vertical_size
                                           : vertical_size>>1;

  if (fname[0]=='-')
    return;

   /* Y */
   sprintf(name,"%s.Y",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     error(errortext);
   }
   fwrite(frame[0],1,horizontal_size*vertical_size,fd);
   fclose(fd);
 
   /* Cb */
   sprintf(name,"%s.U",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     error(errortext);
   }
   fwrite(frame[1],1,chrom_hsize*chrom_vsize,fd);
   fclose(fd);
 
   /* Cr */
   sprintf(name,"%s.V",fname);
   if (!(fd = fopen(name,"wb")))
   {
     sprintf(errortext,"Couldn't create %s\n",name);
     error(errortext);
   }
   fwrite(frame[2],1,chrom_hsize*chrom_vsize,fd);
  fclose(fd);
}
