/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://gnu.org/licenses>.

*******************************************************************************/

//==============================================================================
// Zinc window
//==============================================================================

#include "borderedwidget.h"
#include "corecliutils.h"
#include "coreguiutils.h"
#include "simulationexperimentviewzincwidget.h"
#include "zincwidget.h"

//==============================================================================

#include <QDir>
#include <QDragEnterEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QCheckBox>
#include <QSlider>

//==============================================================================

#include "zincbegin.h"
    #include "opencmiss/zinc/result.hpp"
    #include "opencmiss/zinc/fieldfiniteelement.hpp"
    #include "opencmiss/zinc/streamregion.hpp"
    #include "opencmiss/zinc/spectrum.hpp"
#include "zincend.h"

//==============================================================================

namespace OpenCOR {
namespace SimulationExperimentView {

//==============================================================================

SimulationExperimentViewZincWidget::SimulationExperimentViewZincWidget(QWidget *pParent) :
    Core::Widget(pParent)
{
    QLayout *layout = createLayout();

    mMappingFileLabel = new QLabel(this);
    mMapNodeVariables = new QMap<int, _variable>();
    mMapNodeValues = new QMap<int, double*>();
    //TODO remove this
    loadMappingFile("please open something, gimme a mapping file !");

    layout->addWidget(mMappingFileLabel);

    // Create and connect our menu actions

    mActionAxes = Core::newAction(true,this);
    mActionPoints = Core::newAction(true,this);
    mActionLines = Core::newAction(true,this);
    mActionSurfaces = Core::newAction(true,this);
    mActionIsosurfaces = Core::newAction(true,this);
    mActionTrilinearCube = Core::newAction(this);

    //TODO put this in settings
    mActionAxes->setChecked(true);
    mActionPoints->setChecked(true);
    mActionLines->setChecked(true);
    mActionSurfaces->setChecked(true);
    mActionIsosurfaces->setChecked(true);

    mActionAxes->setText("Axes");
    mActionPoints->setText("Points");
    mActionLines->setText("Lines");
    mActionSurfaces->setText("Surfaces");
    mActionIsosurfaces->setText("Isosurfaces");
    mActionTrilinearCube->setText("Trilinear Cube");

    connect(mActionAxes, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionAxesTriggered);
    connect(mActionPoints, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionPointsTriggered);
    connect(mActionLines, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionLinesTriggered);
    connect(mActionSurfaces, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionSurfacesTriggered);
    connect(mActionIsosurfaces, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionIsosurfacesTriggered);
    connect(mActionTrilinearCube, &QAction::triggered,
            this, &SimulationExperimentViewZincWidget::actionTrilinearCubeTriggered);

    // Create a temporary copy of our trilinear cube mesh file

    static const QString ExFileName = "/ZincWindow/trilinearCube.exfile";

    mTrilinearCubeMeshFileName = Core::canonicalFileName(QDir::tempPath()+ExFileName);

    Core::writeResourceToFile(mTrilinearCubeMeshFileName, ":"+ExFileName);

    // Allow for things to be dropped on us

    setAcceptDrops(true);

    // Create and populate our context menu

    QMenu *contextMenu = new QMenu(this);

    contextMenu->addAction(mActionAxes);
    contextMenu->addAction(mActionPoints);
    contextMenu->addAction(mActionLines);
    contextMenu->addAction(mActionSurfaces);
    contextMenu->addAction(mActionIsosurfaces);
    contextMenu->addSeparator();
    contextMenu->addAction(mActionTrilinearCube);

    // Create and add a Zinc widget

    mZincWidget = new ZincWidget::ZincWidget(this);

    mZincWidget->setContextMenu(contextMenu);

//    connect(mZincWidget, &ZincWidget::ZincWidget::contextAboutToBeDestroyed,
//            this, &SimulationExperimentViewZincWidget::createAndSetZincContext);
    connect(mZincWidget, &ZincWidget::ZincWidget::graphicsInitialized,
            this, &SimulationExperimentViewZincWidget::graphicsInitialized);
    connect(mZincWidget, &ZincWidget::ZincWidget::devicePixelRatioChanged,
            this, &SimulationExperimentViewZincWidget::devicePixelRatioChanged);

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    layout->addWidget(new Core::BorderedWidget(mZincWidget,
                                                     false, true, true, true));
#else
    layout->addWidget(mZincWidget);
#endif

    // Create and add our time label and check box

    Core::Widget *timeWidget = new Core::Widget(QSize(), this);

    timeWidget->createLayout(Core::Widget::Layout::Horizontal);

    mTimeLabel = new QLabel(timeWidget);

    mTimeLabel->setEnabled(false);
    mTimeLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    timeWidget->layout()->addWidget(mTimeLabel);

    mTimeCheckBox = new QCheckBox(timeWidget);

    mTimeCheckBox->setEnabled(false);
    mTimeCheckBox->setText(tr("Auto"));

    connect(mTimeCheckBox, &QCheckBox::toggled,
            this, &SimulationExperimentViewZincWidget::autoMode);

    timeWidget->layout()->addWidget(mTimeCheckBox);

    layout->addWidget(timeWidget);

    // Create and add our time slider

    mTimeSlider = new QSlider(this);

    mTimeSlider->setEnabled(false);
    mTimeSlider->setOrientation(Qt::Horizontal);

    connect(mTimeSlider, &QSlider::valueChanged,
            this, &SimulationExperimentViewZincWidget::timeSliderValueChanged);

    layout->addWidget(mTimeSlider);

    // Create Zinc Context

    createAndSetZincContext();

    // Customise our timer

    connect(&mTimer, &QTimer::timeout,
            this, &SimulationExperimentViewZincWidget::timerTimeOut);

}

//==============================================================================

SimulationExperimentViewZincWidget::~SimulationExperimentViewZincWidget()
{
    // Keep track of the fact that we are shutting down

    mShuttingDown = true;

    // Delete the temporary copy of our .exfile file

    QFile::remove(mTrilinearCubeMeshFileName);

    delete mTimeValues;
}

//==============================================================================

void SimulationExperimentViewZincWidget::retranslateUi()
{
}

//==============================================================================

void SimulationExperimentViewZincWidget::loadMappingFile(QString pFileName)
{
    // save and display the new file name

    mMappingFileName = pFileName;
    mMappingFileLabel->setText(mMappingFileName.split("/").last());

    //TODO reset mMap properly, the fields especially (if possible/useful)
    if (!mMapNodeVariables->isEmpty()) {
        mMapNodeVariables->clear();
    }

    QFile file;
    file.setFileName(pFileName);

    if (!file.exists()) {
        return;
    }

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    file.close();

    //take element from json objects

    QJsonDocument jsonDocument = QJsonDocument::fromJson(val.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();
    QStringList meshFiles = jsonObject.value("meshfiles").toVariant().toStringList();

    int id;
    QString component, variable;

    OpenCMISS::Zinc::Region defaultRegion = mZincContext.getDefaultRegion();

    auto fieldModule = defaultRegion.getFieldmodule();

    fieldModule.beginChange();
        for (auto mapPointJson : jsonObject.value("map").toArray()) {
            QJsonObject mapPoint = mapPointJson.toObject();

            id = mapPoint.value("node").toInt();
            component = mapPoint.value("component").toString();
            variable = mapPoint.value("variable").toString();

            mMapNodeVariables->insert(id,{component, variable, fieldModule.createFieldFiniteElement(1)});
        }
    fieldModule.endChange();

}

//==============================================================================

void SimulationExperimentViewZincWidget::loadZincMeshFiles(const QStringList &pZincMeshFiles)
{
    // Keep track of Zinc mesh files and reset our scene viewer description

    mZincMeshFileNames = pZincMeshFiles;
    mDroppedZincMeshFiles = true;

    // Reset our Zinc widget

    mZincWidget->reset();
}

//==============================================================================

void SimulationExperimentViewZincWidget::initData(quint64 pDataSize, double pMinimumTime, double pMaximumTime,
                  double pTimeInterval, QMap<QString, double *> &pMapVariableValues)
{
    // Initialise our data
    // Note: mTimeValues must be fully populated for Zinc to work as expected.
    //       However, the list of simulation's results' points is effectively
    //       empty when coming here (see the call to this method from
    //       SimulationExperimentViewWidget::checkSimulationResults()), hence we
    //       we create and populate mTimeValues ourselves...

    mDataSize = 0;

    mTimeValues = new double[pDataSize];

    for (quint64 i = 0; i < pDataSize; ++i) {
        mTimeValues[i] = double(i)*pTimeInterval;
    }

    //link node numbers and double *values

    for (int nodeId : mMapNodeVariables->keys()) {
        _variable _variable = mMapNodeVariables->value(nodeId);
        QString variable = _variable.component+"."+_variable.variable;

        // check if values are provided for this node
        if (pMapVariableValues.contains(variable)) {
            mMapNodeValues->insert(nodeId, pMapVariableValues.value(variable));

        } else {
            //filling with 0s
            //TODO usefull ?
            double* toInsert = new double[pDataSize];
            for (quint64 t = 0; t < pDataSize; ++t) {
                toInsert[t] = 0;
            }
            mMapNodeValues->insert(nodeId, toInsert);
        }
    }

    // Initialise our Zinc scene, if needed, or reset it

    if (mNeedZincSceneInitialization) {
        mNeedZincSceneInitialization = false;

        initializeZincScene(int(pDataSize));
    } else {
        // Reset our different fields

        static const double zero = 0.0;
        auto fieldModule = mZincContext.getDefaultRegion().getFieldmodule();
        fieldModule.beginChange();
            OpenCMISS::Zinc::Timesequence timeSequence = fieldModule.getMatchingTimesequence(pDataSize, mTimeValues);
            qDebug("timeSequence %d/%d",timeSequence.isValid(),1==1);
            OpenCMISS::Zinc::Nodeset nodeSet = fieldModule.findNodesetByFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
            qDebug("nodeSet %d/%d",nodeSet.isValid(),1==1);
            mNodeTemplate = nodeSet.createNodetemplate();
            qDebug("mNodeTemplate %d/%d",mNodeTemplate.isValid(),1==1);
            qDebug("defineField %d", mNodeTemplate.defineField(mDataField));

            qDebug("setTimesequence %d", mNodeTemplate.setTimesequence(mDataField,timeSequence));

            for (int nodeId : mMapNodeValues->keys()) {
                OpenCMISS::Zinc::Node node =
                    fieldModule
                    .findNodesetByFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES)
                    .findNodeByIdentifier(nodeId);
                mFieldCache.setNode(node);
                node.merge(mNodeTemplate);

                for (quint64 i = mDataSize; i < pDataSize; ++i) {
                    mFieldCache.setTime(mTimeValues[i]);

                    mDataField.assignReal(mFieldCache, 1, &zero);
                }
            }
        fieldModule.endChange();
    }

    // Set the range of valid times in our default time keeper

    mTimeKeeper.setMinimumTime(pMinimumTime);
    mTimeKeeper.setMaximumTime(pMaximumTime);

    mTimeSlider->setMinimum(int(pMinimumTime/pTimeInterval));
    mTimeSlider->setMaximum(int(pMaximumTime/pTimeInterval));

    // Disable our time-related widgets

    mTimeLabel->setEnabled(false);
    mTimeCheckBox->setEnabled(false);
    mTimeSlider->setEnabled(false);

    mTimeCheckBox->setChecked(false);
    mTimeSlider->setValue(mTimeSlider->minimum());
}

//==============================================================================

void SimulationExperimentViewZincWidget::addData(int pDataSize)
{
    // Assign the time-varying parameters for mR0, mQ1 and mTheta

    auto fieldModule = mZincContext.getDefaultRegion().getFieldmodule();

    fieldModule.beginChange();

        OpenCMISS::Zinc::Timesequence timeSequence = fieldModule.getMatchingTimesequence(pDataSize, mTimeValues);
        qDebug("timeSequence %d/%d",timeSequence.isValid(),true);
        OpenCMISS::Zinc::Nodeset nodeSet = fieldModule.findNodesetByFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
        qDebug("nodeSet %d/%d",nodeSet.isValid(),true);
        mNodeTemplate = nodeSet.createNodetemplate();
        qDebug("mNodeTemplate %d/%d",mNodeTemplate.isValid(),true);
        qDebug("defineField %d", mNodeTemplate.defineField(mDataField));

        qDebug("setTimesequence %d", mNodeTemplate.setTimesequence(mDataField,timeSequence));

        for (int nodeId : mMapNodeValues->keys()) {
            OpenCMISS::Zinc::Node node = nodeSet.findNodeByIdentifier(nodeId);
            qDebug("addData node valid %d/%d",node.isValid(),true);
            mFieldCache.setNode(node);
            node.merge(mNodeTemplate);

            for (int i = mDataSize; i < pDataSize; ++i) {
                mFieldCache.setTime(mTimeValues[i]);

                qDebug("addData assignReal %d %f", mDataField.assignReal(mFieldCache, 1 ,mMapNodeValues->value(nodeId)+i), *(mMapNodeValues->value(nodeId)+i));
            }
        }
    fieldModule.endChange();

    mDataSize = pDataSize;

    // Enable our time-related widgets, if all the data has been added

    if (pDataSize-1 == mTimeSlider->maximum()) {
        mTimeLabel->setEnabled(true);
        mTimeCheckBox->setEnabled(true);
        mTimeSlider->setEnabled(true);

        mTimeCheckBox->setChecked(true);
    }
}

//==============================================================================

void SimulationExperimentViewZincWidget::createAndSetZincContext()
{
    // Make sure that we are not shutting down (i.e. skip the case where we are
    // coming here as a result of closing OpenCOR)

    if (mShuttingDown) {
        return;
    }

    // Keep track of our current scene viewer's description, if needed

    mZincSceneViewerDescription = mDroppedZincMeshFiles?
                                      nullptr:
                                      mZincWidget->sceneViewer().writeDescription();
    mDroppedZincMeshFiles = false;

    // Create and set our Zinc context

    mZincContext = OpenCMISS::Zinc::Context("ZincWindowWindow");

    mZincContext.getMaterialmodule().defineStandardMaterials();
    mZincContext.getGlyphmodule().defineStandardGlyphs();

    mZincWidget->setContext(mZincContext);

    // Use any cached data that we may have

    useCachedData();
}

//==============================================================================

void SimulationExperimentViewZincWidget::initializeZincScene(int pDataSize)
{
    Q_UNUSED(pDataSize);

    // Add the Zinc mesh files to our default region, or show a tri-linear cube
    // if there are no Zinc mesh files

    OpenCMISS::Zinc::Region region = mZincContext.getDefaultRegion();
    OpenCMISS::Zinc::StreaminformationRegion sir = region.createStreaminformationRegion();

    if (mZincMeshFileNames.empty()) {
        sir.createStreamresourceFile(mTrilinearCubeMeshFileName.toUtf8().constData());
    } else {
        for (const auto &zincMeshFileName : mZincMeshFileNames) {
            sir.createStreamresourceFile(zincMeshFileName.toUtf8().constData());
        }
    }

    region.read(sir);

    // Create a field magnitude for our default region

    OpenCMISS::Zinc::Fieldmodule fieldModule = region.getFieldmodule();
    OpenCMISS::Zinc::Sceneviewer sceneViewer = mZincWidget->sceneViewer();
    OpenCMISS::Zinc::Scene scene = sceneViewer.getScene();//region.getScene();

    fieldModule.beginChange();
        OpenCMISS::Zinc::Fielditerator fieldIterator = fieldModule.createFielditerator();
        OpenCMISS::Zinc::Field field = fieldIterator.next();

        while (field.isValid()) {
            if (    field.isTypeCoordinate()
                && (field.getValueType() == OpenCMISS::Zinc::Field::VALUE_TYPE_REAL)
                && (field.getNumberOfComponents() <= 3)
                &&  field.castFiniteElement().isValid()) {
                mCoordinates = field.castFiniteElement();

                break;
            }

            field = fieldIterator.next();
        }

        mMagnitude = fieldModule.createFieldMagnitude(mCoordinates);

        mMagnitude.setManaged(true);

        mDataField = fieldModule.createFieldFiniteElement(1);
        qDebug("mDataField %d/%d", mDataField.isValid(),1==1);

        OpenCMISS::Zinc::Spectrummodule spectrumModule = mZincContext.getSpectrummodule();
        OpenCMISS::Zinc::Spectrum spectrum = spectrumModule.getDefaultSpectrum();

        // setup spectrum
        spectrum.beginChange();
            qDebug("autorange %d", spectrum.autorange(sceneViewer.getScene(),sceneViewer.getScenefilter()));
            spectrum.setMaterialOverwrite(true);
        spectrum.endChange();

        OpenCMISS::Zinc::Fieldcache fieldCache = fieldModule.createFieldcache();

    fieldModule.endChange();

    // Show/hide graphics on the scene of our default region

    scene.beginChange();
        // Axes

        OpenCMISS::Zinc::GraphicsPoints axes = scene.createGraphicsPoints();
        OpenCMISS::Zinc::Materialmodule materialModule = mZincContext.getMaterialmodule();

        axes.setFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_POINT);
        axes.setMaterial(materialModule.findMaterialByName("blue"));

