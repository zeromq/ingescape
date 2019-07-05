/*
 *	IngeScape Expe
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

#ifndef EXPEMODELMANAGER_H
#define EXPEMODELMANAGER_H

#include <QObject>
#include <I2PropertyHelpers.h>
//#include <model/expeenums.h>
#include <controller/ingescapemodelmanager.h>


/**
 * @brief The ExpeModelManager class defines the manager for the data model of our IngeScape Expe application
 */
class ExpeModelManager : public IngeScapeModelManager
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    explicit ExpeModelManager(JsonHelper* jsonHelper,
                              QString rootDirectoryPath,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExpeModelManager();
    //~ExpeModelManager() Q_DECL_OVERRIDE;


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(ExpeModelManager)

#endif // EXPEMODELMANAGER_H
