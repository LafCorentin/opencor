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
// CellML-Zinc Mapping view widget
//==============================================================================

#pragma once

//==============================================================================

#include "corecliutils.h"
#include "cellmlzincmappingvieweditingwidget.h"
#include "viewwidget.h"

//==============================================================================

#include <QMap>

//==============================================================================

namespace OpenCOR {

//==============================================================================

namespace ZincWidget {
    class ZincWidget;
}   // namespace ZincWidget

//==============================================================================

namespace CellMLZincMappingView {

//==============================================================================

class CellMLZincMappingViewWidget : public Core::ViewWidget
{
    Q_OBJECT

public:
    explicit CellMLZincMappingViewWidget(QWidget *pParent);
    ~CellMLZincMappingViewWidget() override;

    void retranslateUi() override;

    void loadSettings(QSettings &pSettings) override;
    void saveSettings(QSettings &pSettings) const override;

    void initialize(const QString &pFileName);
    void finalize(const QString &pFileName);

    CellMLZincMappingViewEditingWidget * editingWidget(const QString &pFileName) const;

    QWidget * widget(const QString &pFileName) override;

    void filePermissionsChanged(const QString &pFileName);
    void fileSaved(const QString &pFileName);
    void fileReloaded(const QString &pFileName);
    void fileRenamed(const QString &pOldFileName, const QString &pNewFileName);

    //bool saveFile(const QString &pOldFileName, const QString &pNewFileName);

    void setDefaultMeshFiles(const QStringList &pFileNames);

private:
    QIntList mEditingWidgetHorizontalSizes;
    QIntList mEditingWidgetVerticalSizes;

    CellMLZincMappingViewEditingWidget* mEditingWidget = nullptr;
    QMap<QString, CellMLZincMappingViewEditingWidget*> mEditingWidgets;

    QStringList mZincMeshFileNames;

private slots:
    void EditingWidgetHorizontalSplitterMoved(const QIntList &pSizes);
    void EditingWidgetVerticalSplitterMoved(const QIntList &pSizes);
};

//==============================================================================

} // namespace MappingView
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
