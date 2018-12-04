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

#ifndef ELEMENTMAPPINGM_H
#define ELEMENTMAPPINGM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

/**
 * @brief The ElementMappingM class defines a model of a mapping node as described in the JSON of the agent mapping.
 */
class ElementMappingM : public QObject
{  
    Q_OBJECT

    // Name with all names formatted: "outputAgent##output-->inputAgent##input"
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
    explicit ElementMappingM(QString inputAgent,
                             QString input,
                             QString outputAgent,
                             QString output,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ElementMappingM();


    /**
     * @brief Get the name (with format "outputAgent##output-->inputAgent##input")
     * of a mapping element from the list of names (of each part)
     * @param inputAgent
     * @param input
     * @param outputAgent
     * @param output
     * @return name with format "outputAgent##output-->inputAgent##input"
     */
    static QString getNameFromNamesList(QString inputAgent, QString input, QString outputAgent, QString output);


    /**
     * @brief Get the list of names (of each parts) from the name (with format "outputAgent##output-->inputAgent##input")
     * of a mapping element
     * @param name with format "outputAgent##output-->inputAgent##input"
     * @return
     */
    static QStringList getNamesListFromName(QString name);

};

QML_DECLARE_TYPE(ElementMappingM)

#endif // ELEMENTMAPPINGM_H
