/* 一致性检测*/


#include <cstdio>
#include <cstdlib>
#include "timeSettings.h"

#include "commonData.h"

/* check for (level independent) parameter limits */
bool rangeChecks()
{
  int i;

  /* range and value checks */

  if (horiSize<1 || horiSize>16383)
    {error("horizontal_size must be between 1 and 16383");return false;}
  if (mpeg1Flag && horiSize>4095)
    {error("horizontal_size must be less than 4096 (MPEG-1)");return false;}
  if ((horiSize&4095)==0)
    {error("horizontal_size must not be a multiple of 4096");return false;}
  if (chromaFormat!=CHROMA444 && horiSize%2 != 0)
    {error("horizontal_size must be a even (4:2:0 / 4:2:2)");return false;}

  if (vertiSize<1 || vertiSize>16383)
    {error("vertical_size must be between 1 and 16383");return false;}
  if (mpeg1Flag && vertiSize>4095)
    {error("vertical size must be less than 4096 (MPEG-1)");return false;}
  if ((vertiSize&4095)==0)
    {error("vertical_size must not be a multiple of 4096");return false;}
  if (chromaFormat==CHROMA420 && vertiSize%2 != 0)
    {error("vertical_size must be a even (4:2:0)");return false;}
  if(fieldPicFlag)
  {
    if (vertiSize%2 != 0)
      {error("vertical_size must be a even (field pictures)");return false;}
    if (chromaFormat==CHROMA420 && vertiSize%4 != 0)
      {error("vertical_size must be a multiple of 4 (4:2:0 field pictures)");return false;}
  }

  if (mpeg1Flag)
  {
    if (aspRatio<1 || aspRatio>14)
      {error("pel_aspect_ratio must be between 1 and 14 (MPEG-1)");return false;}
  }
  else
  {
    if (aspRatio<1 || aspRatio>4)
      {error("aspect_ratio_information must be 1, 2, 3 or 4");return false;}
  }

  if (frameRateCode<1 || frameRateCode>8)
    {error("frame_rate code must be between 1 and 8");return false;}

  if (bitRate<=0.0)
    {error("bit_rate must be positive");return false;}
  if (bitRate > ((1<<30)-1)*400.0)
    {error("bit_rate must be less than 429 Gbit/s");return false;}
  if (mpeg1Flag && bitRate > ((1<<18)-1)*400.0)
    {error("bit_rate must be less than 104 Mbit/s (MPEG-1)");return false;}

  if (VBVBufferSize<1 || VBVBufferSize>0x3ffff)
    {error("vbv_buffer_size must be in range 1..(2^18-1)");return false;}
  if (mpeg1Flag && VBVBufferSize>=1024)
    {error("vbv_buffer_size must be less than 1024 (MPEG-1)");return false;}

  if (chromaFormat<CHROMA420 || chromaFormat>CHROMA444)
    {error("chroma_format must be in range 1...3");return false;}

  if (videoFormat<0 || videoFormat>4)
    {error("video_format must be in range 0...4");return false;}

  if (colorPrimaries<1 || colorPrimaries>7 || colorPrimaries==3)
    {error("color_primaries must be in range 1...2 or 4...7");return false;}

  if (transferCharacteristics<1 || transferCharacteristics>7
      || transferCharacteristics==3)
    {error("transfer_characteristics must be in range 1...2 or 4...7");return false;}

  if (matrixCoefficients<1 || matrixCoefficients>7 || matrixCoefficients==3)
    {error("matrix_coefficients must be in range 1...2 or 4...7");return false;}

  if (displayHorizontalSize<0 || displayHorizontalSize>16383)
    {error("display_horizontal_size must be in range 0...16383");return false;}
  if (displayVerticalSize<0 || displayVerticalSize>16383)
    {error("display_vertical_size must be in range 0...16383");return false;}

  if (DCPrec<0 || DCPrec>3)
    {error("intra_dc_precision must be in range 0...3");return false;}

  for (i=0; i<IPDistance; i++)
  {
    if (motionData[i].forwHorFCode<1 || motionData[i].forwHorFCode>9)
      {error("f_code must be between 1 and 9");return false;}
    if (motionData[i].forwVertFCode<1 || motionData[i].forwVertFCode>9)
     { error("f_code must be between 1 and 9");return false;}
    if (mpeg1Flag && motionData[i].forwHorFCode>7)
      {error("f_code must be le less than 8");return false;}
    if (mpeg1Flag && motionData[i].forwVertFCode>7)
      {error("f_code must be le less than 8");return false;}
    if (motionData[i].sxf<=0)
      {error("search window must be positive");return false;} /* doesn't belong here */
    if (motionData[i].syf<=0)
      {error("search window must be positive");return false;}
    if (i!=0)
    {
      if (motionData[i].backHorFCode<1 || motionData[i].backHorFCode>9)
        {error("f_code must be between 1 and 9");return false;}
      if (motionData[i].backVertFCode<1 || motionData[i].backVertFCode>9)
        {error("f_code must be between 1 and 9");return false;}
      if (mpeg1Flag && motionData[i].backHorFCode>7)
        {error("f_code must be le less than 8");return false;}
      if (mpeg1Flag && motionData[i].backVertFCode>7)
        {error("f_code must be le less than 8");return false;}
      if (motionData[i].sxb<=0)
        {error("search window must be positive");return false;}
      if (motionData[i].syb<=0)
        {error("search window must be positive");return false;}
    }
  }
  return true;
}

