/*******************************************************************************

Copyright (C) The University of Auckland

OpenCOR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenCOR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

//==============================================================================
// Splash screen window
//==============================================================================

#pragma once

//==============================================================================

#include <QColor>
#include <QWidget>

//==============================================================================

class QSvgWidget;

//==============================================================================

namespace Ui {
    class SplashScreenWindow;
}   // namespace Ui

//==============================================================================

namespace OpenCOR {

//==============================================================================

class SplashScreenWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreenWindow();
    ~SplashScreenWindow() override;

    void closeAndDeleteAfter(QWidget *pWindow);

protected:
    void changeEvent(QEvent *pEvent) override;
    void closeEvent(QCloseEvent *pEvent) override;
    void mousePressEvent(QMouseEvent *pEvent) override;

private:
    Ui::SplashScreenWindow *mGui;

    QSvgWidget *mSvgWidget;

    void paletteChanged();
};

//==============================================================================

}   // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
