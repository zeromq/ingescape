/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2CUSTOMITEMHASHMODEL_H_
#define _I2CUSTOMITEMHASHMODEL_H_

#include <QAbstractListModel>
#include <QObject>
#include <QtQml>
#include <QDebug>
#include <QMetaProperty>
#include <QQmlEngine>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2CustomHashItemData class is used to store infos (key, value) in our hashtable
 */
class I2QUICK_EXPORT I2CustomItemHashPair : public QObject
{
    Q_OBJECT

    // Key
    Q_PROPERTY (QString key READ key)

    // Value
    Q_PROPERTY (QObject* value READ value WRITE setValue NOTIFY valueChanged)

public:
    /**
     * @brief Constructor
     * @param key
     * @param value
     * @param parent
     */
    explicit I2CustomItemHashPair (QString key, QObject* value, QObject* parent = 0);

    /**
      * @brief Destructor
      */
    ~I2CustomItemHashPair();

    /**
     * @brief Get the key of our pair
     * @return
     */
    QString key() const;


    /**
     * @brief Value of our pair
     * @return
     */
    QObject* value() const;


    /**
     * @brief Set our value
     * @param value
     */
    void setValue(QObject* value);


Q_SIGNALS:
    /**
     * @brief Signal used to notify that our value has changed
     * @param value
     */
    void valueChanged(QObject* value);

    /**
     * @brief Signal used to notify that our value item has been deleted
     */
    void valueItemDeleted(I2CustomItemHashPair* pair);

protected Q_SLOTS:
    /**
     * @brief Called when our value item is deleted
     * @param value
     */
    void _onValueItemExternalDeletion(QObject* value);

protected:
    QString _key;
    QObject* _value;
};

QML_DECLARE_TYPE(I2CustomItemHashPair)




/**
 * @brief AbstractI2CustomItemHashModel defines the base class of our template for hashtable models
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 */
class I2QUICK_EXPORT AbstractI2CustomItemHashModel : public QAbstractListModel
{
    Q_OBJECT

    // count property to mimic QQmlListProperty
    Q_PROPERTY (int count READ count NOTIFY countChanged)

    // List of keys
    Q_PROPERTY (QStringList keys READ keys NOTIFY keysChanged)

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit AbstractI2CustomItemHashModel (QObject* parent = 0);


public: // Helpers to switch between role and property name

    /**
     * @brief Get the role associated to a given property name
     * @param name
     * @return
     */
    virtual int roleForName(const QByteArray &name) = 0;

    /**
     * @brief Get the property name associated to a given role
     * @param role
     * @return
     */
    virtual QByteArray nameForRole(int role) = 0;


public: // Hash API

    /**
     * @brief Get the size of our list
     * @return
     * NB: Equivalent to count()
     */
    virtual int size() const = 0;

    /**
     * @brief Get the size of our list
     * @return
     * NB: equivalent to size()
     */
    virtual int count() const = 0;

    /**
     * @brief Check if our list is empty
     * @return
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    Q_INVOKABLE virtual QObject* get(int index) = 0;


    /**
     * @brief Get all keys
     * @return
     */
    virtual QStringList keys() const = 0;

    /**
     * @brief Get the item associated to a given key
     * @param key
     * @return
     */
    Q_INVOKABLE virtual QObject* value(QString key) = 0;


    /**
     * @brief Get all items stored in our hash
     * @param key
     * @return
     */
    virtual QList<QObject*> values() = 0;


    /**
     * @brief Insert an item with a given key
     * @param key
     * @param item
     */
    virtual void insert(QString key, QObject* item) = 0;


    /**
     * @brief Insert all items of a given hash model
     *
     * @param hashModel
     */
    virtual void insertContentOfCustomHashModel(AbstractI2CustomItemHashModel* hashModel) = 0;


    /**
     * @brief Remove a given key
     * @param key
     *
     * NB: item will be removed from our hash BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE virtual void removeKey(QString key) = 0;


    /**
     * @brief Check if a given key is in our hash
     * @param item
     * @return
     */
    Q_INVOKABLE virtual bool containsKey(QString key) const = 0;


