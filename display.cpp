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
}

bool Display::parameterPrepare()
{
    int h,m,s,f;
    int i;
    extern int r,Xi,Xb,Xp,d0i,d0p,d0b; /* rate control */
    extern double avg_act;

    strcpy(id_string,"ZYC");

    if(fileNAME.simplified().isEmpty())
        strcpy(tplorg,"bad");
    else strcpy(tplorg,fileNAME.toStdString().c_str());

    if(fileRECONS.simplified().isEmpty())
        strcpy(tplref,"-");
    else strcpy(tplref,fileRECONS.toStdString().c_str());

    strcpy(iqname,"-");
    strcpy(niqname,"-");
    strcpy(statname,"stat.out");

    if(ui->chooseYUVFormat->isChecked())
        inputtype=1;
    else inputtype=2;

    nframes=ui->frameNumber->value();
    frame0=ui->firstFrameNumber->value();
    h=ui->timecode1->value();
    m=ui->timecode2->value();
    s=ui->timecode3->value();
    f=ui->timecode4->value();
    N=ui->framesInGOP->value();
    M=ui->IPDistance->value();
    qDebug()<<"M="<<M;
    if(ui->stream1->isChecked())
        mpeg1=1;
    else mpeg1=0;
    if(ui->fieldPictures->isChecked())
        fieldpic=1;
    else fieldpic=0;
    horizontal_size=ui->horizontalSize->value();
    vertical_size=ui->VerticalSize->value();
    aspectratio=(ui->aspectRatio->currentIndex())+1;
    frame_rate_code=(ui->frameRate->currentIndex())+1;
    bit_rate=ui->bitRates->value();
    vbv_buffer_size=ui->vbvBufferSize->value();
    low_delay=0;
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
        prog_seq=0;
    else prog_seq=1;
    chroma_format=1;
    video_format=ui->videoFormat->currentIndex();
    switch(ui->colorPrimary->currentIndex())
    {
    case 0:color_primaries=1;break;
    case 1:color_primaries=2;break;
    case 2:color_primaries=4;break;
    case 3:color_primaries=5;break;
    case 4:color_primaries=6;break;
    case 5:color_primaries=7;break;
    default:qDebug()<<"wrong";
    }
    switch(ui->transferCo->currentIndex())
    {
    case 0:transfer_characteristics=1;break;
    case 1:transfer_characteristics=2;break;
    case 2:transfer_characteristics=4;break;
    case 3:transfer_characteristics=5;break;
    case 4:transfer_characteristics=6;break;
    case 5:transfer_characteristics=7;break;
    case 6:transfer_characteristics=8;break;
    default:qDebug()<<"wrong";
    }
    switch(ui->matrixCo->currentIndex())
    {
    case 0:matrix_coefficients=1;break;
    case 1:matrix_coefficients=2;break;
    case 2:matrix_coefficients=4;break;
    case 3:matrix_coefficients=5;break;
    case 4:matrix_coefficients=6;break;
    case 5:matrix_coefficients=7;break;
    default:qDebug()<<"wrong";
    }
    display_horizontal_size=ui->dispHorizontalSize->value();
    display_vertical_size=ui->dispVerticalSize->value();
    dc_prec=ui->intraDCPrecision->currentIndex();
    if(ui->topFieldFirstNew->isChecked())
        topfirst=1;
    else topfirst=0;
    if(ui->framePredFrameI->isChecked())
        frame_pred_dct_tab[0]=1;
    else frame_pred_dct_tab[0]=0;
    if(ui->framePredFrameP->isChecked())
        frame_pred_dct_tab[1]=1;
    else frame_pred_dct_tab[1]=0;
    if(ui->framePredFrameB->isChecked())
        frame_pred_dct_tab[2]=1;
    else frame_pred_dct_tab[2]=0;

    conceal_tab[0]=0;
    conceal_tab[1]=0;
    conceal_tab[2]=0;

    if(ui->qScaleTypeI->isChecked())
        qscale_tab[0]=1;
    else qscale_tab[0]=0;
    if(ui->qScaleTypeP->isChecked())
        qscale_tab[1]=1;
    else qscale_tab[1]=0;
    if(ui->qScaleTypeB->isChecked())
        qscale_tab[2]=1;
    else qscale_tab[2]=0;

    if(ui->intraVLCFormatI->isChecked())
        intravlc_tab[0]=1;
    else intravlc_tab[0]=0;
    if(ui->intraVLCFormatP->isChecked())
        intravlc_tab[1]=1;
    else intravlc_tab[1]=0;
    if(ui->intraVLCFormatB->isChecked())
        intravlc_tab[2]=1;
    else intravlc_tab[2]=0;

    if(ui->alternateScanI->isChecked())
        altscan_tab[0]=1;
    else altscan_tab[0]=0;
    if(ui->alternateScanP->isChecked())
        altscan_tab[1]=1;
    else altscan_tab[1]=0;
    if(ui->alternateScanB->isChecked())
        altscan_tab[2]=1;
    else altscan_tab[2]=0;

    if(ui->repeatFirstField->isChecked())
        repeatfirst=1;
    else repeatfirst=0;

    if(ui->progressiveI->isChecked())
        prog_frame=1;
    else prog_frame=0;

    P=0;

    r=ui->rcR->value();
    avg_act=ui->rcAvgAct->value();
    Xi=ui->rcXi->value();
    Xp=ui->rcXp->value();
    Xb=ui->rcXb->value();
    d0i=ui->rcd0i->value();
    d0p=ui->rcd0p->value();
    d0b=ui->rcd0b->value();


    if (N<1)
      {error("N must be positive");return false;}
    if (M<1)
      {error("M must be positive");return false;}
    if (N%M != 0)
      {error("N must be an integer multiple of M");return false;}

    motion_data = (struct motion_data *)malloc(M*sizeof(struct motion_data));
    if (!motion_data)
      {error("malloc failed\n");return false;}

    motion_data[0].forw_hor_f_code=((((QComboBox *)(ui->searchTable->cellWidget(0,0)))->currentIndex()))+1;
    motion_data[0].forw_vert_f_code=((((QComboBox *)(ui->searchTable->cellWidget(0,1)))->currentIndex()))+1;
    motion_data[0].sxf=((QSpinBox *)(ui->searchTable->cellWidget(0,2)))->value();
    motion_data[0].syf=((QSpinBox *)(ui->searchTable->cellWidget(0,3)))->value();

    for(int i=1;i<2*M-1;i++)
    {
        if(i%2==1)
        {
            motion_data[i/2+1].forw_hor_f_code=(((QComboBox *)(ui->searchTable->cellWidget(i,0)))->currentIndex())+1;
            motion_data[i/2+1].forw_vert_f_code=(((QComboBox *)(ui->searchTable->cellWidget(i,1)))->currentIndex())+1;
            motion_data[i/2+1].sxf=((QSpinBox *)(ui->searchTable->cellWidget(i,2)))->value();
            motion_data[i/2+1].syf=((QSpinBox *)(ui->searchTable->cellWidget(i,3)))->value();
        }
        else{
            motion_data[i/2].back_hor_f_code=((QComboBox *)(ui->searchTable->cellWidget(i,0)))->currentIndex()+1;
            motion_data[i/2].back_vert_f_code=((QComboBox *)(ui->searchTable->cellWidget(i,1)))->currentIndex()+1;
            motion_data[i/2].sxb=((QSpinBox *)(ui->searchTable->cellWidget(i,2)))->value();
            motion_data[i/2].syb=((QSpinBox *)(ui->searchTable->cellWidget(i,3)))->value();
        }
    }






    qDebug()<<"===============";
    qDebug()<<"tplorg="<<tplorg;
    qDebug()<<"tplref="<<tplref;
    qDebug()<<"iqname="<<iqname;
    qDebug()<<"niqname="<<niqname;
    qDebug()<<"statname="<<statname;
    qDebug()<<"inputtype="<<inputtype;
    qDebug()<<"nframes="<<nframes;
    qDebug()<<"frame0"<<frame0;
    qDebug()<<"N="<<N;
    qDebug()<<"M="<<M;
    qDebug()<<"mpeg1"<<mpeg1;
    qDebug()<<"fieldPic="<<fieldpic;
    qDebug()<<"horizontalsize="<<horizontal_size;
    qDebug()<<"verticalSize="<<vertical_size;
    qDebug()<<"aspectratio="<<aspectratio;
    qDebug()<<"frameratecode="<<frame_rate_code;
    qDebug()<<"bitrate="<<bit_rate;
    qDebug()<<"vbvbuffersize="<<vbv_buffer_size;
    qDebug()<<"lowdelay="<<low_delay;
    qDebug()<<"constrparams="<<constrparms;
    qDebug()<<"profile"<<profile;
    qDebug()<<"level="<<level;
    qDebug()<<"prog_seq"<<prog_seq;
    qDebug()<<"chroma_format"<<chroma_format;
    qDebug()<<"cideo_format"<<video_format;
    qDebug()<<"color_primary"<<color_primaries;
    qDebug()<<"transfer_charac="<<transfer_characteristics;
    qDebug()<<"matrixCoeffi"<<matrix_coefficients;
    qDebug()<<"displayhorizontal"<<display_horizontal_size;
    qDebug()<<"displayvertical"<<display_vertical_size;
    qDebug()<<"dc_prec="<<dc_prec;
    qDebug()<<"topfirst="<<topfirst;
    qDebug()<<"fram_pred_dct_tab"<<frame_pred_dct_tab[0]<<frame_pred_dct_tab[1]<<frame_pred_dct_tab[2];
    qDebug()<<"conceal_tab"<<conceal_tab[0]<<conceal_tab[1]<<conceal_tab[2];
    qDebug()<<"qscale_tab"<<qscale_tab[0]<<qscale_tab[1]<<qscale_tab[2];
    qDebug()<<"intravlc_tab"<<intravlc_tab[0]<<intravlc_tab[1]<<intravlc_tab[2];
    qDebug()<<"altscan_tab"<<altscan_tab[0]<<altscan_tab[1]<<altscan_tab[2];
    qDebug()<<"repeatfirst="<<repeatfirst;
    qDebug()<<"prog_frame="<<prog_frame;
    qDebug()<<"P="<<P;
    qDebug()<<"r="<<r;
    qDebug()<<"avgact="<<avg_act;
    qDebug()<<"xi="<<Xi;
    qDebug()<<"xp="<<Xp;
    qDebug()<<"Xb="<<Xb;
    qDebug()<<"d0i"<<d0i;
    qDebug()<<"d0p"<<d0p;
    qDebug()<<"d0b="<<d0b;
    for(int i=0;i<M;i++)
    {
        qDebug()<<"motion_data="<<motion_data[i].forw_hor_f_code<<"    "<<motion_data[i].forw_vert_f_code<<"    "<<motion_data[i].sxf<<"    "<<motion_data[i].syf;
        if(i!=0)
        {
            qDebug()<<"motion_data="<<motion_data[i].back_hor_f_code<<"    "<<motion_data[i].back_vert_f_code<<"    "<<motion_data[i].sxb<<"    "<<motion_data[i].syb;
        }
    }










    mpeg1 = !!mpeg1;
    fieldpic = !!fieldpic;
    low_delay = !!low_delay;
    constrparms = !!constrparms;
    prog_seq = !!prog_seq;
    topfirst = !!topfirst;

    for (int i=0; i<3; i++)
    {
      frame_pred_dct_tab[i] = !!frame_pred_dct_tab[i];
      conceal_tab[i] = !!conceal_tab[i];
      qscale_tab[i] = !!qscale_tab[i];
      intravlc_tab[i] = !!intravlc_tab[i];
      altscan_tab[i] = !!altscan_tab[i];
    }
    repeatfirst = !!repeatfirst;
    prog_frame = !!prog_frame;

    if(range_checks()==false)
        return false;

    static double ratetab[8]=
      {24000.0/1001.0,24.0,25.0,30000.0/1001.0,30.0,50.0,60000.0/1001.0,60.0};

    frame_rate = ratetab[frame_rate_code-1];

    tc0 = h;
    tc0 = 60*tc0 + m;
    tc0 = 60*tc0 + s;
    tc0 = (int)(frame_rate+0.5)*tc0 + f;

    if (!mpeg1)
    {
      if(profile_and_level_checks()==false)
          return false;
    }
    else
    {
      /* MPEG-1 */
      if (constrparms)
      {
        if (horizontal_size>768
            || vertical_size>576
            || ((horizontal_size+15)/16)*((vertical_size+15)/16)>396
            || ((horizontal_size+15)/16)*((vertical_size+15)/16)*frame_rate>396*25.0
            || frame_rate>30.0)
        {
          if (!quiet)
            {
              fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
              warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
          }
          constrparms = 0;
        }
      }

      if (constrparms)
      {
        for (i=0; i<M; i++)
        {
          if (motion_data[i].forw_hor_f_code>4)
          {
            if (!quiet)
              {
                fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }

          if (motion_data[i].forw_vert_f_code>4)
          {
            if (!quiet)
              {
                fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
            }
            constrparms = 0;
            break;
          }

          if (i!=0)
          {
            if (motion_data[i].back_hor_f_code>4)
            {
              if (!quiet)
                {
                  fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                  warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
              }
              constrparms = 0;
              break;
            }

            if (motion_data[i].back_vert_f_code>4)
            {
              if (!quiet)
                {
                  fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
                  warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
              }
              constrparms = 0;
              break;
            }
          }
        }
      }
    }

    /* relational checks */

    if (mpeg1)
    {
      if (!prog_seq)
      {
        if (!quiet)
          {
            fprintf(stderr,"Warning: setting progressive_sequence = 1\n");
            warningTextGlobal.append("Warning: setting progressive_sequence = 1");
        }
        prog_seq = 1;
      }

      if (chroma_format!=CHROMA420)
      {
        if (!quiet)
          {
            fprintf(stderr,"Warning: setting chroma_format = 1 (4:2:0)\n");
            warningTextGlobal.append("Warning: setting chroma_format = 1 (4:2:0)");
        }
        chroma_format = CHROMA420;
      }

      if (dc_prec!=0)
      {
        if (!quiet)
          {
            fprintf(stderr,"Warning: setting intra_dc_precision = 0\n");
            warningTextGlobal.append("Warning: setting intra_dc_precision = 0");
        }
        dc_prec = 0;
      }

      for (i=0; i<3; i++)
        if (qscale_tab[i])
        {
          if (!quiet)
            {
              fprintf(stderr,"Warning: setting qscale_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("Warning: setting qscale_tab[%1] = 0").arg(i));
          }
          qscale_tab[i] = 0;
        }

      for (i=0; i<3; i++)
        if (intravlc_tab[i])
        {
          if (!quiet)
            {
              fprintf(stderr,"Warning: setting intravlc_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("Warning: setting intravlc_tab[%1] = 0").arg(i));
          }
          intravlc_tab[i] = 0;
        }

      for (i=0; i<3; i++)
        if (altscan_tab[i])
        {
          if (!quiet)
            {
              fprintf(stderr,"Warning: setting altscan_tab[%d] = 0\n",i);
              warningTextGlobal.append(QString("Warning: setting altscan_tab[%1] = 0").arg(i));
          }
          altscan_tab[i] = 0;
        }
    }

    if (!mpeg1 && constrparms)
    {
      if (!quiet)
        {
          fprintf(stderr,"Warning: setting constrained_parameters_flag = 0\n");
          warningTextGlobal.append("Warning: setting constrained_parameters_flag = 0");
      }
      constrparms = 0;
    }

    if (prog_seq && !prog_frame)
    {
      if (!quiet)
        {
          fprintf(stderr,"Warning: setting progressive_frame = 1\n");
          warningTextGlobal.append("Warning: setting progressive_frame = 1");
      }
      prog_frame = 1;
    }

    if (prog_frame && fieldpic)
    {
      if (!quiet)
        {
          fprintf(stderr,"Warning: setting field_pictures = 0\n");
          warningTextGlobal.append("Warning: setting field_pictures = 0");
      }
      fieldpic = 0;
    }

    if (!prog_frame && repeatfirst)
    {
      if (!quiet)
        {
          fprintf(stderr,"Warning: setting repeat_first_field = 0\n");
          warningTextGlobal.append("Warning: setting repeat_first_field = 0");
      }
      repeatfirst = 0;
    }

    if (prog_frame)
    {
      for (i=0; i<3; i++)
        if (!frame_pred_dct_tab[i])
        {
          if (!quiet)
            {
              fprintf(stderr,"Warning: setting frame_pred_frame_dct[%d] = 1\n",i);
              warningTextGlobal.append(QString("Warning: setting frame_pred_frame_dct[%1] = 1").arg(i));
          }
          frame_pred_dct_tab[i] = 1;
        }
    }

    if (prog_seq && !repeatfirst && topfirst)
    {
      if (!quiet)
        {
          fprintf(stderr,"Warning: setting top_field_first = 0\n");
          warningTextGlobal.append("Warning: setting top_field_first = 0");
      }
      topfirst = 0;
    }

    /* search windows */
    for (i=0; i<M; i++)
    {
      if (motion_data[i].sxf > (4<<motion_data[i].forw_hor_f_code)-1)
      {
        if (!quiet)
          {
            fprintf(stderr,
            "Warning: reducing forward horizontal search width to %d\n",
            (4<<motion_data[i].forw_hor_f_code)-1);
            warningTextGlobal.append(QString("Warning: reducing forward horizontal search width to %1").arg((4<<motion_data[i].forw_hor_f_code)-1));
        }
        motion_data[i].sxf = (4<<motion_data[i].forw_hor_f_code)-1;
      }

      if (motion_data[i].syf > (4<<motion_data[i].forw_vert_f_code)-1)
      {
        if (!quiet)
          {
            fprintf(stderr,
            "Warning: reducing forward vertical search width to %d\n",
            (4<<motion_data[i].forw_vert_f_code)-1);
            warningTextGlobal.append(QString("Warning: reducing forward vertical search width to %1").arg((4<<motion_data[i].forw_vert_f_code)-1));
        }
        motion_data[i].syf = (4<<motion_data[i].forw_vert_f_code)-1;
      }

      if (i!=0)
      {
        if (motion_data[i].sxb > (4<<motion_data[i].back_hor_f_code)-1)
        {
          if (!quiet)
            {
              fprintf(stderr,
              "Warning: reducing backward horizontal search width to %d\n",
              (4<<motion_data[i].back_hor_f_code)-1);
              warningTextGlobal.append(QString("Warning: reducing backward horizontal search width to %1").arg((4<<motion_data[i].back_hor_f_code)-1));
          }
          motion_data[i].sxb = (4<<motion_data[i].back_hor_f_code)-1;
        }

        if (motion_data[i].syb > (4<<motion_data[i].back_vert_f_code)-1)
        {
          if (!quiet)
            {
              fprintf(stderr,
              "Warning: reducing backward vertical search width to %d\n",
              (4<<motion_data[i].back_vert_f_code)-1);
              warningTextGlobal.append(QString("Warning: reducing backward vertical search width to %1").arg((4<<motion_data[i].back_vert_f_code)-1));
          }
          motion_data[i].syb = (4<<motion_data[i].back_vert_f_code)-1;
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
        QMessageBox::information(this,"编码成功","编码成功结束。");
    else QMessageBox::critical(this,"编码失败","编码失败。\n"+errorTextGlobal);
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
    M=arg1;

    ui->searchTable->setRowCount(2*M-1);
    ui->searchTable->setColumnCount(4);
    QStringList header;
    header<<"水平搜索限长"<<"垂直搜索限长"<<"搜索宽度"<<"搜索高度";
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

    for(int i=1;i<M;i++)
    {
        QString tmp="B"+QString::number(i);
        vheader<<tmp+"前向";
        vheader<<tmp+"后向";
    }

    for(int i=1;i<2*M-1;i++)
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
    ui->CustomInMatrix->clear();
}

void Display::on_useCustomInMatrix_clicked()
{
    ui->CustomInMatrix->setEnabled(true);
}

void Display::on_useDefaultOutMatrix_clicked()
{
    ui->CustomOutMatrix->setEnabled(false);
    ui->CustomOutMatrix->clear();

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
        QMessageBox::critical(this,"严重错误",errorTextGlobal);
        return;
    }

    if(readquantmat()==false)
    {
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
       fclose(statfile);
       return;
    }

    if(init()==false)
    {
        QMessageBox::critical(this,"严重错误",errorTextGlobal);
        fclose(outfile);
        fclose(statfile);
        return;
    }

    refreshErrorList();

    groupDATA.clear();
    pictureDATA.clear();

    ifRunning=true;
    checkDataThread *seqThr=new checkDataThread();
    connect(seqThr,SIGNAL(ProcessEnded(bool)),this,SLOT(onProcessEnded(bool)));
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
