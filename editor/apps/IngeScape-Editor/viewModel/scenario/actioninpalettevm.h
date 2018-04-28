/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef ACTIONINPALETTEVM_H
#define ACTIONINPALETTEVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/scenario/actionm.h>


/**
 * @brief The ActionInPaletteVM class defines a view model of action in palette
 */
class ActionInPaletteVM: public QObject
{
    Q_OBJECT

    // Model of our view model of action in palette
    I2_QML_PROPERTY(ActionM*, modelM)

    // Index in the panel "palette"
    I2_QML_PROPERTY(int, indexInPanel)


public:

    /**
     * @brief Constructor
     * @param actionM
     * @param indexInPanel
     * @param parent
     */
    explicit ActionInPaletteVM(ActionM* actionM, int indexInPanel, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionInPaletteVM();

};

QML_DECLARE_TYPE(ActionInPaletteVM)

#endif // ACTIONINPALETTEVM_H
