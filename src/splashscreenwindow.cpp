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

#include "cliutils.h"
#include "guiutils.h"
#include "splashscreenwindow.h"

//==============================================================================

#include "ui_splashscreenwindow.h"

//==============================================================================

#include <QCloseEvent>
#include <QElapsedTimer>
#include <QScreen>
#include <QSvgWidget>
#include <QTimer>
#include <QWindow>

//==============================================================================

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#else
    #include <ctime>
#endif

//==============================================================================

namespace OpenCOR {

//==============================================================================

SplashScreenWindow::SplashScreenWindow() :
    QWidget(nullptr, Qt::SplashScreen|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint),
    mGui(new Ui::SplashScreenWindow)
{
    // Set up the GUI

    mGui->setupUi(this);

    mSvgWidget = new QSvgWidget();

    mSvgWidget->setFixedSize(413, 351);

    mSvgWidget->load(QString(":/app_logo"));

    mGui->layout->insertWidget(0, mSvgWidget);

    QFont newFont = mGui->infoWidget->font();

#if defined(Q_OS_WIN)
    newFont.setPointSize(9);
#elif defined(Q_OS_LINUX)
    newFont.setPointSize(8);
#elif defined(Q_OS_MAC)
    newFont.setPointSize(11);
#endif

    mGui->copyrightValue->setFont(newFont);
    mGui->versionValue->setFont(newFont);

    mGui->copyrightValue->setText(copyright());
    mGui->versionValue->setText(shortVersion());

    // Initialise our "palette"

    paletteChanged();

    // Adjust the size of our splash screen and then move it to the center of
    // our screen

    adjustSize();
    move(qApp->primaryScreen()->geometry().center()-rect().center());
}

//==============================================================================

SplashScreenWindow::~SplashScreenWindow()
{
    // Delete the GUI

    delete mGui;
}

//==============================================================================

void SplashScreenWindow::changeEvent(QEvent *pEvent)
{
    // Default handling of the event

    QWidget::changeEvent(pEvent);

    // Do a few more things for some changes

    if (pEvent->type() == QEvent::PaletteChange) {
        paletteChanged();
    }
}

//==============================================================================

void SplashScreenWindow::closeAndDeleteAfter(QWidget *pWindow)
{
    // Wait for our window to expose itself

    enum {
        TimeOut = 1000,
        ShortDelay = 10
    };

    QWindow *window = pWindow->windowHandle();
    QElapsedTimer timer;
#ifndef Q_OS_WIN
    struct timespec shortDelaySpec = { ShortDelay/1000, 1000000*(ShortDelay%1000) };
#endif

    timer.start();

    while (!window->isExposed()) {
        int remaining = int(TimeOut-timer.elapsed());

        if (remaining <= 0) {
            break;
        }

        QCoreApplication::processEvents(QEventLoop::AllEvents, remaining);
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);

#ifdef Q_OS_WIN
        Sleep(ShortDelay);
#else
        nanosleep(&shortDelaySpec, nullptr);
#endif
    }

    // Close ourselves with a bit of a delay

    QTimer::singleShot(500, this, &SplashScreenWindow::close);
}

//==============================================================================

void SplashScreenWindow::closeEvent(QCloseEvent *pEvent)
{
    // Accept the event and ask for ourselves to be deleted later

    pEvent->accept();

    deleteLater();
}

//==============================================================================

void SplashScreenWindow::mousePressEvent(QMouseEvent *pEvent)
{
    // Accept the event and hide ourselves

    pEvent->accept();

    hide();
}

//==============================================================================

void SplashScreenWindow::paletteChanged()
{
    // Our palette has changed, so update our style sheet

    static const QString StyleSheet = "QSvgWidget {"
                                      "    background-color: %1;"
                                      "}"
#ifdef Q_OS_MAC
                                      "QWidget#infoWidget {"
                                      "    border-top: %2;"
                                      "}"
#else
                                      "QSvgWidget {"
                                      "    border-top: %2;"
                                      "    border-left: %2;"
                                      "    border-right: %2;"
                                      "}"
                                      ""
                                      "QWidget#infoWidget {"
                                      "    border: %2;"
                                      "}"
#endif
                                      ;
    static const QString BorderStyle = "1px solid %1";

    setStyleSheet(StyleSheet.arg(baseColor().name())
                            .arg(BorderStyle.arg(borderColor().name())));
}

//==============================================================================

} // namespace OpenCOR

//==============================================================================
// End of file
//==============================================================================
