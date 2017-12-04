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

#ifndef MASTICLAUNCHERMANAGER_H
#define MASTICLAUNCHERMANAGER_H

#include <QObject>

/**
 * @brief The MasticLauncherManager class defines the manager for launchers of MASTIC agents
 */
class MasticLauncherManager : public QObject
{
    Q_OBJECT
public:
    explicit MasticLauncherManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // MASTICLAUNCHERMANAGER_H
