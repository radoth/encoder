/*比特流控制程序*/



#include <cstdio>
#include <cmath>

#include "timeSettings.h"
#include "commonData.h"

/* compute variance of 8x8 block */
static double varianceCalc(unsigned char *p,int lx)
{
  int i, j;
  unsigned int v, s, s2;

  s = s2 = 0;

  for (j=0; j<8; j++)
  {
    for (i=0; i<8; i++)
    {
      v = *p++;
      s+= v;
      s2+= v*v;
    }
    p+= lx - 8;
  }

  return s2/64.0 - (s/64.0)*(s/64.0);
}

static void actjDetermine(unsigned char *frame)
{
  int i,j,k;
  unsigned char *p;
  double actj,var;

  k = 0;

  for (j=0; j<pictureHeight; j+=16)
    for (i=0; i<width; i+=16)
    {
      p = frame + ((pictStruct==BOTTOM_FIELD)?width:0) + i + pictureWidth*j;

      /* take minimum spatial activity measure of luminance blocks */

      actj = varianceCalc(p,pictureWidth);
      var = varianceCalc(p+8,pictureWidth);
      if (var<actj) actj = var;
      var = varianceCalc(p+8*pictureWidth,pictureWidth);
      if (var<actj) actj = var;
      var = varianceCalc(p+8*pictureWidth+8,pictureWidth);
      if (var<actj) actj = var;

      if (!fieldPicFlag && !progSeq)
      {
        /* field */
        var = varianceCalc(p,width<<1);
        if (var<actj) actj = var;
        var = varianceCalc(p+8,width<<1);
        if (var<actj) actj = var;
        var = varianceCalc(p+width,width<<1);
        if (var<actj) actj = var;
        var = varianceCalc(p+width+8,width<<1);
        if (var<actj) actj = var;
      }

      actj+= 1.0;

      MacroBlockInfo[k++].act = actj;
    }
}


/* rate control variables */
int Xi, Xp, Xb, r, d0i, d0p, d0b;
double avg_act;
static int R, T, d;
static double actsum;
static int Np, Nb, S, Q;
static int prev_mquant;

void feedbackInit()
{
  /* reaction parameter (constant) */
  if (r==0)  r = (int)floor(2.0*bitRate/frameRate + 0.5);

  /* average activity */
  if (avg_act==0.0)  avg_act = 400.0;

  /* remaining # of bits in GOP */
  R = 0;

  /* global complexity measure */
  if (Xi==0) Xi = (int)floor(160.0*bitRate/115.0 + 0.5);
  if (Xp==0) Xp = (int)floor( 60.0*bitRate/115.0 + 0.5);
  if (Xb==0) Xb = (int)floor( 42.0*bitRate/115.0 + 0.5);

  /* virtual buffer fullness */
  if (d0i==0) d0i = (int)floor(10.0*r/31.0 + 0.5);
  if (d0p==0) d0p = (int)floor(10.0*r/31.0 + 0.5);
  if (d0b==0) d0b = (int)floor(1.4*10.0*r/31.0 + 0.5);
/*
  if (d0i==0) d0i = (int)floor(10.0*r/(qscale_tab[0] ? 56.0 : 31.0) + 0.5);
  if (d0p==0) d0p = (int)floor(10.0*r/(qscale_tab[1] ? 56.0 : 31.0) + 0.5);
  if (d0b==0) d0b = (int)floor(1.4*10.0*r/(qscale_tab[2] ? 56.0 : 31.0) + 0.5);
*/

  //fprintf(statfile,"\nrate control: sequence initialization\n");
  //fprintf(statfile,
   // " initial global complexity measures (I,P,B): Xi=%d, Xp=%d, Xb=%d\n",
   // Xi, Xp, Xb);
  globalDATA.xi=Xi;
  globalDATA.xp=Xp;
  globalDATA.xb=Xb;
  //fprintf(statfile," reaction parameter: r=%d\n", r);
  globalDATA.r=r;
  //fprintf(statfile,
    //" initial virtual buffer fullness (I,P,B): d0i=%d, d0p=%d, d0b=%d\n",
   // d0i, d0p, d0b);
  globalDATA.d0i=d0i;
  globalDATA.d0p=d0p;
  globalDATA.d0b=d0b;
  //fprintf(statfile," initial average activity: avg_act=%.1f\n", avg_act);
  globalDATA.avgAct=avg_act;
}

