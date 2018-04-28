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

#include <controller/ingescapeeditorcontroller.h>
#include <misc/ingescapeeditorsettings.h>
#include <misc/ingescapeeditorutils.h>
#include <misc/collapsiblecolumn.h>


/**
 * @brief Register our C++ types and extensions in the QML system
 */
void registerCustomQmlTypes()
{

    //---------------
    //
    // Enums
    //
    //---------------
    qmlRegisterSingletonType<ActionComparisonValueType>("INGESCAPE", 1, 0, "ActionComparisonValueType", &ActionComparisonValueType::qmlSingleton);
    qmlRegisterSingletonType<ActionConditionType>("INGESCAPE", 1, 0, "ActionConditionType", &ActionConditionType::qmlSingleton);
    qmlRegisterSingletonType<ActionEffectTypes>("INGESCAPE", 1, 0, "ActionEffectTypes", &ActionEffectTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentEffectValues>("INGESCAPE", 1, 0, "AgentEffectValues", &AgentEffectValues::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPTypes>("INGESCAPE", 1, 0, "AgentIOPTypes", &AgentIOPTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypes>("INGESCAPE", 1, 0, "AgentIOPValueTypes", &AgentIOPValueTypes::qmlSingleton);
    qmlRegisterSingletonType<AgentIOPValueTypeGroups>("INGESCAPE", 1, 0, "AgentIOPValueTypeGroups", &AgentIOPValueTypeGroups::qmlSingleton);
    qmlRegisterSingletonType<MappingEffectValues>("INGESCAPE", 1, 0, "MappingEffectValues", &MappingEffectValues::qmlSingleton);
    qmlRegisterSingletonType<ValidationDurationType>("INGESCAPE", 1, 0, "ValidationDurationType", &ValidationDurationType::qmlSingleton);


    //----------------
    //
    // Controllers
    //
    //----------------
    // Singleton used as primary access point to our controllers
    qmlRegisterSingletonType<IngeScapeEditorController>("INGESCAPE", 1, 0, "IngeScapeEditorC", &IngeScapeEditorController::qmlSingleton);

    // sub-controllers
    qmlRegisterUncreatableType<ActionEditorController>("INGESCAPE", 1, 0, "ActionEditorController", "Internal Class");
    qmlRegisterUncreatableType<AgentsMappingController>("INGESCAPE", 1, 0, "AgentsMappingController", "Internal Class");
    qmlRegisterUncreatableType<AgentsSupervisionController>("INGESCAPE", 1, 0, "AgentsSupervisionController", "Internal Class");
    qmlRegisterUncreatableType<HostsSupervisionController>("INGESCAPE", 1, 0, "HostsSupervisionController", "Internal Class");
    qmlRegisterUncreatableType<IngeScapeModelManager>("INGESCAPE", 1, 0, "IngeScapeModelManager", "Internal Class");
    qmlRegisterUncreatableType<ScenarioController>("INGESCAPE", 1, 0, "ScenarioController", "Internal Class");
    qmlRegisterUncreatableType<ValuesHistoryController>("INGESCAPE", 1, 0, "ValuesHistoryController", "Internal Class");
    qmlRegisterUncreatableType<AbstractTimeActionslineScenarioViewController>("INGESCAPE", 1, 0, "AbstractTimeActionslineScenarioViewController", "Internal Class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    qmlRegisterUncreatableType<ValuesHistorySortFilter>("INGESCAPE", 1, 0, "ValuesHistorySortFilter", "Internal class");


    //----------------
    //
    // Misc.
    //
    //----------------

    qmlRegisterType<CollapsibleColumn>("INGESCAPE", 1, 0, "CollapsibleColumn");

    // GST not included in master branch
    // qmlRegisterType<GstVideoReceiver>("INGESCAPE", 1, 0, "GstVideoReceiver");



    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<AgentIOPM>("INGESCAPE", 1, 0, "AgentIOPM", "Internal class");
    qmlRegisterUncreatableType<AgentM>("INGESCAPE", 1, 0, "AgentM", "Internal class");
    qmlRegisterUncreatableType<AgentMappingM>("INGESCAPE", 1, 0, "AgentMappingM", "Internal class");
    qmlRegisterUncreatableType<DefinitionM>("INGESCAPE", 1, 0, "DefinitionM", "Internal class");
    qmlRegisterUncreatableType<EffectOnAgentM>("INGESCAPE", 1, 0, "EffectOnAgentM", "Internal class");
    qmlRegisterUncreatableType<ElementMappingM>("INGESCAPE", 1, 0, "ElementMappingM", "Internal class");
    qmlRegisterUncreatableType<Enums>("INGESCAPE", 1, 0, "Enums", "Internal class");
    qmlRegisterUncreatableType<HostM>("INGESCAPE", 1, 0, "HostM", "Internal class");
    qmlRegisterUncreatableType<OutputM>("INGESCAPE", 1, 0, "OutputM", "Internal class");
    qmlRegisterUncreatableType<PublishedValueM>("INGESCAPE", 1, 0, "PublishedValueM", "Internal class");
    qmlRegisterUncreatableType<ActionConditionM>("INGESCAPE", 1, 0, "ActionConditionM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectM>("INGESCAPE", 1, 0, "ActionEffectM", "Internal class");
    qmlRegisterUncreatableType<ActionM>("INGESCAPE", 1, 0, "ActionM", "Internal class");
    qmlRegisterUncreatableType<IOPValueConditionM>("INGESCAPE", 1, 0, "IOPValueConditionM", "Internal class");
    qmlRegisterUncreatableType<IOPValueEffectM>("INGESCAPE", 1, 0, "IOPValueEffectM", "Internal class");
    qmlRegisterUncreatableType<MappingEffectM>("INGESCAPE", 1, 0, "MappingEffectM", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    qmlRegisterUncreatableType<ActionConditionVM>("INGESCAPE", 1, 0, "ActionConditionVM", "Internal class");
    qmlRegisterUncreatableType<ActionEffectVM>("INGESCAPE", 1, 0, "ActionEffectVM", "Internal class");
    qmlRegisterUncreatableType<ActionExecutionVM>("INGESCAPE", 1, 0, "ActionExecutionVM", "Internal class");
    qmlRegisterUncreatableType<ActionInPaletteVM>("INGESCAPE", 1, 0, "ActionInPaletteVM", "Internal class");
    qmlRegisterUncreatableType<ActionVM>("INGESCAPE", 1, 0, "ActionVM", "Internal class");
    qmlRegisterUncreatableType<AgentInMappingVM>("INGESCAPE", 1, 0, "AgentInMappingVM", "Internal class");
    qmlRegisterUncreatableType<AgentIOPVM>("INGESCAPE", 1, 0, "AgentIOPVM", "Internal class");
    qmlRegisterUncreatableType<AgentVM>("INGESCAPE", 1, 0, "AgentVM", "Internal class");
    qmlRegisterUncreatableType<InputVM>("INGESCAPE", 1, 0, "InputVM", "Internal class");
    qmlRegisterUncreatableType<MapBetweenIOPVM>("INGESCAPE", 1, 0, "MapBetweenIOPVM", "Internal class");
    qmlRegisterUncreatableType<OutputVM>("INGESCAPE", 1, 0, "OutputVM", "Internal class");
    qmlRegisterUncreatableType<ParameterVM>("INGESCAPE", 1, 0, "ParameterVM", "Internal class");
    qmlRegisterUncreatableType<PointMapVM>("INGESCAPE", 1, 0, "PointMapVM", "Internal class");


    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeTheme.qml"), "INGESCAPE", 1, 0, "IngeScapeTheme");

    // - Srollview style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeScrollViewStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeScrollViewStyle");

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
    app.setApplicationName("IngeScape-Editor");
    app.setApplicationVersion("0.0.0");

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);


    //------------------------------
    //
    // Root directory
    //
    //------------------------------

    // Get (and create if needed) the root path of our application ([DocumentsLocation]/IngeScape/)
    QString rootDirectoryPath = IngeScapeEditorUtils::getRootPath();
    if (!rootDirectoryPath.isEmpty())
    {
        //------------------------------
        //
        // Logs
        //
        //------------------------------

        // Open our log file
        I2LogsManager::Instance().openLogFile(rootDirectoryPath, "log-IngeScape-Editor.csv", true);

        // Replace the default message handler with our own logger
        qInstallMessageHandler(I2LogMessageHandler);

        qInfo() << "Application" << app.applicationName() << "is running with version" << app.applicationVersion();


        //------------------------------
        //
        // Settings
        //
        //------------------------------

        // Get (and create if needed) the settings path of our application ([DocumentsLocation]/IngeScape/Settings/)
        QString settingsDirectoryPath = IngeScapeEditorUtils::getSettingsPath();
        QString settingsFilePath = QString("%1IngeScape-Editor.ini").arg(settingsDirectoryPath);

        QFile settingsFile(settingsFilePath);
        if (!settingsFile.exists())
        {
            // Copy our .ini file
            if (QFile::copy(":/settings/IngeScape-Editor.ini", settingsFilePath))
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
            IngeScapeEditorSettings& settings = IngeScapeEditorSettings::Instance(settingsFilePath);

            qDebug() << "Settings" << settings.fileName();
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

    // Init streaming capability
    // GST not included ins master branch
    // gst_init (NULL , NULL);

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

    qInfo() << "Starting" << app.applicationName() << app.applicationVersion() << "with processID" << QApplication::applicationPid();
    int exitReturnCode = app.exec();
    qInfo() << "Quitting application with return code" << exitReturnCode;
}
