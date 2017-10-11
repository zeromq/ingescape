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
 *
 */

#ifndef AGENTMAPPINGM_H
#define AGENTMAPPINGM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

//include the mapping element defined
#include <model/mapping/elementmappingm.h>

/**
 * @brief The AgentMappingM class defines a model of agent mapping as is descripted in a json in mastic API
 */
class AgentMappingM : public QObject
{
    Q_OBJECT

    // Name of our agent mapping
    I2_QML_PROPERTY(QString, name)

    // Version of our agent mapping
    I2_QML_PROPERTY(QString, version)

    // Description of our agent mapping description
    I2_QML_PROPERTY(QString, description)

    // List of element mapping of our agent mapping
    I2_QOBJECT_LISTMODEL(ElementMappingM, elementMappingsList)

    // Md5 hash value for the agent mapping string
    I2_QML_PROPERTY_READONLY(QString, md5Hash)

public:
        /**
         * @brief Constructor
         * @param name
         * @param version
         * @param description
         * @param parent
         */
    explicit AgentMappingM(QString name, QString version, QString description, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentMappingM();


Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentMappingM)

#endif // AGENTMAPPINGM_H
