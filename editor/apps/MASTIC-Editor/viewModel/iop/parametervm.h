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

#ifndef PARAMETERVM_H
#define PARAMETERVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

//#include <viewModel/pointmapvm.h>
#include <model/iop/agentiopm.h>

/**
 * @brief The ParameterVM class defines a view model of parameter
 */
class ParameterVM : public QObject
{
    Q_OBJECT


public:
    explicit ParameterVM(QObject *parent = nullptr);

signals:

public slots:
};

QML_DECLARE_TYPE(ParameterVM)

#endif // PARAMETERVM_H
