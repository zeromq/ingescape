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


#ifndef _I2_QML_PROPERTYHELPERS_H_
#define _I2_QML_PROPERTYHELPERS_H_


#include <QObject>
#include <QtQml>
#include <QQmlProperty>
#include <QtGlobal>





/*
 **************************************************************
 *
 *  Basic QML properties (number, string, object)
 *
 **************************************************************
 */


/*!
   Define a property that is readable/writable from QML with a standard setter method

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x WRITE setx NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (_x != value) {
                _x = value;
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (_##name != value) { \
                _##name = value; \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;


/*!
   Define a property that is readable/writable from QML with a standard setter method
   that use qFuzzyCompare (fuzzy equal comparison)
   NB: it should be used for double and float numbers

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_FUZZY_COMPARE (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x WRITE setx NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (!qFuzzyCompare(_x, value)) {
                _x = value;
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_FUZZY_COMPARE(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (!qFuzzyCompare(_##name, value)) { \
                _##name = value; \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is readable/writable from QML with a standard setter method
   AND that is robust to deletion

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_DELETE_PROOF (QObject*, x) will generate the following code section:
    \code
        Q_PROPERTY (QObject* x READ x WRITE setx NOTIFY xChanged)
    public:
        QObject* x() const {
            return _x;
        }
        virtual bool setx (QObject* value) {
            bool hasChanged = false;
            if (_x != value) {
                if (_x != NULL) {
                    disconnect(_x, SIGNAL(destroyed(QObject *)), this, 0);
                }
                _x = value;
                if (_x != 0) {
                    connect(_x, SIGNAL(destroyed(QObject *)), this, SLOT(_onxDestroyed(QObject *)));
                }
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (QObject* value);
    private slots:
        void onxDestroyed(QObject*) {
            _x = NULL;
            Q_EMIT xChanged(NULL);
        }
    protected:
        QObject* _x;
    \endcode
*/
#define I2_QML_PROPERTY_DELETE_PROOF(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (_##name != value) { \
                if (_##name != NULL) { \
                    disconnect(_##name, SIGNAL(destroyed(QObject *)), this, 0); \
                } \
                _##name = value; \
                if (_##name != NULL) { \
                    connect(_##name, SIGNAL(destroyed(QObject *)), this, SLOT(_on##name##Destroyed(QObject *))); \
                } \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
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
   Define a property that is readable/writable from QML with a custom setter method that must be defined
   in .cpp file

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        void set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_CUSTOM_SETTER (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x WRITE setx NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        virtual void setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_CUSTOM_SETTER(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual void set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is readable/writable from QML with a custom setter method that must be defined
   in .cpp file

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x WRITE setx NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN(type, name) \
        Q_PROPERTY (type name READ name WRITE set##name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is readable from QML and readable/writable from C++
   with a standard setter method

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method (QMl and C++)
        bool set{name} ({type} value); // Public setter method (C++ only)
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_READONLY (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        bool setx (qreal value) {
            bool hasChanged = false;
            if (_x != value) {
                _x = value;
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_READONLY(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        bool set##name (type value) { \
            bool hasChanged = false; \
            if (_##name != value) { \
                _##name = value; \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is readable from QML and readable/writable from C++
   with a standard setter method that uses a fuzzy comparison

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method (QMl and C++)
        bool set{name} ({type} value); // Public setter method (C++ only)
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_READONLY_FUZZY_COMPARE (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        bool setx (qreal value) {
            bool hasChanged = false;
            if (!qFuzzyCompare(_x, value)) {
                _x = value;
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_READONLY_FUZZY_COMPARE(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        bool set##name (type value) { \
            bool hasChanged = false; \
            if (!qFuzzyCompare(_##name, value)) { \
                _##name = value; \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
        } \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;


/*!
   Define a property that is readable from QML and readable/writable from C++
   with a standard setter method AND that is robust to deletion

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_READONLY_DELETE_PROOF (QObject*, x) will generate the following code section:
    \code
        Q_PROPERTY (QObject* x READ x NOTIFY xChanged)
    public:
        QObject* x() const {
            return _x;
        }
        virtual bool setx (QObject* value) {
            bool hasChanged = false;
            if (_x != value) {
                if (_x != NULL) {
                    disconnect(_x, SIGNAL(destroyed(QObject *)), this, 0);
                }
                _x = value;
                if (_x != 0) {
                    connect(_x, SIGNAL(destroyed(QObject *)), this, SLOT(_onxDestroyed(QObject *)));
                }
                hasChanged = true;
                Q_EMIT xChanged(value);
            }
            return hasChanged;
        }
    signals:
        void xChanged (QObject* value);
    private slots:
        void onxDestroyed(QObject*) {
            _x = NULL;
            Q_EMIT xChanged(NULL);
        }
    protected:
        QObject* _x;
    \endcode
*/
#define I2_QML_PROPERTY_READONLY_DELETE_PROOF(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (_##name != value) { \
                if (_##name != NULL) {\
                    disconnect(_##name, SIGNAL(destroyed(QObject *)), this, 0); \
                } \
                _##name = value; \
                if (_##name != NULL) { \
                    connect(_##name, SIGNAL(destroyed(QObject *)), this, SLOT(_on##name##Destroyed(QObject *))); \
                } \
                hasChanged = true; \
                Q_EMIT name##Changed(value); \
            } \
            return hasChanged; \
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
   Define a property that is read-only from QML and readable/writable from C++
   with a standard setter method

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method (QML and C++)
        void set{name} ({type} value); // Public setter method (C++ only)
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_READONLY_CUSTOM_SETTER (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        void setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(type, name) \
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
   Define a property that is read-only from QML and readable/writable from C++
   with a standard setter method

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method (QML and C++)
        bool set{name} ({type} value); // Public setter method (C++ only)
        void {name}Changed ({type} value); // Notifier signal
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_QML_PROPERTY_READONLY_CUSTOM_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
        Q_PROPERTY (qreal x READ x NOTIFY xChanged)
    public:
        qreal x() const {
            return _x;
        }
        bool setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_QML_PROPERTY_READONLY_CUSTOM_SETTER_WITH_BOOL_RETURN(type, name) \
        Q_PROPERTY (type name READ name NOTIFY name##Changed) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        bool set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;




#endif // _I2_QML_PROPERTYHELPERS_H_
