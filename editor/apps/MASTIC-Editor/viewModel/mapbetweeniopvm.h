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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef MAPBETWEENIOPVM_H
#define MAPBETWEENIOPVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <viewModel/agentinmappingvm.h>
//#include <viewModel/pointmapvm.h>
//#include <viewModel/iop/inputvm.h>
//#include <viewModel/iop/outputvm.h>


/**
 * @brief The MapBetweenIOPVM class defines the view model of a link between two agents
 */
class MapBetweenIOPVM : public QObject
{
    Q_OBJECT

    // View model of the output agent of our link (link starts from this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, agentFrom)

    // View model of the output slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(OutputVM*, pointFrom)

    // View model of the input agent of our link (link ends to this agent)
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, agentTo)

    // View model of the input slot associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(InputVM*, pointTo)

    // Flag indicating if a new value is published on the output
    I2_QML_PROPERTY_READONLY(bool, isNewValueOnOutput)

public:
    /**
     * @brief Default constructor
     * @param pointFrom The point map which the map will start
     * @param pointTo The point map which the map will stop
     * @param parent
     */
    explicit MapBetweenIOPVM(OutputVM *pointFrom,
                             InputVM *pointTo,
                             QObject *parent = nullptr);

   /**
      *TODOESTIA : Implémenter un constructeur avec juste un 'pointFrom' pour gérer le cas de la construction dans la vue
      * (Clic sur le point de départ et tirer le fil jusqu'au point d'arrivée)
      *
      */

    /**
     * @brief Constructor which only the point map 'from'. Usecase when the user create a map dynamically by mouse clicking on the start point map.
     * @param pointFrom The point map which the map will start
     * @param parent
     */
    /*explicit MapBetweenIOPVM(PointMapVM *pointFrom,
                             QObject *parent = nullptr);*/


    /**
     * @brief Default constructor
     * @param agentFrom The link starts from this agent
     * @param pointFrom The link starts from this output of the agentFrom
     * @param agentTo The link ends to this agent
     * @param pointTo The link ends to this input of the agentTo
     * @param parent
     */
    explicit MapBetweenIOPVM(AgentInMappingVM* agentFrom,
                             OutputVM *pointFrom,
                             AgentInMappingVM* agentTo,
                             InputVM *pointTo,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MapBetweenIOPVM();

Q_SIGNALS:

public Q_SLOTS:

};

QML_DECLARE_TYPE(MapBetweenIOPVM)

#endif // MAPBETWEENIOPVM_H
