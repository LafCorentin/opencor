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
// Widget
//==============================================================================

#pragma once

//==============================================================================

#include "commonwidget.h"
#include "coreglobal.h"

//==============================================================================

#include <QWidget>

//==============================================================================

namespace OpenCOR {
namespace Core {

//==============================================================================

class CORE_EXPORT Widget : public QWidget, public CommonWidget
{
    Q_OBJECT

public:
    enum class Layout {
        Vertical,
        Horizontal,
        Form,
        Grid,
        Stacked
    };

    explicit Widget(const QSize &pSizeHint, QWidget *pParent);
    explicit Widget(QWidget *pParent);

    QLayout * createLayout(Layout pLayoutType = Layout::Vertical);

protected:
    void resizeEvent(QResizeEvent *pEvent) override;

    QSize sizeHint() const override;

private:
    QSize mSizeHint;
};

//==============================================================================

} // namespace Core
} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
