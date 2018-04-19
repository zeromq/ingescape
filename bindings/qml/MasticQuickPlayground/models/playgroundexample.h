/*
 *  Mastic - QML playground
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

#ifndef PLAYGROUNDEXAMPLE_H
#define PLAYGROUNDEXAMPLE_H

#include <QObject>
#include <QtQml>

#include "utils/propertieshelpers.h"


/**
 * @brief The PlaygroundExample class stores info about an example
 */
class PlaygroundExample : public QObject
{
    Q_OBJECT

    PLAYGROUND_QML_PROPERTY_READONLY(QString, file)

    PLAYGROUND_QML_PROPERTY_READONLY(QString, description)

public:
    /**
     * @brief Default constructor
     * @param file
     * @param description
     * @param parent
     */
    explicit PlaygroundExample(QString file, QString description, QObject *parent = nullptr);

};

QML_DECLARE_TYPE(PlaygroundExample)

#endif // PLAYGROUNDEXAMPLE_H
