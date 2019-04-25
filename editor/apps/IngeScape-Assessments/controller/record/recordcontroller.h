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

#ifndef RECORDCONTROLLER_H
#define RECORDCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/experimentationrecordm.h>


/**
 * @brief The RecordController class defines the controller to manage a record of the current experimentation
 */
class RecordController : public QObject
{
    Q_OBJECT

    // The (experimentation) record currently selected
    I2_QML_PROPERTY(ExperimentationRecordM*, currentRecord)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit RecordController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordController();


Q_SIGNALS:


public Q_SLOTS:


private:

};

QML_DECLARE_TYPE(RecordController)

#endif // RECORDCONTROLLER_H
