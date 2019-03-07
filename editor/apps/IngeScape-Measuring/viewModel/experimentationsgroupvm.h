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

#ifndef EXPERIMENTATIONSGROUPVM_H
#define EXPERIMENTATIONSGROUPVM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/experimentationm.h>


/**
 * @brief The ExperimentationsGroupVM class defines the view model of a group of experimentations
 */
class ExperimentationsGroupVM : public QObject
{
    Q_OBJECT

    // Name of our experimentations group
    I2_QML_PROPERTY(QString, name)

    // List of experimentations
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ExperimentationM, experimentations)


public:
    /**
     * @brief Constructor
     * @param name
     * @param parent
     */
    explicit ExperimentationsGroupVM(QString name, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationsGroupVM();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(ExperimentationsGroupVM)

#endif // EXPERIMENTATIONSGROUPVM_H
