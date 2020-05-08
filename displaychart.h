#ifndef DISPLAYCHART_H
#define DISPLAYCHART_H
#include "commonData.h"

class DisplayChart
{
public:
    DisplayChart();
    static void showChartint(QString name);
    static void showCharDouble(QString name);
    static void showCharDoubleDouble(QString name);
    static void showThreeBar();
};

#endif // DISPLAYCHART_H
