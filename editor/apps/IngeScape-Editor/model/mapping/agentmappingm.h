/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
 * @brief The AgentMappingM class defines a model of agent mapping as is descripted in a json in ingescape API
 */
class AgentMappingM : public QObject
{
    Q_OBJECT

    // Name of our agent mapping
    I2_QML_PROPERTY(QString, name)

    // Version of our agent mapping
    I2_QML_PROPERTY(QString, version)

    // Description of our agent mapping
    I2_QML_PROPERTY(QString, description)

    // List of (mapping) elements of our agent mapping
    I2_QOBJECT_LISTMODEL(ElementMappingM, mappingElements)

    // List of the names of mapping elements
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, namesOfMappingElements)


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


private Q_SLOTS:

    /**
     * @brief Slot called when the list of mapping elements changed
     */
    void _onMappingElementsListChanged();


private:

    // Previous list of mapping elements
    //QList<ElementMappingM*> _previousListOfMappingElements;

    // Map from a name to a model of mapping element
    //QHash<QString, ElementMappingM*> _mapFromNameToMappingElement;

};

QML_DECLARE_TYPE(AgentMappingM)

#endif // AGENTMAPPINGM_H
