#include "display.h"
#include "ui_display.h"
#include <QFileDialog>
#include <QDebug>
#include <cstdio>
#include <cstdlib>
#include<QDateTime>
#include <QMessageBox>
#include "checkdatathread.h"
#include "refreshui.h"
#include "refreshbar.h"
#include "displaychart.h"
#include "visual3d.h"

Display::Display(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Display)
{
    ui->setupUi(this);

    on_noReImage_clicked();
    on_useDefaultInMatrix_clicked();
    on_useDefaultOutMatrix_clicked();
    on_IPDistance_valueChanged(1);
    ((QComboBox *)(ui->searchTable->cellWidget(0,0)))->setCurrentIndex(1);
    ((QComboBox *)(ui->searchTable->cellWidget(0,1)))->setCurrentIndex(1);
    ((QSpinBox *)(ui->searchTable->cellWidget(0,2)))->setValue(11);
    ((QSpinBox *)(ui->searchTable->cellWidget(0,3)))->setValue(11);
    ui->outputLine->setReadOnly(true);

    sformat.setFont(QFont("微软雅黑",9));
    iformat.setFont(QFont("微软雅黑",9));
    oformat.setFont(QFont("微软雅黑",9));
    f1format.setFont(QFont("微软雅黑",9));
    f2format.setFont(QFont("微软雅黑",9));
    pformat.setFont(QFont("微软雅黑",9));
    b1format.setFont(QFont("微软雅黑",9));
    b2format.setFont(QFont("微软雅黑",9));
    d1format.setFont(QFont("微软雅黑",9));
    d2format.setFont(QFont("微软雅黑",9));
    qformat.setFont(QFont("微软雅黑",9));
    nformat.setFont(QFont("微软雅黑",9));

    sformat.setForeground(Qt::black);
    iformat.setForeground(Qt::red);
    oformat.setForeground(Qt::green);
    f1format.setForeground(Qt::blue);
    f2format.setForeground(Qt::cyan);
    pformat.setForeground(Qt::magenta);
    b1format.setForeground(Qt::yellow);
    b2format.setForeground(Qt::gray);
    d1format.setForeground(Qt::darkRed);
    d2format.setForeground(Qt::darkBlue);

    qformat.setForeground(Qt::red);
    nformat.setForeground(Qt::green);


    ui->mmap1->setReadOnly(true);
    ui->mmap1->setFocusPolicy(Qt::NoFocus);
    ui->mmap2->setReadOnly(true);
    ui->mmap2->setFocusPolicy(Qt::NoFocus);

    ui->tab_2->setEnabled(false);
    ui->tab_3->setEnabled(false);
    ui->tab_4->setEnabled(false);

    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    setFixedSize(this->width(),this->height());

    setWindowTitle("交互式数据可视化的MPEG-1和MPEG-2图形化视频编码器");
}

