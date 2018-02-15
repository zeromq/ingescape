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
#include "masticquickabstractiopbinding.h"



/**
 * @brief The MasticQuickOutputBinding class defines a custom binding item to update Mastic outputs
 *        when properties of a given QML item (target) change
 */
class MasticQuickOutputBinding : public MasticQuickAbstractIOPBinding
{
    Q_OBJECT

    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsPrefix)

    // Suffix of Mastic outputs
    MASTIC_QML_PROPERTY_CUSTOM_SETTER(QString, outputsSuffix)


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
     * @param name
     * @param value
     */
    void _onQmlPropertyChanged(QString name, QVariant value);


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

};

QML_DECLARE_TYPE(MasticQuickOutputBinding)

#endif // _MASTICQUICKOUTPUTBINDING_H_
