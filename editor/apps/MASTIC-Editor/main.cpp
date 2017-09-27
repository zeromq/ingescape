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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */


#include <QApplication>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QSurfaceFormat>
#include <QDebug>
#include <QtGlobal>
#include <QDate>


#include <I2Quick.h>

#include <controller/masticeditorcontroller.h>
#include <misc/masticeditorsettings.h>
#include <misc/masticeditorutils.h>




/**
 * @brief Register our C++ types and extensions in the QML system
 */
void registerCustomQmlTypes()
{
    // Our custom QML namespace
    const char *uri = "MASTIC";


    //
    // Enum
    //
    qmlRegisterSingletonType<AgentIOPTypes>(uri, 1, 0, "AgentIOPTypes", &AgentIOPTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypes>(uri, 1, 0, "AgentIOPValueTypes", &AgentIOPValueTypes::qmlSingleton);


    //
    // Controller
    //
    // Singleton used as primary access point to our controllers
    qmlRegisterSingletonType<MasticEditorController>(uri, 1, 0, "MasticEditorC", &MasticEditorController::qmlSingleton);

    // sub-controllers
    qmlRegisterUncreatableType<AgentsMappingController>(uri, 1, 0, "AgentsMappingController", "Internal Class");
    qmlRegisterUncreatableType<AgentsSupervisionController>(uri, 1, 0, "AgentsSupervisionController", "Internal Class");
    qmlRegisterUncreatableType<MasticModelManager>(uri, 1, 0, "MasticModelManager", "Internal Class");


    //
    // Model
    //
    qmlRegisterUncreatableType<AgentIOPM>(uri, 1, 0, "AgentIOPM", "Internal class");
    qmlRegisterUncreatableType<AgentM>(uri, 1, 0, "AgentM", "Internal class");


    //
    // View Model
    //
    qmlRegisterUncreatableType<AgentIOPVM>(uri, 1, 0, "AgentIOPVM", "Internal class");
    qmlRegisterUncreatableType<AgentVM>(uri, 1, 0, "AgentVM", "Internal class");


    //
    // QML singletons
    //
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/MasticTheme.qml"), uri, 1, 0, "MasticTheme");

}


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

    // TEMP: Disable disk cache to avoid a bug with QML singletons (e.g. theme files)
    //       see https://bugreports.qt.io/browse/QTBUG-62243
    // TODO: Try to remove this line when Qt 5.9.2 will be available
    qputenv("QML_DISABLE_DISK_CACHE", "1");


    // Automatic high-DPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    // Enable anti-aliasing (2x MSAA) if needed
    // TODO: check device pixel ratio first
    //QSurfaceFormat currentSurfaceFormat = QSurfaceFormat::defaultFormat();
    //currentSurfaceFormat.setSamples(2);
    //QSurfaceFormat::setDefaultFormat(currentSurfaceFormat);


    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("MASTIC-Editor");
    app.setApplicationVersion("0.0.0");

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);


    //------------------------------
    //
    // Root directory
    //
    //------------------------------

    // Get (and create if needed) the root path of our application ([DocumentsLocation]/MASTIC/)
    QString rootDirectoryPath = MasticEditorUtils::getRootPath();
    if (!rootDirectoryPath.isEmpty())
    {
        //------------------------------
        //
        // Logs
        //
        //------------------------------

        // Open our log file
        I2LogsManager::Instance().openLogFile(rootDirectoryPath, "log-MASTIC-Editor.csv", true);

        // Replace the default message handler with our own logger
        qInstallMessageHandler(I2LogMessageHandler);

        qInfo() << "Application" << app.applicationName() << "is running with version" << app.applicationVersion();


        //------------------------------
        //
        // Settings
        //
        //------------------------------

        // Get (and create if needed) the settings path of our application ([DocumentsLocation]/MASTIC/Settings/)
        QString settingsDirectoryPath = MasticEditorUtils::getSettingsPath();
        QString settingsFilePath = QString("%1MASTIC-Editor.ini").arg(settingsDirectoryPath);

        QFile settingsFile(settingsFilePath);
        if (!settingsFile.exists())
        {
            // Copy our .ini file
            if (QFile::copy(":/settings/MASTIC-Editor.ini", settingsFilePath))
            {
                // Update permissions for our .ini file
                QFileDevice::Permissions filePermissions =  QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ReadGroup | QFileDevice::WriteGroup;
                if (!QFile::setPermissions(settingsFilePath, filePermissions))
                {
                    qWarning() << "ERROR during set permissions to file" << settingsFilePath;
                }
            }
            else
            {
                qCritical() << "ERROR during copy of settings into" << settingsFilePath;
            }
        }


        // Récupération des différentes configurations
        if (settingsFile.exists()) {
            // Load our settings
            MasticEditorSettings& settings = MasticEditorSettings::Instance(settingsFilePath);

            qDebug() << "Settings" << settings.fileName();
        }


        //------------------------------
        //
        // Agents definitions
        //
        //------------------------------
        QString agentsDefinitionsDirectoryPath = MasticEditorUtils::getAgentsDefinitionsPath();
        QDir agentsDefinitionsDirectory(agentsDefinitionsDirectoryPath);
        if (!agentsDefinitionsDirectory.exists())
        {
            qCritical() << "ERROR: could not create directory at '" << agentsDefinitionsDirectoryPath << "' !";
        }

        //------------------------------
        //
        // Snapshots directory
        //
        //------------------------------
        QString snapshotsDirectoryPath = MasticEditorUtils::getSnapshotsPath();
        QDir snapshotsDirectory(snapshotsDirectoryPath);
        if (!snapshotsDirectory.exists())
        {
            qCritical() << "ERROR: could not create directory at '" << snapshotsDirectoryPath << "' !";
        }
    }
    else
    {
        qFatal("ERROR: There is no special directory 'Documents' !");
    }


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
    I2Quick::registerTypes();
    I2Quick::initializeEngine(&engine);

    // - Display error images when SVG items are missing (source: "image://I2svg/.....")
    I2SVGImageProvider::showErrorsAsImage = true;


    //
    // Register our custom QML types
    //
    registerCustomQmlTypes();



    //
    // Defines context properties
    //
    QQmlContext* qmlContext = engine.rootContext();
    if (qmlContext != NULL)
    {
        // Build date
        qmlContext->setContextProperty("BUILD_DATE", QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")));

        // Qt version against which the application is compiled
        qmlContext->setContextProperty("QT_BUILD_VERSION", QString(QT_VERSION_STR));

        // Runtime version of Qt. This may be a different version than the version the application was compiled against
        qmlContext->setContextProperty("QT_RUNTIME_VERSION", QString(qVersion()));
    }


    //
    // Load our main QML file
    //
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));



    //------------------------------
    //
    // Mainloop
    //
    //------------------------------

    qInfo() << "Starting" << app.applicationName() << app.applicationVersion();
    int exitReturnCode = app.exec();
    qInfo() << "Quitting application with return code" << exitReturnCode;
}