bool Display::parameterPrepare()
{
    int h,m,s,f;
    int i;
    extern int r,Xi,Xb,Xp,d0i,d0p,d0b; /* rate control */
    extern double avg_act;

    strcpy(id_string,"Zhang Yuanchi homework");

    if(fileNAME.simplified().isEmpty())
        {error("invalid filename");return false;}
    else strcpy(tplorg,fileNAME.toStdString().c_str());

    if(fileRECONS.simplified().isEmpty())
        strcpy(tplref,"-");
    else strcpy(tplref,fileRECONS.toStdString().c_str());


    if(ui->useDefaultInMatrix->isChecked())
    {
        ifLoadIntraQuantFlag = 0;
        for (i=0; i<64; i++)
          intra_q[i] = defaultIntraQuantizerMatrix[i];
    }
    else{
        qDebug()<<"ready!!!!!!!";
        ifLoadIntraQuantFlag = 1;
        intra_q[0]=ui->xq0->value();
        intra_q[1]=ui->xq1->value();
        intra_q[2]=ui->xq2->value();
        intra_q[3]=ui->xq3->value();
        intra_q[4]=ui->xq4->value();
        intra_q[5]=ui->xq5->value();
        intra_q[6]=ui->xq6->value();
        intra_q[7]=ui->xq7->value();
        intra_q[8]=ui->xq8->value();
        intra_q[9]=ui->xq9->value();
        intra_q[10]=ui->xq10->value();
        intra_q[11]=ui->xq11->value();
        intra_q[12]=ui->xq12->value();
        intra_q[13]=ui->xq13->value();
        intra_q[14]=ui->xq14->value();
        intra_q[15]=ui->xq15->value();
        intra_q[16]=ui->xq16->value();
        intra_q[17]=ui->xq17->value();
        intra_q[18]=ui->xq18->value();
        intra_q[19]=ui->xq19->value();
        intra_q[20]=ui->xq20->value();
        intra_q[21]=ui->xq21->value();
        intra_q[22]=ui->xq22->value();
        intra_q[23]=ui->xq23->value();
        intra_q[24]=ui->xq24->value();
        intra_q[25]=ui->xq25->value();
        intra_q[26]=ui->xq26->value();
        intra_q[27]=ui->xq27->value();
        intra_q[28]=ui->xq28->value();
        intra_q[29]=ui->xq29->value();
        intra_q[30]=ui->xq30->value();
        intra_q[31]=ui->xq31->value();
        intra_q[32]=ui->xq32->value();
        intra_q[33]=ui->xq33->value();
        intra_q[34]=ui->xq34->value();
        intra_q[35]=ui->xq35->value();
        intra_q[36]=ui->xq36->value();
        intra_q[37]=ui->xq37->value();
        intra_q[38]=ui->xq38->value();
        intra_q[39]=ui->xq40->value();
        intra_q[40]=ui->xq41->value();
        intra_q[41]=ui->xq42->value();
        intra_q[42]=ui->xq43->value();
        intra_q[43]=ui->xq44->value();
        intra_q[44]=ui->xq45->value();
        intra_q[45]=ui->xq46->value();
        intra_q[46]=ui->xq47->value();
        intra_q[47]=ui->xq48->value();
        intra_q[48]=ui->xq49->value();
        intra_q[49]=ui->xq50->value();
        intra_q[50]=ui->xq51->value();
        intra_q[51]=ui->xq52->value();
        intra_q[52]=ui->xq53->value();
        intra_q[53]=ui->xq54->value();
        intra_q[54]=ui->xq55->value();
        intra_q[55]=ui->xq56->value();
        intra_q[56]=ui->xq57->value();
        intra_q[57]=ui->xq58->value();
        intra_q[58]=ui->xq59->value();
        intra_q[59]=ui->xq60->value();
        intra_q[60]=ui->xq61->value();
        intra_q[61]=ui->xq62->value();
        intra_q[62]=ui->xq63->value();
        intra_q[63]=ui->xq64->value();
    }

    if(ui->useDefaultOutMatrix->isChecked())
    {
        ifLoadCrossQuantFlag = 0;
        for (i=0; i<64; i++)
          inter_q[i] = 16;
    }
    else{
        ifLoadCrossQuantFlag = 1;
        inter_q[0]=ui->xq0_4->value();
        inter_q[1]=ui->xq1_4->value();
        inter_q[2]=ui->xq2_4->value();
        inter_q[3]=ui->xq3_4->value();
        inter_q[4]=ui->xq4_4->value();
        inter_q[5]=ui->xq5_4->value();
        inter_q[6]=ui->xq6_4->value();
        inter_q[7]=ui->xq7_4->value();
        inter_q[8]=ui->xq8_4->value();
        inter_q[9]=ui->xq9_4->value();
        inter_q[10]=ui->xq10_4->value();
        inter_q[11]=ui->xq11_4->value();
        inter_q[12]=ui->xq12_4->value();
        inter_q[13]=ui->xq13_4->value();
        inter_q[14]=ui->xq14_4->value();
        inter_q[15]=ui->xq15_4->value();
        inter_q[16]=ui->xq16_4->value();
        inter_q[17]=ui->xq17_4->value();
        inter_q[18]=ui->xq18_4->value();
        inter_q[19]=ui->xq19_4->value();
        inter_q[20]=ui->xq20_4->value();
        inter_q[21]=ui->xq21_4->value();
        inter_q[22]=ui->xq22_4->value();
        inter_q[23]=ui->xq23_4->value();
        inter_q[24]=ui->xq24_4->value();
        inter_q[25]=ui->xq25_4->value();
        inter_q[26]=ui->xq26_4->value();
        inter_q[27]=ui->xq27_4->value();
        inter_q[28]=ui->xq28_4->value();
        inter_q[29]=ui->xq29_4->value();
        inter_q[30]=ui->xq30_4->value();
        inter_q[31]=ui->xq31_4->value();
        inter_q[32]=ui->xq32_4->value();
        inter_q[33]=ui->xq33_4->value();
        inter_q[34]=ui->xq34_4->value();
        inter_q[35]=ui->xq35_4->value();
        inter_q[36]=ui->xq36_4->value();
        inter_q[37]=ui->xq37_4->value();
        inter_q[38]=ui->xq38_4->value();
        inter_q[39]=ui->xq40_4->value();
        inter_q[40]=ui->xq41_4->value();
        inter_q[41]=ui->xq42_4->value();
        inter_q[42]=ui->xq43_4->value();
        inter_q[43]=ui->xq44_4->value();
        inter_q[44]=ui->xq45_4->value();
        inter_q[45]=ui->xq46_4->value();
        inter_q[46]=ui->xq47_4->value();
        inter_q[47]=ui->xq48_4->value();
        inter_q[48]=ui->xq49_4->value();
        inter_q[49]=ui->xq50_4->value();
        inter_q[50]=ui->xq51_4->value();
        inter_q[51]=ui->xq52_4->value();
        inter_q[52]=ui->xq53_4->value();
        inter_q[53]=ui->xq54_4->value();
        inter_q[54]=ui->xq55_4->value();
        inter_q[55]=ui->xq56_4->value();
        inter_q[56]=ui->xq57_4->value();
        inter_q[57]=ui->xq58_4->value();
        inter_q[58]=ui->xq59_4->value();
        inter_q[59]=ui->xq60_4->value();
        inter_q[60]=ui->xq61_4->value();
        inter_q[61]=ui->xq62_4->value();
        inter_q[62]=ui->xq63_4->value();
        inter_q[63]=ui->xq64_4->value();
    }

    if(ui->chooseYUVFormat->isChecked())
        inputtype=1;
    else inputtype=2;

    framesCount=ui->frameNumber->value();
    frame0=ui->firstFrameNumber->value();
    h=ui->timecode1->value();
    m=ui->timecode2->value();
    s=ui->timecode3->value();
    f=ui->timecode4->value();
    framePerGOP=ui->framesInGOP->value();
    IPDistance=ui->IPDistance->value();
    qDebug()<<"M="<<IPDistance;
    if(ui->stream1->isChecked())
        mpeg1Flag=1;
    else mpeg1Flag=0;
    if(ui->fieldPictures->isChecked())
        fieldPicFlag=1;
    else fieldPicFlag=0;
    horiSize=ui->horizontalSize->value();
    vertiSize=ui->VerticalSize->value();
    aspRatio=(ui->aspectRatio->currentIndex())+1;
    frameRateCode=(ui->frameRate->currentIndex())+1;
    bitRate=ui->bitRates->value();
    VBVBufferSize=ui->vbvBufferSize->value();
    lowDelayFlag=0;
    if(ui->constrainedParameter->isChecked())
        constrparms=1;
    else constrparms=0;
    if(ui->ProfileIDMain->isChecked())
        profile=4;
    else profile=5;
    switch(ui->levelID->currentIndex())
    {
    case 0:level=4;break;
    case 1:level=6;break;
    case 2:level=8;break;
    case 3:level=10;break;
    default:qDebug()<<"wrong option";
    }
    if(ui->ProgressiveISeq->isChecked())
        progSeq=0;
    else progSeq=1;
    chromaFormat=1;
    videoFormat=ui->videoFormat->currentIndex();
    switch(ui->colorPrimary->currentIndex())
    {
    case 0:colorPrimaries=1;break;
    case 1:colorPrimaries=2;break;
    case 2:colorPrimaries=4;break;
    case 3:colorPrimaries=5;break;
    case 4:colorPrimaries=6;break;
    case 5:colorPrimaries=7;break;
    default:qDebug()<<"wrong";
    }
    switch(ui->transferCo->currentIndex())
    {
    case 0:transferCharacteristics=1;break;
    case 1:transferCharacteristics=2;break;
    case 2:transferCharacteristics=4;break;
    case 3:transferCharacteristics=5;break;
    case 4:transferCharacteristics=6;break;
    case 5:transferCharacteristics=7;break;
    case 6:transferCharacteristics=8;break;
    default:qDebug()<<"wrong";
    }
    switch(ui->matrixCo->currentIndex())
    {
    case 0:matrixCoefficients=1;break;
    case 1:matrixCoefficients=2;break;
    case 2:matrixCoefficients=4;break;
    case 3:matrixCoefficients=5;break;
    case 4:matrixCoefficients=6;break;
    case 5:matrixCoefficients=7;break;
    default:qDebug()<<"wrong";
    }
    displayHorizontalSize=ui->dispHorizontalSize->value();
    displayVerticalSize=ui->dispVerticalSize->value();
    DCPrec=ui->intraDCPrecision->currentIndex();
    if(ui->topFieldFirstNew->isChecked())
        topFirstFlag=1;
    else topFirstFlag=0;
    if(ui->framePredFrameI->isChecked())
        framePredDctTab[0]=1;
    else framePredDctTab[0]=0;
    if(ui->framePredFrameP->isChecked())
        framePredDctTab[1]=1;
    else framePredDctTab[1]=0;
    if(ui->framePredFrameB->isChecked())
        framePredDctTab[2]=1;
    else framePredDctTab[2]=0;

    concealTab[0]=0;
    concealTab[1]=0;
    concealTab[2]=0;

    if(ui->qScaleTypeI->isChecked())
        qscaleTab[0]=1;
    else qscaleTab[0]=0;
    if(ui->qScaleTypeP->isChecked())
        qscaleTab[1]=1;
    else qscaleTab[1]=0;
    if(ui->qScaleTypeB->isChecked())
        qscaleTab[2]=1;
    else qscaleTab[2]=0;

    if(ui->intraVLCFormatI->isChecked())
        intravlcTab[0]=1;
    else intravlcTab[0]=0;
    if(ui->intraVLCFormatP->isChecked())
        intravlcTab[1]=1;
    else intravlcTab[1]=0;
    if(ui->intraVLCFormatB->isChecked())
        intravlcTab[2]=1;
    else intravlcTab[2]=0;

    if(ui->alternateScanI->isChecked())
        altscanTab[0]=1;
    else altscanTab[0]=0;
    if(ui->alternateScanP->isChecked())
        altscanTab[1]=1;
    else altscanTab[1]=0;
    if(ui->alternateScanB->isChecked())
        altscanTab[2]=1;
    else altscanTab[2]=0;

    if(ui->repeatFirstField->isChecked())
        ifRepeatFirstFlag=1;
    else ifRepeatFirstFlag=0;

    if(ui->progressiveI->isChecked())
        progFrame=1;
    else progFrame=0;

    P=0;

    r=ui->rcR->value();
    avg_act=ui->rcAvgAct->value();
    Xi=ui->rcXi->value();
    Xp=ui->rcXp->value();
    Xb=ui->rcXb->value();
    d0i=ui->rcd0i->value();
    d0p=ui->rcd0p->value();
    d0b=ui->rcd0b->value();


    if (framePerGOP<1)
      {error("N must be positive");return false;}
    if (IPDistance<1)
      {error("M must be positive");return false;}
    if (framePerGOP%IPDistance != 0)
      {error("N must be an integer multiple of M");return false;}

    motionData = (struct motionData *)malloc(IPDistance*sizeof(struct motionData));
    if (!motionData)
      {error("malloc failed\n");return false;}

    motionData[0].forwHorFCode=((((QComboBox *)(ui->searchTable->cellWidget(0,0)))->currentIndex()))+1;
    motionData[0].forwVertFCode=((((QComboBox *)(ui->searchTable->cellWidget(0,1)))->currentIndex()))+1;
    motionData[0].sxf=((QSpinBox *)(ui->searchTable->cellWidget(0,2)))->value();
    motionData[0].syf=((QSpinBox *)(ui->searchTable->cellWidget(0,3)))->value();

    for(int i=1;i<2*IPDistance-1;i++)
    {
        if(i%2==1)
        {
            motionData[i/2+1].forwHorFCode=(((QComboBox *)(ui->searchTable->cellWidget(i,0)))->currentIndex())+1;
            motionData[i/2+1].forwVertFCode=(((QComboBox *)(ui->searchTable->cellWidget(i,1)))->currentIndex())+1;
            motionData[i/2+1].sxf=((QSpinBox *)(ui->searchTable->cellWidget(i,2)))->value();
            motionData[i/2+1].syf=((QSpinBox *)(ui->searchTable->cellWidget(i,3)))->value();
        }
        else{
            motionData[i/2].backHorFCode=((QComboBox *)(ui->searchTable->cellWidget(i,0)))->currentIndex()+1;
            motionData[i/2].backVertFCode=((QComboBox *)(ui->searchTable->cellWidget(i,1)))->currentIndex()+1;
            motionData[i/2].sxb=((QSpinBox *)(ui->searchTable->cellWidget(i,2)))->value();
            motionData[i/2].syb=((QSpinBox *)(ui->searchTable->cellWidget(i,3)))->value();
        }
    }






    qDebug()<<"===============";
    qDebug()<<"tplorg="<<tplorg;
    qDebug()<<"tplref="<<tplref;
    qDebug()<<"inputtype="<<inputtype;
    qDebug()<<"nframes="<<framesCount;
    qDebug()<<"frame0"<<frame0;
    qDebug()<<"N="<<framePerGOP;
    qDebug()<<"M="<<IPDistance;
    qDebug()<<"mpeg1"<<mpeg1Flag;
    qDebug()<<"fieldPic="<<fieldPicFlag;
    qDebug()<<"horizontalsize="<<horiSize;
    qDebug()<<"verticalSize="<<vertiSize;
    qDebug()<<"aspectratio="<<aspRatio;
    qDebug()<<"frameratecode="<<frameRateCode;
    qDebug()<<"bitrate="<<bitRate;
    qDebug()<<"vbvbuffersize="<<VBVBufferSize;
    qDebug()<<"lowdelay="<<lowDelayFlag;
    qDebug()<<"constrparams="<<constrparms;
    qDebug()<<"profile"<<profile;
    qDebug()<<"level="<<level;
    qDebug()<<"prog_seq"<<progSeq;
    qDebug()<<"chroma_format"<<chromaFormat;
    qDebug()<<"cideo_format"<<videoFormat;
    qDebug()<<"color_primary"<<colorPrimaries;
    qDebug()<<"transfer_charac="<<transferCharacteristics;
    qDebug()<<"matrixCoeffi"<<matrixCoefficients;
    qDebug()<<"displayhorizontal"<<displayHorizontalSize;
    qDebug()<<"displayvertical"<<displayVerticalSize;
    qDebug()<<"dc_prec="<<DCPrec;
    qDebug()<<"topfirst="<<topFirstFlag;
    qDebug()<<"fram_pred_dct_tab"<<framePredDctTab[0]<<framePredDctTab[1]<<framePredDctTab[2];
    qDebug()<<"conceal_tab"<<concealTab[0]<<concealTab[1]<<concealTab[2];
    qDebug()<<"qscale_tab"<<qscaleTab[0]<<qscaleTab[1]<<qscaleTab[2];
    qDebug()<<"intravlc_tab"<<intravlcTab[0]<<intravlcTab[1]<<intravlcTab[2];
    qDebug()<<"altscan_tab"<<altscanTab[0]<<altscanTab[1]<<altscanTab[2];
    qDebug()<<"repeatfirst="<<ifRepeatFirstFlag;
    qDebug()<<"prog_frame="<<progFrame;
    qDebug()<<"P="<<P;
    qDebug()<<"r="<<r;
    qDebug()<<"avgact="<<avg_act;
    qDebug()<<"xi="<<Xi;
    qDebug()<<"xp="<<Xp;
    qDebug()<<"Xb="<<Xb;
    qDebug()<<"d0i"<<d0i;
    qDebug()<<"d0p"<<d0p;
    qDebug()<<"d0b="<<d0b;
    for(int i=0;i<IPDistance;i++)
    {
        qDebug()<<"motion_data="<<motionData[i].forwHorFCode<<"    "<<motionData[i].forwVertFCode<<"    "<<motionData[i].sxf<<"    "<<motionData[i].syf;
        if(i!=0)
        {
            qDebug()<<"motion_data="<<motionData[i].backHorFCode<<"    "<<motionData[i].backVertFCode<<"    "<<motionData[i].sxb<<"    "<<motionData[i].syb;
        }
    }










    mpeg1Flag = !!mpeg1Flag;
    fieldPicFlag = !!fieldPicFlag;
    lowDelayFlag = !!lowDelayFlag;
    constrparms = !!constrparms;
    progSeq = !!progSeq;
    topFirstFlag = !!topFirstFlag;

    for (int i=0; i<3; i++)
    {
      framePredDctTab[i] = !!framePredDctTab[i];
      concealTab[i] = !!concealTab[i];
      qscaleTab[i] = !!qscaleTab[i];
      intravlcTab[i] = !!intravlcTab[i];
      altscanTab[i] = !!altscanTab[i];
    }
    ifRepeatFirstFlag = !!ifRepeatFirstFlag;
    progFrame = !!progFrame;

    if(rangeChecks()==false)
        return false;

    static double ratetab[8]=
      {24000.0/1001.0,24.0,25.0,30000.0/1001.0,30.0,50.0,60000.0/1001.0,60.0};

    frameRate = ratetab[frameRateCode-1];

    tc0 = h;
    tc0 = 60*tc0 + m;
    tc0 = 60*tc0 + s;
    tc0 = (int)(frameRate+0.5)*tc0 + f;

    if (!mpeg1Flag)
    {
      if(profileAndLevelChecks()==false)
          return false;
    }
    else
    {
      /* MPEG-1 */
      if (constrparms)
      {
        if (horiSize>768
            || vertiSize>576
            || ((horiSize+15)/16)*((vertiSize+15)/16)>396
            || ((horiSize+15)/16)*((vertiSize+15)/16)*frameRate>396*25.0
            || frameRate>30.0)
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
              warningTextGlobal.append("警告：设置constrained_parameters_flag = 0");
          }
          constrparms = 0;
        }
      }

      if (constrparms)
      {
        for (i=0; i<IPDistance; i++)
        {
          if (motionData[i].forwHorFCode>4)
          {
            if (!quiet)
              {
                //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }

          if (motionData[i].forwVertFCode>4)
          {
            if (!quiet)
              {
                //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("警告：设置 setting constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }

          if (i!=0)
          {
            if (motionData[i].backHorFCode>4)
            {
              if (!quiet)
                {
                  //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                  warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
              }
              constrparms = 0;
              break;
            }

            if (motionData[i].backVertFCode>4)
            {
              if (!quiet)
                {
                  //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                  warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
              }
              constrparms = 0;
              break;
            }
          }
        }
      }
    }

    /* relational checks */

    if (mpeg1Flag)
    {
      if (!progSeq)
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting progressive_sequence = 1\n");
            warningTextGlobal.append("警告：设置 progressive_sequence = 1");
        }
        progSeq = 1;
      }

      if (chromaFormat!=CHROMA420)
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting chroma_format = 1 (4:2:0)\n");
            warningTextGlobal.append("警告：设置 chroma_format = 1 (4:2:0)");
        }
        chromaFormat = CHROMA420;
      }

      if (DCPrec!=0)
      {
        if (!quiet)
          {
            //fprintf(stderr,"Warning: setting intra_dc_precision = 0\n");
            warningTextGlobal.append("警告：设置 intra_dc_precision = 0");
        }
        DCPrec = 0;
      }

      for (i=0; i<3; i++)
        if (qscaleTab[i])
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting qscale_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("警告：设置 qscale_tab[%1] = 0").arg(i));
          }
          qscaleTab[i] = 0;
        }

      for (i=0; i<3; i++)
        if (intravlcTab[i])
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting intravlc_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("警告：设置 intravlc_tab[%1] = 0").arg(i));
          }
          intravlcTab[i] = 0;
        }

      for (i=0; i<3; i++)
        if (altscanTab[i])
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting altscan_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("警告：设置 altscan_tab[%1] = 0").arg(i));
          }
          altscanTab[i] = 0;
        }
    }

    if (!mpeg1Flag && constrparms)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
          warningTextGlobal.append("警告：设置 constrained_parameters_flag = 0");
      }
      constrparms = 0;
    }

    if (progSeq && !progFrame)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting progressive_frame = 1\n");
          warningTextGlobal.append("警告：设置 progressive_frame = 1");
      }
      progFrame = 1;
    }

    if (progFrame && fieldPicFlag)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting field_pictures = 0\n");
          warningTextGlobal.append("警告：设置 field_pictures = 0");
      }
      fieldPicFlag = 0;
    }

    if (!progFrame && ifRepeatFirstFlag)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting repeat_first_field = 0\n");
          warningTextGlobal.append("警告：设置 repeat_first_field = 0");
      }
      ifRepeatFirstFlag = 0;
    }

    if (progFrame)
    {
      for (i=0; i<3; i++)
        if (!framePredDctTab[i])
        {
          if (!quiet)
            {
              //fprintf(stderr,"Warning: setting frame_pred_frame_dct[%d] = 1\n",i);
              warningTextGlobal.append(QString("警告：设置 frame_pred_frame_dct[%1] = 1").arg(i));
          }
          framePredDctTab[i] = 1;
        }
    }

    if (progSeq && !ifRepeatFirstFlag && topFirstFlag)
    {
      if (!quiet)
        {
          //fprintf(stderr,"Warning: setting top_field_first = 0\n");
          warningTextGlobal.append("警告：设置 top_field_first = 0");
      }
      topFirstFlag = 0;
    }

    /* search windows */
    for (i=0; i<IPDistance; i++)
    {
      if (motionData[i].sxf > (4<<motionData[i].forwHorFCode)-1)
      {
        if (!quiet)
          {
            //fprintf(stderr,
            //"Warning: reducing forward horizontal search width to %d\n",
            //(4<<motion_data[i].forw_hor_f_code)-1);
            warningTextGlobal.append(QString("警告：将向前的水平搜索宽度减小到 %1").arg((4<<motionData[i].forwHorFCode)-1));
        }
        motionData[i].sxf = (4<<motionData[i].forwHorFCode)-1;
      }

      if (motionData[i].syf > (4<<motionData[i].forwVertFCode)-1)
      {
        if (!quiet)
          {
            //fprintf(stderr,
            //"Warning: reducing forward vertical search width to %d\n",
            //(4<<motion_data[i].forw_vert_f_code)-1);
            warningTextGlobal.append(QString("警告：将向前的垂直搜索宽度减小到 %1").arg((4<<motionData[i].forwVertFCode)-1));
        }
        motionData[i].syf = (4<<motionData[i].forwVertFCode)-1;
      }

      if (i!=0)
      {
        if (motionData[i].sxb > (4<<motionData[i].backHorFCode)-1)
        {
          if (!quiet)
            {
              //fprintf(stderr,
              //"Warning: reducing backward horizontal search width to %d\n",
              //(4<<motion_data[i].back_hor_f_code)-1);
              warningTextGlobal.append(QString("警告：将向后的水平搜索宽度减小到 %1").arg((4<<motionData[i].backHorFCode)-1));
          }
          motionData[i].sxb = (4<<motionData[i].backHorFCode)-1;
        }

        if (motionData[i].syb > (4<<motionData[i].backVertFCode)-1)
        {
          if (!quiet)
            {
              //fprintf(stderr,
              //"Warning: reducing backward vertical search width to %d\n",
              //(4<<motion_data[i].back_vert_f_code)-1);
              warningTextGlobal.append(QString("警告：将向后的垂直搜索宽度减小到 %1").arg((4<<motionData[i].backVertFCode)-1));
          }
          motionData[i].syb = (4<<motionData[i].backVertFCode)-1;
        }
      }
    }

    return true;
}

