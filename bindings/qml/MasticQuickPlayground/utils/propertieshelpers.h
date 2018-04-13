/*
 *  Mastic - QML playground
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _PROPERTIES_HELPERS_H_
#define _PROPERTIES_HELPERS_H_


#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QtGlobal>
#include <QMetaType>




/*
 **************************************************************
 *
 *  Basic QML properties (number, string, etc.)
 *
 **************************************************************
 */


/*!
   Define a property that is readable/writable from QML
*/
#define PLAYGROUND_QML_PROPERTY(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value) { \
            if (_##name != value) { \
                _##name = value; \
                Q_EMIT name##Changed(value); \
            } \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;




/*!
   Define a property that is readable/writable from QML with a custom setter method
*/
#define PLAYGROUND_QML_PROPERTY_CUSTOM_SETTER(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is read-only from QML
*/
#define PLAYGROUND_QML_PROPERTY_READONLY(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value) { \
            if (_##name != value) { \
                _##name = value; \
                Q_EMIT name##Changed(value); \
            } \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;


/*!
   Define a property that is read-only from QML with a custom setter
*/
#define PLAYGROUND_QML_PROPERTY_READONLY_CUSTOM_SETTER(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;


/*!
   Define a constant
*/
#define PLAYGROUND_QML_PROPERTY_CONSTANT(type, name) \
        Q_PROPERTY (type name READ name CONSTANT) \
    public: \
        type name () const { \
            return _##name ; \
        } \
    protected: \
        type _##name;




/*
 **************************************************************
 *
 *  Pointers
 *
 **************************************************************
 */

/*!
   Define a property that is readable/writable from QML
*/
#define PLAYGROUND_QML_PROPERTY_DELETE_PROOF(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value) { \
            if (_##name != value) { \
                if (_##name != NULL) { \
                    disconnect(_##name, SIGNAL(destroyed(QObject *)), this, 0); \
                } \
                _##name = value; \
                if (_##name != NULL) { \
                    connect(_##name, SIGNAL(destroyed(QObject *)), this, SLOT(_on##name##Destroyed(QObject *))); \
                } \
                Q_EMIT name##Changed(value); \
            } \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    private Q_SLOTS: \
        void _on##name##Destroyed(QObject*) { \
            _##name = NULL; \
            Q_EMIT name##Changed(NULL); \
        } \
    protected: \
        type _##name;



/*!
   Define a property that is read-only from QML
*/
#define PLAYGROUND_QML_PROPERTY_READONLY_DELETE_PROOF(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        void set##name (type value) { \
            if (_##name != value) { \
                if (_##name != NULL) {\
                    disconnect(_##name, SIGNAL(destroyed(QObject *)), this, 0); \
                } \
                _##name = value; \
                if (_##name != NULL) { \
                    connect(_##name, SIGNAL(destroyed(QObject *)), this, SLOT(_on##name##Destroyed(QObject *))); \
                } \
                Q_EMIT name##Changed(value); \
            } \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    private Q_SLOTS: \
        void _on##name##Destroyed(QObject*) { \
            _##name = NULL; \
            Q_EMIT name##Changed(NULL); \
        } \
    protected: \
        type _##name;






/*
 **************************************************************
 *
 *  Enum QML properties
 *
 **************************************************************
 */



/*!
  Defines a new class that provides an enumeration to QML (and C++)
  with a custom 'enumToString' method

  \param name The name of our enum class
  \param list of all values

  */
#define PLAYGROUND_QML_ENUM(name, ...) \
    class name : public AbstractPlaygroundEnumClass { \
        Q_OBJECT \
    public: \
        enum Value { __VA_ARGS__ }; \
        Q_ENUMS (Value) \
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
        static void registerMetaType() { \
            _registerMetaType<name::Value>(); \
        } \
    }; \
    QML_DECLARE_TYPE(name) \
    Q_DECLARE_METATYPE(name::Value)




/*!
    Base class of enums
 */
class AbstractPlaygroundEnumClass: public QObject {
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    AbstractPlaygroundEnumClass(QObject *parent = 0);


    /**
     * @brief Call this function to register the type T
     *
     * @remark We use this function as a proxy to qRegisterMetaType because we can not call
     *         qRegisterMetaType in macros. Otherwise, it does not compile due to
     *         "explicit specialization of 'QMetaTypeId<MasticLogLevel::Value>' after instantiation" errors
     */
    template <class T> static void _registerMetaType()
    {
        qRegisterMetaType<T>();
    }


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




#endif // _PROPERTIES_HELPERS_H_
