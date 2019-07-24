/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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

#include <misc/collapsiblecolumn.h>
#include <misc/qquickwindowblocktouches.h>
#include <misc/textfielddoublevalidator.h>
#include <misc/numberconstants.h>


//
// IngeScape Common
//
#include <ingescapecommon.h>
#include <settings/ingescapesettings.h>
#include <misc/ingescapeutils.h>


/**
 * @brief Handler for "Log Message"
 * @param type
 * @param context
 * @param message
 */
void LogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    fprintf(stdout, "%s\n", message.toLatin1().constData());
    fflush(stdout);

    // "context.function" can be null
    QString function = "Undefined function";
    if (context.function != nullptr) {
        function = QString(context.function);
    }

    // Get the current log level
    igs_logLevel_t logLevel = igs_getLogLevel();

    switch (type)
    {
    case QtDebugMsg:
    {
        // Allows to prevent to log in the file useless logs (with type TRACE or DEBUG)
        if (logLevel <= IGS_LOG_DEBUG) {
            igs_log(IGS_LOG_DEBUG, function.toStdString().c_str(), message.toStdString().c_str(), "%s");
        }
        break;
    }
    case QtInfoMsg:
    {
        igs_log(IGS_LOG_INFO, function.toStdString().c_str(), message.toStdString().c_str(), "%s");
        break;
    }
    case QtWarningMsg:
    {
        igs_log(IGS_LOG_WARN, function.toStdString().c_str(), message.toStdString().c_str(), "%s");
        break;
    }
    case QtCriticalMsg:
    {
        igs_log(IGS_LOG_ERROR, function.toStdString().c_str(), message.toStdString().c_str(), "%s");
        break;
    }
    case QtFatalMsg:
    {
        igs_log(IGS_LOG_FATAL, function.toStdString().c_str(), message.toStdString().c_str(), "%s");
        break;
    }
    default:
        // NO LOG IT (QtSystemMsg)
        break;
    }
}


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
    qmlRegisterSingletonType<ReplayStates>("INGESCAPE", 1, 0, "ReplayStates", &ReplayStates::qmlSingleton);


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
    qmlRegisterUncreatableType<EditorModelManager>("INGESCAPE", 1, 0, "EditorModelManager", "Internal Class");
    qmlRegisterUncreatableType<HostsSupervisionController>("INGESCAPE", 1, 0, "HostsSupervisionController", "Internal Class");
    qmlRegisterUncreatableType<LogStreamController>("INGESCAPE", 1, 0, "LogStreamController", "Internal Class");
    qmlRegisterUncreatableType<RecordsSupervisionController>("INGESCAPE", 1, 0, "RecordsSupervisionController", "Internal Class");
    qmlRegisterUncreatableType<ValuesHistoryController>("INGESCAPE", 1, 0, "ValuesHistoryController", "Internal Class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    qmlRegisterUncreatableType<LogsSortFilter>("INGESCAPE", 1, 0, "LogsSortFilter", "Internal class");
    qmlRegisterUncreatableType<ValuesHistorySortFilter>("INGESCAPE", 1, 0, "ValuesHistorySortFilter", "Internal class");


    //----------------
    //
    // Misc.
    //
    //----------------
    qmlRegisterType<CollapsibleColumn>("INGESCAPE", 1, 0, "CollapsibleColumn");
    qmlRegisterType<QQuickWindowBlockTouches>("INGESCAPE", 1, 0, "WindowBlockTouches");
    qmlRegisterType<TextFieldDoubleValidator>("INGESCAPE", 1, 0, "TextFieldDoubleValidator");
    qmlRegisterSingletonType<NumberConstants>("INGESCAPE", 1, 0, "NumberConstants", &NumberConstants::qmlSingleton);


    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<EditorEnums>("INGESCAPE", 1, 0, "EditorEnums", "Internal class");
    qmlRegisterUncreatableType<LogM>("INGESCAPE", 1, 0, "LogM", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    qmlRegisterUncreatableType<ActionInMappingVM>("INGESCAPE", 1, 0, "ActionInMappingVM", "Internal class");
    qmlRegisterUncreatableType<AgentInMappingVM>("INGESCAPE", 1, 0, "AgentInMappingVM", "Internal class");
    qmlRegisterUncreatableType<LinkConnectorVM>("INGESCAPE", 1, 0, "LinkConnectorVM", "Internal class");
    qmlRegisterUncreatableType<LinkInputVM>("INGESCAPE", 1, 0, "LinkInputVM", "Internal class");
    qmlRegisterUncreatableType<LinkOutputVM>("INGESCAPE", 1, 0, "LinkOutputVM", "Internal class");
    qmlRegisterUncreatableType<LinkVM>("INGESCAPE", 1, 0, "LinkVM", "Internal class");
    qmlRegisterUncreatableType<ObjectInMappingVM>("INGESCAPE", 1, 0, "ObjectInMappingVM", "Internal class");


    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeEditorTheme.qml"), "INGESCAPE", 1, 0, "IngeScapeEditorTheme");

    // - Scrollview style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeScrollViewStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeScrollViewStyle");

    // - Combobox style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeComboboxStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeComboboxStyle");

    // - Combobox for ItemModel with tooltip delegate
    qmlRegisterType(QUrl("qrc:/qml/scenario/IngeScapeComboboxItemModel.qml"), "INGESCAPE", 1, 0, "IngeScapeComboboxItemModel");

    // - Combobox for Agents IOP model with a custom tooltip delegate to show the color circles
    qmlRegisterType(QUrl("qrc:/qml/scenario/IngeScapeComboboxItemModelAgentsIOP.qml"), "INGESCAPE", 1, 0, "IngeScapeComboboxItemModelAgentsIOP");

    // - Combobox delegate with tooltip
    qmlRegisterType(QUrl("qrc:/qml/scenario/IngeScapeToolTipComboboxDelegate.qml"), "INGESCAPE", 1, 0, "IngeScapeToolTipComboboxDelegate");

    // - Combobox delegate for Agents IOP comboboxes to show the color circles
    qmlRegisterType(QUrl("qrc:/qml/scenario/IngeScapeToolTipComboboxDelegateAgentsIOP.qml"), "INGESCAPE", 1, 0, "IngeScapeToolTipComboboxDelegateAgentsIOP");

    // - StringValidator for text fields
    qmlRegisterType(QUrl("qrc:/qml/validator/StringValidator.qml"), "INGESCAPE", 1, 0, "StringValidator");

    // - DoubleValidator for text fields
    qmlRegisterType(QUrl("qrc:/qml/validator/BoundsDoubleValidator.qml"), "INGESCAPE", 1, 0, "BoundsDoubleValidator");

    // - Int32Validator for text fields
    qmlRegisterType(QUrl("qrc:/qml/validator/Int32Validator.qml"), "INGESCAPE", 1, 0, "Int32Validator");

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


    // Enable automatic High-DPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    // Print out general information about various parts of the scene graph and the graphics stack
    QLoggingCategory::setFilterRules("qt.scenegraph.general = true");



    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("IngeScape-Editor");
    app.setApplicationVersion(VERSION);

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);


    //------------------------------
    //
    // Root directory
    //
    //------------------------------

    // Get (and create if needed) the root path of our application ([DocumentsLocation]/IngeScape/)
    QString rootDirectoryPath = IngeScapeUtils::getRootPath();
    if (!rootDirectoryPath.isEmpty())
    {
        //------------------------------
        //
        // Logs
        //
        //------------------------------

        QString logFilePath = QString("%1log-IngeScape-Editor.csv").arg(rootDirectoryPath);

        // Set the log level from which logs are printed in the console
        //igs_setLogLevel(IGS_LOG_DEBUG);
        igs_setLogLevel(IGS_LOG_INFO);

        igs_setLogPath(logFilePath.toStdString().c_str());
        igs_setLogInFile(true);

#ifdef QT_DEBUG
        // Use a custom message format
        qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} %{if-category}%{category} %{endif}file://%{file}:%{line} %{message}");
#else
        // Replace the default message handler with our own logger
        qInstallMessageHandler(LogMessageHandler);
#endif

        qInfo() << "Application" << app.applicationName() << "is running with version" << app.applicationVersion();


        //------------------------------
        //
        // Settings
        //
        //------------------------------

        // Get (and create if needed) the settings path of our application ([DocumentsLocation]/IngeScape/Settings/)
        QString settingsDirectoryPath = IngeScapeUtils::getSettingsPath();
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
            IngeScapeSettings &settings = IngeScapeSettings::Instance(settingsFilePath);

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


    //
    // Register Ingescape Common QML types
    //
    IngeScapeCommon::registerIngeScapeQmlTypes();


    //
    // Register our custom QML types
    //
    registerCustomQmlTypes();



    //
    // Defines context properties
    //
    QQmlContext* qmlContext = engine.rootContext();
    if (qmlContext != nullptr)
    {
        // Build date
        qmlContext->setContextProperty("BUILD_DATE", QLocale(QLocale::C).toDate(QString(__DATE__).simplified(), QLatin1String("MMM d yyyy")));

        // Qt version against which the application is compiled
        qmlContext->setContextProperty("QT_BUILD_VERSION", QString(QT_VERSION_STR));

        // Runtime version of Qt. This may be a different version than the version the application was compiled against
        qmlContext->setContextProperty("QT_RUNTIME_VERSION", QString(qVersion()));
        
        // Version of our application
        qmlContext->setContextProperty("VERSION", app.applicationVersion());

        // Debug menu
#ifdef QT_DEBUG
        qmlContext->setContextProperty("SHOW_DEBUG_MENU", true);
#else
        qmlContext->setContextProperty("SHOW_DEBUG_MENU", false);
#endif
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