void Display::refreshErrorList()
{
    ui->errorList->clear();
    auto tmpCopy=warningTextGlobal;
    for(auto i=tmpCopy.rbegin();i!=tmpCopy.rend();i++)
    {
        ui->errorList->addItem(*i);
    }
}

void Display::setFrameData(int num)
{
    if(num>=pictureDATA.size())
        return;

    ui->no->setText(QString::number(pictureDATA[num].no));
    ui->dispNo->setText(QString::number(pictureDATA[num].dispNo));
    ui->codeNo->setText(QString::number(pictureDATA[num].codeNo));
    ui->picType->setText(QString(pictureDATA[num].picType)+"帧");
    ui->tempRef->setText(QString::number(pictureDATA[num].tempRef));
    ui->framePredDCT->setText(pictureDATA[num].framePredDct==1?"是":"否");
    ui->qScaleType->setText(pictureDATA[num].qScaleType==1?"是":"否");
    ui->intravlc->setText(pictureDATA[num].intravlc==1?"是":"否");
    ui->altscan->setText(pictureDATA[num].altscan==1?"是":"否");
    ui->pictureTime->setText(QString::number(pictureDATA[num].pictureTime));
    ui->targetNumberofBits->setText(QString::number(pictureDATA[num].targetNumberOfBits));
    ui->vbvDelay->setText(QString::number(pictureDATA[num].vbvDelay));
    ui->vbvBitCount->setText(QString::number(pictureDATA[num].bitcount));
    ui->bitcntEOP->setText(QString::number(pictureDATA[num].bitcnt_EOP));
    ui->vbvdcdtime->setText(QString::number(pictureDATA[num].vbvDcdTime));
    ui->actualBits->setText(QString::number(pictureDATA[num].actualBits));
    ui->avgQuanPara->setText(QString::number(pictureDATA[num].avgQuanPara));
    ui->reaminNumberGOP->setText(QString::number(pictureDATA[num].reaminNumberGOP));
    ui->Xiii->setText(QString::number(pictureDATA[num].Xi));
    ui->Xppp->setText(QString::number(pictureDATA[num].Xp));
    ui->Xbbb->setText(QString::number(pictureDATA[num].Xb));
    ui->d0iiiiii->setText(QString::number(pictureDATA[num].d0i));
    ui->d0ppppppp->setText(QString::number(pictureDATA[num].d0p));
    ui->d0bbbbbbb->setText(QString::number(pictureDATA[num].d0b));
    ui->npppppppp->setText(QString::number(pictureDATA[num].Np));
    ui->nbbbbbbbbb->setText(QString::number(pictureDATA[num].Nb));
    ui->avg_acttttttt->setText(QString::number(pictureDATA[num].avg_act));
    ui->YV->setText(QString::number(pictureDATA[num].YV));
    ui->YMSEDB->setText(QString::number(pictureDATA[num].YMSE)+" ( "+QString::number(pictureDATA[num].YMSEdb)+" dB )");
    ui->YSNR->setText(QString::number(pictureDATA[num].YSNR));
    ui->UV->setText(QString::number(pictureDATA[num].UV));
    ui->UMSEDB->setText(QString::number(pictureDATA[num].UMSE)+" ( "+QString::number(pictureDATA[num].UMSEdb)+" dB )");
    ui->USNR->setText(QString::number(pictureDATA[num].USNR));
    ui->VV->setText(QString::number(pictureDATA[num].VV));
    ui->VMSEDB->setText(QString::number(pictureDATA[num].VMSE)+" ( "+QString::number(pictureDATA[num].VMSEdb)+" dB )");
    ui->VSNR->setText(QString::number(pictureDATA[num].VSNR));
    ui->hash1->setText(QString::number(pictureDATA[num].hash1)+" ( "+QString::number(pictureDATA[num].hashp1)+"% )");
    ui->hash2->setText(QString::number(pictureDATA[num].hash2)+" ( "+QString::number(pictureDATA[num].hashp2)+"% )");
    ui->hash3->setText(QString::number(pictureDATA[num].hash3)+" ( "+QString::number(pictureDATA[num].hashp3)+"% )");
    ui->hash4->setText(QString::number(pictureDATA[num].hash4)+" ( "+QString::number(pictureDATA[num].hashp4)+"% )");
    ui->hash5->setText(QString::number(pictureDATA[num].hash5)+" ( "+QString::number(pictureDATA[num].hashp5)+"% )");
    ui->hash6->setText(QString::number(pictureDATA[num].hash6)+" ( "+QString::number(pictureDATA[num].hashp6)+"% )");
    ui->hash7->setText(QString::number(pictureDATA[num].hash7)+" ( "+QString::number(pictureDATA[num].hashp7)+"% )");
    if(pictureDATA[num].picType=='P'||pictureDATA[num].picType=='B')
    {
    ui->forSeaWind->setText("宽度范围[-"+QString::number(pictureDATA[num].sxf)+","+QString::number(pictureDATA[num].sxf)+"] 高度范围[-"+QString::number(pictureDATA[num].syf)+","+QString::number(pictureDATA[num].syf)+"]");
    ui->forSeaRange->setText("水平范围[-"+QString::number(4<<(pictureDATA[num].forw_hor_f_code))+","+QString::number((4<<(pictureDATA[num].forw_hor_f_code))-1)+"] 垂直范围[-"+QString::number(4<<(pictureDATA[num].forw_vert_f_code))+","+QString::number((4<<(pictureDATA[num].forw_vert_f_code))-1)+"]");
    }
    else
    {
        ui->forSeaWind->setText("当前帧是I帧");
        ui->forSeaRange->setText("当前帧是I帧");
    }

    if(pictureDATA[num].picType=='B')
    {
    ui->backSeaWind->setText("宽度范围[-"+QString::number(pictureDATA[num].sxb)+","+QString::number(pictureDATA[num].sxb)+"] 高度范围[-"+QString::number(pictureDATA[num].syb)+","+QString::number(pictureDATA[num].syb)+"]");
    ui->backSeaScope->setText("水平范围[-"+QString::number(4<<(pictureDATA[num].back_hor_f_code))+","+QString::number((4<<(pictureDATA[num].back_hor_f_code))-1)+"] 垂直范围[-"+QString::number(4<<(pictureDATA[num].back_vert_f_code))+","+QString::number((4<<(pictureDATA[num].back_vert_f_code))-1)+"]");
    }
    else
    {
        ui->backSeaWind->setText("当前帧是"+QString(pictureDATA[num].picType)+"帧");
        ui->backSeaScope->setText("当前帧是"+QString(pictureDATA[num].picType)+"帧");
    }


}

