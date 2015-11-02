/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef GVLE2_SIM_LOG_H
#define GVLE2_SIM_LOG_H

#include <QObject>
#include <QList>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QThread>
#include <vle/gvle2/plugin_sim.h>
#include <vle/gvle2/logger.h>
#include "thread.h"
#include "simtab.h"
#include "widtoolbar.h"

namespace vle {
namespace gvle2 {


class SimLog : public QObject, public PluginSimulator
{
    Q_OBJECT
    Q_INTERFACES(vle::gvle2::PluginSimulator)

public:
    SimLog();
    ~SimLog();
    QString getname();
    QWidget *getWidget();
    void     delWidget();
    QWidget *getWidgetToolbar();
    void     delWidgetToolbar();
    void setSettings(QSettings *s);
    void setLogger(Logger *logger);
    void init(vleVpm *vpm);
    void *getVpm();
    void setPackage(vle::utils::Package *pkg);

public slots:
    void startStop();
    void addPlotSig(QString view, QString port);
    void delPlotSig(QString port);
    void simulationGetStep();
    void simulationFinished();

private:
    void log(QString message);
    void simulationStart();

private:
    QString name;
    QSettings             *  mSettings;
    Logger                *  mLogger;
    SimTabLog             *  mWidgetTab;
    widToolbar            *  mWidgetToolbar;
    vleVpm                *  mVpm;
    vle::utils::Package   *  mCurrPackage;
    QList<QString>           mObsPorts;
    QThread               *  mThread;
    simLogThread         *  mSimThread;
    vle::value::Map       *  mOutputs;
    QMap<QString, int>       mViewLastRow;
    bool                     mSimDataValid;
    int                      mStepCount;
    double                   mDuration;
};

}} //namespaces

#endif // SIM_PLOT_H
