/* 全局变量及函数声明*/

#pragma once
#include "initialize.h"
#include "gettimeofday.h"
#include "signalshooter.h"
#include <QString>
#include <QVector>

#ifndef GLOBAL
#define EXTERN extern
#else
#define EXTERN
#endif

/* prototypes of global functions */

/* conform.c */
bool rangeChecks ();
bool profileAndLevelChecks ();

/* mpeg2enc.c */
bool init ();

/* fdctref.c */
void initFdct ();
void fDCTCalc (short *block);

/* idct.c */
void idct (short *block);
void initIdct (void);

/* motion.c */
void motionEstimation (unsigned char *oldorg, unsigned char *neworg,
  unsigned char *oldref, unsigned char *newref, unsigned char *cur,
  unsigned char *curref, int sxf, int syf, int sxb, int syb,
  struct mbinfo *mbi, int secondfield, int ipflag);

/* mpeg2enc.c */
void error (QString text);

/* predict.c */
void mainPredictCtrl (unsigned char *reff[], unsigned char *refb[],
  unsigned char *cur[3], int secondfield, struct mbinfo *mbi);

/* putbits.c */
void writeInit (void);
void writeData (int val, int n);
void dataAlign (void);
int dataCount (void);

/* puthdr.c */
void mainHeaderAdd (void);
void seqExtHeaderAdd (void);
void seqDispExtHeaderAdd (void);
void usrDataHeaderAdd (char *userdata);
void GOPHeaderAdd (int frame, int closed_gop);
void picHeaderAdd (void);
void picCodeExtHeaderAdd (void);
void fileEndAdd (void);

/* putmpg.c */
bool innerBlockCodeCtrl (short *blk, int cc);
bool crossBlockCodeCtrl (short *blk);
void motionVectorCodeCtrl (int dmv, int f_code);

/* putpic.c */
bool putpict (unsigned char *frame);

/* putseq.c */
bool routineCtrl (void);

/* putvlc.c */
bool dcYGenerate (int val);
bool dcUVGenerate (int val);
bool acGenerateBegin (int run, int val);
bool acGenerateElse (int run, int signed_level, int vlcformat);
void addressCodeGenerate (int addrinc);
void macroTypeCodeGene (int pict_type, int mb_type);
void motionCodeGenerate (int motion_code);
void DMVectorCodeGene (int dmv);
void codedBlockPatternCodeGene (int cbp);

/* quantize.c */
int innerQuan (short *src, short *dst, int dc_prec,
  unsigned char *quant_mat, int mquant);
int crossQuan (short *src, short *dst,
  unsigned char *quant_mat, int mquant);
void innerIQuan (short *src, short *dst, int dc_prec,
  unsigned char *quant_mat, int mquant);
void outerIQuan (short *src, short *dst,
  unsigned char *quant_mat, int mquant);

/* ratectl.c */
void feedbackInit (void);
void GOPControlInit (int np, int nb);
void picControlInit (unsigned char *frame);
void updateCalc (void);
int stepSizeQuantization (void);
int virtualBufferMeasure (int j);
void endPictureBitPut (void);
void delayCalc (void);

/* readpic.c */
bool frameReadControl (char *fname, unsigned char *frame[],int framenum);

/* stats.c */
void calcRatio (unsigned char *org[3], unsigned char *rec[3]);
void insertStatistics (void);

/* transfrm.c */
void matrixTransform (unsigned char *pred[], unsigned char *cur[],
  struct mbinfo *mbi, short blocks[][64]);
void matrixInverseTransform (unsigned char *pred[], unsigned char *cur[],
  struct mbinfo *mbi, short blocks[][64]);
void chooseDCT (unsigned char *pred, unsigned char *cur,
  struct mbinfo *mbi);

/* writepic.c */
  bool reconstructPicture (char *fname, unsigned char *frame[]);


/* zig-zag scan */
EXTERN unsigned char zig_zag_scan[64]
#ifdef GLOBAL
=
{
  0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,
  12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
  35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,
  58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
}
#endif
;

