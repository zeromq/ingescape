/*
 *	IngeScape Measuring
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef INGESCAPEMODELMANAGER_H
#define INGESCAPEMODELMANAGER_H

#include <QObject>

#include <I2PropertyHelpers.h>


/**
 * @brief The IngeScapeModelManager class defines the manager for the data model of our IngeScape measuring application
 */
class IngeScapeModelManager : public QObject
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeModelManager();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(IngeScapeModelManager)

#endif // INGESCAPEMODELMANAGER_H
