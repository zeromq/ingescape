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

#ifndef LINKVM_H
#define LINKVM_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief The LinkVM class defines a view model of link
 */
class LinkVM : public QObject
{
    Q_OBJECT


public:
    explicit LinkVM(QObject *parent = nullptr);

signals:

public slots:
};

#endif // LINKVM_H
