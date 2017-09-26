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


#ifndef _I2_CPP_ONLY_PROPERTYHELPERS_H_
#define _I2_CPP_ONLY_PROPERTYHELPERS_H_

#include <QObject>
#include <QtQml>
#include <QQmlProperty>
#include <QtGlobal>



/*
 **************************************************************
 *
 *  Basic C++ properties (number, string, object (reference))
 *  that emit a signal when they change
 *
 **************************************************************
 */


/*!
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML)

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

    For example, I2_CPP_PROPERTY (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (_x != value) {
                _x = value;
                hasChanged = true
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
#define I2_CPP_PROPERTY(type, name) \
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
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML) that uses a fuzzy comparison

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

    For example, I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (!qFuzzyCompare(_x, value)) {
                _x = value;
                hashChanged = true;
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
#define I2_CPP_PROPERTY_FUZZY_COMPARE(type, name) \
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
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML) AND that is robust to deletion

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

    For example, I2_CPP_PROPERTY_DELETE_PROOF (QObject*, x) will generate the following code section:
    \code
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
                if (_x != NULL) {
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
#define I2_CPP_PROPERTY_DELETE_PROOF(type, name) \
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
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with a custom setter method that must be defined in .cpp file

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

    For example, I2_CPP_PROPERTY_CUSTOM_SETTER (qreal, x) will generate the following code section:
    \code
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
#define I2_CPP_PROPERTY_CUSTOM_SETTER(type, name) \
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
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with a custom setter method that must be defined in .cpp file.
   This method should return true if the property has changed, false otherwise

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

    For example, I2_CPP_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
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
#define I2_CPP_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN(type, name) \
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
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with custom getter and setter methods that must be defined in .cpp file

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

    For example, I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER (qreal, x) will generate the following code section:
    \code
    public:
        qreal x();
        virtual void setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER(type, name) \
    public: \
        type name (); \
        virtual void set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with custom getter and setter methods that must be defined in .cpp file

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

    For example, I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
    public:
        qreal x();
        virtual bool setx (qreal value);
    signals:
        void xChanged (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER_WITH_BOOL_RETURN(type, name) \
    public: \
        type name (); \
        virtual bool set##name (type value); \
    Q_SIGNALS: \
        void name##Changed (type value); \
    protected: \
        type _##name;






/*
 **************************************************************
 *
 *  Basic C++ properties (number, string, object (reference))
 *  that do not emit a signal when they change
 *
 **************************************************************
 */


/*!
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML)

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (_x != value) {
                _x = value;
                hasChanged = true
            }
            return hasChanged;
        }
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY(type, name) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (_##name != value) { \
                _##name = value; \
                hasChanged = true; \
            } \
            return hasChanged; \
        } \
    protected: \
        type _##name;


/*!
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML) that uses a fuzzy comparison

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_FUZZY_COMPARE (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value) {
            bool hasChanged = false;
            if (!qFuzzyCompare(_x, value)) {
                _x = value;
                hashChanged = true;
            }
            return hasChanged;
        }
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_FUZZY_COMPARE(type, name) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value) { \
            bool hasChanged = false; \
            if (!qFuzzyCompare(_##name, value)) { \
                _##name = value; \
                hasChanged = true; \
            } \
            return hasChanged; \
        } \
    protected: \
        type _##name;



/*!
   Define a property that is only readable/writable by C++ with a standard setter method
   (not accessible from QML) AND that is robust to deletion

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter will be implemented in the .h file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_DELETE_PROOF (QObject*, x) will generate the following code section:
    \code
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
                if (_x != NULL) {
                    connect(_x, SIGNAL(destroyed(QObject *)), this, SLOT(_onxDestroyed(QObject *)));
                }
                hasChanged = true;
            }
            return hasChanged;
        }
    private slots:
        void onxDestroyed(QObject*) {
            _x = NULL;
        }
    protected:
        QObject* _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_DELETE_PROOF(type, name) \
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
            } \
            return hasChanged; \
        } \
    private Q_SLOTS: \
        void _on##name##Destroyed(QObject*) { \
            _##name = NULL; \
        } \
    protected: \
        type _##name;


/*!
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with a custom setter method that must be defined in .cpp file

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        void set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_SETTER (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual void setx (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_SETTER(type, name) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual void set##name (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with a custom setter method that must be defined in .cpp file.
   This method should return true if the property has changed, false otherwise

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
    public:
        qreal x() const {
            return _x;
        }
        virtual bool setx (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_SETTER_WITH_BOOL_RETURN(type, name) \
    public: \
        type name () const { \
            return _##name ; \
        } \
        virtual bool set##name (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with custom getter and setter methods that must be defined in .cpp file

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        void set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_GETTER_AND_SETTER (qreal, x) will generate the following code section:
    \code
    public:
        qreal x();
        virtual void setx (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_GETTER_AND_SETTER(type, name) \
    public: \
        type name (); \
        virtual void set##name (type value); \
    protected: \
        type _##name;



/*!
   Define a property that is only readable/writable by C++ (not accessible from QML)
   with custom getter and setter methods that must be defined in .cpp file

   \param type The C++ type of our property
   \param name The name of our property

    It generates the following code :
    \code
        {type} _{name}; // Protected variable used to store the current value
        {type} {name}() const; // Public getter method
        bool set{name} ({type} value); // Public setter method
    \endcode

    NB: The setter must be implemented in your .cpp file
    NB: The property name is not capitalized in the setter name (x => setx and not setX)

    For example, I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_GETTER_AND_SETTER_WITH_BOOL_RETURN (qreal, x) will generate the following code section:
    \code
    public:
        qreal x();
        virtual bool setx (qreal value);
    protected:
        qreal _x;
    \endcode
*/
#define I2_CPP_NOSIGNAL_PROPERTY_CUSTOM_GETTER_AND_SETTER_WITH_BOOL_RETURN(type, name) \
    public: \
        type name (); \
        virtual bool set##name (type value); \
    protected: \
        type _##name;






#endif // _I2_CPP_ONLY_PROPERTYHELPERS_H_