        mAxesFontPointSize = axes.getGraphicspointattributes().getFont().getPointSize();
        mAxesAttributes = axes.getGraphicspointattributes();

        // Points

        OpenCMISS::Zinc::GraphicsPoints points = scene.createGraphicsPoints();

        points.setCoordinateField(mCoordinates);
        points.setFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES);
        //points.setMaterial(mZincContext.getMaterialmodule().findMaterialByName("green"));

        mPointsAttributes = points.getGraphicspointattributes();
        points.setSpectrum(spectrum);
        points.setDataField(mDataField);

        // Lines

        mLines = scene.createGraphicsLines();

        mLines.setMaterial(mZincContext.getMaterialmodule().findMaterialByName("black"));


        // Surfaces

        mSurfaces = scene.createGraphicsSurfaces();

        mSurfaces.setMaterial(mZincContext.getMaterialmodule().findMaterialByName("white"));

        // Isosurfaces

        OpenCMISS::Zinc::Tessellation tessellation = mZincContext.getTessellationmodule().createTessellation();
        int intValue = 8;

        tessellation.setManaged(true);
        tessellation.setMinimumDivisions(1, &intValue);

        mIsosurfaces = scene.createGraphicsContours();

        double doubleValue = 1.0;

        mIsosurfaces.setIsoscalarField(mMagnitude);
        mIsosurfaces.setListIsovalues(1, &doubleValue);
        mIsosurfaces.setMaterial(mZincContext.getMaterialmodule().findMaterialByName("gold"));
        mIsosurfaces.setTessellation(tessellation);
    scene.endChange();