void Display::setGroupData(int num)
{
    if(num>=groupDATA.size())
        return;
    ui->RRRRRRRR->setText(QString::number(groupDATA[num].R));
    ui->Np->setText(QString::number(groupDATA[num].Np));
    ui->Nb->setText(QString::number(groupDATA[num].Nb));
}

void Display::setGlobalData()
{
    ui->Xi->setText(QString::number(globalDATA.xi));
    ui->Xp->setText(QString::number(globalDATA.xp));
    ui->Xb->setText(QString::number(globalDATA.xb));
    ui->rrrrr->setText(QString::number(globalDATA.r));
    ui->d0i->setText(QString::number(globalDATA.d0i));
    ui->d0p->setText(QString::number(globalDATA.d0p));
    ui->d0b->setText(QString::number(globalDATA.d0b));
    ui->avg_act_2->setText(QString::number(globalDATA.avgAct));
    ui->totalTime->setText(QString::number(globalDATA.totalTime,'f'));
    ui->fps->setText(QString::number(globalDATA.fps));
    ui->nframes->setText(QString::number(globalDATA.nframes));

}

void Display::setMMap1(int num)
{
    if(num>=pictureDATA.size())
        return;

    ui->mmap1->setReadOnly(true);
    ui->mmap1->clear();
    ui->mmap1->moveCursor(QTextCursor::End);
    QTextCursor cursor=ui->mmap1->textCursor();


    for(int i=0;i<pictureDATA[num].rountineOut;i++)
    {
        for(int j=0;j<pictureDATA[num].rountineIn;j++)
        {
            switch((pictureDATA[num].mmap1)[i][j])
            {
            case 'S':cursor.insertText("●",sformat);break;
            case 'I':cursor.insertText("●",iformat);break;
            case '0':cursor.insertText("●",oformat);break;
            case 'F':cursor.insertText("●",f1format);break;
            case 'f':cursor.insertText("●",f2format);break;
            case 'p':cursor.insertText("●",pformat);break;
            case 'B':cursor.insertText("●",b1format);break;
            case 'b':cursor.insertText("●",b2format);break;
            case 'D':cursor.insertText("●",d1format);break;
            case 'd':cursor.insertText("●",d2format);break;
            default:break;
            }
        }
        cursor.insertText("\n");
    }
    ui->mmap1->setFocusPolicy(Qt::NoFocus);
}