    /**
     * @brief Removes all items from our hash
     *
     * NB: items will be removed from our hash BUT WILL NOT BE DELETED
     */
    virtual void clear() = 0;


    /**
    * @brief Delete all items in our hash
    */
    virtual void deleteAllItems() = 0;


    /**
     * @brief Role value used to get a pointer to the Qt object stored in our list
     * @return
     */
    int QtObjectRole() const
    {
        return Qt::UserRole;
    }


Q_SIGNALS:
    /**
     * @brief Signal used when our items count has changed
     */
    void countChanged ();

    /**
     * @brief Signal used when our list of keys has changed
     */
    void keysChanged ();


protected Q_SLOTS:
    /**
     * @brief Callback called when an item is deleted outside of our hash
     * @param pair
     */
    virtual void _onHashItemExternalDeletion(I2CustomItemHashPair* pair) = 0;

    /**
      * @brief Callback called when an item stored in our list has changed
      * (i.e. one of its properties has changed)
      */
    virtual void _onListItemChanged() = 0;

    /**
     * @brief Callback called when an item is deleted outside of our list
     * @param item
     */
    virtual void _onListItemExternalDeletion(QObject *item) = 0;
};




/**
 * @brief I2CustomItemHashModel defines a template to create a hashtable model
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 *
 * TODO: Find a way to define methods in our .cpp file: on OSX, clang triggers "symbols not found" errors
 *       when we try to define methods in our .cpp file
 */
