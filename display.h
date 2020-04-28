#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include"global.h"

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
    Display(QWidget *parent = nullptr);
    bool parameterPrepare();
    void refreshErrorList();
    void setFrameData(int num);
    void setGroupData(int num);
    void setGlobalData();
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

private:
    Ui::Display *ui;
};
#endif // DISPLAY_H
