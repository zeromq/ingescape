/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef I2STRINGUTILS_H
#define I2STRINGUTILS_H

#include <QObject>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2StringUtils class defines a set of utility functions to manipulate strings
 */
class I2QUICK_EXPORT I2StringUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2StringUtils(QObject *parent = nullptr);


    /**
     * @brief Remove all accents i.e. replace all diacritic mark with standard characters
     *        e.g. "é" => "e"
     * @param value
     * @return
     */
    static QString unaccent(const QString &value);

    /**
     * @brief Add whitespaces before capitalize letters ie. "AppleBanana" => "Apple Banana"
     * @param value
     * @return
     */
    static QString uncamelize(const QString &value);

    /**
     * @brief uppercase the first caracter of a given string;
     * @param value
     * @return
     */
    static QString ucFirst(const QString &value);

    /**
     * @brief uppercase the first caracter of each word in a given string;
     * @param value
     * @return
     */
    static QString ucWords(const QString &value);

    /**
     * @brief lowercase the first caracter of a given string;
     * @param value
     * @return
     */
    static QString lcFirst(const QString &value);

    /**
     * @brief lowercase the first caracter of each word in a given string;
     * @param value
     * @return
     */
    static QString lcWords(const QString &value);

    /**
     * @brief generate a random string
     * @param length of generate string
     * @param withNumeric default true
     * @param withAlpha default true
     * @param withUpperCase default true
     * @param withSpecial default true
     * @param withSpace default false
     * @return generated String
     */
    static QString generateString(int length = 12, bool withNumeric = true, bool withAlpha = true , bool withUpperCase = true , bool withSpecial = true, bool withSpace = false);

};

#endif // I2STRINGUTILS_H
