/*块和运动向量编码函数*/


#include <cstdio>
#include "timeSettings.h"
#include "commonData.h"

static void vectorCode(int MV[2][2][2],int PMV[2][2][2],int mv_field_sel[2][2],int dmvector[2],int s,int motion_type,
  int hor_f_code,int vert_f_code)
{
  if (pictStruct==FRAME_PICTURE)
  {
    if (motion_type==MC_FRAME)
    {
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      motionVectorCodeCtrl(MV[0][s][1]-PMV[0][s][1],vert_f_code);
      PMV[0][s][0]=PMV[1][s][0]=MV[0][s][0];
      PMV[0][s][1]=PMV[1][s][1]=MV[0][s][1];
    }
    else if (motion_type==MC_FIELD)
    {
      writeData(mv_field_sel[0][s],1);
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      motionVectorCodeCtrl((MV[0][s][1]>>1)-(PMV[0][s][1]>>1),vert_f_code);
      writeData(mv_field_sel[1][s],1);
      motionVectorCodeCtrl(MV[1][s][0]-PMV[1][s][0],hor_f_code);
      motionVectorCodeCtrl((MV[1][s][1]>>1)-(PMV[1][s][1]>>1),vert_f_code);
      PMV[0][s][0]=MV[0][s][0];
      PMV[0][s][1]=MV[0][s][1];
      PMV[1][s][0]=MV[1][s][0];
      PMV[1][s][1]=MV[1][s][1];
    }
    else
    {
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      DMVectorCodeGene(dmvector[0]);
      motionVectorCodeCtrl((MV[0][s][1]>>1)-(PMV[0][s][1]>>1),vert_f_code);
      DMVectorCodeGene(dmvector[1]);
      PMV[0][s][0]=PMV[1][s][0]=MV[0][s][0];
      PMV[0][s][1]=PMV[1][s][1]=MV[0][s][1];
    }
  }
  else
  {
    if (motion_type==MC_FIELD)
    {
      writeData(mv_field_sel[0][s],1);
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      motionVectorCodeCtrl(MV[0][s][1]-PMV[0][s][1],vert_f_code);
      PMV[0][s][0]=PMV[1][s][0]=MV[0][s][0];
      PMV[0][s][1]=PMV[1][s][1]=MV[0][s][1];
    }
    else if (motion_type==MC_16X8)
    {
      writeData(mv_field_sel[0][s],1);
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      motionVectorCodeCtrl(MV[0][s][1]-PMV[0][s][1],vert_f_code);
      writeData(mv_field_sel[1][s],1);
      motionVectorCodeCtrl(MV[1][s][0]-PMV[1][s][0],hor_f_code);
      motionVectorCodeCtrl(MV[1][s][1]-PMV[1][s][1],vert_f_code);
      PMV[0][s][0]=MV[0][s][0];
      PMV[0][s][1]=MV[0][s][1];
      PMV[1][s][0]=MV[1][s][0];
      PMV[1][s][1]=MV[1][s][1];
    }
    else
    {
      motionVectorCodeCtrl(MV[0][s][0]-PMV[0][s][0],hor_f_code);
      DMVectorCodeGene(dmvector[0]);
      motionVectorCodeCtrl(MV[0][s][1]-PMV[0][s][1],vert_f_code);
      DMVectorCodeGene(dmvector[1]);
      PMV[0][s][0]=PMV[1][s][0]=MV[0][s][0];
      PMV[0][s][1]=PMV[1][s][1]=MV[0][s][1];
    }
  }
}