    showHideGraphics(GraphicsType::All);

    // Update our scene using our initial device pixel ratio

    devicePixelRatioChanged(mZincWidget->devicePixelRatio());

    // Make sure that the mesh is visible

    mZincWidget->viewAll();

    // Customise the size of our axes and points

    double left, right, bottom, top, nearPlane, farPlane;

    mZincWidget->sceneViewer().getViewingVolume(&left, &right, &bottom, &top, &nearPlane, &farPlane);

    doubleValue = 0.65*right;

    mAxesAttributes.setBaseSize(1, &doubleValue);

    doubleValue = 0.02*right;

    mPointsAttributes.setBaseSize(1, &doubleValue);
}

//==============================================================================

void SimulationExperimentViewZincWidget::useCachedData()
{
    // (Re)initialise our Zinc scene, if we had one before

    if (!mNeedZincSceneInitialization) {
        initializeZincScene(mDataSize);
    }


    // Use the cached time-varying parameters for mR0, mQ1 and mTheta, if any

    auto fieldModule = mZincContext.getDefaultRegion().getFieldmodule();
    fieldModule.beginChange();
        for (int i = 0; i < mDataSize; ++i) {
            mFieldCache.setTime(mTimeValues[i]);

            for (int nodeId : mMapNodeValues->keys()) {
                OpenCMISS::Zinc::Node node =
                        fieldModule
                        .findNodesetByFieldDomainType(OpenCMISS::Zinc::Field::DOMAIN_TYPE_NODES)
                        .findNodeByIdentifier(nodeId);

                mFieldCache.setNode(node);
                qDebug("useCacheData assignReal %d", mDataField.assignReal(mFieldCache, 1 ,mMapNodeValues->value(nodeId)+i));
            }
        }
    fieldModule.endChange();
}

