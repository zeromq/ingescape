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



#include "playgroundexample.h"


#include <QQmlEngine>


/**
 * @brief Default constructor
 * @param file
 * @param description
 * @param parent
 */
PlaygroundExample::PlaygroundExample(QString file, QString description, QObject *parent)
    : QObject(parent),
      _file(file),
      _description(description)
{
    // Force C++ ownership, otherwise our singleton will be owned by the QML engine
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}
