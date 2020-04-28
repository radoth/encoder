#include "displaychart.h"
#include <QMainWindow>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

DisplayChart::DisplayChart()
{

}

void DisplayChart::showCharDouble(QString name)
{
    QLineSeries *series0 = new QLineSeries();
    series0->setName(name);
    series0->setObjectName("这是啥");
    series0->setPointsVisible(true);
    //for(int i=0;i<fireData.size();i++)
        //series0->append(i,fireData[i].w);
    for(int i=0;i<chartsDouble.size();i++)
        series0->append(i,chartsDouble[i]);
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series0);
    chart->createDefaultAxes();
    chart->setTitle("用鼠标框选某一区域可以放大到该区域，右键单击可以缩小显示区域。");
    chart->setFont(QFont("微软雅黑",20,10));
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::VerticalRubberBand);
    chartView->setRubberBand(QChartView::HorizontalRubberBand);
    chartView->setFont(QFont("微软雅黑",20,10));
    chartView->setStyleSheet("font: 75 12px \"微软雅黑\";");
    QMainWindow *window=new QMainWindow();
    window->setCentralWidget(chartView);
    window->setFont(QFont("微软雅黑",20,10));
    window->resize(600, 500);
    window->show();
}

void DisplayChart::showCharDoubleDouble(QString name)
{
    QLineSeries *series0 = new QLineSeries();
    series0->setName(name);
    series0->setObjectName("绝对值");
    series0->setPointsVisible(true);
    //for(int i=0;i<fireData.size();i++)
        //series0->append(i,fireData[i].w);
    for(int i=0;i<chartsDouble.size();i++)
        series0->append(i,chartsDouble[i]);

    QLineSeries *series1 = new QLineSeries();
    series1->setName(name);
    series1->setPointsVisible(true);
    series1->setObjectName("分贝数");
    //for(int i=0;i<fireData.size();i++)
        //series0->append(i,fireData[i].w);
    for(int i=0;i<chartsDoubleDouble.size();i++)
        series1->append(i,chartsDoubleDouble[i]);


    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series0);
    chart->addSeries(series1);
    chart->createDefaultAxes();
    chart->setTitle("用鼠标框选某一区域可以放大到该区域，右键单击可以缩小显示区域。");
    chart->setFont(QFont("微软雅黑",20,10));
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::VerticalRubberBand);
    chartView->setRubberBand(QChartView::HorizontalRubberBand);
    chartView->setFont(QFont("微软雅黑",20,10));
    chartView->setStyleSheet("font: 75 12px \"微软雅黑\";");
    QMainWindow *window=new QMainWindow();
    window->setCentralWidget(chartView);
    window->setFont(QFont("微软雅黑",20,10));
    window->resize(600, 500);
    window->show();
}

void DisplayChart::showThreeBar()
{
    QBarSet *set0 = new QBarSet("I帧");
    QBarSet *set1 = new QBarSet("P帧");
    QBarSet *set2 = new QBarSet("B帧");

    int inum=0,pnum=0,bnum=0;

    for(int i=0;i<pictureDATA.size();i++)
    {
        if(pictureDATA[i].picType=='I')
            inum++;
        else if(pictureDATA[i].picType=='P')
            pnum++;
        else if(pictureDATA[i].picType=='B')
            bnum++;
    }

    *set0<<inum;
    *set1<<pnum;
    *set2<<bnum;

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);

    QChart *chart = new QChart();
    chart->addSeries(series);

    chart->setTitle("帧类型");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow *window=new QMainWindow();
    window->setCentralWidget(chartView);
    window->setFont(QFont("微软雅黑",20,10));
    window->resize(600, 500);
    window->show();
}

void DisplayChart::showChartint(QString name)
{
       QLineSeries *series0 = new QLineSeries();
       series0->setName(name);
       series0->setObjectName("这是啥");
       series0->setPointsVisible(true);
       //for(int i=0;i<fireData.size();i++)
           //series0->append(i,fireData[i].w);
       for(int i=0;i<chartsInt.size();i++)
           series0->append(i,chartsInt[i]);
       QChart *chart = new QChart();
       chart->legend()->hide();
       chart->addSeries(series0);
       chart->createDefaultAxes();
       chart->setTitle("用鼠标框选某一区域可以放大到该区域，右键单击可以缩小显示区域。");
       chart->setFont(QFont("微软雅黑",20,10));
       QChartView *chartView = new QChartView(chart);
       chartView->setRenderHint(QPainter::Antialiasing);
       chartView->setRubberBand(QChartView::VerticalRubberBand);
       chartView->setRubberBand(QChartView::HorizontalRubberBand);
       chartView->setFont(QFont("微软雅黑",20,10));
       chartView->setStyleSheet("font: 75 12px \"微软雅黑\";");
       QMainWindow *window=new QMainWindow();
       window->setCentralWidget(chartView);
       window->setFont(QFont("微软雅黑",20,10));
       window->resize(600, 500);
       window->show();
}