/* alternate scan */
EXTERN unsigned char alternate_scan[64]
#ifdef GLOBAL
=
{
  0,8,16,24,1,9,2,10,17,25,32,40,48,56,57,49,
  41,33,26,18,3,11,4,12,19,27,34,42,50,58,35,43,
  51,59,20,28,5,13,6,14,21,29,36,44,52,60,37,45,
  53,61,22,30,7,15,23,31,38,46,54,62,39,47,55,63
}
#endif
;

/* default intra quantization matrix */
EXTERN unsigned char default_intra_quantizer_matrix[64]
#ifdef GLOBAL
=
{
   8, 16, 19, 22, 26, 27, 29, 34,
  16, 16, 22, 24, 27, 29, 34, 37,
  19, 22, 26, 27, 29, 34, 34, 38,
  22, 22, 26, 27, 29, 34, 37, 40,
  22, 26, 27, 29, 32, 35, 40, 48,
  26, 27, 29, 32, 35, 40, 48, 58,
  26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83
}
#endif
;

/* non-linear quantization coefficient table */
EXTERN unsigned char non_linear_mquant_table[32]
#ifdef GLOBAL
=
{
   0, 1, 2, 3, 4, 5, 6, 7,
   8,10,12,14,16,18,20,22,
  24,28,32,36,40,44,48,52,
  56,64,72,80,88,96,104,112
}
#endif
;

/* non-linear mquant table for mapping from scale to code
 * since reconstruction levels are not bijective with the index map,
 * it is up to the designer to determine most of the quantization levels
 */

EXTERN unsigned char map_non_linear_mquant[113] 
#ifdef GLOBAL
=
{
0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,
16,17,17,17,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,
22,22,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,
26,26,26,26,26,26,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,29,
29,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31
}
#endif
;

/* picture data arrays */

/* reconstructed frames */
EXTERN unsigned char *newrefframe[3], *oldrefframe[3], *auxframe[3];
/* original frames */
EXTERN unsigned char *neworgframe[3], *oldorgframe[3], *auxorgframe[3];
/* prediction of current frame */
EXTERN unsigned char *predframe[3];
/* 8*8 block data */
EXTERN short (*blocks)[64];
/* intra / non_intra quantization matrices */
EXTERN unsigned char intra_q[64], inter_q[64];
EXTERN unsigned char chrom_intra_q[64],chrom_inter_q[64];
/* prediction values for DCT coefficient (0,0) */
EXTERN int dc_dct_pred[3];
/* macroblock side information array */
EXTERN struct mbinfo *mbinfo;
/* motion estimation parameters */
EXTERN struct motion_data *motion_data;
/* clipping (=saturation) table */
EXTERN unsigned char *clp;

/* name strings */
EXTERN char id_string[256], tplorg[256], tplref[256];
EXTERN char errortext[256];

EXTERN FILE *outfile; /* file descriptors */
EXTERN int inputtype; /* format of input frames */

EXTERN int quiet; /* suppress warnings */


/* coding model parameters */

EXTERN int N; /* number of frames in Group of Pictures */
EXTERN int M; /* distance between I/P frames */
EXTERN int P; /* intra slice refresh interval */
EXTERN int nframes; /* total number of frames to encode */
EXTERN int frame0, tc0; /* number and timecode of first frame */
EXTERN int mpeg1; /* ISO/IEC IS 11172-2 sequence */
EXTERN int fieldpic; /* use field pictures */

/* sequence specific data (sequence header) */

EXTERN int horizontal_size, vertical_size; /* frame size (pels) */
EXTERN int width, height; /* encoded frame size (pels) multiples of 16 or 32 */
EXTERN int chrom_width,chrom_height,block_count;
EXTERN int mb_width, mb_height; /* frame size (macroblocks) */
EXTERN int width2, height2, mb_height2, chrom_width2; /* picture size */
EXTERN int aspectratio; /* aspect ratio information (pel or display) */
EXTERN int frame_rate_code; /* coded value of frame rate */
EXTERN double frame_rate; /* frames per second */
EXTERN double bit_rate; /* bits per second */
EXTERN int vbv_buffer_size; /* size of VBV buffer (* 16 kbit) */
EXTERN int constrparms; /* constrained parameters flag (MPEG-1 only) */
EXTERN int load_iquant, load_niquant; /* use non-default quant. matrices */
EXTERN int load_ciquant,load_cniquant;


