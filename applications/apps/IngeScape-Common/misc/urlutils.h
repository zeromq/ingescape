/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Mathieu Soum       <soum@ingenuity.io>
 *
 */

#ifndef URLUTILS_H
#define URLUTILS_H

#include <QObject>
#include <QtQml>
#include <QUrl>

/**
 * @brief Utility class to process paths and other URLs
 * This class is stateless and its methods are availale from QML
 */
class URLUtils : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Default QObject constructor
     * @param parent
     */
    explicit URLUtils(QObject *parent = nullptr) : QObject(parent) {}

    /**
     * @brief Defaulted destructor (since the class is stateless)
     */
    virtual ~URLUtils() = default;

    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine*, QJSEngine*) { return new URLUtils(); }

    /**
     * @brief Extract the filename of the given file:// URL.
     * Only QUrl with 'file://' as scheme or an empty scheme are accepted.
     * In other cases, an empry string is returned
     * @param url
     * @return
     */
    Q_INVOKABLE static QString fileNameFromFileUrl(const QUrl& url);
};

#endif // URLUTILS_H