void Display::setMMap2(int num)
{
    if(num>=pictureDATA.size())
        return;

    ui->mmap2->setReadOnly(true);
    ui->mmap2->clear();
    ui->mmap2->moveCursor(QTextCursor::End);
    QTextCursor cursor=ui->mmap2->textCursor();


    for(int i=0;i<pictureDATA[num].rountineOut;i++)
    {
        for(int j=0;j<pictureDATA[num].rountineIn;j++)
        {
            switch((pictureDATA[num].mmap2)[i][j])
            {
            case ' ':cursor.insertText("●",f1format);break;
            case 'Q':cursor.insertText("●",qformat);break;
            case 'N':cursor.insertText("●",nformat);break;
            default:break;
            }
        }
        cursor.insertText("\n");
    }
    ui->mmap2->setFocusPolicy(Qt::NoFocus);
}

Display::~Display()
{
    delete ui;
}

void Display::onRefreshSignal()
{
    refreshErrorList();
}

void Display::onRefreshBar()
{
    ui->currentFrame->setText(QString::number(currentFrame));
    ui->frameAll->setText(QString::number(frameAll));
    ui->currentGroup->setText(QString::number(currentGroup));
    ui->currentField->setText(QString::number(currentField));
    ui->progressBar->setValue(fmin(ceil(((double)(currentFrame+1))/frameAll*100),100));
    //qDebug()<<((double)(currentFrame))/frameAll;
}

