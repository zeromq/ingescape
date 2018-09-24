#include "qquickwindowblocktouches.h"

QQuickWindowBlockTouches::QQuickWindowBlockTouches(QWindow *parent) : QQuickWindow(parent)
{
    connect(this, SIGNAL(closing(QQuickCloseEvent*)), this, SIGNAL(closingVersion()));
}


/**
 * @brief This overriden function receives events to an object and should return true if the event "evt" was recognized and processed.
 * @param evt
 * @return
 */
bool QQuickWindowBlockTouches::event(QEvent* evt)
{
    // Block Touches
    if ( (evt->type() == QEvent::TouchUpdate) || (evt->type() == QEvent::TouchBegin)
         || (evt->type() == QEvent::TouchEnd) || (evt->type() == QEvent::TouchCancel) )
    {
        //qDebug() << "TOUCH" << evt->type() << QTime::currentTime().toString("hh:mm:ss.zzz");
        return true;
    }
    else
    {
        /*if ((evt->type() == QEvent::MouseButtonPress) || (evt->type() == QEvent::MouseButtonRelease))
        {
            qDebug() << "MOUSE" << evt->type() << QTime::currentTime().toString("hh:mm:ss.zzz");
        }
        else if ((evt->type() != QEvent::MouseMove) && (evt->type() != QEvent::UpdateRequest) && (evt->type() != QEvent::Timer)) {
            qDebug() << "evt" << evt->type() << QTime::currentTime().toString("hh:mm:ss.zzz");
        }*/

        return QQuickWindow::event(evt);
    }
}

