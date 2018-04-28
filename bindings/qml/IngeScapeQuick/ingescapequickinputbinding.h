/*
 *  IngeScape - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _INGESCAPEQUICKINPUTBINDING_H_
#define _INGESCAPEQUICKINPUTBINDING_H_

#include <QObject>
#include <QtQml>

#include "ingescapequick_helpers.h"
#include "ingescapequickabstractiopbinding.h"



/**
 * @brief The IngeScapeQuickInputBinding class defines a custom binding item to update properties
 *         of a given QML item (target) when IngeScape inputs change
 *
 */
class IngeScapeQuickInputBinding : public IngeScapeQuickAbstractIOPBinding
{
    Q_OBJECT

    // Prefix of IngeScape inputs
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(QString, inputsPrefix)

    // Suffix of IngeScape inputs
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(QString, inputsSuffix)

    // Name of our IngeScape Input
    // (only when our component is used as a property value source OR if a single property is referenced by 'properties')
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(QString, inputName)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit IngeScapeQuickInputBinding(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeQuickInputBinding();



protected Q_SLOTS:
    /**
     * @brief Called when a IngeScape input changes
     * @param name
     * @param value
     */
    void _onIngeScapeObserveInput(QString name, QVariant value);



protected:
    /**
     * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
     *        At this point all static values and binding values have been assigned to the class.
     */
    virtual void componentComplete() Q_DECL_OVERRIDE;



protected:
    /**
     * @brief Connect to IngeScapeQuick
     */
    void _connectToIngeScapeQuick() Q_DECL_OVERRIDE;


    /**
     * @brief Disconnect to IngeScapeQuick
     */
    void _disconnectToIngeScapeQuick() Q_DECL_OVERRIDE;


    /**
     * @brief Clear internal data
     */
    void _clearInternalData() Q_DECL_OVERRIDE;


    /**
     * @brief Update internal data
     */
    void _updateInternalData() Q_DECL_OVERRIDE;



protected:
     // Hashtable (IngeScape input name, QML property)
     QHash<QString, QQmlProperty> _qmlPropertiesByIngeScapeInputName;
};

QML_DECLARE_TYPE(IngeScapeQuickInputBinding)

#endif // _INGESCAPEQUICKINPUTBINDING_H_
