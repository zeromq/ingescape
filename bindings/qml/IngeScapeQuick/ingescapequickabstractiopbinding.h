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


#ifndef _INGESCAPEQUICKABSTRACTIOPBINDING_H_
#define _INGESCAPEQUICKABSTRACTIOPBINDING_H_

#include <QObject>
#include <QtQml>

#include "ingescapequick_helpers.h"
#include "ingescapequick_enums.h"


/**
 * @brief The IngeScapeQuickAbstractIOPBinding class is the base class of custom Binding item to bind QML properties
 *        to IngeScape IOP
 */
class IngeScapeQuickAbstractIOPBinding : public QObject, public QQmlPropertyValueSource, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_INTERFACES(QQmlPropertyValueSource)

    // Target: the object associated to our binding
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(QObject*, target)

    // List of properties of our target (set via our "target" property and not via setTarget (QQmlPropertyValueSource API))
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(QString, properties)

    // Flag indicating if our binding is active or not
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(bool, when)

    // Remove IngeScape IOPs created by our component when needed i.e. when its properties are updated or when it is destroyed
    INGESCAPE_QML_PROPERTY_CUSTOM_SETTER(bool, removeOnUpdatesAndDestruction)


public:
    /**
     * @brief Default constructor
     * @param parent
     * @param qmlPropertiesMustBeWritable
     */
    explicit IngeScapeQuickAbstractIOPBinding(QObject *parent = nullptr, bool qmlPropertiesMustBeWritable = false);


    /**
     * @brief Destructor
     */
    ~IngeScapeQuickAbstractIOPBinding();



private Q_SLOTS:
    /**
     * @brief Called when the object associated to our target property is destroyed
     * @param sender
     */
    void _ontargetDestroyed(QObject *sender);



protected:
    /**
     * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
     *        with the following syntax: IngeScapeInputBinding on property { } or  IngeScapeOutputBinding on property { }
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
    virtual void componentComplete() Q_DECL_OVERRIDE;



protected:
    /**
     * @brief Get the pretty type name of a given object
     * @param object
     * @return
     */
    QString prettyObjectTypeName(QObject* object);


    /**
     * @brief Check if a given property is supported by IngeScape
     * @param property
     * @return
     */
    bool checkIfPropertyIsSupported(const QQmlProperty &property);


    /**
     * @brief Get the pretty name of a given property
     * @param property
     * @return
     */
    QString prettyPropertyTypeName(const QQmlProperty &property);


    /**
     * @brief Get the IngeScapeIopType of a given property
     * @param property
     * @return
     *
     * @remarks we assume that checkIfPropertyIsSupported has been called before using this method
     */
    IngeScapeIopType::Value getIngeScapeIOPTypeForProperty(const QQmlProperty &property);


    /**
     * @brief Update our component
     *
     * @remarks this function will call _updateInternalData()
     */
    void update();


    /**
     * @brief Clear our component
     *
     * @remarks this function will call _clearInternalData() and _disconnectToIngeScapeQuick()
     */
    void clear();


    /**
     * @brief Manage connect/disconnect calls to associate our item to IngeScapeQuick
     *
     * @remarks this function will call _connectToIngeScapeQuick() and _disconnectToIngeScapeQuick()
     */
    void connectOrDisconnectToIngeScapeQuick();




//---------------------------------------------------
//
// Methods to override in derived classes
//
//---------------------------------------------------
protected:
    /**
     * @brief Connect to IngeScapeQuick
     */
     virtual void _connectToIngeScapeQuick();


    /**
     * @brief Disconnect to IngeScapeQuick
     */
     virtual void _disconnectToIngeScapeQuick();


    /**
     * @brief Clear internal data
     */
     virtual void _clearInternalData();


    /**
     * @brief Update internal data
     */
     virtual void _updateInternalData();



private:
    /**
     * @brief Build our list of QML properties by introspection
     * @param object
     * @param prefix
     */
    void _buildListOfQmlPropertiesByIntrospection(QObject* object, QString prefix);



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



protected:
     // List of properties excluded from introspection
     static QList<QString> _propertiesExcludedFromIntrospection;

     // List of signal handlers excluded from introspection
     static QList<QString> _signalHandlersExcludedFromIntrospection;

     // List of supported types for IngeScapeIopType.INTEGER
     static QList<QMetaType::Type> _supportedTypesForIngeScapeIopTypeInteger;

     // List of supported types for IngeScapeIopType.DOUBLE
     static QList<QMetaType::Type> _supportedTypesForIngeScapeIopTypeDouble;
};

QML_DECLARE_TYPE(IngeScapeQuickAbstractIOPBinding)

#endif // _INGESCAPEQUICKABSTRACTIOPBINDING_H_