template<class CustomItemType> class I2CustomItemHashModel : public AbstractI2CustomItemHashModel
{
public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2CustomItemHashModel(QObject* parent = 0): AbstractI2CustomItemHashModel(parent)
    {
        // Get index of our callback called when an item changes (one of its properties has changed)
        _onListItemChangedCallback = metaObject()->method(metaObject()->indexOfMethod("_onListItemChanged()"));


        //
        // Use introspection mecanisms to associate a role to each property of our QmlEnumListItemData
        //
        // - list of reserved keywords (QML default properties and existing Qt roles)
        QList<QByteArray> qmlReservedRoleNames;
        qmlReservedRoleNames << QByteArrayLiteral("id")
                             << QByteArrayLiteral("display")
                             << QByteArrayLiteral("decoration")
                             << QByteArrayLiteral("edit")
                             << QByteArrayLiteral("toolTip")
                             << QByteArrayLiteral("statusTip")
                             << QByteArrayLiteral("whatsThis");

        // Specific role that will be used to return our item
        _roleNames.insert(QtObjectRole(), QByteArrayLiteral ("QtObject"));

        QMetaObject customItemHashPairMetaObject = I2CustomItemHashPair::staticMetaObject;
        int propertyCount = customItemHashPairMetaObject.propertyCount ();
        int propertyRoleStartIndex = QtObjectRole() + 1;
        for (int index = 0; index < propertyCount; index++)
        {
            // Get name of our property
            QMetaProperty metaProperty = customItemHashPairMetaObject.property(index);
            QByteArray propertyName = QByteArray (metaProperty.name());

            // Check if we can add it to our list of roles
            if (!qmlReservedRoleNames.contains(propertyName))
            {
                // NB: Roles must be greater than QtObjectRole()
                int role = propertyRoleStartIndex + index;
                _roleNames.insert(role, propertyName);

                // Check if a notify signal is associated to this property
                if (metaProperty.hasNotifySignal())
                {
                    int signalIndex = metaProperty.notifySignalIndex();
                    _propertyNotifySignals.append(signalIndex);
                    _signalRoles.insert(signalIndex, role);
                }
            }
        }
    }


    /**
      * Destructor
      */
    ~I2CustomItemHashModel()
    {
        // Clear data
        // NB: Item values will not be deleted
        //     Programmers must call deleteAllItems to delete all values before deleting our hash
        //     because we don't know if our hash owns values or not, thus we can not always call
        //     deleteAllItems in this destructor
        clear();
    }


public: // QAbstractListModel API

    /**
     * @brief Returns the data stored under the given role for the item referred to by the index
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE
    {
        QVariant result;

        // Check if index is valid
        if (index.isValid() && (index.row() >= 0) && (index.row() < _list.size()))
        {
            I2CustomItemHashPair* item = _list.at(index.row());
            if (item != NULL)
            {
                if (role != Qt::DisplayRole)
                {
                    const QByteArray propertyName = _roleNames.value (role, QByteArrayLiteral (""));
                    if (!propertyName.isEmpty())
                    {
                        // Check if we must return our item OR the value of one of its properties
                        if (role != QtObjectRole())
                        {
                            result.setValue(item->property(propertyName));
                        }
                        else
                        {
                            result.setValue(QVariant::fromValue(static_cast<QObject *> (item)));
                        }
                    }
                    else
                    {
                        qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): invalid role" << role
                                 << "for item at index " << index.row() << "(" << item << ")";
                    }
                }
                //Else: role is Qt::DisplayRole
            }
            else
            {
                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): item at index " << index.row() << " is undefined";
            }
        }
        // Else: We don't have an item at this index, we will return an empty value
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): invalid index " << index.row();
        }

        return result;
    }


    /**
     * @brief Sets the role data for the item at index to value.
     * @param index
     * @param value
     * @param role
     * @return true if successful;false otherwise
     */
    bool setData (const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE
    {
        bool result = false;

        // Check if index is valid
        if (index.isValid() && (index.row() >= 0) && (index.row() < _list.size()))
        {
            I2CustomItemHashPair* item = _list.at(index.row ());
            if (item != NULL)
            {
                if ((role != Qt::DisplayRole) && (role != QtObjectRole()))
                {
                    const QByteArray propertyName = _roleNames.value (role, QByteArrayLiteral (""));

                    if (!propertyName.isEmpty())
                    {
                        if (propertyName != "key")
                        {
                            result = item->setProperty (propertyName, value);
                        }
                        else
                        {
                            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): key can not be edited (index "<< index.row() << ")";
                        }
                    }
                    else
                    {
                        qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): invalid role" << role
                                 << "for item at index " << index.row() << "(" << item << ")";
                    }
                }
                // Else: role is Qt::DisplayRole OR QtObjectRole
                else if (role == QtObjectRole())
                {
                    qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): item at index " << index.row() << " can not be redefined";
                }
            }
            else
            {
                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): item at index " << index.row() << " is undefined";
            }
        }
        // Else: index is invalid
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): invalid index " << index.row();
        }

        return result;
    }


    /**
     * @brief Returns the number of rows under the given parent.
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);
        return _list.size();
    }


    /**
     * @brief Append an item to our list
     * @param item
     */
    void appendRow(I2CustomItemHashPair* item)
    {
        if (item != NULL)
        {
            QString key = item->key();

            // Check if we can use this key
            if ((!key.isEmpty()) && !_hash.contains(key))
            {
                // Update our list
                beginInsertRows (QModelIndex(), rowCount(), rowCount());

                // - add item to our list and hash
                _addItem(item);

                endInsertRows ();

                // Update properties (count, etc.)
                _hashUpdated();
            }
            else
            {
                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: key '" << key << "' is already used or empty";

                //NB: we assume that I2CustomItemHashPair items can only be created by I2CustomItemHashModel
                //    i.e. appendRow MUST only be called by I2CustomItemHashModel
                delete item;
            }
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning:can not append a NULL item";
        }
    }


    /**
     * @brief Append multiple items to our list
     * @param items
     */
    void appendRows(const QList<I2CustomItemHashPair *> &items)
    {
        if (items.size() != 0)
        {
            // NB: We don't check if our key already exists or is empty
            //     BECAUSE the test MUST be performed before calling this function

            beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);

            for(auto item: items)
            {
                _addItem(item);
            }

            endInsertRows();


            // Update properties (count, etc.)
            _hashUpdated();
        }
    }


    /**
     * @brief Removes the given row from the child items of the parent specified
     * @param row
     * @param parent
     *
     * @return true if row has been removed, false otherwise (i.e. row is invalid)
     */
    bool removeRow(int row, const QModelIndex& parent = QModelIndex())
    {
        Q_UNUSED(parent);

        bool result = false;

        // Check bounds
        if ((row >= 0) && (row < _list.size()))
        {
            // Update list
            beginRemoveRows(QModelIndex(), row, row);

            // - get our item
            I2CustomItemHashPair* item = _list.takeAt(row);
            if (item != NULL)
            {
                // Remove connections
                _unsubscribeToItem(item);

                // Remove our key
                _hash.remove(item->key());

                // Clean-up property (notify QML, remove connections, etc.)
                item->setValue(NULL);

                // Delete our item
                delete item;
            }
            // Else: should not happen because our list can not store a NULL value

            endRemoveRows();


            // Update properties (count, etc.)
            _hashUpdated();

            result = true;
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> : invalid index" << row << "in call to removeRow(index)";
        }

        return result;
    }


    /**
     * @brief Removes count rows starting with the given row under parent parent from the model.
     *
     * @param row
     * @param count
     * @param parent
     * @return
     */
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE
    {
        Q_UNUSED(parent);

        bool result = false;

        // Check if we have valid parameters
        if ((count > 0) && (row >= 0) && ((row+count) <= _list.size()))
        {
            // Update list
            beginRemoveRows(QModelIndex(), row, row+count-1);

            QList<I2CustomItemHashPair*> itemsToDelete;

            for(int index = 0; index < count; index++)
            {
                I2CustomItemHashPair* item = _list.takeAt(row);
                if (item != NULL)
                {
                    // Remove connections
                    _unsubscribeToItem(item);

                    // Remove our key
                    _hash.remove(item->key());

                    // Clean-up property (notify QML, remove connections, etc.)
                    item->setValue(NULL);

                    // Add item to our list
                    itemsToDelete.append(item);
                }
            }

            // Delete all items of our list
            qDeleteAll(itemsToDelete);

            endRemoveRows();


            // Update properties (count, etc.)
            _hashUpdated();

            result = true;
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className()
                     << "> : invalid parameters row=" << row << ", count=" << count
                     << "in call to removeRows(row, index)";
        }

        return result;
    }


