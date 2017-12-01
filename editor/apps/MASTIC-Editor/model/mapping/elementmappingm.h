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

    // Identifier with all names: [outputAgent##output-->inputAgent##input]
    I2_CPP_NOSIGNAL_PROPERTY(QString, id)

    // Input agent name
    I2_QML_PROPERTY_READONLY(QString, inputAgent)

    // Input name to connect with
    I2_QML_PROPERTY_READONLY(QString, input)

    // Output agent name to connect with
    I2_QML_PROPERTY_READONLY(QString, outputAgent)

    // Output name to connect with
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

};

QML_DECLARE_TYPE(ElementMappingM)

#endif // ELEMENTMAPPINGM_H