void GOPControlInit(int np,int nb)
{
  R += (int) floor((1 + np + nb) * bitRate / frameRate + 0.5);
  Np = fieldPicFlag ? 2*np+1 : np;
  Nb = fieldPicFlag ? 2*nb : nb;

  //fprintf(statfile,"\nrate control: new group of pictures (GOP)\n");
  //fprintf(statfile," target number of bits for GOP: R=%d\n",R);
  //fprintf(statfile," number of P pictures in GOP: Np=%d\n",Np);
  //fprintf(statfile," number of B pictures in GOP: Nb=%d\n",Nb);
  tmpGrp.R=R;
  tmpGrp.Np=Np;
  tmpGrp.Nb=Nb;
  groupDATA.push_back(tmpGrp);
}

/* Note: we need to substitute K for the 1.4 and 1.0 constants -- this can
   be modified to fit image content */

/* Step 1: compute target bits for current picture being coded */
void picControlInit(unsigned char *frame)
{
  double Tmin;

  switch (pictType)
  {
  case I_TYPE:
    T = (int) floor(R/(1.0+Np*Xp/(Xi*1.0)+Nb*Xb/(Xi*1.4)) + 0.5);
    d = d0i;
    break;
  case P_TYPE:
    T = (int) floor(R/(Np+Nb*1.0*Xb/(1.4*Xp)) + 0.5);
    d = d0p;
    break;
  case B_TYPE:
    T = (int) floor(R/(Nb+Np*1.4*Xp/(1.0*Xb)) + 0.5);
    d = d0b;
    break;
  }

  Tmin = (int) floor(bitRate/(8.0*frameRate) + 0.5);

  if (T<Tmin)
    T = Tmin;

  S = dataCount();
  Q = 0;

  actjDetermine(frame);
  actsum = 0.0;

  //fprintf(statfile,"\nrate control: start of picture\n");
  //fprintf(statfile," target number of bits: T=%d\n",T);
  tmpPicture.targetNumberOfBits=T;
}



void updateCalc()
{
  double X;

  S = dataCount() - S; /* total # of bits in picture */
  R-= S; /* remaining # of bits in GOP */
  X = (int) floor(S*((0.5*(double)Q)/(macroBlockWidth*mbHeight2)) + 0.5);
  d+= S - T;
  avg_act = actsum/(macroBlockWidth*mbHeight2);

  switch (pictType)
  {
  case I_TYPE:
    Xi = X;
    d0i = d;
    break;
  case P_TYPE:
    Xp = X;
    d0p = d;
    Np--;
    break;
  case B_TYPE:
    Xb = X;
    d0b = d;
    Nb--;
    break;
  }

  //fprintf(statfile,"\nrate control: end of picture\n");
  //fprintf(statfile," actual number of bits: S=%d\n",S);
  //fprintf(statfile," average quantization parameter Q=%.1f\n",
  //  (double)Q/(mb_width*mb_height2));
  //fprintf(statfile," remaining number of bits in GOP: R=%d\n",R);
  //fprintf(statfile,
  //  " global complexity measures (I,P,B): Xi=%d, Xp=%d, Xb=%d\n",
  //  Xi, Xp, Xb);
  //fprintf(statfile,
   // " virtual buffer fullness (I,P,B): d0i=%d, d0p=%d, d0b=%d\n",
   // d0i, d0p, d0b);
  //fprintf(statfile," remaining number of P pictures in GOP: Np=%d\n",Np);
  //fprintf(statfile," remaining number of B pictures in GOP: Nb=%d\n",Nb);
  //fprintf(statfile," average activity: avg_act=%.1f\n", avg_act);
  tmpPicture.actualBits=S;
  tmpPicture.avgQuanPara=(double)Q/(macroBlockWidth*mbHeight2);
  tmpPicture.reaminNumberGOP=R;
  tmpPicture.Xi=Xi;
  tmpPicture.Xp=Xp;
  tmpPicture.Xb=Xb;
  tmpPicture.d0i=d0i;
  tmpPicture.d0p=d0p;
  tmpPicture.d0b=d0b;
  tmpPicture.Np=Np;
  tmpPicture.Nb=Nb;
  tmpPicture.avg_act=avg_act;
}

