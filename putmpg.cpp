/* putmpg.c, 块和运动向量编码函数*/


#include <cstdio>
#include "config.h"
#include "global.h"

/* generate variable length codes for an intra-coded block (6.2.6, 6.3.17) */
void putintrablk(short *blk,int cc)
{
  int n, dct_diff, run, signed_level;

  /* DC coefficient (7.2.1) */
  dct_diff = blk[0] - dc_dct_pred[cc]; /* difference to previous block */
  dc_dct_pred[cc] = blk[0];

  if (cc==0)
    putDClum(dct_diff);
  else
    putDCchrom(dct_diff);

  /* AC coefficients (7.2.2) */
  run = 0;
  for (n=1; n<64; n++)
  {
    /* use appropriate entropy scanning pattern */
    signed_level = blk[(altscan ? alternate_scan : zig_zag_scan)[n]];
    if (signed_level!=0)
    {
      putAC(run,signed_level,intravlc);
      run = 0;
    }
    else
      run++; /* count zero coefficients */
  }

  /* End of Block -- normative block punctuation */
  if (intravlc)
    putbits(6,4); /* 0110 (Table B-15) */
  else
    putbits(2,2); /* 10 (Table B-14) */
}

/* generate variable length codes for a non-intra-coded block (6.2.6, 6.3.17) */
void putnonintrablk(short *blk)
{
  int n, run, signed_level, first;

  run = 0;
  first = 1;

  for (n=0; n<64; n++)
  {
    /* use appropriate entropy scanning pattern */
    signed_level = blk[(altscan ? alternate_scan : zig_zag_scan)[n]];

    if (signed_level!=0)
    {
      if (first)
      {
        /* first coefficient in non-intra block */
        putACfirst(run,signed_level);
        first = 0;
      }
      else
        putAC(run,signed_level,0);

      run = 0;
    }
    else
      run++; /* count zero coefficients */
  }

  /* End of Block -- normative block punctuation  */
  putbits(2,2);
}

/* generate variable length code for a motion vector component (7.6.3.1) */
void putmv(int dmv,int f_code)
{
  int r_size, f, vmin, vmax, dv, temp, motion_code, motion_residual;

  r_size = f_code - 1; /* number of fixed length code ('residual') bits */
  f = 1<<r_size;
  vmin = -16*f; /* lower range limit */
  vmax = 16*f - 1; /* upper range limit */
  dv = 32*f;

  /* fold vector difference into [vmin...vmax] */
  if (dmv>vmax)
    dmv-= dv;
  else if (dmv<vmin)
    dmv+= dv;

  /* check value */
  if (dmv<vmin || dmv>vmax)
    if (!quiet)
      fprintf(stderr,"invalid motion vector\n");

  /* split dmv into motion_code and motion_residual */
  temp = ((dmv<0) ? -dmv : dmv) + f - 1;
  motion_code = temp>>r_size;
  if (dmv<0)
    motion_code = -motion_code;
  motion_residual = temp & (f-1);

  putmotioncode(motion_code); /* variable length code */

  if (r_size!=0 && motion_code!=0)
    putbits(motion_residual,r_size); /* fixed length code */
}