/* sequence specific data (sequence extension) */

EXTERN int profile, level; /* syntax / parameter constraints */
EXTERN int prog_seq; /* progressive sequence */
EXTERN int chroma_format;
EXTERN int low_delay; /* no B pictures, skipped pictures */


/* sequence specific data (sequence display extension) */

EXTERN int video_format; /* component, PAL, NTSC, SECAM or MAC */
EXTERN int color_primaries; /* source primary chromaticity coordinates */
EXTERN int transfer_characteristics; /* opto-electronic transfer char. (gamma) */
EXTERN int matrix_coefficients; /* Eg,Eb,Er / Y,Cb,Cr matrix coefficients */
EXTERN int display_horizontal_size, display_vertical_size; /* display size */


/* picture specific data (picture header) */

EXTERN int temp_ref; /* temporal reference */
EXTERN int pict_type; /* picture coding type (I, P or B) */
EXTERN int vbv_delay; /* video buffering verifier delay (1/90000 seconds) */


/* picture specific data (picture coding extension) */

EXTERN int forw_hor_f_code, forw_vert_f_code;
EXTERN int back_hor_f_code, back_vert_f_code; /* motion vector ranges */
EXTERN int dc_prec; /* DC coefficient precision for intra coded blocks */
EXTERN int pict_struct; /* picture structure (frame, top / bottom field) */
EXTERN int topfirst; /* display top field first */
/* use only frame prediction and frame DCT (I,P,B,current) */
EXTERN int frame_pred_dct_tab[3], frame_pred_dct;
EXTERN int conceal_tab[3]; /* use concealment motion vectors (I,P,B) */
EXTERN int qscale_tab[3], q_scale_type; /* linear/non-linear quantizaton table */
EXTERN int intravlc_tab[3], intravlc; /* intra vlc format (I,P,B,current) */
EXTERN int altscan_tab[3], altscan; /* alternate scan (I,P,B,current) */
EXTERN int repeatfirst; /* repeat first field after second field */
EXTERN int prog_frame; /* progressive frame */
EXTERN SignalShooter* signalshooter;

/* custom */
EXTERN int hasError;
EXTERN QString errorTextGlobal;
EXTERN QVector<QString> warningTextGlobal;
EXTERN bool putSeqStatus,ifRunning;
EXTERN int currentFrame,frameAll,currentGroup,currentField;



/* statictics */
struct globalStatistics
{
    long int xi,xp,xb;
    long int r;
    long int d0i,d0p,d0b;
    double avgAct;
    double totalTime,fps;
    int nframes;
};
EXTERN globalStatistics globalDATA;

struct groupStatictics
{
    long int R,Np,Nb;
};

EXTERN QVector<groupStatictics> groupDATA;
EXTERN groupStatictics tmpGrp;

struct pictureStatictics
{
    int no,dispNo,codeNo;
    char picType;
    int tempRef,framePredDct,qScaleType,intravlc,altscan;
    int sxf,syf,sxb,syb;
    int forw_hor_f_code,forw_vert_f_code,back_hor_f_code,back_vert_f_code;
    double pictureTime;
    int targetNumberOfBits;
    int vbvDelay,bitcount,bitcnt_EOP;
    double vbvDcdTime;
    int actualBits;
    double avgQuanPara;
    int reaminNumberGOP;
    int Xi,Xp,Xb;
    int d0i,d0p,d0b;
    int Np,Nb;
    double avg_act;
    double YV,YMSE,YMSEdb,YSNR;
    double UV,UMSE,UMSEdb,USNR;
    double VV,VMSE,VMSEdb,VSNR;
    int hash1,hash2,hash3,hash4,hash5,hash6,hash7;
    double hashp1,hashp2,hashp3,hashp4,hashp5,hashp6,hashp7;
    QVector<QVector<char>> mmap1,mmap2;
    QVector<QVector<int>> quantmap;
    int rountineOut,rountineIn;
};

EXTERN QVector<pictureStatictics> pictureDATA;
EXTERN pictureStatictics tmpPicture;

/* charts */
EXTERN QVector<int> chartsInt;
EXTERN QVector<double> chartsDouble;
EXTERN QVector<double> chartsDoubleDouble;
EXTERN int dispPointe3D;
