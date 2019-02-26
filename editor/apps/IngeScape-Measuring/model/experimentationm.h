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

#ifndef EXPERIMENTATIONM_H
#define EXPERIMENTATIONM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The ExperimentationM class defines a model of experimentation
 */
class ExperimentationM : public QObject
{
    Q_OBJECT

    // Name of our experimentation
    I2_QML_PROPERTY(QString, name)

    //Records (): liste des enregistrements


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ExperimentationM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(ExperimentationM)

#endif // EXPERIMENTATIONM_H
