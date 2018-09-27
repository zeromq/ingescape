/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#ifndef QQUICKWINDOWBLOCKTOUCHES_H
#define QQUICKWINDOWBLOCKTOUCHES_H

#include <QObject>
#include <QtQml>
#include <qquickwindow.h>


/**
 * @brief The QQuickWindowBlockTouches class
 */
class QQuickWindowBlockTouches : public QQuickWindow
{
    Q_OBJECT


public:
    explicit QQuickWindowBlockTouches(QWindow *parent = nullptr);


    /**
     * @brief This overriden function receives events to an object and should return true if the event "evt" was recognized and processed.
     * @param evt
     * @return
     */
    bool event(QEvent* evt) Q_DECL_OVERRIDE;


Q_SIGNALS:

    /**
     * @brief Same signal "closing" but allow to workaround "onClosing is not available in version"
     */
    void closingVersion();


};

QML_DECLARE_TYPE(QQuickWindowBlockTouches)

#endif // QQUICKWINDOWBLOCKTOUCHES_H
