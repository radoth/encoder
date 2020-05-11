
#include "scatterdatamodifier.h"
#include <QtDataVisualization/qscatterdataproxy.h>
#include <QtDataVisualization/qvalue3daxis.h>
#include <QtDataVisualization/q3dscene.h>
#include <QtDataVisualization/q3dcamera.h>
#include <QtDataVisualization/qscatter3dseries.h>
#include <QtDataVisualization/q3dtheme.h>
#include <QtCore/qmath.h>
#include <QtCore/qrandom.h>
#include <QtWidgets/QComboBox>
#include "commonData.h"
using namespace QtDataVisualization;

//#define RANDOM_SCATTER // Uncomment this to switch to random scatter


const int lowerNumberOfItems = 900;
const float lowerCurveDivider = 0.75f;

ScatterDataModifier::ScatterDataModifier(Q3DScatter *scatter)
    : m_graph(scatter),
      m_fontSize(40.0f),
      m_style(QAbstract3DSeries::MeshSphere),
      m_smooth(true),
      m_itemCount(lowerNumberOfItems),
      m_curveDivider(lowerCurveDivider)
{
    //! [0]
    m_graph->activeTheme()->setType(Q3DTheme::ThemePrimaryColors);
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(m_fontSize);
    m_graph->activeTheme()->setFont(font);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);
    //! [0]

    //! [2]
    QScatterDataProxy *proxy = new QScatterDataProxy;
    QScatter3DSeries *series = new QScatter3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    series->setMeshSmooth(true);
    m_graph->addSeries(series);
    m_graph->activeTheme()->setGridEnabled(true);
    m_graph->activeTheme()->setBackgroundEnabled(true);
    //! [2]

    //! [3]
    addData();
    //! [3]
}

ScatterDataModifier::~ScatterDataModifier()
{
    delete m_graph;
}

void ScatterDataModifier::addData()
{
    m_itemCount=(pictureDATA[dispPointe3D].rountineOut)*(pictureDATA[dispPointe3D].rountineIn);
    // Configure the axes according to the data
    //! [4]
    m_graph->axisX()->setTitle("X");
    m_graph->axisY()->setTitle("Y");
    m_graph->axisZ()->setTitle("Z");
    //! [4]

    //! [5]
    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(m_itemCount);
    QScatterDataItem *ptrToDataArray = &dataArray->first();
    //! [5]

#ifdef RANDOM_SCATTER
    for (int i = 0; i < m_itemCount; i++) {
        ptrToDataArray->setPosition(randVector());
        ptrToDataArray++;
    }
#else
    //! [6]
    for(int i=0;i<pictureDATA[dispPointe3D].rountineOut;i++)
    {
        for(int j=0;j<pictureDATA[dispPointe3D].rountineIn;j++)
        {
            ptrToDataArray->setPosition(QVector3D(i,(pictureDATA[dispPointe3D].quantmap)[i][j],j));
            ptrToDataArray++;
        }
    }


    //! [6]
#endif

    //! [7]
    m_graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);
    //! [7]
}

void ScatterDataModifier::shadowManage(bool st)
{
    if(st)
    {
        QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(4);
        m_graph->setShadowQuality(sq);
    }
    else{
        QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(0);
        m_graph->setShadowQuality(sq);
    }

}

