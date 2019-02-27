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

#ifndef SUBJECTM_H
#define SUBJECTM_H

#include <QObject>
#include <I2PropertyHelpers.h>


/**
 * @brief The SubjectM class defines a model of subject
 */
class SubjectM : public QObject
{
    Q_OBJECT

    // Name of our subject
    I2_QML_PROPERTY(QString, name)

    // First name of our subject
    //I2_QML_PROPERTY(QString, firstName)

    // Last name of our subject
    //I2_QML_PROPERTY(QString, lastName)

    // Values of the characteristics of our subject
    // Hash table from a (unique) id of characteristic to the characteristic value
    //I2_QOBJECT_HASHMODEL(QVariant, mapFromCharacteristicIdToValue)

    //I2_QML_PROPERTY(QHash<QString, QVariant>, mapFromCharacteristicIdToValue)

    // FIXME for tests
    //I2_QML_PROPERTY(QList<QObject*>, temp)
    //I2_QML_PROPERTY(QHash<QString, QObject*>, temp)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit SubjectM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~SubjectM();


Q_SIGNALS:


public Q_SLOTS:


private:

    // FIXME accessible for QML
    // Values of the characteristics of our subject
    // Hash table from a (unique) id of characteristic to the characteristic value
    QHash<QString, QVariant> _mapFromCharacteristicIdToValue;

};

QML_DECLARE_TYPE(SubjectM)

#endif // SUBJECTM_H