/* compute initial quantization stepsize (at the beginning of picture) */
int stepSizeQuantization()
{
  int mquant;

  if (qScaleType)
  {
    mquant = (int) floor(2.0*d*31.0/r + 0.5);

    /* clip mquant to legal (linear) range */
    if (mquant<1)
      mquant = 1;
    if (mquant>112)
      mquant = 112;

    /* map to legal quantization level */
    mquant = nonLinearMquantTable[mapNonLinearMquant[mquant]];
  }
  else
  {
    mquant = (int) floor(d*31.0/r + 0.5);
    mquant <<= 1;

    /* clip mquant to legal (linear) range */
    if (mquant<2)
      mquant = 2;
    if (mquant>62)
      mquant = 62;

    prev_mquant = mquant;
  }

/*
  fprintf(statfile,"rc_start_mb:\n");
  fprintf(statfile,"mquant=%d\n",mquant);
*/

  return mquant;
}

/* Step 2: measure virtual buffer - estimated buffer discrepancy */
int virtualBufferMeasure(int j)
{
  int mquant;
  double dj, Qj, actj, N_actj;

  /* measure virtual buffer discrepancy from uniform distribution model */
  dj = d + (dataCount()-S) - j*(T/(macroBlockWidth*mbHeight2));

  /* scale against dynamic range of mquant and the bits/picture count */
  Qj = dj*31.0/r;
/*Qj = dj*(q_scale_type ? 56.0 : 31.0)/r;  */

  actj = MacroBlockInfo[j].act;
  actsum+= actj;

  /* compute normalized activity */
  N_actj = (2.0*actj+avg_act)/(actj+2.0*avg_act);

  if (qScaleType)
  {
    /* modulate mquant with combined buffer and local activity measures */
    mquant = (int) floor(2.0*Qj*N_actj + 0.5);

    /* clip mquant to legal (linear) range */
    if (mquant<1)
      mquant = 1;
    if (mquant>112)
      mquant = 112;

    /* map to legal quantization level */
    mquant = nonLinearMquantTable[mapNonLinearMquant[mquant]];
  }
  else
  {
    /* modulate mquant with combined buffer and local activity measures */
    mquant = (int) floor(Qj*N_actj + 0.5);
    mquant <<= 1;

    /* clip mquant to legal (linear) range */
    if (mquant<2)
      mquant = 2;
    if (mquant>62)
      mquant = 62;

    /* ignore small changes in mquant */
    if (mquant>=8 && (mquant-prev_mquant)>=-4 && (mquant-prev_mquant)<=4)
      mquant = prev_mquant;

    prev_mquant = mquant;
  }

  Q+= mquant; /* for calculation of average mquant */

/*
  fprintf(statfile,"rc_calc_mquant(%d): ",j);
  fprintf(statfile,"bitcount=%d, dj=%f, Qj=%f, actj=%f, N_actj=%f, mquant=%d\n",
    bitcount(),dj,Qj,actj,N_actj,mquant);
*/

  return mquant;
}



/* VBV calculations
 *
 * generates warnings if underflow or overflow occurs
 */

/* vbv_end_of_picture
 *
 * - has to be called directly after writing picture_data()
 * - needed for accurate VBV buffer overflow calculation
 * - assumes there is no byte stuffing prior to the next start code
 */

static int bitcnt_EOP;

void endPictureBitPut()
{
  bitcnt_EOP = dataCount();
  bitcnt_EOP = (bitcnt_EOP + 7) & ~7; /* account for bit stuffing */
}

/* calc_vbv_delay
 *
 * has to be called directly after writing the picture start code, the
 * reference point for vbv_delay
 */