void Display::onProcessEnded(bool status)
{
    if(status)
    {
        QMessageBox::information(this,"编码成功","编码成功结束。");
        setGlobalData();
        setFrameData(0);
        setGroupData(0);
        ui->gopSpin->setMinimum(0);
        ui->gopSpin->setMaximum(groupDATA.size()-1);
        ui->gopSpin->setSingleStep(1);
        ui->gopSlider->setMinimum(0);
        ui->gopSlider->setMaximum(groupDATA.size()-1);
        ui->gopSlider->setSingleStep(1);
        ui->frameSpin->setMinimum(0);
        ui->frameSpin->setMaximum(pictureDATA.size()-1);
        ui->frameSpin->setSingleStep(1);
        ui->frameSlider->setMinimum(0);
        ui->frameSlider->setMaximum(pictureDATA.size()-1);
        ui->frameSlider->setSingleStep(1);



        ui->tab_2->setEnabled(true);
        ui->tab_3->setEnabled(true);
        ui->tab_4->setEnabled(true);

        ui->visualSlider->setMinimum(0);
        ui->visualSlider->setMaximum(pictureDATA.size()-1);
        ui->visualSlider->setSingleStep(1);
        ui->visualSpin->setMinimum(0);
        ui->visualSpin->setMaximum(pictureDATA.size()-1);
        ui->visualSpin->setSingleStep(1);
        ui->cubeSlider->setMinimum(0);
        ui->cubeSlider->setMaximum(pictureDATA.size()-1);
        ui->cubeSlider->setSingleStep(1);
        ui->cubeSpin->setMinimum(0);
        ui->cubeSpin->setMaximum(pictureDATA.size()-1);
        ui->cubeSpin->setSingleStep(1);
    }
    else QMessageBox::critical(this,"编码失败","编码失败。\n"+errorTextGlobal);
    ui->pushButton->setEnabled(true);
}


void Display::on_sourceBrowse_clicked()
{
    QFileDialog *fileDialog=new QFileDialog(this);
    QString formatName;
    if(ui->chooseYUVFormat->isChecked())
        formatName="YUV";
    else formatName="PPM";
    fileDialog->setWindowTitle("打开"+formatName+"文件");
    if(formatName=="YUV")
        fileDialog->setNameFilter("YUV文件(*.yuv)");
    else fileDialog->setNameFilter("PPM文件(*.ppm)");
    if(fileDialog->exec())
    {
        auto fileName=fileDialog->selectedFiles();
        for(auto i:fileName)
        {
            fileNAME=i.left(i.length()-4);
            ui->sourceFile->setText(fileNAME);
            ui->sourceFile->setReadOnly(true);
        }
    }
}

