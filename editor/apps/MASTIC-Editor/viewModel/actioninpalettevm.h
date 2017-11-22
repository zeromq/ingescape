
/*
 *	ActionInPaletteVM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#ifndef ACTIONINPALETTEVM_H
#define ACTIONINPALETTEVM_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "model/scenario/actionm.h"

/**
  * Action in palette state type: ENABLE, DISABLE
  */
I2_ENUM(ActionInPaletteState, ENABLE, DISABLE)


/**
 * @brief The ActionInPaletteVM class defines an action in palette view model
 */
class ActionInPaletteVM: public QObject
{
    Q_OBJECT

    // Action model
    I2_QML_PROPERTY_CUSTOM_SETTER(ActionM*, actionModel)

    // State
    I2_QML_PROPERTY(int, indexInPanel)

    // State
    I2_QML_PROPERTY(ActionInPaletteState::Value, status)



public:

    /**
     * @brief Default constructor
     * @param action model
     * @param parent
     */
    explicit ActionInPaletteVM(ActionM *actionM, int indexInPanel, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ActionInPaletteVM();



Q_SIGNALS:


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(ActionInPaletteVM)

#endif // ACTIONINPALETTEVM_H
