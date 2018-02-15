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


#ifndef _MASTICQUICKABSTRACTIOPBINDING_H_
#define _MASTICQUICKABSTRACTIOPBINDING_H_

#include <QObject>
#include <QtQml>

#include "masticquick_helpers.h"


/**
 * @brief The MasticQuickAbstractIOPBinding class is the base class of custom Binding item to bind QML properties
 *        to Mastic IOP
 */
class MasticQuickAbstractIOPBinding : public QObject, public QQmlPropertyValueSource, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_INTERFACES(QQmlPropertyValueSource)

    // Target: the object associated to our binding
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QObject*, target)

    // List of properties of our object
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, properties)

    // Flag indicating if our binding is active or not
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(bool, when)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticQuickAbstractIOPBinding(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticQuickAbstractIOPBinding();



protected Q_SLOTS:
    /**
     * @brief Called when the object associated to our target property is destroyed
     * @param sender
     */
    void _ontargetDestroyed(QObject *sender);


protected:
    /**
     * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
     *        with the following syntax    MasticInputBinding on property { }
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



protected:
    /**
     * @brief Update our component
     *
     * @remarks this function will call _updateInternalData()
     */
    void update();


    /**
     * @brief Clear our component
     *
     * @remarks this function will call _clearInternalData();
     */
    void clear();


    /**
     * @brief Manage connect/disconnect calls to associate our item to MasticQuick
     *
     * @remarks this function will call _connectToMasticQuick() and _disconnectToMasticQuick()
     */
    void connectOrDisconnectToMasticQuick();


protected:
    /**
     * @brief Connect to MasticQuick
     */
     virtual void _connectToMasticQuick();


    /**
     * @brief Disconnect to MasticQuick
     */
     virtual void _disconnectToMasticQuick();


    /**
     * @brief Clear internal data
     */
     virtual void _clearInternalData();


    /**
     * @brief Update internal data
     */
     virtual void _updateInternalData();


protected:
    // Flag indicating if our component is completed
     bool _isCompleted;

     // Flag indicating if our component is used has a QQmlPropertyValueSource
     bool _isUsedAsQQmlPropertyValueSource;

     // Flag indicating if our component requires weritable QML properties
     bool _qmlPropertiesMustBeWritable;

     // Target property when our component is used as a property value source
     QQmlProperty _propertyValueSourceTarget;

     // Hashtable of QML properties by name
     QHash<QString, QQmlProperty> _qmlPropertiesByName;
};

QML_DECLARE_TYPE(MasticQuickAbstractIOPBinding)

#endif // _MASTICQUICKABSTRACTIOPBINDING_H_
