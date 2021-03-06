/**
 ******************************************************************************
 *
 * @file       scopegadgetwidget.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Scope Plugin Gadget Widget
 * @see        The GNU Public License (GPL) Version 3
 * @defgroup   scopeplugin
 * @{
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef SCOPEGADGETWIDGET_H_
#define SCOPEGADGETWIDGET_H_

#include "plotdata.h"
#include "uavobjects/uavobject.h"
#include "uavobjects/baroaltitude.h"
#include "uavobjects/positionactual.h"


#include "qwt/src/qwt.h"
#include "qwt/src/qwt_plot.h"
#include "qwt/src/qwt_plot_curve.h"
#include "qwt/src/qwt_scale_draw.h"
#include "qwt/src/qwt_scale_widget.h"

#include <QTimer>
#include <QTime>
#include <QVector>

/*!
  \brief This class is used to render the time values on the horizontal axis for the
  ChronoPlot.
  */
class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw() {
        //baseTime = QDateTime::currentDateTime().toTime_t();
    }
    virtual QwtText label(double v) const {
        uint seconds = (uint)(v);
        QDateTime upTime = QDateTime::fromTime_t(seconds);
        QTime timePart = upTime.time().addMSecs((v - seconds )* 1000);
        upTime.setTime(timePart);
        return upTime.toLocalTime().toString("hh:mm:ss");
    }
private:
//    double baseTime;
};

/*!
  \brief This class is used to inject UAVTalk messages for testing.
  */
class TestDataGen : QObject
{
    Q_OBJECT

public:

    TestDataGen();
    ~TestDataGen();

private:
    BaroAltitude* baroAltitude;
    PositionActual* gps;
    AttitudeRaw* attRaw;
    ManualControlCommand* manCtrlCmd;

    QTimer *timer;
    double testTime;
    int periodMs;

    int debugCounter;

private slots:
    void genTestData();
};


class ScopeGadgetWidget : public QwtPlot
{
    Q_OBJECT

public:
    ScopeGadgetWidget(QWidget *parent = 0);
    ~ScopeGadgetWidget();

    void setupSequencialPlot();
    void setupChronoPlot();
    void setupUAVObjectPlot();
    PlotType plotType(){return m_plotType;}

    void setXWindowSize(double xWindowSize){m_xWindowSize = xWindowSize;}
    double xWindowSize(){return m_xWindowSize;}
    void setRefreshInterval(double refreshInterval){m_refreshInterval = refreshInterval;}
    int refreshInterval(){return m_refreshInterval;}


    void addCurvePlot(QString uavObject, QString uavFieldSubField, int scaleOrderFactor = 0, QPen pen = QPen(Qt::black));
    //void removeCurvePlot(QString uavObject, QString uavField);
    void clearCurvePlots();


private slots:
    void uavObjectReceived(UAVObject*);
    void replotNewData();
    void showCurve(QwtPlotItem *item, bool on);

private:

    void preparePlot(PlotType plotType);
    void setupExamplePlot();

    PlotType m_plotType;

    double m_xWindowSize;
    int m_refreshInterval;
    QList<QString> m_connectedUAVObjects;
    QMap<QString, PlotData*> m_curvesData;

    static TestDataGen* testDataGen;    

    QTimer *replotTimer;
};


#endif /* SCOPEGADGETWIDGET_H_ */
