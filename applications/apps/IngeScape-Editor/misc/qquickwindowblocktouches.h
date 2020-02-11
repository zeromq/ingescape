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
    /**
     * @brief Constructor
     * @param parent
     */
    explicit QQuickWindowBlockTouches(QWindow *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~QQuickWindowBlockTouches() Q_DECL_OVERRIDE;


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


    /**
     * @brief Signal emitted when our window has lost the focus
     */
    void focusLost();


private Q_SLOTS:

    /**
     * @brief Slot called when the property "Active Focus Item" changed
     */
    void _onActiveFocusItemChanged();


};

QML_DECLARE_TYPE(QQuickWindowBlockTouches)

#endif // QQUICKWINDOWBLOCKTOUCHES_H