bool putpict(unsigned char *frame)
{
  int i, j, k, comp, cc;
  int mb_type;
  int PMV[2][2][2];
  int prev_mquant;
  int cbp, MBAinc;

  picControlInit(frame);

  picHeaderAdd();

  if (!mpeg1Flag)
    picCodeExtHeaderAdd();

  prev_mquant = stepSizeQuantization();

  k = 0;

  for (j=0; j<mbHeight2; j++)
  {

    for (i=0; i<macroBlockWidth; i++)
    {
      if (i==0)
      {
        dataAlign();

        if (mpeg1Flag || vertiSize<=2800)
          writeData(SLICE_MIN_START+j,32);
        else
        {
          writeData(SLICE_MIN_START+(j&127),32);
          writeData(j>>7,3);
        }
  
        writeData(qScaleType ? mapNonLinearMquant[prev_mquant]
                             : prev_mquant >> 1, 5);
  
        writeData(0,1);
  

        for (cc=0; cc<3; cc++)
          dc_dct_pred[cc] = 0;

        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
        PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
  
        MBAinc = i + 1;
      }

      mb_type = MacroBlockInfo[k].blockType;

      MacroBlockInfo[k].mquant = virtualBufferMeasure(k);

      if (mb_type & MB_INTRA)
      {
        for (comp=0; comp<blockCount; comp++)
          innerQuan(blocks[k*blockCount+comp],blocks[k*blockCount+comp],
                      DCPrec,intra_q,MacroBlockInfo[k].mquant);
        MacroBlockInfo[k].cbp = cbp = (1<<blockCount) - 1;
      }
      else
      {
        cbp = 0;
        for (comp=0;comp<blockCount;comp++)
          cbp = (cbp<<1) | crossQuan(blocks[k*blockCount+comp],
                                           blocks[k*blockCount+comp],
                                           inter_q,MacroBlockInfo[k].mquant);

        MacroBlockInfo[k].cbp = cbp;

        if (cbp)
          mb_type|= MB_PATTERN;
      }

      if (cbp && prev_mquant!=MacroBlockInfo[k].mquant)
        mb_type|= MB_QUANT;

      if (i!=0 && i!=macroBlockWidth-1 && !cbp)
      {

        if (pictType==P_TYPE && !(mb_type&MB_FORWARD))
        {

          for (cc=0; cc<3; cc++)
            dc_dct_pred[cc] = 0;

          PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
          PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;

          MacroBlockInfo[k].blockType = mb_type;
          MacroBlockInfo[k].skipped = 1;
          MBAinc++;
          k++;
          continue;
        }

        if (pictType==B_TYPE && pictStruct==FRAME_PICTURE
            && MacroBlockInfo[k].motionType==MC_FRAME
            && ((MacroBlockInfo[k-1].blockType^mb_type)&(MB_FORWARD|MB_BACKWARD))==0
            && (!(mb_type&MB_FORWARD) ||
                (PMV[0][0][0]==MacroBlockInfo[k].MV[0][0][0] &&
                 PMV[0][0][1]==MacroBlockInfo[k].MV[0][0][1]))
            && (!(mb_type&MB_BACKWARD) ||
                (PMV[0][1][0]==MacroBlockInfo[k].MV[0][1][0] &&
                 PMV[0][1][1]==MacroBlockInfo[k].MV[0][1][1])))
        {
          MacroBlockInfo[k].blockType = mb_type;
          MacroBlockInfo[k].skipped = 1;
          MBAinc++;
          k++;
          continue;
        }

        if (pictType==B_TYPE && pictStruct!=FRAME_PICTURE
            && MacroBlockInfo[k].motionType==MC_FIELD
            && ((MacroBlockInfo[k-1].blockType^mb_type)&(MB_FORWARD|MB_BACKWARD))==0
            && (!(mb_type&MB_FORWARD) ||
                (PMV[0][0][0]==MacroBlockInfo[k].MV[0][0][0] &&
                 PMV[0][0][1]==MacroBlockInfo[k].MV[0][0][1] &&
                 MacroBlockInfo[k].mvFieldSel[0][0]==(pictStruct==BOTTOM_FIELD)))
            && (!(mb_type&MB_BACKWARD) ||
                (PMV[0][1][0]==MacroBlockInfo[k].MV[0][1][0] &&
                 PMV[0][1][1]==MacroBlockInfo[k].MV[0][1][1] &&
                 MacroBlockInfo[k].mvFieldSel[0][1]==(pictStruct==BOTTOM_FIELD))))
        {

          MacroBlockInfo[k].blockType = mb_type;
          MacroBlockInfo[k].skipped = 1;
          MBAinc++;
          k++;
          continue;
        }
      }

      MacroBlockInfo[k].skipped = 0;

      if (pictType==P_TYPE && !cbp && !(mb_type&MB_FORWARD))
        mb_type|= MB_FORWARD;

      addressCodeGenerate(MBAinc);
      MBAinc = 1;

      macroTypeCodeGene(pictType,mb_type);

      if (mb_type & (MB_FORWARD|MB_BACKWARD) && !framePredDct)
        writeData(MacroBlockInfo[k].motionType,2);

      if (pictStruct==FRAME_PICTURE && cbp && !framePredDct)
        writeData(MacroBlockInfo[k].DCTType,1);

      if (mb_type & MB_QUANT)
      {
        writeData(qScaleType ? mapNonLinearMquant[MacroBlockInfo[k].mquant]
                             : MacroBlockInfo[k].mquant>>1,5);
        prev_mquant = MacroBlockInfo[k].mquant;
      }

      if (mb_type & MB_FORWARD)
      {
        vectorCode(MacroBlockInfo[k].MV,PMV,MacroBlockInfo[k].mvFieldSel,MacroBlockInfo[k].dmvector,0,
          MacroBlockInfo[k].motionType,forwHorFCode,forwVertFCode);
      }

      if (mb_type & MB_BACKWARD)
      {
        vectorCode(MacroBlockInfo[k].MV,PMV,MacroBlockInfo[k].mvFieldSel,MacroBlockInfo[k].dmvector,1,
          MacroBlockInfo[k].motionType,backHorFCode,backVertFCode);
      }

      if (mb_type & MB_PATTERN)
      {
        codedBlockPatternCodeGene((cbp >> (blockCount-6)) & 63);
        if (chromaFormat!=CHROMA420)
          writeData(cbp,blockCount-6);
      }

      for (comp=0; comp<blockCount; comp++)
      {
        if (cbp & (1<<(blockCount-1-comp)))
        {
          if (mb_type & MB_INTRA)
          {
            cc = (comp<4) ? 0 : (comp&1)+1;
            if(innerBlockCodeCtrl(blocks[k*blockCount+comp],cc)==false)
                return false;
          }
          else
            if(crossBlockCodeCtrl(blocks[k*blockCount+comp])==false)
                return false;
        }
      }

      if (!(mb_type & MB_INTRA))
        for (cc=0; cc<3; cc++)
          dc_dct_pred[cc] = 0;

      if (mb_type & MB_INTRA || (pictType==P_TYPE && !(mb_type & MB_FORWARD)))
      {
        PMV[0][0][0]=PMV[0][0][1]=PMV[1][0][0]=PMV[1][0][1]=0;
        PMV[0][1][0]=PMV[0][1][1]=PMV[1][1][0]=PMV[1][1][1]=0;
      }

      MacroBlockInfo[k].blockType = mb_type;
      k++;
    }
  }

  updateCalc();
  endPictureBitPut();
  return true;
}
