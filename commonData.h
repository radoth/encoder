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
EXTERN unsigned char ZZScanTable[64]
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
EXTERN unsigned char alternateScanTable[64]
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
EXTERN unsigned char defaultIntraQuantizerMatrix[64]
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
EXTERN unsigned char nonLinearMquantTable[32]
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
EXTERN unsigned char mapNonLinearMquant[113]
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
EXTERN unsigned char *newrefframe[3], *oldrefframe[3], *auxframe[3];
EXTERN unsigned char *neworgframe[3], *oldorgframe[3], *auxorgframe[3];
EXTERN unsigned char *predframe[3];
EXTERN short (*blocks)[64];
EXTERN unsigned char intra_q[64], inter_q[64];
EXTERN unsigned char chrom_intra_q[64],chrom_inter_q[64];
EXTERN int dc_dct_pred[3];
EXTERN struct MacroBlockInfo *MacroBlockInfo;
EXTERN struct motionData *motionData;
EXTERN unsigned char *clp;
EXTERN char id_string[256], tplorg[256], tplref[256];
EXTERN char errortext[256];
EXTERN FILE *outfile;
EXTERN int inputtype;
EXTERN int quiet;
EXTERN int framePerGOP;
EXTERN int IPDistance;
EXTERN int P;
EXTERN int framesCount;
EXTERN int frame0, tc0;
EXTERN int mpeg1Flag;
EXTERN int fieldPicFlag;
EXTERN int horiSize, vertiSize;
EXTERN int width, height;
EXTERN int chromWidth,chromHeight,blockCount;
EXTERN int macroBlockWidth, macroBlockHeight;
EXTERN int pictureWidth, pictureHeight, mbHeight2, chromWidth2;
EXTERN int aspRatio;
EXTERN int frameRateCode;
EXTERN double frameRate;
EXTERN double bitRate;
EXTERN int VBVBufferSize;
EXTERN int constrparms;
EXTERN int ifLoadIntraQuantFlag, ifLoadCrossQuantFlag;
EXTERN int ifLoadCIquantFlag,ifLoadCCrossQuantFlag;
EXTERN int profile, level;
EXTERN int progSeq;
EXTERN int chromaFormat;
EXTERN int lowDelayFlag;
EXTERN int videoFormat;
EXTERN int colorPrimaries;
EXTERN int transferCharacteristics;
EXTERN int matrixCoefficients;
EXTERN int displayHorizontalSize, displayVerticalSize;
EXTERN int tempRef;
EXTERN int pictType;
EXTERN int VBVDelay;
EXTERN int forwHorFCode, forwVertFCode;
EXTERN int backHorFCode, backVertFCode;
EXTERN int DCPrec;
EXTERN int pictStruct;
EXTERN int topFirstFlag;
EXTERN int framePredDctTab[3], framePredDct;
EXTERN int concealTab[3];
EXTERN int qscaleTab[3], qScaleType;
EXTERN int intravlcTab[3], intravlc;
EXTERN int altscanTab[3], altscan;
EXTERN int ifRepeatFirstFlag;
EXTERN int progFrame;
EXTERN SignalShooter* signalshooter;
EXTERN int hasError;
EXTERN QString errorTextGlobal;
EXTERN QVector<QString> warningTextGlobal;
EXTERN bool putSeqStatus,ifRunning;
EXTERN int currentFrame,frameAll,currentGroup,currentField;
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
EXTERN QVector<int> chartsInt;
EXTERN QVector<double> chartsDouble;
EXTERN QVector<double> chartsDoubleDouble;
EXTERN int dispPointe3D;
bool rangeChecks ();
bool profileAndLevelChecks ();
bool init ();
void initFdct ();
void fDCTCalc (short *block);
void idct (short *block);
void initIdct (void);
void motionEstimation (unsigned char *oldorg, unsigned char *neworg,
  unsigned char *oldref, unsigned char *newref, unsigned char *cur,
  unsigned char *curref, int sxf, int syf, int sxb, int syb,
  struct MacroBlockInfo *mbi, int secondfield, int ipflag);
void error (QString text);
void mainPredictCtrl (unsigned char *reff[], unsigned char *refb[],
  unsigned char *cur[3], int secondfield, struct MacroBlockInfo *mbi);
void writeInit (void);
void writeData (int val, int n);
void dataAlign (void);
int dataCount (void);
void mainHeaderAdd (void);
void seqExtHeaderAdd (void);
void seqDispExtHeaderAdd (void);
void usrDataHeaderAdd (char *userdata);
void GOPHeaderAdd (int frame, int closed_gop);
void picHeaderAdd (void);
void picCodeExtHeaderAdd (void);
void fileEndAdd (void);
bool innerBlockCodeCtrl (short *blk, int cc);
bool crossBlockCodeCtrl (short *blk);
void motionVectorCodeCtrl (int dmv, int f_code);
bool putpict (unsigned char *frame);
bool routineCtrl (void);
bool dcYGenerate (int val);
bool dcUVGenerate (int val);
bool acGenerateBegin (int run, int val);
bool acGenerateElse (int run, int signed_level, int vlcformat);
void addressCodeGenerate (int addrinc);
void macroTypeCodeGene (int pictType, int mb_type);
void motionCodeGenerate (int motion_code);
void DMVectorCodeGene (int dmv);
void codedBlockPatternCodeGene (int cbp);
int innerQuan (short *src, short *dst, int DCPrec,
  unsigned char *quant_mat, int mquant);
int crossQuan (short *src, short *dst,
  unsigned char *quant_mat, int mquant);
void innerIQuan (short *src, short *dst, int DCPrec,
  unsigned char *quant_mat, int mquant);
void outerIQuan (short *src, short *dst,
  unsigned char *quant_mat, int mquant);
void feedbackInit (void);
void GOPControlInit (int np, int nb);
void picControlInit (unsigned char *frame);
void updateCalc (void);
int stepSizeQuantization (void);
int virtualBufferMeasure (int j);
void endPictureBitPut (void);
void delayCalc (void);
bool frameReadControl (char *fname, unsigned char *frame[],int framenum);
void calcRatio (unsigned char *org[3], unsigned char *rec[3]);
void insertStatistics (void);
void matrixTransform (unsigned char *pred[], unsigned char *cur[],
  struct MacroBlockInfo *mbi, short blocks[][64]);
void matrixInverseTransform (unsigned char *pred[], unsigned char *cur[],
  struct MacroBlockInfo *mbi, short blocks[][64]);
void chooseDCT (unsigned char *pred, unsigned char *cur,
  struct MacroBlockInfo *mbi);
bool reconstructPicture (char *fname, unsigned char *frame[]);
