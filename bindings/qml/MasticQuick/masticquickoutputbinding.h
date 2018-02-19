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
#include "masticquick_enums.h"
#include "masticquickabstractiopbinding.h"



/**
 * @brief The MasticQuickOutputBinding class defines a custom binding item to update Mastic outputs
 *        when properties of a given QML item (target) change
 */
class MasticQuickOutputBinding : public MasticQuickAbstractIOPBinding, public QQmlPropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QQmlPropertyValueSource)


    // Prefix of Mastic outputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsPrefix)

    // Suffix of Mastic outputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsSuffix)

    // Name of our Mastic output
    // (only when our component is used as a property value source OR if a single property is referenced by 'properties')
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputName)


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
     * @brief Called when a QML property changes
     */
    void _onQmlPropertyChanged();



protected:
    /**
     * @brief QQmlPropertyValueSource API: This method will be called by the QML engine when assigning a value source
     *        with the following syntax    MasticInputBinding on property { }
     *
     * @param property
     */
    void setTarget(const QQmlProperty &property) Q_DECL_OVERRIDE;



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
    // List of Mastic outputs (name, type) by QML property
    QHash<QQmlProperty, QPair<QString, MasticIopType::Value>> _masticOutputsByQmlProperty;

    // List of QML property by notify signal index
    QHash<int, QQmlProperty> _qmlPropertiesByNotifySignalIndex;

    // Meta method associated to our _onQmlPropertyChanged callback
    QMetaMethod _onQmlPropertyChangedMetaMethod;
};

QML_DECLARE_TYPE(MasticQuickOutputBinding)

#endif // _MASTICQUICKOUTPUTBINDING_H_
