/*
 *  Mastic - QML binding
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


#ifndef _MASTICQUICKINPUTBINDING_H_
#define _MASTICQUICKINPUTBINDING_H_

#include <QObject>
#include <QtQml>

#include "masticquick_helpers.h"
#include "masticquickabstractiopbinding.h"



/**
 * @brief The MasticQuickInputBinding class defines a custom binding item to update properties
 *         of a given QML item (target) when Mastic inputs change
 *
 */
class MasticQuickInputBinding : public MasticQuickAbstractIOPBinding
{
    Q_OBJECT

    // Prefix of Mastic inputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, inputsPrefix)

    // Suffix of Mastic inputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, inputsSuffix)

    // Name of our Mastic Input
    // (only when our component is used as a property value source OR if a single property is referenced by 'properties')
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, inputName)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickInputBinding(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticQuickInputBinding();



protected Q_SLOTS:
    /**
     * @brief Called when a Mastic input changes
     * @param name
     * @param value
     */
    void _onMasticObserveInput(QString name, QVariant value);



protected:
    /**
     * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
     *        At this point all static values and binding values have been assigned to the class.
     */
    virtual void componentComplete() Q_DECL_OVERRIDE;



protected:
    /**
     * @brief Connect to MasticQuick
     */
    void _connectToMasticQuick() Q_DECL_OVERRIDE;


    /**
     * @brief Disconnect to MasticQuick
     */
    void _disconnectToMasticQuick() Q_DECL_OVERRIDE;


    /**
     * @brief Clear internal data
     */
    void _clearInternalData() Q_DECL_OVERRIDE;


    /**
     * @brief Update internal data
     */
    void _updateInternalData() Q_DECL_OVERRIDE;


protected:
     // Hashtable (Mastic input name, QML property)
     QHash<QString, QQmlProperty> _qmlPropertiesByMasticInputName;
};

QML_DECLARE_TYPE(MasticQuickInputBinding)

#endif // _MASTICQUICKINPUTBINDING_H_