void delayCalc()
{
  double picture_delay;
  static double next_ip_delay; /* due to frame reordering delay */
  static double decoding_time;

  /* number of 1/90000 s ticks until next picture is to be decoded */
  if (pictType == B_TYPE)
  {
    if (progSeq)
    {
      if (!ifRepeatFirstFlag)
        picture_delay = 90000.0/frameRate; /* 1 frame */
      else
      {
        if (!topFirstFlag)
          picture_delay = 90000.0*2.0/frameRate; /* 2 frames */
        else
          picture_delay = 90000.0*3.0/frameRate; /* 3 frames */
      }
    }
    else
    {
      /* interlaced */
      if (fieldPicFlag)
        picture_delay = 90000.0/(2.0*frameRate); /* 1 field */
      else
      {
        if (!ifRepeatFirstFlag)
          picture_delay = 90000.0*2.0/(2.0*frameRate); /* 2 flds */
        else
          picture_delay = 90000.0*3.0/(2.0*frameRate); /* 3 flds */
      }
    }
  }
  else
  {
    /* I or P picture */
    if (fieldPicFlag)
    {
      if(topFirstFlag==(pictStruct==TOP_FIELD))
      {
        /* first field */
        picture_delay = 90000.0/(2.0*frameRate);
      }
      else
      {
        /* second field */
        /* take frame reordering delay into account */
        picture_delay = next_ip_delay - 90000.0/(2.0*frameRate);
      }
    }
    else
    {
      /* frame picture */
      /* take frame reordering delay into account*/
      picture_delay = next_ip_delay;
    }

    if (!fieldPicFlag || topFirstFlag!=(pictStruct==TOP_FIELD))
    {
      /* frame picture or second field */
      if (progSeq)
      {
        if (!ifRepeatFirstFlag)
          next_ip_delay = 90000.0/frameRate;
        else
        {
          if (!topFirstFlag)
            next_ip_delay = 90000.0*2.0/frameRate;
          else
            next_ip_delay = 90000.0*3.0/frameRate;
        }
      }
      else
      {
        if (fieldPicFlag)
          next_ip_delay = 90000.0/(2.0*frameRate);
        else
        {
          if (!ifRepeatFirstFlag)
            next_ip_delay = 90000.0*2.0/(2.0*frameRate);
          else
            next_ip_delay = 90000.0*3.0/(2.0*frameRate);
        }
      }
    }
  }

  if (decoding_time==0.0)
  {
    /* first call of calc_vbv_delay */
    /* we start with a 7/8 filled VBV buffer (12.5% back-off) */
    picture_delay = ((VBVBufferSize*16384*7)/8)*90000.0/bitRate;
    if (fieldPicFlag)
      next_ip_delay = (int)(90000.0/frameRate+0.5);
  }

  /* VBV checks */

  /* check for underflow (previous picture) */
  if (!lowDelayFlag && (decoding_time < bitcnt_EOP*90000.0/bitRate))
  {
    /* picture not completely in buffer at intended decoding time */
    if (!quiet)
      {
        //fprintf(stderr,"vbv_delay underflow! (decoding_time=%.1f, t_EOP=%.1f\n)",
        //decoding_time, bitcnt_EOP*90000.0/bit_rate);
        warningTextGlobal.append(QString("vbv_delay 下溢。 (解码时间= %1 , t_EOP= %2 )").arg(decoding_time).arg(bitcnt_EOP*90000.0/bitRate));
    }
  }

  /* when to decode current frame */
  decoding_time += picture_delay;

  /* warning: bitcount() may overflow (e.g. after 9 min. at 8 Mbit/s */
  VBVDelay = (int)(decoding_time - dataCount()*90000.0/bitRate);

  /* check for overflow (current picture) */
  if ((decoding_time - bitcnt_EOP*90000.0/bitRate)
      > (VBVBufferSize*16384)*90000.0/bitRate)
  {
    if (!quiet)
      {
        //fprintf(stderr,"vbv_delay overflow!\n");
        warningTextGlobal.append("vbv_delay 上溢");
    }
  }

  //fprintf(statfile,
    //"\nvbv_delay=%d (bitcount=%d, decoding_time=%.2f, bitcnt_EOP=%d)\n",
    //vbv_delay,bitcount(),decoding_time,bitcnt_EOP);
  tmpPicture.vbvDelay=VBVDelay;
  tmpPicture.bitcount=dataCount();
  tmpPicture.vbvDcdTime=decoding_time;
  tmpPicture.bitcnt_EOP=bitcnt_EOP;

  if (VBVDelay<0)
  {
    if (!quiet)
      {
        //fprintf(stderr,"vbv_delay underflow: %d\n",vbv_delay);
        warningTextGlobal.append(QString("vbv_delay 下溢: vbv delay: %1").arg(VBVDelay));
    }
    VBVDelay = 0;
  }

  if (VBVDelay>65535)
  {
    if (!quiet)
      {
        //fprintf(stderr,"vbv_delay overflow: %d\n",vbv_delay);
        warningTextGlobal.append(QString("vbv_delay 上溢： vbv delay: %1").arg(VBVDelay));
    }
    VBVDelay = 65535;
  }
}
