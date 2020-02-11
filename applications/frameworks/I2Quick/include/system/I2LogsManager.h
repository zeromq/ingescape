/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Luc Sorignet <sorignet@ingenuity.io>
 *
 */


#ifndef I2LOGSMANAGER_H
#define I2LOGSMANAGER_H

#include <QFile>
#include <QMutex>

// To export symbols
#include "i2quick_global.h"



/**
 * @brief The I2LogLevel enum : used to define the different level for log
 */
enum I2LogLevel {LOG_LEVEL_VERBOSE, LOG_LEVEL_INFO, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR, LOG_LEVEL_SILENT};


/**
 * @brief Handler
 * @param type
 * @param context
 * @param message
 */
I2QUICK_EXPORT void I2LogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);


/**
 * @brief The LogsManager class defines a manager for logs (allow to register logs in a file)
 */
class I2QUICK_EXPORT I2LogsManager
{
public:
    /**
     * @brief Public static instance
     * @return
     */
    static I2LogsManager &Instance()
    {
        static I2LogsManager singleton;
        return singleton;
    }


    /**
     * @brief openLogFile open a log File
     * @param path path where is store the log file
     * @param filename name of the log file
     * @param isAppend set true if you want concat log file false to overwrite
     */
    void openLogFile(QString path, QString filename, bool isAppend=true);


    /**
     * @brief closeLogFile close the log file
     */
    void closeLogFile();


    /**
     * @brief Register a log in a file
     * @param type qt message type (FATAL,DEBUG,WARN,INFO...)
     * @param message message (log)
     */
    void registerLog(QtMsgType type, const QString &message);


    /**
     * @brief set the log level by default log level is INFO (Debug is ignore)
     */
    void setLogLevel(I2LogLevel logLevel){this->_logLevel = logLevel;}

private:
    /**
     * @brief Constructor
     * @param parent
     */
     I2LogsManager();

    /**
     * Destructor
     */
    ~I2LogsManager();

    // Log file
    QFile *_logFile;

    QString _filePath;

    bool _isAppend;

    I2LogLevel _logLevel;

    QMutex _lockedFile;
};

#endif // I2LOGSMANAGER_H
