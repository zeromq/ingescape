/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
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
#include <QDebug>
#include <QtGlobal>
#include <QDate>

#include <I2Quick.h>

#include <controller/ingescapeassessmentscontroller.h>

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
    qmlRegisterSingletonType<CharacteristicValueTypes>("INGESCAPE", 1, 0, "CharacteristicValueTypes", &CharacteristicValueTypes::qmlSingleton);
    qmlRegisterSingletonType<IndependentVariableValueTypes>("INGESCAPE", 1, 0, "IndependentVariableValueTypes", &IndependentVariableValueTypes::qmlSingleton);


    //----------------
    //
    // Controllers
    //
    //----------------
    // Singleton used as primary access point to our controllers
    qmlRegisterSingletonType<IngeScapeAssessmentsController>("INGESCAPE", 1, 0, "IngeScapeAssessmentsC", &IngeScapeAssessmentsController::qmlSingleton);

    // sub-controllers
    qmlRegisterUncreatableType<AssessmentsModelManager>("INGESCAPE", 1, 0, "AssessmentsModelManager", "Internal Class");
    qmlRegisterUncreatableType<ExperimentationController>("INGESCAPE", 1, 0, "ExperimentationController", "Internal Class");
    qmlRegisterUncreatableType<ExperimentationsListController>("INGESCAPE", 1, 0, "ExperimentationsListController", "Internal Class");
    qmlRegisterUncreatableType<ExportController>("INGESCAPE", 1, 0, "ExportController", "Internal Class");
    qmlRegisterUncreatableType<TaskInstanceController>("INGESCAPE", 1, 0, "TaskInstanceController", "Internal Class");
    qmlRegisterUncreatableType<SubjectsController>("INGESCAPE", 1, 0, "SubjectsController", "Internal Class");
    qmlRegisterUncreatableType<TasksController>("INGESCAPE", 1, 0, "TasksController", "Internal Class");


    //----------------
    //
    // Sort and Filter
    //
    //----------------
    //qmlRegisterUncreatableType<LogsSortFilter>("INGESCAPE", 1, 0, "LogsSortFilter", "Internal class");
    //qmlRegisterUncreatableType<ValuesHistorySortFilter>("INGESCAPE", 1, 0, "ValuesHistorySortFilter", "Internal class");


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
    qmlRegisterUncreatableType<AgentNameAndOutputsM>("INGESCAPE", 1, 0, "AgentNameAndOutputsM", "Internal class");
    qmlRegisterUncreatableType<AssessmentsEnums>("INGESCAPE", 1, 0, "AssessmentsEnums", "Internal class");
    qmlRegisterUncreatableType<CharacteristicM>("INGESCAPE", 1, 0, "CharacteristicM", "Internal class");
    qmlRegisterUncreatableType<DependentVariableM>("INGESCAPE", 1, 0, "DependentVariableM", "Internal class");
    qmlRegisterUncreatableType<ExperimentationM>("INGESCAPE", 1, 0, "ExperimentationM", "Internal class");
    qmlRegisterUncreatableType<IndependentVariableM>("INGESCAPE", 1, 0, "IndependentVariableM", "Internal class");
    qmlRegisterUncreatableType<TaskInstanceM>("INGESCAPE", 1, 0, "TaskInstanceM", "Internal class");
    qmlRegisterUncreatableType<SubjectM>("INGESCAPE", 1, 0, "SubjectM", "Internal class");
    qmlRegisterUncreatableType<TaskM>("INGESCAPE", 1, 0, "TaskM", "Internal class");
    qmlRegisterUncreatableType<QQmlPropertyMap>("INGESCAPE", 1, 0, "QQmlPropertyMap", "Internal class");


    //---------------
    //
    // View Models
    //
    //---------------
    qmlRegisterUncreatableType<ExperimentationsGroupVM>("INGESCAPE", 1, 0, "ExperimentationsGroupVM", "Internal class");


    //------------------
    //
    // QML components
    //
    //------------------
    // - Singleton used to enable consistent app styling through predefined colors, fonts, etc.
    qmlRegisterSingletonType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsTheme.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsTheme");

    // - Radio Button style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeRadioButtonStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeRadioButtonStyle");

    // - Combobox style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsComboboxStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsComboboxStyle");

    // - Button Style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsButtonStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsButtonStyle");
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsSvgButtonStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsSvgButtonStyle");
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsSvgAndTextButtonStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsSvgAndTextButtonStyle");

    // - ScrollView style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsScrollViewStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsScrollViewStyle");

    // - TextArea style
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsTextAreaStyle.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsTextAreaStyle");

    // - List Header
    qmlRegisterType(QUrl("qrc:/qml/theme/IngeScapeAssessmentsListHeader.qml"), "INGESCAPE", 1, 0, "IngeScapeAssessmentsListHeader");

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


#ifdef Q_OS_WIN
    // Fix crash with some Intel graphic cards (e.g. Intel HD Graphics 620)
    // Qt bug https://bugreports.qt.io/browse/QTBUG-64697
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
#endif


    //------------------------------
    //
    // Configure our application
    //
    //------------------------------
    QApplication app(argc, argv);
    app.setOrganizationName("Ingenuity i/o");
    app.setOrganizationDomain("ingenuity.io");
    app.setApplicationName("IngeScape-Assessments");
    app.setApplicationVersion(VERSION);

    // - behavior when our last window is closed
    app.setQuitOnLastWindowClosed(true);

    // Set the name of our agent
    igs_setAgentName(app.applicationName().toStdString().c_str());

    //------------------------------
    //
    // Snippet to test the integration of the exporting code
    // TOREMOVE : only for ESTIA you can remove it when you want
    //
    //------------------------------

    //ExportController exportControllerTest;
    //exportControllerTest.exportExperimentationTest("cf113ce0-9bda-11e9-9d27-8dfc860aae8b");


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

        //QString logsDirectoryPath = IngeScapeUtils::getLogsPath();
        //QString logFilePath = QString("%1%2_log.csv").arg(logsDirectoryPath, app.applicationName());
        //igs_setLogPath(logFilePath.toStdString().c_str());

        igs_setVerbose(true);
        igs_setLogInFile(true);

#ifdef QT_DEBUG
        igs_setLogLevel(IGS_LOG_DEBUG);

        // Use a custom message format
        qSetMessagePattern("%{time hh:mm:ss.zzz} %{type} %{if-category}%{category} %{endif}file://%{file}:%{line} %{message}");
#else
        igs_setLogLevel(IGS_LOG_INFO);

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
        QString settingsFilePath = QString("%1IngeScape-Assessments.ini").arg(settingsDirectoryPath);

        QFile settingsFile(settingsFilePath);
        if (!settingsFile.exists())
        {
            // Copy our .ini file
            if (QFile::copy(":/settings/IngeScape-Assessments.ini", settingsFilePath))
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


        //
        // Specific context properties
        //
        qmlContext->setContextProperty("CHARACTERISTIC_SUBJECT_ID", CHARACTERISTIC_SUBJECT_ID);
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
