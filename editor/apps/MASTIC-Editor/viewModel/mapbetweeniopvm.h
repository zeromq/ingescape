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
#include <viewModel/pointmapvm.h>

/**
 * @brief The MapBetweenIOPVM class defines the view model of a link between two agents
 */
class MapBetweenIOPVM : public QObject
{
    Q_OBJECT


    // View model of the output slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(PointMapVM*, pointFrom)

    // View model of the input slot  associated to our link
    I2_QML_PROPERTY_DELETE_PROOF(PointMapVM*, pointTo)


public:
    /**
     * @brief Default constructor
     * @param pointFrom The point map which the map will start
     * @param pointTo The point map which the map will stop
     * @param parent
     */
    explicit MapBetweenIOPVM(PointMapVM *pointFrom,
                             PointMapVM *pointTo,
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
    explicit MapBetweenIOPVM(PointMapVM *pointFrom,
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
