/*
 *	IngeScape Measuring
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include <QApplication>
//#include <QGuiApplication>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QtGlobal>

//#include <I2Quick.h>


/**
 * @brief Entry point of our application program
 * @param argc Argument count
 * @param argv List of arguments
 * @return
 */
int main(int argc, char *argv[])
{
    //------------------------------
    //
    // Configure Qt
    //
    //------------------------------

    // Automatic high-DPI scaling
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("IngeScape-Measuring");
    app.setApplicationVersion("0.1.0.0");

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);

    //QGuiApplication app(argc, argv);

    qInfo() << "Application" << app.applicationName() << "is running with version" << app.applicationVersion();


    //------------------------------
    //
    // Configure our QML engine
    //
    //------------------------------
    QQmlApplicationEngine engine;


    //
    // Register and configure I2Quick
    //
    // - register QML extensions
    //I2Quick::registerTypes();
    //I2Quick::initializeEngine(&engine);

    // - Display error images when SVG items are missing (source: "image://I2svg/.....")
    //I2SVGImageProvider::showErrorsAsImage = true;

    //
    // Register our custom QML types
    //
    //registerCustomQmlTypes();


    //
    // Load our main QML file
    //
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    /*if (engine.rootObjects().isEmpty())
        return -1;*/


    //------------------------------
    //
    // Mainloop
    //
    //------------------------------

    qInfo() << "Starting" << app.applicationName() << app.applicationVersion() << "with processID" << QApplication::applicationPid();
    int exitReturnCode = app.exec();
    qInfo() << "Quitting application with return code" << exitReturnCode;
}
