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

#ifndef MAPPINGELEMENTM_H
#define MAPPINGELEMENTM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

/**
 * @brief The MappingElementM class defines a model of a mapping node as described in the JSON of the agent mapping.
 */
class MappingElementM : public QObject
{  
    Q_OBJECT

    // Link name with names of each part (format: "outputAgent##output-->inputAgent##input")
    // FIXME TODO rename "name" --> "linkName")
    I2_CPP_NOSIGNAL_PROPERTY(QString, name)

    // Input agent name
    I2_QML_PROPERTY_READONLY(QString, inputAgent)

    // Input name of input agent
    I2_QML_PROPERTY_READONLY(QString, input)

    // Output agent name
    I2_QML_PROPERTY_READONLY(QString, outputAgent)

    // Output name of output agent
    I2_QML_PROPERTY_READONLY(QString, output)


public:

    /**
     * @brief Constructor with input and output agent and IOP
     * @param inputAgent
     * @param input
     * @param outputAgent
     * @param output
     * @param parent
     */
    explicit MappingElementM(QString inputAgent,
                             QString input,
                             QString outputAgent,
                             QString output,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MappingElementM();


    /**
     * @brief Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
     * @param outputAgent
     * @param output
     * @param inputAgent
     * @param input
     * @return link name with format "outputAgent##output-->inputAgent##input"
     */
    static QString getLinkNameFromNamesList(QString outputAgent, QString output, QString inputAgent, QString input);


    /**
     * @brief Get the list of names (each parts of a mapping element) from the link name (with format "outputAgent##output-->inputAgent##input")
     * @param link name with format "outputAgent##output-->inputAgent##input"
     * @return
     */
    static QStringList getNamesListFromLinkName(QString linkName);

};

QML_DECLARE_TYPE(MappingElementM)

#endif // MAPPINGELEMENTM_H