public: // Helpers to switch between role and property name

    /**
     * @brief Returns the model's role names
     * @return
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE
    {
        return _roleNames;
    }


    /**
     * @brief Get role for a given name
     * @param name
     * @return
     */
    int roleForName(const QByteArray &name) Q_DECL_OVERRIDE
    {
        return _roleNames.key (name, -1);
    }


    /**
     * @brief Get the property name associated to a given role
     * @param role
     * @return
     */
    QByteArray nameForRole(int role) Q_DECL_OVERRIDE
    {
        return _roleNames.value(role, QByteArrayLiteral (""));
    }


public: // Hash API

    /**
     * @brief Get the size of our list
     * @return
     * NB: Equivalent to count()
     */
    int size() const Q_DECL_OVERRIDE
    {
        return _list.size();
    }


    /**
     * @brief Get the size of our list
     * @return
     * NB: equivalent to size()
     */
    int count() const Q_DECL_OVERRIDE
    {
        return _list.count();
    }


    /**
     * @brief Check if our list is empty
     * @return
     */
    bool isEmpty() const Q_DECL_OVERRIDE
    {
        return _list.isEmpty();
    }


    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    QObject* get(int index) Q_DECL_OVERRIDE
    {
        QObject* item = NULL;

        if ((index >= 0) && (index < _list.size()))
        {
            item = _list.at(index);
        }

        return item;
    }


    /**
     * @brief Get all keys
     * @return
     */
    QStringList keys() const Q_DECL_OVERRIDE
    {
        // NB: we call uniqueKeys because our hash can only store one value per key
        return _hash.uniqueKeys();
    }


    /**
     * @brief Get the item associated to a given key
     * @param key
     * @return
     */
    Q_INVOKABLE QObject* value(QString key) Q_DECL_OVERRIDE
    {
        QObject* result = NULL;

        // Check if we have a key
        if (!key.isEmpty())
        {
            I2CustomItemHashPair* pair = _hash.value(key);
            if (pair != NULL)
            {
                result = pair->value();
            }
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not call value(key) with an empty key";
        }

        return result;
    }



    /**
     * @brief Get the item associated to a given key
     * @param key
     * @return
     */
    Q_INVOKABLE CustomItemType* castedValue(QString key)
    {
        CustomItemType* result = NULL;

        // Check if we have a key
        if (!key.isEmpty())
        {
            I2CustomItemHashPair* pair = _hash.value(key);
            if (pair != NULL)
            {
                result = dynamic_cast<CustomItemType*>(pair->value());
            }
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not call castedValue(key) with an empty key";
        }

        return result;
    }


    /**
     * @brief Get all items stored in our hash
     * @param key
     * @return
     */
    QList<QObject*> values() Q_DECL_OVERRIDE
    {
        QList<QObject*> result;

        // Check if have at least one item in our hash
        if (!_list.isEmpty())
        {
            for (auto item: _list)
            {
                if (item != NULL)
                {
                    QObject* value = item->value();
                    if (value != NULL)
                    {
                        result.append(value);
                    }
                    // Else: should not happen because we can not store NULL values
                }
                // Else: should not happen because our list can not store NULL values
            }
        }

        return result;
    }


    /**
     * @brief Get all items stored in our hash
     * @param key
     * @return
     */
    QList<CustomItemType*> castedValues()
    {
        QList<CustomItemType*> result;

        // Check if we have at least one item in our hash
        if (!_list.isEmpty())
        {
            for (auto item: _list)
            {
                if (item != NULL)
                {
                    CustomItemType* value = dynamic_cast<CustomItemType *>(item->value());
                    if (value != NULL)
                    {
                        result.append(value);
                    }
                    // Else: should not happen because we can not store NULL values
                }
                // Else: should not happen because we can not store NULL values
            }
        }

        return result;
    }


    /**
     * @brief Insert an item with a given key
     * @param key
     * @param item
     */
    void insert(QString key, QObject* item) Q_DECL_OVERRIDE
    {
        // Check if key and value are defined
        if (!key.isEmpty() && (item != NULL))
        {
            // Check if key does not exist
            if (!_hash.contains(key))
            {
                I2CustomItemHashPair* pair = new I2CustomItemHashPair(key, item);
                appendRow(pair);
            }
            else
            {
                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not insert item"
                         << item << "because key '" << key << "' is already used";
            }
        }
        else if (key.isEmpty())
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not insert item"
                     << item << "with an empty key";
        }
    }


    /**
     * @brief Insert all items of a given hash model
     *
     * @param hashModel
     */
    void insertContentOfCustomHashModel(AbstractI2CustomItemHashModel* hashModel) Q_DECL_OVERRIDE
    {
        if (hashModel != NULL)
        {
            // Try to cast our hashmodel
            I2CustomItemHashModel<CustomItemType>* castedHashModel = dynamic_cast<I2CustomItemHashModel<CustomItemType> *>(hashModel);
            if (castedHashModel != NULL)
            {
                // Check if we have at least one item in this hash
                if (castedHashModel->count())
                {
                    QList<I2CustomItemHashPair *> validItems;
                    QStringList validNewkeys;

                    QList<QString> hashModelKeys = castedHashModel->keys();
                    for (auto key : hashModelKeys)
                    {
                        // Check if we can use this key
                        if (!key.isEmpty() && !(_hash.contains(key)) && !(validNewkeys.contains(key)))
                        {
                            CustomItemType* value = castedHashModel->castedValue(key);
                            if (value != NULL)
                            {
                                I2CustomItemHashPair* pair = new I2CustomItemHashPair(key, value);
                                validItems.append(pair);
                                validNewkeys.append(key);
                            }
                            else
                            {
                                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: key '" << key << "' can not have an undefined value";
                            }
                        }
                        else
                        {
                            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: key '" << key << "' already exists or is empty";
                        }
                    }

                    appendRows(validItems);
                }
                // Else: empty hash, nothing to do
            }
            else
            {
                qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className()
                           << "> warning: can not insert hash model"
                           << hashModel << "because it has an invalid item type";
            }
        }
    }



    /**
     * @brief Insert multiple items in our hashtable i.e. add an exiting hashtable
     * @param hash
     */
    void insert(QHash<QString, CustomItemType*> hash)
    {
        if (!hash.isEmpty())
        {
            QList<I2CustomItemHashPair *> validItems;
            QStringList validNewkeys;

            for (auto key : hash.uniqueKeys())
            {
                // Check if we can use this key
                if (!key.isEmpty() && !(_hash.contains(key)) && !(validNewkeys.contains(key)))
                {
                    CustomItemType* value = hash.value(key);
                    if (value != NULL)
                    {
                        I2CustomItemHashPair* pair = new I2CustomItemHashPair(key, value);
                        validItems.append(pair);
                        validNewkeys.append(key);
                    }
                    else
                    {
                        qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: key '" << key << "' can not have an undefined value";
                    }
                }
                else
                {
                    qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: key '" << key << "' already exists or is empty";
                }
            }

            appendRows(validItems);
        }
    }



    /**
     * @brief Remove a given key
     * @param key
     *
     * NB: item will be removed from our hash BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE void removeKey(QString key) Q_DECL_OVERRIDE
    {
        // Check if key is defined
        if (!key.isEmpty())
        {
            I2CustomItemHashPair* pair = _hash.value(key);
            if (pair != NULL)
            {
                int index = _list.indexOf(pair);
                removeRow(index);
            }
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not remove an empty key";
        }
    }


    /**
     * @brief Check if a given key is in our hash
     * @param item
     * @return
     */
    Q_INVOKABLE bool containsKey(QString key) const Q_DECL_OVERRIDE
    {
        bool result = false;

        if (!key.isEmpty())
        {
            result = _hash.contains(key);
        }
        else
        {
            qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not call containsKey with an empty key";
        }

        return result;
    }


    /**
     * @brief Removes all items from our hash
     *
     * NB: items will be removed from our hash BUT WILL NOT BE DELETED
     */
    void clear() Q_DECL_OVERRIDE
    {
        // Check if we have at least one item in our hashtable
        if (!_list.isEmpty())
        {
            // Update our list
            beginRemoveRows (QModelIndex(), 0, _list.size()-1);

            // Unsubscribe to all items
            for(auto item: _list)
            {
                // Remove connections
                _unsubscribeToItem(item);

                // Remove our key
                _hash.remove(item->key());

                // Clean-up property (to remove internal connections, notify QML, etc.)
                item->setValue(NULL);
            }

            // Delete all items of our list (I2CustomItemHashPair)
             qDeleteAll(_list);

             // Clear our hashtable
             _hash.clear();

             // Clear our list
             _list.clear();

             endRemoveRows ();


             // Update properties (count, etc.)
            _hashUpdated();
        }
    }


    /**
     * @brief Delete all items in our list
     *
     * NB: it will also delete values stored in our hash
     */
    void deleteAllItems() Q_DECL_OVERRIDE
    {
         // Check if we have at least one item in our hashtable
        if (!_list.isEmpty())
        {
            // Update our list
            beginRemoveRows (QModelIndex(), 0, _list.size()-1);

            // Unsubscribe to all items and delete item value
            for (auto item: _list)
            {
                // Remove connections
                _unsubscribeToItem(item);

                // Remove our key
                _hash.remove(item->key());

                // Delete our item value
                QObject* value = item->value();
                if (value != NULL)
                {
                    // Clean-up property (to remove internal connections, notify QML, etc.)
                    item->setValue(NULL);

                    // Delete our value
                    delete value;
                }
            }

            // Delete all items of our list (I2CustomItemHashPair)
            qDeleteAll(_list);

            // Clear our hashtable
            _hash.clear();

            // Clear list
            _list.clear();

            endRemoveRows ();


            // Update properties (count, etc.)
            _hashUpdated();
        }
    }


    /**
     * @brief Callback called when an item is deleted outside of our hash
     * @param pair
     */
    void _onHashItemExternalDeletion(I2CustomItemHashPair* pair) Q_DECL_OVERRIDE
    {
        // Ensure that we have an I2CustomItemHashPair
        if (pair != NULL)
        {
            // Try to find the index of our pair
            int indexOfPair = _list.indexOf(pair);

            // Check if we have a valid index
            if (indexOfPair != -1)
            {
                 // Update our list
                 beginRemoveRows(QModelIndex(), indexOfPair, indexOfPair);

                 // - remove item at the invalid index
                 _list.takeAt( indexOfPair );

                 // - remove connections
                 _unsubscribeToItem( pair );

                 // - remove our key
                 _hash.remove( pair->key() );

                 // - clean-up
                 delete pair;

                 // End of list update
                 endRemoveRows();

                 // Update properties (counts, etc.)
                 _hashUpdated();
            }
            else
            {
                 qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className()
                            << "> warning: strange behavior - an item has been deleted by an external code but we can't find it in our hash to perform clean-up";
            }
        }
        // Else: should not happen because the signal is triggered by an I2CustomItemHashPair
    }


    /**
     * @brief Called when an item of our list has changed
     */
    void _onListItemChanged() Q_DECL_OVERRIDE
    {
        // Check if we have a sender
        QObject *sourceObject = sender();
        if (sourceObject != NULL)
        {
            // Try to cast our sender
            I2CustomItemHashPair *item = dynamic_cast<I2CustomItemHashPair *>(sourceObject);
            if (item != NULL)
            {
                // Check if this item is in our list
                QModelIndex index = _modelIndexFromItem(item);
                if (index.isValid())
                {
                    // Check if we can avoid a brute force QML update
                    int sourceSignalIndex = senderSignalIndex();
                    if (_signalRoles.contains(sourceSignalIndex))
                    {
                        // We can define the role (i.e. the property) that has been udpated
                        QVector<int> updatedRoles;
                        updatedRoles.append(_signalRoles.value(sourceSignalIndex));

                        Q_EMIT dataChanged(index, index, updatedRoles);
                    }
                    else
                    {
                        // Signal index is unknown OR not associated to a role
                        // We can only perform a brute force QML update i.e. notify that all properties have changed
                        Q_EMIT dataChanged(index, index);
                    }
                }
            }
            // Else: should not happen because the signal is triggered by an I2CustomItemHashPair
        }
        // Else: should not happen because the signal is triggered by an I2CustomItemHashPair
    }


    /**
     * @brief Get model index of a given item in our list
     * @param item
     * @return
     */
    QModelIndex _modelIndexFromItem(I2CustomItemHashPair *item)
    {
        QModelIndex result;

        if (item != NULL)
        {
            int itemIndex = _list.indexOf(item);
            if (itemIndex >= 0)
            {
                result = index(itemIndex);
            }
        }

        return result;
    }


    /**
     * @brief Callback called when an item is deleted outside of our list
     * @param item
     */
    void _onListItemExternalDeletion(QObject* deletedItem) Q_DECL_OVERRIDE
    {
        Q_UNUSED(deletedItem);
        qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className() << "> error: I2CustomItemHashPair MUST not be deleted by external code";


        //-----------------------------
        //
        // Try to perform clean-up
        //
        //-----------------------------

        //
        // Hashtable clean-up
        //

        // - try to remove it from our hash key
        QString invalidKey = "";
        int tempIndex = 0;
        QList<QString> hashKeys = _hash.keys();

        while (invalidKey.isEmpty() && (tempIndex < hashKeys.count()))
        {
            QString key = hashKeys.at(tempIndex);
            I2CustomItemHashPair* hashItem = _hash.value(key);
            if (hashItem != NULL)
            {
                // NB: comparison of pointer addresses
                if (hashItem == deletedItem)
                {
                    invalidKey = key;
                }
            }
            // Else: should not happen because we don't store NULL values in our hash

            tempIndex++;
        }

        // - remove item if we have found it
        if (!invalidKey.isEmpty())
        {
            _hash.remove(invalidKey);
        }
        else
        {
             qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className()
                        << "> error: I2CustomItemHashPair has been deleted by an external code AND we can't perform clean-up of our internal hashtable";
        }


        //
        // List clean-up
        //

        // - try to remove it from our list
        int invalidIndex = -1;
        tempIndex = 0;
        while ((invalidIndex == -1) && (tempIndex < _list.count()))
        {
            I2CustomItemHashPair* listItem = _list.at(tempIndex);
            // NB: comparison of pointer addresses
            if (listItem == deletedItem)
            {
                invalidIndex = tempIndex;
            }

            tempIndex++;
        }

        if (invalidIndex != -1)
        {
            // Update our list
            beginRemoveRows(QModelIndex(), invalidIndex, invalidIndex);

            // - remove item at the invalid index
            _list.takeAt(invalidIndex);

            // - unsubscribe to all signals
            disconnect(deletedItem, 0, this, 0);

            // - end of list update
            endRemoveRows();


            // Update properties (counts, etc.)
            _hashUpdated();
        }
        else
        {
             qWarning() << "I2CustomItemHashModel<" << CustomItemType::staticMetaObject.className()
                        << "> error: I2CustomItemHashPair has been deleted by an external code AND we can't perform clean-up of our internal list";
        }
    }