//==============================================================================

void SimulationExperimentViewZincWidget::graphicsInitialized()
{
    // Set our 'new' scene viewer's description

    mZincWidget->sceneViewer().readDescription(mZincSceneViewerDescription);
}

//==============================================================================

void SimulationExperimentViewZincWidget::devicePixelRatioChanged(int pDevicePixelRatio)
{
    // Update our scene using the given device pixel ratio

    OpenCMISS::Zinc::Scene scene = mZincContext.getDefaultRegion().getScene();

    scene.beginChange();
        scene.createGraphicsPoints().getGraphicspointattributes().getFont().setPointSize(pDevicePixelRatio*mAxesFontPointSize);
    scene.endChange();
}

//==============================================================================

void SimulationExperimentViewZincWidget::timeSliderValueChanged(int pTime)
{
    // Update our scene

    double time = 0.01*pTime;

    mTimeLabel->setText(tr("Time: %1 s").arg(time));

    mTimeKeeper.setTime(time);
}


//==============================================================================

void SimulationExperimentViewZincWidget::timerTimeOut()
{
    // Update our scene

    int value = mTimeSlider->value();

    if (value == mTimeSlider->maximum()) {
        value = 0;
    } else {
        ++value;
    }

    mTimeSlider->setValue(value);
}

//==============================================================================