void Display::on_IPDistance_valueChanged(int arg1)
{
    IPDistance=arg1;

    ui->searchTable->setRowCount(2*IPDistance-1);
    ui->searchTable->setColumnCount(4);
    QStringList header;
    header<<"水平向量长度"<<"垂直向量长度"<<"搜索宽度"<<"搜索高度";
    ui->searchTable->setHorizontalHeaderLabels(header);
    QStringList vheader;
    vheader<<"P帧";
    QComboBox *tmpCombo1=new QComboBox();
    tmpCombo1->addItem("[-8,7]");
    tmpCombo1->addItem("[-16,15]");
    tmpCombo1->addItem("[-32,31]");
    tmpCombo1->addItem("[-64,63]");
    tmpCombo1->addItem("[-128,127]");
    tmpCombo1->addItem("[-256,255]");
    tmpCombo1->addItem("[-512,511]");
    tmpCombo1->addItem("[-1023,1024]");
    tmpCombo1->addItem("[-2048,2047]");
    ui->searchTable->setCellWidget(0,0,tmpCombo1);
    QComboBox *tmpCombo2=new QComboBox();
    tmpCombo2->addItem("[-8,7]");
    tmpCombo2->addItem("[-16,15]");
    tmpCombo2->addItem("[-32,31]");
    tmpCombo2->addItem("[-64,63]");
    tmpCombo2->addItem("[-128,127]");
    tmpCombo2->addItem("[-256,255]");
    tmpCombo2->addItem("[-512,511]");
    tmpCombo2->addItem("[-1023,1024]");
    tmpCombo2->addItem("[-2048,2047]");
    ui->searchTable->setCellWidget(0,1,tmpCombo2);

    QSpinBox *tmpspin1=new QSpinBox();
    ui->searchTable->setCellWidget(0,2,tmpspin1);
    QSpinBox *tmpspin2=new QSpinBox();
    ui->searchTable->setCellWidget(0,3,tmpspin2);

    for(int i=1;i<IPDistance;i++)
    {
        QString tmp="B"+QString::number(i);
        vheader<<tmp+"前向";
        vheader<<tmp+"后向";
    }

    for(int i=1;i<2*IPDistance-1;i++)
    {
        QComboBox *tmpCombo1=new QComboBox();
        tmpCombo1->addItem("[-8,7]");
        tmpCombo1->addItem("[-16,15]");
        tmpCombo1->addItem("[-32,31]");
        tmpCombo1->addItem("[-64,63]");
        tmpCombo1->addItem("[-128,127]");
        tmpCombo1->addItem("[-256,255]");
        tmpCombo1->addItem("[-512,511]");
        tmpCombo1->addItem("[-1023,1024]");
        tmpCombo1->addItem("[-2048,2047]");
        ui->searchTable->setCellWidget(i,0,tmpCombo1);
        QComboBox *tmpCombo2=new QComboBox();
        tmpCombo2->addItem("[-8,7]");
        tmpCombo2->addItem("[-16,15]");
        tmpCombo2->addItem("[-32,31]");
        tmpCombo2->addItem("[-64,63]");
        tmpCombo2->addItem("[-128,127]");
        tmpCombo2->addItem("[-256,255]");
        tmpCombo2->addItem("[-512,511]");
        tmpCombo2->addItem("[-1023,1024]");
        tmpCombo2->addItem("[-2048,2047]");
        ui->searchTable->setCellWidget(i,1,tmpCombo2);

        QSpinBox *tmpspin1=new QSpinBox();
        ui->searchTable->setCellWidget(i,2,tmpspin1);
        QSpinBox *tmpspin2=new QSpinBox();
        ui->searchTable->setCellWidget(i,3,tmpspin2);
    }

    ui->searchTable->setVerticalHeaderLabels(vheader);
    ui->searchTable->horizontalHeader()->setStretchLastSection(true);
    ui->searchTable->horizontalHeader()->resizeSection(0,100);
    ui->searchTable->horizontalHeader()->resizeSection(1,100);
    ui->searchTable->horizontalHeader()->resizeSection(2,90);
    ui->searchTable->horizontalHeader()->resizeSection(3,90);
}

void Display::on_reBrowse_clicked()
{
    auto outputPath=QFileDialog::getExistingDirectory(this,"选择输出路径",".");
    if(!outputPath.isEmpty())
    {
        fileRECONS=outputPath+"/output"+QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
        ui->reImage->setText(fileRECONS);
        ui->reImage->setReadOnly(true);
    }
}

void Display::on_noReImage_clicked()
{
    ui->reBrowse->setEnabled(false);
    ui->reImage->setEnabled(false);
    ui->reImage->setText("");
    fileRECONS="";
}

void Display::on_yesReImage_clicked()
{
    ui->reBrowse->setEnabled(true);
    ui->reImage->setEnabled(true);

}

void Display::on_useDefaultInMatrix_clicked()
{
    ui->CustomInMatrix->setEnabled(false);
}

void Display::on_useCustomInMatrix_clicked()
{
    ui->CustomInMatrix->setEnabled(true);
}

void Display::on_useDefaultOutMatrix_clicked()
{
    ui->CustomOutMatrix->setEnabled(false);

}

void Display::on_useCustomOutMatrix_clicked()
{
    ui->CustomOutMatrix->setEnabled(true);
}

void Display::on_pushButton_clicked()
{
    hasError=0;

    if(parameterPrepare()==false)
    {
        if(errorTextGlobal.simplified().isEmpty())
            errorTextGlobal="参数设置有误";
        QMessageBox::critical(this,"严重错误",errorTextGlobal);
        return;
    }

    extern int r,Xi,Xb,Xp,d0i,d0p,d0b; /* rate control */
    extern double avg_act;

    if(fileOUTPUT.isEmpty()||fileNAME.isEmpty())
    {
        sprintf(errortext,"Invalid input or output folder");
        QMessageBox::critical(this,"严重错误","Invalid input or output folder");
        return;
    }

    if (!(outfile=fopen(fileOUTPUT.toStdString().c_str(),"wb")))
    {
       sprintf(errortext,"Couldn't create output file");
       QMessageBox::critical(this,"严重错误","Couldn't create output file");
       fclose(outfile);
       return;
    }

    if(init()==false)
    {
        if(errorTextGlobal.simplified().isEmpty())
            errorTextGlobal="参数设置有误";
        QMessageBox::critical(this,"严重错误",errorTextGlobal);
        fclose(outfile);
        return;
    }

    refreshErrorList();

    groupDATA.clear();
    pictureDATA.clear();

    ifRunning=true;
    checkDataThread *seqThr=new checkDataThread();
    connect(seqThr,SIGNAL(ProcessEnded(bool)),this,SLOT(onProcessEnded(bool)));
    ui->pushButton->setEnabled(false);

    seqThr->start();
    RefreshUI *refUI=new RefreshUI();
    connect(refUI,SIGNAL(refreshSignal()),this,SLOT(onRefreshSignal()));
    refUI->start();
    RefreshBar *refBar=new RefreshBar();
    connect(refBar,SIGNAL(refreshBar()),this,SLOT(onRefreshBar()));
    refBar->start();

/*
    if(putseq()==false)
    {
        QMessageBox::critical(this,"严重错误",errorTextGlobal);
        fclose(outfile);
        fclose(statfile);
        return;
    }
*/

}

void Display::on_outputBrowse_clicked()
{
    auto outpuPath=QFileDialog::getExistingDirectory(this,"选择输出路径",".");
    if(!outpuPath.isEmpty())
    {
        fileOUTPUT=outpuPath+"/video"+QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())+".m2v";
        ui->outputLine->setText(fileOUTPUT);
        ui->outputLine->setReadOnly(true);
    }
}

void Display::on_gopSlider_valueChanged(int value)
{
    ui->gopSpin->setValue(value);
    setGroupData(value);
}

