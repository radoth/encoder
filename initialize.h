/* 定义和数据类型*/

#define PICTURE_START_CODE 0x100L
#define SLICE_MIN_START    0x101L
#define SLICE_MAX_START    0x1AFL
#define USER_START_CODE    0x1B2L
#define SEQ_START_CODE     0x1B3L
#define EXT_START_CODE     0x1B5L
#define SEQ_END_CODE       0x1B7L
#define GOP_START_CODE     0x1B8L
#define ISO_END_CODE       0x1B9L
#define PACK_START_CODE    0x1BAL
#define SYSTEM_START_CODE  0x1BBL
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4
#define TOP_FIELD     1
#define BOTTOM_FIELD  2
#define FRAME_PICTURE 3
#define MB_INTRA    1
#define MB_PATTERN  2
#define MB_BACKWARD 4
#define MB_FORWARD  8
#define MB_QUANT    16
#define MC_FIELD 1
#define MC_FRAME 2
#define MC_16X8  2
#define MC_DMV   3
#define MV_FIELD 0
#define MV_FRAME 1
#define CHROMA420 1
#define CHROMA422 2
#define CHROMA444 3
#define SEQ_ID       1
#define DISP_ID      2
#define QUANT_ID     3
#define SEQSCAL_ID   5
#define PANSCAN_ID   7
#define CODING_ID    8
#define SPATSCAL_ID  9
#define TEMPSCAL_ID 10
#define T_Y_U_V 0
#define T_YUV   1
#define T_PPM   2

struct MacroBlockInfo {
  int blockType;
  int motionType;
  int DCTType;
  int mquant;
  int cbp;
  int skipped;
  int MV[2][2][2];
  int mvFieldSel[2][2];
  int dmvector[2];
  double act;
  int var;
};
struct motionData {
  int forwHorFCode,forwVertFCode;
  int sxf,syf;
  int backHorFCode,backVertFCode;
  int sxb,syb;
};
