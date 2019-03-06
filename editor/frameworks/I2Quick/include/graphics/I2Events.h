/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2EVENTS_H_
#define _I2EVENTS_H_

#include <QQuickItem>


// To export symbols
#include "i2quick_global.h"

#include "I2PropertyHelpers.h"



/**
 * @brief The I2MouseEvent class defines a wrapper for mouse events. It is used
 *  as a parameter in mouse input signals
 */
class I2QUICK_EXPORT I2MouseEvent : public QObject
{
    Q_OBJECT

    /**
      * The abscissa of the position supplied by the mouse
      */
    I2_QML_PROPERTY_FUZZY_COMPARE(qreal, x)

    /**
      * The ordinate of the position supplied by the mouse
      */
    I2_QML_PROPERTY_FUZZY_COMPARE(qreal, y)

    /**
     * This property holds the button that caused the event
     * (Qt.LeftButton, Qt.RightButton, Qt.MiddleButton, Qt.NoButton)
     */
    I2_QML_PROPERTY (Qt::MouseButton, button)

    /**
     * This property holds all buttons that are pressed down when the event is generated
     */
    I2_QML_PROPERTY (Qt::MouseButtons, buttons)

public:
    /**
     * @brief Constructor
     * @param x
     * @param y
     * @param button
     * @param buttons
     */
    explicit I2MouseEvent(qreal x, qreal y, Qt::MouseButton button, Qt::MouseButtons buttons);

};


QML_DECLARE_TYPE(I2MouseEvent)

#endif // _I2EVENTS_H_
