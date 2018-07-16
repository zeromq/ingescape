//
//  maincontroller.cpp
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QQmlApplicationEngine>

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MainController();

    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);

    /**
     * @brief Aims at sending a command to ingescape network
     * @param command
     */
    Q_INVOKABLE void sendMessage(QString command);

signals:

public slots:
};

#endif // MAINCONTROLLER_H
