/*
 *	I2Quick
 *
 *  Copyright (c) 2018-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */



#ifndef I2DYNAMICTRANSLATOR_H
#define I2DYNAMICTRANSLATOR_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>
#include <QTranslator>

#include "I2PropertyHelpers.h"

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2DynamicTranslator class is a singleton used to allow dynamic language switching in a QML application
 */
class I2QUICK_EXPORT I2DynamicTranslator : public QObject
{
    Q_OBJECT

    // Current language
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, language)

    // Property used to refresh calls to qsTr()
    I2_QML_PROPERTY_READONLY(QString, refreshFlag)

    // Our QML engine
    I2_CPP_PROPERTY_DELETE_PROOF(QQmlEngine*, qmlEngine)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2DynamicTranslator(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~I2DynamicTranslator();


    /**
     * @brief Get our singleton instance
     * @return
     */
    static I2DynamicTranslator* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);



public Q_SLOTS:
     /**
     * @brief Load all translations files in a given directory
     * @param directory
     * @param prefix
     */
    void loadAllTranslationFiles(QString directory, QString prefix);


    /**
     * @brief Load a specific translation file
     * @param file
     * @param language
     */
    void loadTranslationFile(QString file, QString language);


    /**
     * @brief Init with a given locale
     * @param locale
     */
    void initWithLocale(QLocale locale);


    /**
     * @brief Clear all loaded translation files and reset language
     */
    void clear();


protected:
    /**
     * @brief Clear all loaded translation files
     */
    void _clear();


protected:
    // Loaded translation files
    QHash<QString, QList<QTranslator*>> _loadedTranslationFilesByLanguage;
};

QML_DECLARE_TYPE(I2DynamicTranslator)


#endif // I2DYNAMICTRANSLATOR_H