/* identifies valid profile / level combinations */
static char profile_level_defined[5][4] =
{
/* HL   H-14 ML   LL  */
  {1,   1,   1,   0},  /* HP   */
  {0,   1,   0,   0},  /* Spat */
  {0,   0,   1,   1},  /* SNR  */
  {1,   1,   1,   1},  /* MP   */
  {0,   0,   1,   0}   /* SP   */
};

static struct level_limits {
  int hor_f_code;
  int vert_f_code;
  int hor_size;
  int vert_size;
  int sample_rate;
  int bit_rate; /* Mbit/s */
  int vbv_buffer_size; /* 16384 bit steps */
} maxval_tab[4] =
{
  {9, 5, 1920, 1152, 62668800, 80, 597}, /* HL */
  {9, 5, 1440, 1152, 47001600, 60, 448}, /* H-14 */
  {8, 5,  720,  576, 10368000, 15, 112}, /* ML */
  {7, 4,  352,  288,  3041280,  4,  29}  /* LL */
};

#define SP   5
#define MP   4
#define SNR  3
#define SPAT 2
#define HP   1

#define LL  10
#define ML   8
#define H14  6
#define HL   4

bool profileAndLevelChecks()
{
  int i;
  struct level_limits *maxval;

  if (profile<0 || profile>15)
    {error("profile must be between 0 and 15");return false;}

  if (level<0 || level>15)
    {error("level must be between 0 and 15");return false;}

  if (profile>=8)
  {
    if (!quiet)
      {
        //fprintf(stderr,"Warning: profile uses a reserved value, conformance checks skipped\n");
        warningTextGlobal.append("警告：配置文件使用保留值，跳过一致性检查。");
      }
    return true;
  }

  if (profile<HP || profile>SP)
    {error("undefined Profile");return false;}

  if (profile==SNR || profile==SPAT)
    {error("This encoder currently generates no scalable bitstreams");return false;}

  if (level<HL || level>LL || level&1)
    {error("undefined Level");return false;}

  maxval = &maxval_tab[(level-4) >> 1];

  /* check profile@level combination */
  if(!profile_level_defined[profile-1][(level-4) >> 1])
    {error("undefined profile@level combination");return false;}
  

  /* profile (syntax) constraints */

  if (profile==SP && IPDistance!=1)
    {error("Simple Profile does not allow B pictures");return false;}

  if (profile!=HP && chromaFormat!=CHROMA420)
    {error("chroma format must be 4:2:0 in specified Profile");return false;}

  if (profile==HP && chromaFormat==CHROMA444)
    {error("chroma format must be 4:2:0 or 4:2:2 in High Profile");return false;}

  if (profile>=MP) /* SP, MP: constrained repeat_first_field */
  {
    if (frameRateCode<=2 && ifRepeatFirstFlag)
      {error("repeat_first_first must be zero");return false;}
    if (frameRateCode<=6 && progSeq && ifRepeatFirstFlag)
      {error("repeat_first_first must be zero");return false;}
  }

  if (profile!=HP && DCPrec==3)
   { error("11 bit DC precision only allowed in High Profile");return false;}


  /* level (parameter value) constraints */

  /* Table 8-8 */
  if (frameRateCode>5 && level>=ML)
    {error("Picture rate greater than permitted in specified Level");return false;}

  for (i=0; i<IPDistance; i++)
  {
    if (motionData[i].forwHorFCode > maxval->hor_f_code)
      {error("forward horizontal f_code greater than permitted in specified Level");return false;}

    if (motionData[i].forwVertFCode > maxval->vert_f_code)
      {error("forward vertical f_code greater than permitted in specified Level");return false;}

    if (i!=0)
    {
      if (motionData[i].backHorFCode > maxval->hor_f_code)
        {error("backward horizontal f_code greater than permitted in specified Level");return false;}
  
      if (motionData[i].backVertFCode > maxval->vert_f_code)
        {error("backward vertical f_code greater than permitted in specified Level");return false;}
    }
  }

  /* Table 8-10 */
  if (horiSize > maxval->hor_size)
    {error("Horizontal size is greater than permitted in specified Level");return false;}

  if (vertiSize > maxval->vert_size)
    {error("Horizontal size is greater than permitted in specified Level");return false;}

  /* Table 8-11 */
  if (horiSize*vertiSize*frameRate > maxval->sample_rate)
    {error("Sample rate is greater than permitted in specified Level");return false;}

  /* Table 8-12 */
  if (bitRate> 1.0e6 * maxval->bit_rate)
    {error("Bit rate is greater than permitted in specified Level");return false;}

  /* Table 8-13 */
  if (VBVBufferSize > maxval->vbv_buffer_size)
    {error("vbv_buffer_size exceeds High Level limit");return false;}

  return true;
}
