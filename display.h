#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include"commonData.h"
#include <QTextDocument>
#include <QTextFrame>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Display; }
QT_END_NAMESPACE

class Display : public QWidget
{
    Q_OBJECT

public:
    QString fileNAME;
    QString fileRECONS;
    QString fileOUTPUT;
    QTextCharFormat sformat,iformat,oformat,f1format,f2format,pformat,b1format,b2format,d1format,d2format;
    QTextCharFormat qformat,nformat;
    Display(QWidget *parent = nullptr);
    bool parameterPrepare();
    void refreshErrorList();
    void setFrameData(int num);
    void setGroupData(int num);
    void setGlobalData();
    void setMMap1(int num);
    void setMMap2(int num);
    ~Display();

public slots:
    void onRefreshSignal();
    void onRefreshBar();
    void onProcessEnded(bool status);

private slots:
    void on_sourceBrowse_clicked();

    void on_IPDistance_valueChanged(int arg1);

    void on_reBrowse_clicked();

    void on_noReImage_clicked();

    void on_yesReImage_clicked();

    void on_useDefaultInMatrix_clicked();

    void on_useCustomInMatrix_clicked();

    void on_useDefaultOutMatrix_clicked();

    void on_useCustomOutMatrix_clicked();

    void on_pushButton_clicked();

    void on_outputBrowse_clicked();

    void on_gopSlider_valueChanged(int value);

    void on_gopSpin_valueChanged(int arg1);

    void on_frameSlider_valueChanged(int value);

    void on_frameSpin_valueChanged(int arg1);

    void on_RRRRRRRRButton_clicked();

    void on_NpButton_clicked();

    void on_NbButton_clicked();

    void on_dispNoButton_clicked();

    void on_dispNoButton_2_clicked();

    void on_pictureTimeButton_clicked();

    void on_picTypeButton_clicked();

    void on_vbvDelayButton_clicked();

    void on_vbvBitCountButton_clicked();

    void on_vbvdcdTime_clicked();

    void on_bitcntEOPButton_clicked();

    void on_targetNumberofBitsButton_clicked();

    void on_actualBitsButton_clicked();

    void on_avgQuanParaButton_clicked();

    void on_reaminNumberGOPButton_clicked();

    void on_XiiiButton_clicked();

    void on_XpppButton_clicked();

    void on_XbbbButton_clicked();

    void on_tempRefButton_clicked();

    void on_d0iiiiiiiButton_clicked();

    void on_d0pppppButton_clicked();

    void on_d0bbbbbbbbButton_clicked();

    void on_nppppppppButton_clicked();

    void on_nbbbbbbbButton_clicked();

    void on_avg_acttttttButton_clicked();

    void on_YVButton_clicked();

    void on_YMSEDBButton_clicked();

    void on_YSNRButton_clicked();

    void on_UVButton_clicked();

    void on_UMSEDBButton_clicked();

    void on_USNRButton_clicked();

    void on_VVButton_clicked();

    void on_VMSEDBButton_clicked();

    void on_VSNRButton_clicked();

    void on_hash1Button_clicked();

    void on_hash2Button_clicked();

    void on_hash3Button_clicked();

    void on_hash4Button_clicked();

    void on_hash5Button_clicked();

    void on_hash6Button_clicked();

    void on_hash7Button_clicked();

    void on_visualSlider_valueChanged(int value);

    void on_visualSpin_valueChanged(int arg1);

    void on_confirmVisual_clicked();

    void on_cubeSlider_valueChanged(int value);

    void on_cubeSpin_valueChanged(int arg1);

    void on_confirmCube_clicked();


signals:
    void shadow(bool);

private:
    Ui::Display *ui;
};
#endif // DISPLAY_H
