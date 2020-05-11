/* 产生头信息                                         */


#include <cstdio>
#include <cmath>
#include "timeSettings.h"
#include "commonData.h"

/* private prototypes */
static int TCConvert (int frame);

void mainHeaderAdd()
{
  int i;

  dataAlign();
  writeData(SEQ_START_CODE,32); /* sequence_header_code */
  writeData(horiSize,12); /* horizontal_size_value */
  writeData(vertiSize,12); /* vertical_size_value */
  writeData(aspRatio,4); /* aspect_ratio_information */
  writeData(frameRateCode,4); /* frame_rate_code */
  writeData((int)ceil(bitRate/400.0),18); /* bit_rate_value */
  writeData(1,1); /* marker_bit */
  writeData(VBVBufferSize,10); /* vbv_buffer_size_value */
  writeData(constrparms,1); /* constrained_parameters_flag */

  writeData(ifLoadIntraQuantFlag,1); /* load_intra_quantizer_matrix */
  if (ifLoadIntraQuantFlag)
    for (i=0; i<64; i++)  /* matrices are always downloaded in zig-zag order */
      writeData(intra_q[ZZScanTable[i]],8); /* intra_quantizer_matrix */

  writeData(ifLoadCrossQuantFlag,1); /* load_non_intra_quantizer_matrix */
  if (ifLoadCrossQuantFlag)
    for (i=0; i<64; i++)
      writeData(inter_q[ZZScanTable[i]],8); /* non_intra_quantizer_matrix */
}

/* generate sequence extension (6.2.2.3, 6.3.5) header (MPEG-2 only) */
void seqExtHeaderAdd()
{
  dataAlign();
  writeData(EXT_START_CODE,32); /* extension_start_code */
  writeData(SEQ_ID,4); /* extension_start_code_identifier */
  writeData((profile<<4)|level,8); /* profile_and_level_indication */
  writeData(progSeq,1); /* progressive sequence */
  writeData(chromaFormat,2); /* chroma_format */
  writeData(horiSize>>12,2); /* horizontal_size_extension */
  writeData(vertiSize>>12,2); /* vertical_size_extension */
  writeData(((int)ceil(bitRate/400.0))>>18,12); /* bit_rate_extension */
  writeData(1,1); /* marker_bit */
  writeData(VBVBufferSize>>10,8); /* vbv_buffer_size_extension */
  writeData(0,1); /* low_delay  -- currently not implemented */
  writeData(0,2); /* frame_rate_extension_n */
  writeData(0,5); /* frame_rate_extension_d */
}

/* generate sequence display extension (6.2.2.4, 6.3.6)
 *
 * content not yet user setable
 */
void seqDispExtHeaderAdd()
{
  dataAlign();
  writeData(EXT_START_CODE,32); /* extension_start_code */
  writeData(DISP_ID,4); /* extension_start_code_identifier */
  writeData(videoFormat,3); /* video_format */
  writeData(1,1); /* colour_description */
  writeData(colorPrimaries,8); /* colour_primaries */
  writeData(transferCharacteristics,8); /* transfer_characteristics */
  writeData(matrixCoefficients,8); /* matrix_coefficients */
  writeData(displayHorizontalSize,14); /* display_horizontal_size */
  writeData(1,1); /* marker_bit */
  writeData(displayVerticalSize,14); /* display_vertical_size */
}

/* output a zero terminated string as user data (6.2.2.2.2, 6.3.4.1)
 *
 * string must not emulate start codes
 */
void usrDataHeaderAdd(char *userdata)
{
  dataAlign();
  writeData(USER_START_CODE,32); /* user_data_start_code */
  while (*userdata)
    writeData(*userdata++,8);
}

/* generate group of pictures header (6.2.2.6, 6.3.9)
 *
 * uses tc0 (timecode of first frame) and frame0 (number of first frame)
 */
void GOPHeaderAdd(int frame,int closed_gop)
{
  int tc;

  dataAlign();
  writeData(GOP_START_CODE,32); /* group_start_code */
  tc = TCConvert(tc0+frame);
  writeData(tc,25); /* time_code */
  writeData(closed_gop,1); /* closed_gop */
  writeData(0,1); /* broken_link */
}

/* convert frame number to time_code
 *
 * drop_frame not implemented
 */
static int TCConvert(int frame)
{
  int fps, pict, sec, minute, hour, tc;

  fps = (int)(frameRate+0.5);
  pict = frame%fps;
  frame = (frame-pict)/fps;
  sec = frame%60;
  frame = (frame-sec)/60;
  minute = frame%60;
  frame = (frame-minute)/60;
  hour = frame%24;
  tc = (hour<<19) | (minute<<13) | (1<<12) | (sec<<6) | pict;

  return tc;
}

/* generate picture header (6.2.3, 6.3.10) */
void picHeaderAdd()
{
  dataAlign();
  writeData(PICTURE_START_CODE,32); /* picture_start_code */
  delayCalc();
  writeData(tempRef,10); /* temporal_reference */
  writeData(pictType,3); /* picture_coding_type */
  writeData(VBVDelay,16); /* vbv_delay */

  if (pictType==P_TYPE || pictType==B_TYPE)
  {
    writeData(0,1); /* full_pel_forward_vector */
    if (mpeg1Flag)
      writeData(forwHorFCode,3);
    else
      writeData(7,3); /* forward_f_code */
  }

  if (pictType==B_TYPE)
  {
    writeData(0,1); /* full_pel_backward_vector */
    if (mpeg1Flag)
      writeData(backHorFCode,3);
    else
      writeData(7,3); /* backward_f_code */
  }

  writeData(0,1); /* extra_bit_picture */
}

/* generate picture coding extension (6.2.3.1, 6.3.11)
 *
 * composite display information (v_axis etc.) not implemented
 */
void picCodeExtHeaderAdd()
{
  dataAlign();
  writeData(EXT_START_CODE,32); /* extension_start_code */
  writeData(CODING_ID,4); /* extension_start_code_identifier */
  writeData(forwHorFCode,4); /* forward_horizontal_f_code */
  writeData(forwVertFCode,4); /* forward_vertical_f_code */
  writeData(backHorFCode,4); /* backward_horizontal_f_code */
  writeData(backVertFCode,4); /* backward_vertical_f_code */
  writeData(DCPrec,2); /* intra_dc_precision */
  writeData(pictStruct,2); /* picture_structure */
  writeData((pictStruct==FRAME_PICTURE)?topFirstFlag:0,1); /* top_field_first */
  writeData(framePredDct,1); /* frame_pred_frame_dct */
  writeData(0,1); /* concealment_motion_vectors  -- currently not implemented */
  writeData(qScaleType,1); /* q_scale_type */
  writeData(intravlc,1); /* intra_vlc_format */
  writeData(altscan,1); /* alternate_scan */
  writeData(ifRepeatFirstFlag,1); /* repeat_first_field */
  writeData(progFrame,1); /* chroma_420_type */
  writeData(progFrame,1); /* progressive_frame */
  writeData(0,1); /* composite_display_flag */
}

/* generate sequence_end_code (6.2.2) */
void fileEndAdd()
{
  dataAlign();
  writeData(SEQ_END_CODE,32);
}
