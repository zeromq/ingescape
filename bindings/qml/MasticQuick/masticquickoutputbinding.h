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

#ifndef _MASTICQUICKOUTPUTBINDING_H_
#define _MASTICQUICKOUTPUTBINDING_H_


#include <QObject>
#include <QtQml>

#include "masticquick_helpers.h"



/**
 * @brief The MasticQuickOutputBinding class defines a custom binding item to update Mastic outputs
 *        when properties of a given QML item (target) change
 */
class MasticQuickOutputBinding : public QObject, public QQmlPropertyValueSource, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_INTERFACES(QQmlPropertyValueSource)

    // Target: the object to be updated
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QObject*, target)

    // List of properties of our object
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, properties)

    // Prefix of Mastic outputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsPrefix)

    // Suffix of Mastic outputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsSuffix)

    // Flag indicating if our binding is active or not
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, when)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickOutputBinding(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticQuickOutputBinding();



protected Q_SLOTS:
    /**
     * @brief Called when the object associated to our target property is destroyed
     * @param sender
     */
    void _ontargetDestroyed(QObject *sender);


    /**
     * @brief Called when a QML property changes
     * @param name
     * @param value
     */
    void _onQmlPropertyChanged(QString name, QVariant value);


protected:
    /**
     * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
     *        with the following syntax    MasticOutputBinding on property { }
     *
     * @param property
     */
    void setTarget(const QQmlProperty &property) Q_DECL_OVERRIDE;


    /**
     * @brief QQmlParserStatus API: Invoked after class creation, but before any properties have been set
     */
    void classBegin() Q_DECL_OVERRIDE;


    /**
     * @brief QQmlParserStatus API: Invoked after the root component that caused this instantiation has completed construction.
     *        At this point all static values and binding values have been assigned to the class.
     */
    void componentComplete() Q_DECL_OVERRIDE;


    /**
     * @brief Clear internal data
     */
    void _clear();


    /**
     * @brief Update internal data
     */
    void _update();


    /**
     * @brief Manage connect/disconnect of our properties
     */
    void _connectOrDisconnectProperties();


protected:
    // Flag indicating if our component is completed
     bool _isCompleted;

     // Flag indicating if our component is used has a QQmlPropertyValueSource
     bool _isUsedAsQQmlPropertyValueSource;

     // Target property when our component is used as a property value source
     QQmlProperty _propertyValueSourceTarget;

     // Hashtable of QML properties by name
     QHash<QString, QQmlProperty> _qmlPropertiesByName;

     // Hashtable (Mastic input name, property name)
     QHash<QString, QQmlProperty> _qmlPropertiesByMasticInputName;
};

QML_DECLARE_TYPE(MasticQuickOutputBinding)

#endif // _MASTICQUICKOUTPUTBINDING_H_
