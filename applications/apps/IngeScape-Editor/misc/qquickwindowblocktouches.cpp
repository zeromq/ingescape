#include "qquickwindowblocktouches.h"

/**
 * @brief Constructor
 * @param parent
 */
QQuickWindowBlockTouches::QQuickWindowBlockTouches(QWindow *parent) : QQuickWindow(parent)
{
    connect(this, SIGNAL(closing(QQuickCloseEvent*)), this, SIGNAL(closingVersion()));
    connect(this, &QQuickWindow::activeFocusItemChanged, this, &QQuickWindowBlockTouches::_onActiveFocusItemChanged);
}


/**
 * @brief Destructor
 */
QQuickWindowBlockTouches::~QQuickWindowBlockTouches()
{
    disconnect(this, SIGNAL(closing(QQuickCloseEvent*)), this, SIGNAL(closingVersion()));
    disconnect(this, &QQuickWindow::activeFocusItemChanged, this, &QQuickWindowBlockTouches::_onActiveFocusItemChanged);

    // Mother class is automatically called
    //QQuickWindow::~QQuickWindow();
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


/**
 * @brief Slot called when the property "Active Focus Item" changed
 */
void QQuickWindowBlockTouches::_onActiveFocusItemChanged()
{
    if (activeFocusItem() == nullptr)
    {
        //qDebug() << "NONE item of our window has the focus";
        Q_EMIT focusLost();
    }
    /*else
    {
        qDebug() << "An item of our window has the focus";
    }*/
}