void Display::on_gopSpin_valueChanged(int arg1)
{
    ui->gopSlider->setValue(arg1);
}

void Display::on_frameSlider_valueChanged(int value)
{
    ui->frameSpin->setValue(value);
    setFrameData(value);
}

void Display::on_frameSpin_valueChanged(int arg1)
{
    ui->frameSlider->setValue(arg1);
}

void Display::on_RRRRRRRRButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<groupDATA.size();i++)
        chartsInt.push_back(groupDATA[i].R);
    DisplayChart::showChartint("GOP 目标编码位数");
}

void Display::on_NpButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<groupDATA.size();i++)
        chartsInt.push_back(groupDATA[i].Np);
    DisplayChart::showChartint("GOP 中P帧数量");
}


void Display::on_NbButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<groupDATA.size();i++)
        chartsInt.push_back(groupDATA[i].Nb);
    DisplayChart::showChartint("GOP 中B帧数量");
}

void Display::on_dispNoButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].dispNo);
    DisplayChart::showChartint("帧顺序号");
}

void Display::on_dispNoButton_2_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].codeNo);
    DisplayChart::showChartint("帧编码序号");
}

void Display::on_pictureTimeButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].pictureTime);
    DisplayChart::showChartint("编码耗时");
}

void Display::on_picTypeButton_clicked()
{
    DisplayChart::showThreeBar();
}

void Display::on_vbvDelayButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].vbvDelay);
    DisplayChart::showChartint("VBV延迟");
}

void Display::on_vbvBitCountButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].bitcount);
    DisplayChart::showChartint("VBV延迟");
}

void Display::on_vbvdcdTime_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].vbvDcdTime);
    DisplayChart::showCharDouble("VBV解码时间");
}

void Display::on_bitcntEOPButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].bitcnt_EOP);
    DisplayChart::showChartint("VBV最终位数");
}

void Display::on_targetNumberofBitsButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].targetNumberOfBits);
    DisplayChart::showChartint("目标编码位数");
}

void Display::on_actualBitsButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].actualBits);
    DisplayChart::showChartint("实际编码位数");
}

void Display::on_avgQuanParaButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].avgQuanPara);
    DisplayChart::showCharDouble("平均量化参数");
}

void Display::on_reaminNumberGOPButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].reaminNumberGOP);
    DisplayChart::showChartint("当前GOP中剩余位数");
}

void Display::on_XiiiButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].Xi);
    DisplayChart::showChartint("Xi");
}

void Display::on_XpppButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].Xp);
    DisplayChart::showChartint("Xp");
}

void Display::on_XbbbButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].Xb);
    DisplayChart::showChartint("Xb");
}

void Display::on_tempRefButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].tempRef);
    DisplayChart::showChartint("时间参考系坐标");
}

void Display::on_d0iiiiiiiButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].d0i);
    DisplayChart::showChartint("d0i");
}

void Display::on_d0pppppButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].d0p);
    DisplayChart::showChartint("d0p");
}

void Display::on_d0bbbbbbbbButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].d0b);
    DisplayChart::showChartint("d0b");
}

void Display::on_nppppppppButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].Np);
    DisplayChart::showChartint("Np");
}

void Display::on_nbbbbbbbButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].Nb);
    DisplayChart::showChartint("Nb");
}

void Display::on_avg_acttttttButton_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].avg_act);
    DisplayChart::showChartint("avg_act");
}

void Display::on_YVButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].YV);
    DisplayChart::showCharDouble("Y分量幅度");
}

void Display::on_YMSEDBButton_clicked()
{
    chartsDouble.clear();
    chartsDoubleDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
    {
        chartsDouble.push_back(pictureDATA[i].YMSE);
        chartsDoubleDouble.push_back(pictureDATA[i].YMSEdb);
    }
    DisplayChart::showCharDoubleDouble("Y分量信噪比");
}

void Display::on_YSNRButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].YSNR);
    DisplayChart::showCharDouble("Y分量信噪比");
}

void Display::on_UVButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].UV);
    DisplayChart::showCharDouble("U分量幅度");
}

void Display::on_UMSEDBButton_clicked()
{
    chartsDouble.clear();
    chartsDoubleDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
    {
        chartsDouble.push_back(pictureDATA[i].UMSE);
        chartsDoubleDouble.push_back(pictureDATA[i].UMSEdb);
    }
    DisplayChart::showCharDoubleDouble("U分量信噪比");
}

void Display::on_USNRButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].USNR);
    DisplayChart::showCharDouble("U分量信噪比");
}

void Display::on_VVButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].VV);
    DisplayChart::showCharDouble("V分量幅度");
}

void Display::on_VMSEDBButton_clicked()
{
    chartsDouble.clear();
    chartsDoubleDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
    {
        chartsDouble.push_back(pictureDATA[i].VMSE);
        chartsDoubleDouble.push_back(pictureDATA[i].VMSEdb);
    }
    DisplayChart::showCharDoubleDouble("V分量信噪比");
}

void Display::on_VSNRButton_clicked()
{
    chartsDouble.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsDouble.push_back(pictureDATA[i].VSNR);
    DisplayChart::showCharDouble("V分量信噪比");
}

void Display::on_hash1Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash1);
    DisplayChart::showChartint("帧内编码宏块");
}

void Display::on_hash2Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash2);
    DisplayChart::showChartint("已编码宏块");
}

void Display::on_hash3Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash3);
    DisplayChart::showChartint("未编码宏块");
}

void Display::on_hash4Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash4);
    DisplayChart::showChartint("已跳过宏块");
}

void Display::on_hash5Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash5);
    DisplayChart::showChartint("前向预测宏块");
}

void Display::on_hash6Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash6);
    DisplayChart::showChartint("后向预测宏块");
}

void Display::on_hash7Button_clicked()
{
    chartsInt.clear();
    for(int i=0;i<pictureDATA.size();i++)
        chartsInt.push_back(pictureDATA[i].hash7);
    DisplayChart::showChartint("双向内插预测宏块");
}

void Display::on_visualSlider_valueChanged(int value)
{
    ui->visualSpin->setValue(value);
}

void Display::on_visualSpin_valueChanged(int arg1)
{
    ui->visualSlider->setValue(arg1);
}

void Display::on_confirmVisual_clicked()
{
    ui->confirmVisual->setText("正在查询...");
    ui->confirmVisual->setEnabled(false);
    setMMap1(ui->visualSpin->value());
    setMMap2(ui->visualSpin->value());
    ui->confirmVisual->setText("查询");
    ui->confirmVisual->setEnabled(true);
}

void Display::on_cubeSlider_valueChanged(int value)
{
    ui->cubeSpin->setValue(value);
}

void Display::on_cubeSpin_valueChanged(int arg1)
{
    ui->cubeSlider->setValue(arg1);
}

void Display::on_confirmCube_clicked()
{
    dispPointe3D=ui->cubeSpin->value();
    if(dispPointe3D>=pictureDATA.size())
        return;
    Visual3D::disp();
}