void SimulationExperimentViewZincWidget::autoMode()
{
    // Enable/disable our timer

    if (mTimeCheckBox->isChecked()) {
        mTimer.start();
    } else {
        mTimer.stop();
    }
}

//==============================================================================

void SimulationExperimentViewZincWidget::showHideGraphics(GraphicsType pGraphicsType)
{
    // Show/hide graphics on the scene of our default region

    OpenCMISS::Zinc::Region region = mZincContext.getDefaultRegion();
    OpenCMISS::Zinc::Scene scene = region.getScene();

    scene.beginChange();
        // Axes
Q_UNUSED(pGraphicsType)

        if (   (pGraphicsType == GraphicsType::All)
            || (pGraphicsType == GraphicsType::Axes)) {
            mAxesAttributes.setGlyphShapeType(mActionAxes->isChecked()?
                                                  OpenCMISS::Zinc::Glyph::SHAPE_TYPE_AXES_XYZ:
                                                  OpenCMISS::Zinc::Glyph::SHAPE_TYPE_NONE);
        }

        // Points

        if (   (pGraphicsType == GraphicsType::All)
            || (pGraphicsType == GraphicsType::Points)) {
            mPointsAttributes.setGlyphShapeType(mActionPoints->isChecked()?
                                                    OpenCMISS::Zinc::Glyph::SHAPE_TYPE_SPHERE:
                                                    OpenCMISS::Zinc::Glyph::SHAPE_TYPE_NONE);
        }

        // Lines

        if (   (pGraphicsType == GraphicsType::All)
            || (pGraphicsType == GraphicsType::Lines)) {
            mLines.setCoordinateField(mActionLines->isChecked()?
                                          mCoordinates:
                                          OpenCMISS::Zinc::Field());
        }

        // Surfaces

        if (   (pGraphicsType == GraphicsType::All)
            || (pGraphicsType == GraphicsType::Surfaces)) {
            mSurfaces.setCoordinateField(mActionSurfaces->isChecked()?
                                             mCoordinates:
                                             OpenCMISS::Zinc::Field());
        }

        // Isosurfaces

        if (   (pGraphicsType == GraphicsType::All)
            || (pGraphicsType == GraphicsType::Isosurfaces)) {
            mIsosurfaces.setCoordinateField(mActionIsosurfaces->isChecked()?
                                                mCoordinates:
                                                OpenCMISS::Zinc::Field());
        }
    scene.endChange();
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionAxesTriggered()
{
    // Show/hide our axes

    showHideGraphics(GraphicsType::Axes);
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionPointsTriggered()
{
    // Show/hide our points

    showHideGraphics(GraphicsType::Points);
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionLinesTriggered()
{
    // Show/hide our lines

    showHideGraphics(GraphicsType::Lines);
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionSurfacesTriggered()
{
    // Show/hide our surfaces

    showHideGraphics(GraphicsType::Surfaces);
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionIsosurfacesTriggered()
{
    // Show/hide our isosurfaces

    showHideGraphics(GraphicsType::Isosurfaces);
}

//==============================================================================

void SimulationExperimentViewZincWidget::actionTrilinearCubeTriggered()
{
    // Load the trilinear cube mesh

    loadZincMeshFiles(QStringList() << mTrilinearCubeMeshFileName);
}

//==============================================================================

} // namespace ZincWindow
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================