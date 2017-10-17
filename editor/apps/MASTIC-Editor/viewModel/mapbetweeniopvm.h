/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef MAPBETWEENIOPVM_H
#define MAPBETWEENIOPVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/agentmappingm.h>

#include <viewModel/iop/agentiopvm.h>



/**
 * @brief The MapBetweenIOPVM class defines the view model of a link between two agents
 */
class MapBetweenIOPVM : public QObject
{
    Q_OBJECT


    // View model of the output slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(AgentIOPVM*, pointFrom)

    // View model of the input slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(AgentIOPVM*, pointTo)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MapBetweenIOPVM(AgentIOPVM *pointFrom, AgentIOPVM *pointTo, QObject *parent = nullptr);

    /**
      *TODOESTIA : Implémenter un constructeur avec juste un 'pointFrom' pour gérer le cas de la construction dans la vue
      * (Clic sur le point de départ et tirer le fil jusqu'au point d'arrivée)
      *
      */


    /**
     * @brief Destructor
     */
    ~MapBetweenIOPVM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(MapBetweenIOPVM)

#endif // MAPBETWEENIOPVM_H
