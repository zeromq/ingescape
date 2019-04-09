/*
 *	IngeScape Assessments
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

#ifndef RECORDM_H
#define RECORDM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/subject/subjectm.h>
#include <model/task/taskm.h>
#include <model/assessmentsenums.h>


/**
 * @brief The RecordM class defines a model of record
 */
class RecordM : public QObject
{
    Q_OBJECT

    // Name of our record
    I2_QML_PROPERTY(QString, name)

    // Subject of our record
    I2_QML_PROPERTY(SubjectM*, subject)

    // Task of our record
    I2_QML_PROPERTY(TaskM*, task)

    // Start date and time of our record
    I2_QML_PROPERTY(QDateTime, startDateTime)

    // End date and time of our record
    I2_QML_PROPERTY_CUSTOM_SETTER(QDateTime, endDateTime)

    // Duration of our record
    //I2_QML_PROPERTY_QTime(duration)
    I2_QML_PROPERTY(QDateTime, duration)

    // IndependentVariableValues (Table de Hash [UID V.I. - Valeur de la V.I.])
    // liste des valeurs des V.I. de la tâche

    // DependentVariableValues (TODO ?): Les valeurs des VD (sorties d’agents)
    // sont stockées avec le temps correspondant au changement de la valeur d’une sortie


public:

    /**
     * @brief Constructor
     * @param parent
     */
    //explicit RecordM(QObject *parent = nullptr);


    /**
     * @brief Constructor
     * @param name
     * @param subject
     * @param task
     * @param parent
     */
    explicit RecordM(QString name,
                     SubjectM* subject = nullptr,
                     TaskM* task = nullptr,
                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(RecordM)

#endif // RECORDM_H
