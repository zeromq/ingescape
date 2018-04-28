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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>


// Check if IngeScapeQuick is included via a .pri file OR if it is used as a QML extension plugin library
#ifdef INGESCAPEQUICK_INCLUDED_VIA_PRI
#include <IngeScapeQuick.h>
#endif


/**
 * @brief Entry point of our application
 * @param argc Argument count
 * @param argv List of arguments
 * @return
 */
int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif


    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("SimpleQmlIngeScapeAgent");
    app.setApplicationVersion("0.0");

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
        QString settingsDirectoryPath = QString("%1%2%3%2").arg(documentsDirectoryPath, QDir::separator(), "IngeScapeQuick");

        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDirectoryPath);
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


// Check if IngeScapeQuick is included via a .pri file OR if it is used as a QML extension plugin library
#ifdef INGESCAPEQUICK_INCLUDED_VIA_PRI
    // Register our IngeScapeQuick plugin
    IngeScapeQuick::registerTypes();
#else
    qWarning() << "IngeScapeQuick MUST be installed as a QML extension plugin library, otherwise this application will not work";
#endif

    //
    // Load our main QML file
    //
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
    {
        qWarning() << "Failed to load main QML file";
        return -1;
    }


    //------------------------------
    //
    // Mainloop
    //
    //------------------------------

    return app.exec();
}
