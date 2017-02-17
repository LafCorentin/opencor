/*******************************************************************************

Copyright The University of Auckland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

//==============================================================================
// PMR Workspaces window new workspace dialog
//==============================================================================

#pragma once

//==============================================================================

#include "coreguiutils.h"

//==============================================================================

namespace Ui {
    class PmrWorkspacesWindowNewWorkspaceDialog;
}

//==============================================================================

namespace OpenCOR {
namespace PMRWorkspacesWindow {

//==============================================================================

class PmrWorkspacesWindowNewWorkspaceDialog : public Core::Dialog
{
    Q_OBJECT

public:
    explicit PmrWorkspacesWindowNewWorkspaceDialog(QSettings *pSettings,
                                                   QWidget *pParent);
    ~PmrWorkspacesWindowNewWorkspaceDialog();

    virtual void retranslateUi();

    QString title() const;
    QString description() const;
    QString path() const;

private:
    Ui::PmrWorkspacesWindowNewWorkspaceDialog *mGui;

private slots:
    void updateOkButton();

    void choosePath();
};

//==============================================================================

}   // namespace PMRWorkspacesWindow
}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
