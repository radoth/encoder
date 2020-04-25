/* putbits.c, ±ÈÌØÊä³ö                                             */


#include <stdio.h>
#include "config.h"

extern FILE *outfile; /* the only global var we need here */

/* private data */
static unsigned char outbfr;
static int outcnt;
static int bytecnt;

/* initialize buffer, call once before first putbits or alignbits */
void initbits()
{
  outcnt = 8;
  bytecnt = 0;
}

/* write rightmost n (0<=n<=32) bits of val to outfile */
void putbits(val,n)
int val;
int n;
{
  int i;
  unsigned int mask;

  mask = 1 << (n-1); /* selects first (leftmost) bit */

  for (i=0; i<n; i++)
  {
    outbfr <<= 1;

    if (val & mask)
      outbfr|= 1;

    mask >>= 1; /* select next bit */
    outcnt--;

    if (outcnt==0) /* 8 bit buffer full */
    {
      putc(outbfr,outfile);
      outcnt = 8;
      bytecnt++;
    }
  }
}

/* zero bit stuffing to next byte boundary (5.2.3, 6.2.1) */
void alignbits()
{
  if (outcnt!=8)
    putbits(0,outcnt);
}

/* return total number of generated bits */
int bitcount()
{
  return 8*bytecnt + (8-outcnt);
}
