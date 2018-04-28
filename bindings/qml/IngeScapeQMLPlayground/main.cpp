/*
 *  ingeScape - QML playground
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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>


// IngeScapeQuick MUST be included via a .pri file
#include <IngeScapeQuick.h>


#include "controllers/playgroundcontroller.h"

#include "utils/resourcehelpers.h"
#include "utils/qmlsyntaxhighlighter.h"



/**
 * @brief Register our C++ types and extensions in the QML system
 */
void registerCustomQmlTypes()
{
    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<PlaygroundExample>("IngeScapePlayground", 1, 0, "PlaygroundExample", "Internal class");


    //----------------
    //
    // Utils
    //
    //----------------
    qmlRegisterType<QMLSyntaxHighlighter>("IngeScapePlayground", 1, 0, "QMLSyntaxHighlighter");


    //----------------
    //
    // Controllers
    //
    //----------------

    qmlRegisterSingletonType<PlaygroundController>("IngeScapePlayground", 1, 0, "PlaygroundController", &PlaygroundController::qmlSingleton);
}




/**
 * @brief Entry point of our application
 * @param argc Argument count
 * @param argv List of arguments
 * @return
 */
int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif


    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("IngeScapeQMLPlayground");
    app.setApplicationVersion("0.0.0.0");

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);


    //------------------------------
    //
    // Configure QML settings
    //
    //------------------------------

    QStringList documentsLocation = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (documentsLocation.count() > 0)
    {
        QString documentsDirectoryPath = documentsLocation.first();
        QString settingsDirectoryPath = QString("%1%2%3%2").arg(documentsDirectoryPath, QDir::separator(), app.applicationName());

        // Configure QSettings (diretory for INI format)
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectoryPath);

        // Dump our embedded settings file if needed
        QString settingsFilePath = QString("%1%2%3%2%4.ini").arg(settingsDirectoryPath, QDir::separator(), app.organizationDomain(), app.applicationName());
        ResourceHelpers::dumpEmbeddedResourceFileIfNeeded(":/resources/settings.ini", settingsFilePath);
    }
    else
    {
        qWarning() << "Warning: Documents location not found. QSetting will use a default location";
    }

    // Force INIT format
    QSettings::setDefaultFormat(QSettings::IniFormat);


    //------------------------------
    //
    // Configure our QML engine
    //
    //------------------------------
    QQmlApplicationEngine engine;

    // Register our IngeScapeQuick plugin
    IngeScapeQuick::registerTypes();

    // Register custom QML types
    registerCustomQmlTypes();


    //
    // Load our main QML file
    //
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        qWarning() << "Failed to load QML main file";
        return -1;
    }


    //------------------------------
    //
    // Mainloop
    //
    //------------------------------

    return app.exec();
}
