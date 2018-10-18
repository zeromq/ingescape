/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


/*!
    This header defines a couple of helper macros that can help saving development time,
    by avoiding manual code duplication, often leading to heavy copy-and-paste, which is
    largely error-prone and not productive at all.
*/


#ifndef _I2_ENUM_PROPERTYHELPERS_H_
#define _I2_ENUM_PROPERTYHELPERS_H_


#include <QObject>
#include <QtQml>
#include <QQmlProperty>
#include <QtGlobal>
#include <QMetaType>


#include "i2quick_global.h"
#include "base/I2EnumListModel.h"



/*
 **************************************************************
 *
 *  List of enum values
 *
 **************************************************************
 */


/*!
   Define a list property that can contain enum values
   The list can be associated to a sort-filter proxy based on type I2EnumListItemData
   (a QObject class used to wrap enum values)

   \param type The C++ enum class associated to our list property. The enum class must be defined with I2_ENUM macros
   \param name The name of our property

    It generates the following code :
    \code
        AbstractI2EnumListModel* {name}(); // Public getter method
        I2EnumListModel<{type} *> _{name}; // Protected list used to store our items
    \endcode
*/
#define I2_ENUM_LISTMODEL(type, name) \
      Q_PROPERTY(AbstractI2EnumListModel * name READ name CONSTANT) \
    public: \
        AbstractI2EnumListModel * name() { \
            return &_##name; \
        } \
    protected: \
        I2EnumListModel<type> _##name;



/*
 **************************************************************
 *
 *  Enums
 *
 **************************************************************
 */


/*!
  Defines a new class that provides an enumeration to QML (and C++)
  with generic methods to convert an enum value to string and vice-versa

  \param name The name of our enum class
  \param list of all values


  For example
  \code
  I2_ENUM (Fruit, APPLE, BANANA, KIWI) will generate the following code section:
  \endcode

  NB: To use the 'enumToString' method in QML, the enum class must be registered as a singleton.
      If it is registered as an uncreatable type, the 'enumToString' method will not be available in QML

  \code
  Fruit::qmlRegister("MyUri", 1, 0);
  \endcode

  */
#define I2_ENUM(name, ...) \
    class name : public I2AbstractQmlEnumClass { \
        Q_OBJECT \
    public: \
        explicit name(QObject* parent = 0) : I2AbstractQmlEnumClass(parent) { \
        } \
        enum Value { __VA_ARGS__ }; \
        Q_ENUM (Value) \
        static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) { \
            Q_UNUSED(engine); \
            Q_UNUSED(scriptEngine); \
            return new name (); \
        } \
        int getEnumeratorIndex() Q_DECL_OVERRIDE { \
            return staticMetaObject.indexOfEnumerator("Value"); \
        } \
        QMetaEnum getEnumerator(int index) Q_DECL_OVERRIDE { \
            return staticMetaObject.enumerator(index); \
        } \
        Q_INVOKABLE QList<name::Value> allValues() { \
            QList<name::Value> result; \
            static int enumIndex = getEnumeratorIndex(); \
            if (enumIndex != -1) { \
                QMetaEnum enumType = getEnumerator(enumIndex); \
                for (int index = 0; index < enumType.keyCount(); ++index) { \
                    name::Value currentEnumItemValue = static_cast<name::Value>(enumType.value(index)); \
                    result.append(currentEnumItemValue); \
                } \
            } \
            return result; \
        } \
        static QString staticEnumToString(int value) { \
            name instance; \
            return instance.enumToString(value); \
        } \
        static QString staticEnumToKey(int value) { \
            name instance; \
            return instance.enumToKey(value); \
        } \
        static int staticEnumFromKey(QString key) { \
            name instance; \
            return instance.enumFromKey(key); \
        } \
        static QStringList staticAllKeys() { \
            name instance; \
            return instance.allKeys(); \
        } \
        static QList<name::Value> staticAllValues() { \
            name instance; \
            return instance.allValues(); \
        } \
        static void qmlRegister(const char* uri, int versionMajor, int versionMinor, const char* qmlName = #name) { \
            qmlRegisterSingletonType<name>(uri, versionMajor, versionMinor, qmlName, &name::qmlSingleton); \
        } \
    }; \
    QML_DECLARE_TYPE(name) \
    Q_DECLARE_METATYPE(name::Value)



/*!
  Defines a new class that provides an enumeration to QML (and C++)
  with a custom 'enumToString' method

  \param name The name of our enum class
  \param list of all values


  For example

  \code
  I2_ENUM_CUSTOM (Fruit, APPLE, BANANA, KIWI) will generate the following code section:
  \endcode

  NB: The method 'enumToString' method must be implemented in your .cpp file

  NB: To use the 'enumToString' method in QML, the enum class must be registered as a singleton.
      If it is registered as an uncreatable type, the 'enumToString' method will not be available in QML

  \code
  Fruit::qmlRegister("MyUri", 1, 0);
  \endcode

  */
