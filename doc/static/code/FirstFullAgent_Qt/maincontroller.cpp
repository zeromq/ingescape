//
//  maincontroller.cpp
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#include "maincontroller.h"
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QDebug>

extern "C"{
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape/ingescape.h>
#include "regexp.h" //regexp utilities
}

/**
 * @brief Constructor
 * @param parent
 */
MainController::MainController(QObject *parent) : QObject(parent)
{

}

/**
 * @brief Destructor
 */
MainController::~MainController()
{

}

/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* MainController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new MainController();
}

/**
 * @brief Aims at sending a command to ingescape network
 * @param command
 */
void MainController::sendMessage(QString command){

    std::string commandString = command.toStdString();
    const char* message = commandString.c_str();

    if ((message[0] == '/')&&(strlen (message) > 1)) {
        //we have something to parse!
        matches_t my_matches;
        clean_matches(&my_matches); //initializes the matches_t structure
        regex_t r;

        const char *reg1 = "/([[:alnum:]]+)"; //single command
        const char *reg2 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)"; //command + parameter (single word)
        const char *reg3 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)[[:space:]]{1}([[:print:]]+)"; //command + parameter + string

        //command + parameter + string
        compile_regex(&r, reg3);
        match_regex(&r, message, &my_matches);
        regfree (&r);
        if (my_matches.nb > 0){
            char *command = my_matches.results[0];
            char *parameter = my_matches.results[1];
            char *message = my_matches.results[2];
            qDebug("command: %s\nparameter: %s\nmessage: %s\n",command,parameter,message);
            clean_matches(&my_matches);
        }
        //command + parameter (single word)
        compile_regex(&r, reg2);
        match_regex(&r, message, &my_matches);
        regfree (&r);
        if (my_matches.nb > 0){
            char *command = my_matches.results[0];
            char *parameter = my_matches.results[1];
            qDebug("command: %s\nparameter: %s\n",command,parameter);
            clean_matches(&my_matches);
        }
        //single command
        compile_regex(&r, reg1);
        match_regex(&r, message, &my_matches);
        regfree (&r);
        if (my_matches.nb > 0){
            char *command = my_matches.results[0];
            qDebug("command: %s\n",command);
            if (strcmp(command, "quit") == 0){
                QCoreApplication::exit();//simply stops the application
            }
            clean_matches(&my_matches);
        }
    }
}