private:

    /**
     * @brief Add an item to our collection
     * @param item
     */
    void _addItem(I2CustomItemHashPair* item)
    {
        if (item != NULL)
        {
            _subscribeToItem(item);

            _list.append(item);

            // NB: We don't check if our key already exists or is empty
            //     BECAUSE the test MUST be performed before calling this function
            QString key = item->key();
            _hash.insert(key, item);
        }
    }


    /**
     * @brief Subscribe to signals of a specific item
     * @param item
     */
    void _subscribeToItem(I2CustomItemHashPair* item)
    {
        if (item != NULL)
        {
            // Subscribe to each notify signal associated to a property of our item class
            for(auto notifySignalIndex: _propertyNotifySignals)
            {
                QMetaMethod notifySignal = item->metaObject()->method(notifySignalIndex);
                // NB: Qt::UniqueConnection to ensure that we only subscribe once to each item
                connect (item, notifySignal, this, _onListItemChangedCallback, Qt::UniqueConnection);
            }

            // Subscribe to deletion of our item
            connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(_onListItemExternalDeletion(QObject*)), Qt::UniqueConnection);

            // Subscribe to the deletion of its value
            connect(item, SIGNAL(valueItemDeleted(I2CustomItemHashPair*)), this, SLOT(_onHashItemExternalDeletion(I2CustomItemHashPair*)), Qt::UniqueConnection);
        }
    }


    /**
     * @brief Unsubscribe to signals of a specific item
     * @param item
     */
    void _unsubscribeToItem(I2CustomItemHashPair *item)
    {
        if (item != NULL)
        {
            // Remove all subscriptions to signals of our item
            disconnect(item, 0, this, 0);
        }
    }


    /**
     * @brief Called when our hash is updated, to check if our count has changed
     */
    void _hashUpdated()
    {
        // Check items count
        if (_lastItemCount != _list.count())
        {
            _lastItemCount = _list.count();
            Q_EMIT countChanged();
        }

        // Keys
        Q_EMIT keysChanged();
    }


private:
    // Last number of items
    int _lastItemCount;

    // List used to store our pairs (key, value)
    QList<I2CustomItemHashPair *> _list;

    // Hashtable used to store our items
    QHash<QString, I2CustomItemHashPair *> _hash;

    // Hashtable associating a role value (int) to a property name
    QHash<int, QByteArray> _roleNames;

    // Hashtable associating a signal index to a role value
    QHash<int, int> _signalRoles;

    // List of notify signals of our item class
    QList<int> _propertyNotifySignals;

    // Callback used when an item has been updated (for optimization purpose only)
    QMetaMethod _onListItemChangedCallback;
};



#endif // _I2CUSTOMITEMHASHMODEL_H_
