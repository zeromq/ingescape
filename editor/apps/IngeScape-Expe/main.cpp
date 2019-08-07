/*
 *	IngeScape Expe
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
#include <QtQml>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QtGlobal>
#include <QDate>

#include <I2Quick.h>

#include <controller/ingescapeexpecontroller.h>

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
    qmlRegisterSingletonType<RecordStates>("INGESCAPE", 1, 0, "RecordStates", &RecordStates::qmlSingleton);
    qmlRegisterSingletonType<TimeLineStates>("INGESCAPE", 1, 0, "TimeLineStates", &TimeLineStates::qmlSingleton);


    //----------------
    //
    // Controllers
    //
    //----------------
    // Singleton used as primary access point to our controllers
    qmlRegisterSingletonType<IngeScapeExpeController>("INGESCAPE", 1, 0, "IngeScapeExpeC", &IngeScapeExpeController::qmlSingleton);

    // sub-controllers
    qmlRegisterUncreatableType<ExpeModelManager>("INGESCAPE", 1, 0, "ExpeModelManager", "Internal Class");
    //qmlRegisterUncreatableType<ExperimentationController>("INGESCAPE", 1, 0, "ExperimentationController", "Internal Class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    qmlRegisterUncreatableType<PlatformAgentSortFilter>("INGESCAPE", 1, 0, "PlatformAgentSortFilter", "Internal class");


    //----------------
    //
    // Misc.
    //
    //----------------
    //qmlRegisterType<CollapsibleColumn>("INGESCAPE", 1, 0, "CollapsibleColumn");
    //qmlRegisterType<QQuickWindowBlockTouches>("INGESCAPE", 1, 0, "WindowBlockTouches");


    //----------------
    //
    // Models
    //
    //----------------
    qmlRegisterUncreatableType<PlatformM>("INGESCAPE", 1, 0, "PlatformM", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    //qmlRegisterUncreatableType<ExperimentationsGroupVM>("INGESCAPE", 1, 0, "ExperimentationsGroupVM", "Internal class");


    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeExpeTheme.qml"), "INGESCAPE", 1, 0, "IngeScapeExpeTheme");

    // - Radio Button style
    //qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeRadioButtonStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeRadioButtonStyle");

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
    app.setApplicationName("IngeScape-Expe");
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

        QString logFilePath = QString("%1log-IngeScape-Expe.csv").arg(rootDirectoryPath);

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

        // Get (and create if needed) the settings path of our application ([DocumentsLocation]/IngeScape/settings/)
        QString settingsDirectoryPath = IngeScapeUtils::getSettingsPath();
        QString settingsFilePath = QString("%1IngeScape-Expe.ini").arg(settingsDirectoryPath);

        QFile settingsFile(settingsFilePath);
        if (!settingsFile.exists())
        {
            // Copy our .ini file
            if (QFile::copy(":/settings/IngeScape-Expe.ini", settingsFilePath))
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