#define I2_ENUM_CUSTOM(name, ...) \
    class name : public I2AbstractQmlEnumClass { \
        Q_OBJECT \
    public: \
        enum Value { __VA_ARGS__ }; \
        Q_ENUM (Value) \
        static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) { \
            Q_UNUSED(engine); \
            Q_UNUSED(scriptEngine); \
            return new name (); \
        } \
        Q_INVOKABLE virtual QString enumToString(int value) Q_DECL_OVERRIDE; \
        int getEnumeratorIndex() Q_DECL_OVERRIDE { \
            return staticMetaObject.indexOfEnumerator("Value"); \
        } \
        QMetaEnum getEnumerator(int index) Q_DECL_OVERRIDE { \
            return staticMetaObject.enumerator(index); \
        } \
        Q_INVOKABLE QList<name::Value> allValues() { \
            QList<name::Value> result; \
            static int enumIndex = getEnumeratorIndex(); \
            if (enumIndex != -1) { \
                QMetaEnum enumType = getEnumerator(enumIndex); \
                for (int index = 0; index < enumType.keyCount(); ++index) { \
                    name::Value currentEnumItemValue = static_cast<name::Value>(enumType.value(index)); \
                    result.append(currentEnumItemValue); \
                } \
            } \
            return result; \
        } \
        static QString staticEnumToString(int value) { \
            name instance; \
            return instance.enumToString(value); \
        } \
        static QString staticEnumToKey(int value) { \
            name instance; \
            return instance.enumToKey(value); \
        } \
        static int staticEnumFromKey(QString key) { \
            name instance; \
            return instance.enumFromKey(key); \
        } \
        static QStringList staticAllKeys() { \
            name instance; \
            return instance.allKeys(); \
        } \
        static QList<name::Value> staticAllValues() { \
            name instance; \
            return instance.allValues(); \
        } \
        static void qmlRegister(const char* uri, int versionMajor, int versionMinor, const char* qmlName = #name) { \
            qmlRegisterSingletonType<name>(uri, versionMajor, versionMinor, qmlName, &name::qmlSingleton); \
        } \
    }; \
    QML_DECLARE_TYPE(name) \
    Q_DECLARE_METATYPE(name::Value)



/*!
  NB: If we create a macro with Q_GADGET or Q_OBJECT, it generates a compilation error if we
      don't define a real class    "Note: No relevant classes found. No output generated"

  Thus, we define at least one real QObject class in this header file.
  That's why we define the base class of enums in this header instead of using another header file
 */
class I2QUICK_EXPORT I2AbstractQmlEnumClass: public QObject {
    Q_OBJECT

public:
    I2AbstractQmlEnumClass(QObject *parent = 0);


    /**
     * @brief Get index of our enumerator
     * @return
     */
    virtual int getEnumeratorIndex() = 0;


    /**
     * @brief Get a given enumerator
     * @param index
     * @return
     */
    virtual QMetaEnum getEnumerator(int index) = 0;


    /**
     * @brief Convert an enum value into a string
     * @param value
     * @return
     */
    Q_INVOKABLE virtual QString enumToString(int value)
    {
      // If 'enumToString' is not overridden, there is no difference between the key (C++ identifier)
      // and the string representation of an enum value
      return enumToKey(value);
    }


    /**
     * @brief Convert an enum value into a key
     * @param value
     * @return Key of our enum i.e. the identifier of our enum value as declared in C++
     */
    Q_INVOKABLE QString enumToKey(int value)
    {
        QString result;

        int enumIndex = getEnumeratorIndex();
        if (enumIndex != -1)
        {
            QMetaEnum enumType = getEnumerator(enumIndex);
            result = QString::fromLatin1(enumType.valueToKey(value));
        }
        // Else our child class does not define an enum named "Value"
        // NB: It should not happen if our enum class is created with our macro

        return result;
    }


    /**
     * @brief Get an enum value from an identifier
     * @param key Key of our enum i.e. the identifier of our enum value as declared in C++
     * @return -1 if no value is associated to this key, otherwise it returns the enum value
     */
    Q_INVOKABLE virtual int enumFromKey(QString key)
    {
      int result = -1;

      static int enumIndex = getEnumeratorIndex();
      if (enumIndex != -1)
      {
        QMetaEnum enumType = getEnumerator(enumIndex);
        bool ok;
        result = enumType.keyToValue(key.toLatin1(), &ok);
        if (!ok)
        {
          result = -1;
        }
      }
      // Else our child class does not define an enum named "Value"
      // NB: It should not happen if our enum class is created with our macro

      return result;
    }


    /**
     * @brief Get all keys defined by our enums
     * @return
     */
    Q_INVOKABLE QStringList allKeys()
    {
        QStringList result;

        static int enumIndex = getEnumeratorIndex();
        if (enumIndex != -1)
        {
            QMetaEnum enumType = getEnumerator(enumIndex);

            for (int index = 0; index < enumType.keyCount(); ++index)
            {
                QString currentEnumItemKey = QString::fromLatin1(enumType.key(index));
                result.append(currentEnumItemKey);
            }
        }

        return result;
    }
};



#endif // _I2_ENUM_PROPERTYHELPERS_H_
