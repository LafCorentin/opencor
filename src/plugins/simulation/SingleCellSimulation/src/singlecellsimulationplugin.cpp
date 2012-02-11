//==============================================================================
// SingleCellSimulation plugin
//==============================================================================

#include "singlecellsimulationplugin.h"
#include "cellmlmodel.h"
#include "cellmlsupportplugin.h"

//==============================================================================

#include <QFileInfo>
#include <QMainWindow>
#include <QPen>
#include <QTime>
#include <QVector>

//==============================================================================

#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"

//==============================================================================

#include "llvm/Module.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"

//==============================================================================

namespace OpenCOR {
namespace SingleCellSimulation {

//==============================================================================

PLUGININFO_FUNC SingleCellSimulationPluginInfo()
{
    Descriptions descriptions;

    descriptions.insert("en", "A plugin to run single cell simulations");
    descriptions.insert("fr", "Une extension pour ex�cuter des simulations unicellulaires");

    return PluginInfo(PluginInfo::V001,
                      PluginInfo::Gui,
                      PluginInfo::Simulation,
                      true,
                      QStringList() << "CoreSimulation" << "CellMLSupport" << "Qwt",
                      descriptions);
}

//==============================================================================

Q_EXPORT_PLUGIN2(SingleCellSimulation, SingleCellSimulationPlugin)

//==============================================================================

SingleCellSimulationPlugin::SingleCellSimulationPlugin()
{
    // Set our settings

    mGuiSettings->addView(GuiViewSettings::Simulation, 0);
}

//==============================================================================

void SingleCellSimulationPlugin::initialize()
{
    // Create our simulation view widget

    mSimulationView = new QwtPlot(mMainWindow);

    // Hide our simulation view widget since it may not initially be shown in
    // our central widget

    mSimulationView->setVisible(false);

    // Customise our simulation view widget

    mSimulationView->setCanvasBackground(Qt::white);

    // Remove the canvas' border as it otherwise looks odd, not to say ugly,
    // with one

    mSimulationView->setCanvasLineWidth(0);

    // Add a grid to our simulation view widget

    QwtPlotGrid *grid = new QwtPlotGrid;

    grid->setMajPen(QPen(Qt::gray, 0, Qt::DotLine));

    grid->attach(mSimulationView);
}

//==============================================================================

void SingleCellSimulationPlugin::finalize()
{
    // Delete some internal objects

    resetCurves();
}
//==============================================================================

void SingleCellSimulationPlugin::resetCurves()
{
    // Remove any existing curve

    foreach (QwtPlotCurve *curve, mCurves) {
        curve->detach();

        delete curve;
    }

    mCurves.clear();
}

//==============================================================================

QWidget * SingleCellSimulationPlugin::viewWidget(const QString &pFileName,
                                                 const int &)
{
    // Check that we are dealing with a CellML file and, if so, return our
    // generic simulation view widget

    if (CellMLSupport::isCellmlFile(pFileName))
        // Not the expected file extension, so...

        return 0;
    else
{
//--- TESTING --- BEGIN ---

        qDebug("=======================================");
        qDebug("%s:", qPrintable(pFileName));

        // Load the CellML model

        CellMLSupport::CellmlModel *cellmlModel = new CellMLSupport::CellmlModel(pFileName);

        // Check the model's validity

        if (cellmlModel->isValid()) {
            // The model is valid, but let's see whether warnings were generated

            int warningsCount = cellmlModel->issues().count();

            if (warningsCount)
                qDebug(" - The model was properly loaded:");
            else
                qDebug(" - The model was properly loaded.");
        } else {
            qDebug(" - The model was NOT properly loaded:");
        }

        // Output any warnings/errors that were generated

        foreach (const CellMLSupport::CellmlModelIssue &issue, cellmlModel->issues()) {
            QString type = QString((issue.type() == CellMLSupport::CellmlModelIssue::Error)?"Error":"Warrning");
            QString message = issue.formattedMessage();
            uint32_t line = issue.line();
            uint32_t column = issue.column();
            QString importedModel = issue.importedModel();

            if (line && column) {
                if (importedModel.isEmpty())
                    qDebug("    [%s at line %s column %s] %s", qPrintable(type),
                                                               qPrintable(QString::number(issue.line())),
                                                               qPrintable(QString::number(issue.column())),
                                                               qPrintable(message));
                else
                    qDebug("    [%s at line %s column %s from imported model %s] %s", qPrintable(type),
                                                                                      qPrintable(QString::number(issue.line())),
                                                                                      qPrintable(QString::number(issue.column())),
                                                                                      qPrintable(importedModel),
                                                                                      qPrintable(message));
            } else {
                if (importedModel.isEmpty())
                    qDebug("    [%s] %s", qPrintable(type),
                                          qPrintable(message));
                else
                    qDebug("    [%s from imported model %s] %s", qPrintable(type),
                                                                 qPrintable(importedModel),
                                                                 qPrintable(message));
            }
        }

        // Get a runtime for the model

        CellMLSupport::CellmlModelRuntime *cellmlModelRuntime = cellmlModel->runtime();

        if (cellmlModelRuntime->isValid()) {
            qDebug(" - The model's runtime was properly generated.");
            qDebug("    [Information] Model type = %s", (cellmlModelRuntime->modelType() == CellMLSupport::CellmlModelRuntime::Ode)?"ODE":"DAE");
        } else {
            qDebug(" - The model's runtime was NOT properly generated:");

            foreach (const CellMLSupport::CellmlModelIssue &issue,
                     cellmlModelRuntime->issues()) {
                QString type = QString((issue.type() == CellMLSupport::CellmlModelIssue::Error)?"Error":"Warrning");
                QString message = issue.formattedMessage();

                qDebug("    [%s] %s", qPrintable(type), qPrintable(message));
            }
        }

        // Remove any existing curve

        resetCurves();

        // Compute the model, if supported

        enum Model {
            Unknown,
            VanDerPol1928,
            Hodgkin1952,
            Noble1962,
            Noble1984,
            Noble1991,
            Noble1998,
            Zhang2000,
            Mitchell2003
        } model;

        QString fileBaseName = QFileInfo(pFileName).baseName();

        if (!fileBaseName.compare("van_der_pol_model_1928"))
            model = VanDerPol1928;
        else if (   !fileBaseName.compare("hodgkin_huxley_squid_axon_model_1952")
                 || !fileBaseName.compare("hodgkin_huxley_squid_axon_model_1952_modified")
                 || !fileBaseName.compare("hodgkin_huxley_squid_axon_model_1952_original"))
            model = Hodgkin1952;
        else if (!fileBaseName.compare("noble_model_1962"))
            model = Noble1962;
        else if (!fileBaseName.compare("noble_noble_SAN_model_1984"))
            model = Noble1984;
        else if (!fileBaseName.compare("noble_model_1991"))
            model = Noble1991;
        else if (!fileBaseName.compare("noble_model_1998"))
            model = Noble1998;
        else if (   !fileBaseName.compare("zhang_SAN_model_2000_0D_capable")
                 || !fileBaseName.compare("zhang_SAN_model_2000_1D_capable")
                 || !fileBaseName.compare("zhang_SAN_model_2000_all")
                 || !fileBaseName.compare("zhang_SAN_model_2000_published"))
            model = Zhang2000;
        else if (!fileBaseName.compare("mitchell_schaeffer_2003"))
            model = Mitchell2003;

        if (model != Unknown) {
            typedef QVector<double> Doubles;

            int statesCount = cellmlModelRuntime->statesCount();

            Doubles xData;
            Doubles yData[statesCount];

            double voi = 0;   // ms
            double voiStep;   // ms
            double voiMax;    // ms
            double constants[cellmlModelRuntime->constantsCount()];
            double rates[cellmlModelRuntime->ratesCount()];
            double states[statesCount];
            double algebraic[cellmlModelRuntime->algebraicCount()];
            int voiCount = 0;
            int voiOutputCount;   // ms

            switch (model) {
            case Hodgkin1952:
                voiStep        = 0.01;   // ms
                voiMax         = 50;     // ms
                voiOutputCount = 10;

                break;
            case Noble1962:
            case Mitchell2003:
                voiStep        = 0.01;   // ms
                voiMax         = 1000;   // ms
                voiOutputCount = 100;

                break;
            case Noble1984:
            case Noble1991:
            case Noble1998:
            case Zhang2000:
                voiStep        = 0.00001;   // s
                voiMax         = 1;         // s
                voiOutputCount = 100;

                break;
            default:   // van der Pol 1928
                voiStep        = 0.01;   // s
                voiMax         = 10;     // s
                voiOutputCount = 1;
            }

            CellMLSupport::CellmlModelRuntimeOdeFunctions odeFunctions = cellmlModelRuntime->odeFunctions();

            QTime time;

            time.start();

            // Initialise the constants and compute the rates and variables

            odeFunctions.initializeConstants(constants, rates, states);
            odeFunctions.computeRates(voi, constants, rates, states, algebraic);
            odeFunctions.computeVariables(voi, constants, rates, states, algebraic);

            do {
                // Output the current data, if needed

                if(voiCount % voiOutputCount == 0) {
                    xData.append(voi);

                    for (int i = 0; i < statesCount; ++i)
                        yData[i].append(states[i]);
                }

                // Compute the rates and variables

                odeFunctions.computeRates(voi, constants, rates, states, algebraic);
                odeFunctions.computeVariables(voi, constants, rates, states, algebraic);

                // Go to the next voiStep and integrate the states

                voi = ++voiCount*voiStep;

                for (int i = 0; i < statesCount; ++i)
                    states[i] += voiStep*rates[i];
            } while (voi < voiMax);

            xData.append(voi);

            for (int i = 0; i < statesCount; ++i)
                yData[i].append(states[i]);

            qDebug(" - Simulation time: %s s", qPrintable(QString::number(0.001*time.elapsed(), 'g', 3)));

            // Add some curves to our plotting area

            for (int i = 0, iMax = (model == VanDerPol1928)?statesCount:1; i < iMax; ++i) {
                QwtPlotCurve *curve = new QwtPlotCurve();

                curve->setRenderHint(QwtPlotItem::RenderAntialiased);
                curve->setPen(QPen(i%2?Qt::darkBlue:Qt::darkRed));

                curve->setSamples(xData, yData[i]);

                curve->attach(mSimulationView);

                // Keep track of the curve

                mCurves.append(curve);
            }

            // Update the axes range

            mSimulationView->replot();
        }

        // Done with our testing, so...

        delete cellmlModel;

//--- TESTING --- END ---

        // The expected file extension, so return our generic simulation view
        // widget

        return mSimulationView;
}
}

//==============================================================================

QString SingleCellSimulationPlugin::viewName(const int &pViewIndex)
{
    // We have only one view, so return its name otherwise call the GuiInterface
    // implementation of viewName

    switch (pViewIndex) {
    case 0:
        return tr("Single Cell");
    default:
        return GuiInterface::viewName(pViewIndex);
    }
}

//==============================================================================

}   // namespace SingleCellSimulation
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
