/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2_CUSTOMITEMLISTMODEL_H_
#define _I2_CUSTOMITEMLISTMODEL_H_

#include <QAbstractListModel>
#include <QDebug>
#include <QMetaProperty>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief I2AbstractCustomItemListModel defines the base class of our template for list models
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 */
class I2QUICK_EXPORT AbstractI2CustomItemListModel : public QAbstractListModel
{
    Q_OBJECT

    // Count property, i.e. the number of items in our list, to mimic QQmlListProperty
    Q_PROPERTY (int count READ count NOTIFY countChanged)

    // Indicates if our list can contain duplicate items in our list
    Q_PROPERTY (bool allowDuplicateItems READ allowDuplicateItems WRITE setAllowDuplicateItems NOTIFY allowDuplicateItemsChanged)

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit AbstractI2CustomItemListModel (QObject * parent = nullptr);


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


public: // List API

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
     * @brief Check if our list allows duplicate items or not
     * @return
     */
    virtual bool allowDuplicateItems() const  = 0;

    /**
     * @brief Set if our list allows duplicate items or not
     * @return
     */
    virtual void setAllowDuplicateItems(bool value) = 0;


    /**
     * @brief Append an item to our list (end of our list)
     * @param item
     */
    virtual void append(QObject *item) = 0;

    /**
     * @brief Prepend an item to our list (beginning of our list)
     * @param item
     */
    virtual void prepend(QObject *item) = 0;

    /**
     * @brief Insert an item at a given position
     * @param index
     * @param item
     */
    Q_INVOKABLE virtual void insert(int index, QObject *item) = 0;


    /**
     * @brief Replaces the item at index position "index with newItem
     *
     * @param index must be a valid index position in the list (i.e., 0 <= index < size())
     * @param newItem
     *
     * @return the previous item
     */
    Q_INVOKABLE virtual QObject* replace(int index, QObject* newItem) = 0;


    /**
     * @brief Remove a given item
     * @param item
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE virtual void remove(QObject *item) = 0;


    /**
     * @brief Remove item at the given position
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE virtual void remove(int index) = 0;


    /**
     * @brief Moves n items from one position to another.
     *        The [from, from + n - 1] and [to, to + n - 1] ranges must exist (be valid)
     *
     * @param from Index of origin
     * @param to Index of destination
     * @param n Number of items to move
     */
    Q_INVOKABLE virtual void move(int from, int to, int n) = 0;



    /**
     * @brief Check if a given item is in our list
     * @param item
     * @return
     */
    Q_INVOKABLE virtual bool contains(QObject *item) const = 0;


    /**
     * @brief Get index of a given item in our list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE virtual int indexOf(QObject *item) const = 0;


    /**
     * @brief Removes all items from the list
     *
     * NB: items will be removed from our list BUT WILL NOT BE DELETED
     */
    virtual void clear() = 0;


    /**
     * @brief Delete all items in our list
     */
    virtual void deleteAllItems() = 0;


    /**
     * @brief Get a generic QList, i.e. a QList of QObjects, representation of our list model
     * @return A QList of QObjects
     */
    virtual QList<QObject *> toQObjectList() const = 0;


    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    Q_INVOKABLE virtual QObject* get(int index) = 0;

    /**
     * @brief Role value used to get a pointer to the Qt object stored in our list
     * @return
     */
    int QtObjectRole() const
    {
      return Qt::UserRole;
    }


//
// QList-like comfort API
//
public:
    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    AbstractI2CustomItemListModel& operator+= (QObject* item)
    {
        append(item);
        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    AbstractI2CustomItemListModel& operator<< (QObject* item)
    {
        append(item);
        return (*this);
    }



Q_SIGNALS:
    /**
     * @brief Triggered when our number of items has changed
     */
    void countChanged ();

    /**
     * @brief Triggered when our allowDuplicateItems property has changed
     * @param value
     */
    void allowDuplicateItemsChanged(bool value);


protected Q_SLOTS:
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
 * @brief I2CustomItemListModel defines a template to create a list model for custom items (QObject derived class)
 *
 * NB: Custom items must not defined the following properties: id, display, decoration, edit, toolTip,
 *      statusTip, whatsThis
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 *
 * NB: we implement our methods in our header file instead of using a .cpp file because we don't want to
 *     force programmers to explicitly instantiate our template for each type
 *     (template class I2CustomItemListModel<MyType>;) before using it
 *     => it is required by some compilers such as Visutal Studio or clang to avoid linker errors
 *    (e.g. symbol(s) not found for architecture x86_64 with XCode/clang on OSX)
 */
template<class CustomItemType> class I2CustomItemListModel : public AbstractI2CustomItemListModel
{
public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2CustomItemListModel(QObject *parent = nullptr): AbstractI2CustomItemListModel(parent),
        _allowDuplicateItems(true),
        _lastItemCount(0)
    {
        // Get index of our callback called when an item changes (one of its properties has changed)
        _onListItemChangedCallback = metaObject()->method(metaObject()->indexOfMethod("_onListItemChanged()"));

        //
        // Use introspection mecanisms to associate a role to each property of our CustomItemType
        //
        // - list of reserved keywords (QML default properties and existing Qt roles)
        QList<QByteArray> qmlReservedRoleNames;
        qmlReservedRoleNames << QByteArrayLiteral("id")
                             << QByteArrayLiteral("display")
                             << QByteArrayLiteral("decoration")
                             << QByteArrayLiteral("edit")
                             << QByteArrayLiteral("toolTip")
                             << QByteArrayLiteral("statusTip")
                             << QByteArrayLiteral("whatsThis")
                             << QByteArrayLiteral("QtObject");

        // - list of keywords that can create issues with QML
        QList<QByteArray> qmlWarningRoleNames;
        qmlWarningRoleNames << QByteArrayLiteral("index")
                            << QByteArrayLiteral("model");

        // Specific role that will be used to return our item
        _roleNames.insert(QtObjectRole(), QByteArrayLiteral ("QtObject"));

        QMetaObject metaObject = CustomItemType::staticMetaObject;
        int propertyCount = metaObject.propertyCount ();
        int propertyRoleStartIndex = QtObjectRole() + 1;
        for (int index = 0; index < propertyCount; index++)
        {
            // Get name of our property
            QMetaProperty metaProperty = metaObject.property(index);
            QByteArray propertyName = QByteArray (metaProperty.name());

            // Check if we can add it to our list of roles
            if (!qmlReservedRoleNames.contains(propertyName))
            {
                // Check if we must display a warning
                if (qmlWarningRoleNames.contains(propertyName))
                {
                    qWarning () << "I2CustomItemListModel warning: class " << metaObject.className()
                                << " should not have a property named " << propertyName
                                << ", it can create issues with QML";
                }

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
            else
            {
                qCritical () << "I2CustomItemListModel error: class " << metaObject.className()
                             << " must not have a property named " << propertyName
                             << ", it is a reserved keyword and can create issues with QML";
            }
        }
    }


    /**
      * Destructor
      */
    ~I2CustomItemListModel()
    {
        // Clear our list
        clear();

        // Clear our hashtable associating a role value (int) to a property name
        _roleNames.clear();

        // Clear our hashtable associating a signal index to a role value
        _signalRoles.clear();

        // Clear our list of notify signals of our item class
        _propertyNotifySignals.clear();
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
            CustomItemType *item = _list.at(index.row());
            if (item != nullptr)
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
                  qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): invalid role" << role
                            << "for item at index " << index.row() << "(" << item << ")";
                }
              }
              //Else: role is Qt::DisplayRole
            }
            else
            {
              qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): item at index " << index.row() << " is undefined";
            }
        }
        // Else: We don't have an item at this index, we will return an empty value
        else
        {
          qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in data(): invalid index " << index.row();
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
            CustomItemType *item = _list.at(index.row ());
            if (item != nullptr)
            {
              if ((role != Qt::DisplayRole) && (role != QtObjectRole()))
              {
                const QByteArray propertyName = _roleNames.value (role, QByteArrayLiteral (""));

                if (!propertyName.isEmpty())
                {
                  result = item->setProperty (propertyName, value);
                }
                else
                {
                  qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): invalid role" << role
                           << "for item at index " << index.row() << "(" << item << ")";
                }
              }
              // Else: role is Qt::DisplayRole OR QtObjectRole
            }
            else
            {
              qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): item at index " << index.row() << " is undefined";
            }
        }
        // Else: index is invalid
        else
        {
          qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning in setData(): invalid index " << index.row();
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
    void appendRow(CustomItemType *item)
    {
        if (item != nullptr)
        {
            // Check if we can add this item
            // Option 1: our list allows duplicate items
            // Option 2: duplicate items are not allowed and our list does not contain this item
            if ( _allowDuplicateItems || !_list.contains(item))
            {
                 // Update our list
                beginInsertRows (QModelIndex(), rowCount(), rowCount());
                _subscribeToItem(item);
                _list.append(item);
                endInsertRows ();

                 // Update properties (count, etc.)
                _listUpdated();
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not append item " << item << "because our list does not allow duplicate items";
            }
        }
    }


    /**
     * @brief Append multiple items to our list
     * @param items
     */
    void appendRows(const QList<CustomItemType *> &items)
    {
        // Check if we have at least one item
        if (items.size() != 0)
        {
            // Check if we can add duplicate items
            if (_allowDuplicateItems)
            {
                // We don't need to filter our items before adding them

                // Update our list
                beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
                for (auto item: items)
                {
                    _subscribeToItem(item);
                    _list.append(item);
                }
                endInsertRows();

                // Update properties (count, etc.)
                _listUpdated();
            }
            else
            {
                // We must filter our items to remove duplicate items
                QList<CustomItemType *> itemsToAdd;
                for(auto item: items)
                {
                    if (!_list.contains(item) && !itemsToAdd.contains(item))
                    {
                        itemsToAdd.append(item);
                    }
                    else
                    {
                        qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not append item " << item << "because our list does not allow duplicate items";
                    }
                }

                // Add items if needed
                if (itemsToAdd.size() != 0)
                {
                    // Update our list
                    beginInsertRows(QModelIndex(), rowCount(), rowCount() + itemsToAdd.size() - 1);
                    for (auto item: itemsToAdd)
                    {
                        _subscribeToItem(item);
                        _list.append(item);
                    }
                    endInsertRows();

                    // Update properties (count, etc.)
                    _listUpdated();
                }
            }
        }
    }


    /**
     * @brief Inserts a single item before the given row
     * @param row
     * @param item
     */
    void insertRow(int row, CustomItemType *item)
    {
        if (item != nullptr)
        {
            // Check if we can add this item
            if ( _allowDuplicateItems || !_list.contains(item))
            {
                // Update our list
                beginInsertRows(QModelIndex(), row, row);

                // - subscribe to our new item
                _subscribeToItem(item);

                // - add it to our list
                _list.insert(row, item);

                endInsertRows();

                // Update properties (count, etc.)
                _listUpdated();
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not insert item " << item << "because our list does not allow duplicate items";
            }
        }
    }


    /**
     * @brief Prepend an item to our list
     * @param item
     */
    void prependRow(CustomItemType *item)
    {
        if (item != nullptr)
        {
            // Check if we can add this item
            if ( _allowDuplicateItems || !_list.contains(item))
            {
                // Update our list
                beginInsertRows (QModelIndex(), 0, 0);
                _subscribeToItem(item);
                _list.prepend(item);
                endInsertRows ();

                // Update properties (count, etc.)
                _listUpdated();
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not prepend item " << item << "because our list does not allow duplicate items";
            }
        }
    }


    /**
     * @brief Removes the given row from the child items of the parent specified
     * @param row
     * @param parent
     * @return
     */
    bool removeRow(int row, const QModelIndex& parent = QModelIndex())
    {
        Q_UNUSED(parent);

        bool result = false;

        if ((row >= 0) && (row < _list.size()))
        {
            // Update our list
            beginRemoveRows(QModelIndex(), row, row);

            CustomItemType* item = _list.takeAt(row);
            if (item != nullptr)
            {
                // Check if we can unsubscribe
                if (!_allowDuplicateItems || !_list.contains(item))
                {
                    _unsubscribeToItem(item);
                }
                // Else: our list allows duplicate items and this item is in our list at another index position
            }

            endRemoveRows();

            // Update properties (count, etc.)
            _listUpdated();

            result = true;
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

        if ((count > 0) && (row >= 0) && ((row + count) <= _list.size()))
        {
            // Update our list
            beginRemoveRows(QModelIndex(), row, row+count-1);
            for (int index = 0; index < count; index++)
            {
                CustomItemType* item = _list.takeAt(row);
                if (item != nullptr)
                {
                    // Check if we can unsubscribe
                    if (!_allowDuplicateItems || !_list.contains(item))
                    {
                        _unsubscribeToItem(item);
                    }
                    // Else: our list allows duplicate items and this item is in our list at another index position
                }
            }
            endRemoveRows();

            // Update properties (count, etc.)
            _listUpdated();

            result = true;
        }

        return result;
    }


    /**
     * @brief Moves count rows starting with the given sourceRow under parent sourceParent to row destinationChild under parent destinationParent
     *
     * @param sourceParent
     * @param sourceRow
     * @param count
     * @param destinationParent
     * @param destinationChild
     *
     * @return
     */
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) Q_DECL_OVERRIDE
    {
        bool result = false;

        // Check conditions
        if (
            (count > 0)
            &&
            (sourceRow >= 0)
            &&
            ((sourceRow + count) <= _list.count())
            &&
            (sourceRow != destinationChild)
            &&
            (destinationChild >= 0) && (destinationChild <= _list.count())
            )
        {
            // Check if items are moved forward or backward
            if (sourceRow + count - 1 < destinationChild)
            {
                //
                // Forward
                //

                // try to notify start of changes
                if (beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild + 1))
                {
                    // Update our list
                    for (int index = 0; index < count; index++)
                    {
                        // Move our item
                        // NB: the source position does not change because our item is moved forward
                        _list.move(sourceRow, destinationChild);
                    }

                    // Notify end of changes
                    endMoveRows();

                    // Everything is ok
                    result = true;
                }
                else
                {
                    qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className()
                               << "> warning: beginMoveRows has failed, can not move " << count << "item(s) from"
                               << sourceRow << "to" << destinationChild;
                }
            }
            else if (sourceRow > destinationChild)
            {
                //
                // Backward
                //

                // try to notify start of changes
                if (beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
                {
                    // Update our list
                    for (int index = 0; index < count; index++)
                    {
                        // Move our item
                         _list.move(sourceRow + index, destinationChild + index);
                    }

                    // Notify end of changes
                    endMoveRows();

                    // Everything is ok
                    result = true;
                }
                else
                {
                    qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className()
                               << "> warning: beginMoveRows has failed, can not move " << count << "item(s) from"
                               << sourceRow << "to" << destinationChild;
                }
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className()
                           << "> warning: unhandle case, can not move " << count << "item(s) from"
                           << sourceRow << "to" << destinationChild;
            }
        }

        return result;
    }



    /**
     * @brief Removes the item at index position and returns it
     * @param row
     * @return
     */
    CustomItemType *takeRow(int row)
    {
        CustomItemType *item = nullptr;

        if ((row >= 0) && (row < _list.size()))
        {
            // Update our list
            beginRemoveRows(QModelIndex(), row, row);

            item = _list.takeAt(row);
            if (item != nullptr)
            {
                // Check if we can unsubscribe
                if (!_allowDuplicateItems || !_list.contains(item))
                {
                    _unsubscribeToItem(item);
                }
                // Else: our list allows duplicate items and this item is in our list at another index position
            }

            endRemoveRows();


             // Update properties (count, etc.)
            _listUpdated();
        }

        return item;
    }


    /**
     * @brief Get the item at index position
     * @param row
     * @return
     */
    CustomItemType *itemAtRow(int row)
    {
        CustomItemType *item = nullptr;

        if ((row >= 0) && (row < _list.size()))
        {
            item = _list.at(row);
        }

        return item;
    }


    /**
     * @brief Get index of a given item in our list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    int indexOfItem(CustomItemType *item) const
    {
        int index = -1;

        if (item != nullptr)
        {
            index = _list.indexOf(item);
        }

        return index;
    }


    /**
     * @brief Check if our list contains a given item
     * @param item
     * @return
     */
    bool containsItem(CustomItemType *item) const
    {
        bool result = false;

        if (item != nullptr)
        {
            result = _list.contains(item);
        }

        return result;
    }



    /**
     * @brief Get model index of a given item in our list
     * @param item
     * @return
     */
    QModelIndex modelIndexFromItem(CustomItemType *item)
    {
        QModelIndex result;

        if (item != nullptr)
        {
            int itemIndex = _list.indexOf(item);
            if (itemIndex >= 0)
            {
                result = index(itemIndex);
            }
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


public: // List API

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
     * @brief Check if our list allows duplicate items or not
     * @return
     */
    bool allowDuplicateItems() const  Q_DECL_OVERRIDE
    {
        return _allowDuplicateItems;
    }


    /**
     * @brief Set if our list allows duplicate items or not
     * @param value
     * @return
     */
    void setAllowDuplicateItems(bool value) Q_DECL_OVERRIDE
    {
        if (_allowDuplicateItems != value)
        {
            _allowDuplicateItems = value;
            Q_EMIT allowDuplicateItemsChanged(value);
        }
    }



    /**
     * @brief Append an item to our list (end of our list)
     * @param item
     */
    void append(QObject *item) Q_DECL_OVERRIDE
    {
        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                appendRow(customItem);
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not add item" << item << " because it has an invalid type";
            }
        }
    }


    /**
     * @brief Prepend an item to our list (beginning of our list)
     * @param item
     */
    void prepend(QObject *item) Q_DECL_OVERRIDE
    {
        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                prependRow(customItem);
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not prepend item" << item << " because it has an invalid type";
            }
        }
    }


    /**
     * @brief Insert an item at a given position
     * @param index
     * @param item
     */
    Q_INVOKABLE void insert(int index, QObject *item) Q_DECL_OVERRIDE
    {
        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                insertRow(index, customItem);
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not insert item" << item << " at position" << index << "because it has an invalid type";
            }
        }
    }


    /**
     * @brief Replaces the item at index position "index with newItem
     *
     * @param index must be a valid index position in the list (i.e., 0 <= index < size())
     * @param newItem
     *
     * @return the previous item
     */
    Q_INVOKABLE virtual QObject* replace(int index, QObject* newItem) Q_DECL_OVERRIDE
    {
        QObject* previousItem = nullptr;

        // Check if we have a valid index position
        if ((index >= 0) && (index < _list.size()))
        {
            if (newItem != nullptr)
            {
                // try to cast our item
                CustomItemType *customItem = qobject_cast<CustomItemType *>(newItem);
                if (customItem != nullptr)
                {
                    // Remove previous item
                    previousItem = takeAt(index);

                    // Insert our new item
                    insertRow(index, customItem);
                }
                else
                {
                    qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: invalid parameter in call to replace(index, item) - "
                             << newItem << "does not match the type of our list";
                }
            }
            else
            {
                  qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: invalid parameter in call to replace(index, item) - can not replace an item by NULL";
            }
        }
        else
        {
            qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: invalid index position "
                     << index << "in call to replace(index, item) - index out of range";
        }

        return previousItem;
    }



    /**
     * @brief Remove a given item
     * @param item
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void remove(QObject *item) Q_DECL_OVERRIDE
    {
        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                int itemIndex = _list.indexOf(customItem);
                if  (itemIndex >= 0)
                {
                    removeRow(itemIndex);
                }
            }
            else
            {
                qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: can not remove item" << item << " because it has an invalid type";
            }
        }
    }


    /**
     * @brief Remove item at the given position
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void remove(int index) Q_DECL_OVERRIDE
    {
        removeRow(index);
    }


    /**
     * @brief Moves n items from one position to another.
     *
     * @param from Index of origin
     * @param to Index of destination
     * @param count Number of items to move
     */
    Q_INVOKABLE void move(int from, int to, int count) Q_DECL_OVERRIDE
    {
        const QModelIndex parent = QModelIndex();
        moveRows(parent, from, count, parent, to);
    }



    /**
     * @brief Check if a given item is in our list
     * @param item
     * @return
     */
    Q_INVOKABLE bool contains(QObject *item) const Q_DECL_OVERRIDE
    {
        bool result = false;

        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                result = containsItem(customItem);
            }
        }

        return result;
    }


    /**
     * @brief Get index of a given item in our list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE int indexOf(QObject *item) const Q_DECL_OVERRIDE
    {
        int index = -1;

        if (item != nullptr)
        {
            CustomItemType *customItem = qobject_cast<CustomItemType *>(item);
            if (customItem != nullptr)
            {
                index = indexOfItem(customItem);
            }
        }

        return index;
    }


    /**
     * @brief Removes all items from the list
     *
     * NB: items will be removed from our list BUT WILL NOT BE DELETED
     */
    void clear() Q_DECL_OVERRIDE
    {
        if (!_list.isEmpty())
        {
            // Update list
            beginRemoveRows (QModelIndex(), 0, _list.size()-1);

            // Unsubscribe to all items
            for(auto item: _list)
            {
                _unsubscribeToItem(item);
            }

            // Clear list
            _list.clear();

            endRemoveRows ();


            // Update properties (count, etc.)
            _listUpdated();
        }
    }


    /**
     * @brief Delete all items in our list
     */
    void deleteAllItems() Q_DECL_OVERRIDE
    {
        if (!_list.isEmpty())
        {
            // Update list
            beginRemoveRows (QModelIndex(), 0, _list.size()-1);

            // Unsubscribe to all items
            for(auto item: _list)
            {
                _unsubscribeToItem(item);
            }

            // Delete all items
             qDeleteAll(_list);

            // Clear list
            _list.clear();

            endRemoveRows ();


            // Update properties (count, etc.)
            _listUpdated();
        }
    }


    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    QObject* get(int index) Q_DECL_OVERRIDE
    {
        return at(index);
    }


public: // Extra list API

    /**
     * @brief Append multiple items to our list
     * @param items
     */
    void append(const QList<CustomItemType *> &items)
    {
        appendRows(items);
    }


    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    CustomItemType *at(int index)
    {
        return itemAtRow(index);
    }


    /**
     * @brief Removes the item at index position and returns it
     * @param index
     * @return
     */
    CustomItemType *takeAt(int index)
    {
        return takeRow(index);
    }


    /**
     * @brief Get a QList representation of our list model
     * @return A casted QList
     */
    const QList<CustomItemType *> toList() const
    {
        return QList<CustomItemType *>(_list);
    }


    /**
     * @brief Get a generic QList, i.e. a QList of QObjects, representation of our list model
     * @return A QList of QObjects
     */
    QList<QObject *> toQObjectList() const Q_DECL_OVERRIDE
    {
        QList<QObject *> objectList;

        for (CustomItemType* item : _list)
        {
            objectList.append(item);
        }

        return objectList;
    }


    /**
     * @brief Returns an STL-style iterator pointing to the first item in the list
     * @return
     */
    typename QList<CustomItemType *>::const_iterator begin() const
    {
        return _list.begin();
    }


    /**
     * @brief Returns an STL-style iterator pointing to the imaginary item after the last item in the list
     * @return
     */
    typename QList<CustomItemType *>::const_iterator end() const
    {
        return _list.end();
    }


    /**
     * @brief Returns a const STL-style iterator pointing to the first item in the list
     * @return
     */
    typename QList<CustomItemType *>::const_iterator constBegin() const
    {
        return _list.constBegin();
    }


    /**
     * @brief Returns a const STL-style iterator pointing to the imaginary item after the last item in the list
     * @return
     */
    typename QList<CustomItemType *>::const_iterator constEnd() const
    {
        return _list.constEnd();
    }



//
// QList-like comfort API
//
public:
    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    I2CustomItemListModel& operator+= (CustomItemType* item)
    {
        appendRow(item);
        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    I2CustomItemListModel& operator<< (CustomItemType* item)
    {
        appendRow(item);
        return (*this);
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemListModel& operator+= (const QList<CustomItemType*> &list)
    {
        append(list);
        return (*this);
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemListModel& operator<< (const QList<CustomItemType*> &list)
    {
        append(list);
        return (*this);
    }



public:
    /**
     * @brief Called when an item of our list has changed
     */
    void _onListItemChanged() Q_DECL_OVERRIDE
    {
        // get information about our sender
        QObject *sourceObject = sender();
        if (sourceObject != nullptr)
        {
            CustomItemType *item = dynamic_cast<CustomItemType *>(sourceObject);
            if (item != nullptr)
            {
                // Check if this item is in our list
                if (_list.contains(item))
                {
                    // Find positions of all occurrences of our item
                    QList<int> indexes;

                    // Get the first index position of our item
                    int firstIndexPosition = _list.indexOf(item);
                    indexes.append(firstIndexPosition);

                    // Check if our list allows duplicate items
                    if (_allowDuplicateItems)
                    {
                        // Try to find other instances

                        // - get last index
                        int lastIndexPosition = _list.lastIndexOf(item);

                        // Check if we have more than one instance
                        if (firstIndexPosition != lastIndexPosition)
                        {
                            int nextIndex = _list.indexOf(item, firstIndexPosition + 1);

                            while (nextIndex >= 0)
                            {
                                // Save index
                                indexes.append(nextIndex);

                                // Compute new value
                                if (nextIndex != lastIndexPosition)
                                {
                                    nextIndex = _list.indexOf(item, nextIndex + 1);
                                }
                                else
                                {
                                    // We have found our last instance
                                    nextIndex = -1;
                                }
                            }
                        }
                        // Else: we have a single instance
                    }


                    // Notify change
                    // - check if we can avoid a brute force QML update
                    int sourceSignalIndex = senderSignalIndex();
                    if (_signalRoles.contains(sourceSignalIndex))
                    {
                        // Notify a property change for each instance
                        for (int indexPosition : indexes)
                        {
                            // Create an index
                            QModelIndex modelIndex = index(indexPosition);

                            // We can define the role (i.e. the property) that has been udpated
                            QVector<int> updatedRoles;
                            updatedRoles.append(_signalRoles.value(sourceSignalIndex));

                            Q_EMIT dataChanged(modelIndex, modelIndex, updatedRoles);
                        }
                    }
                    else
                    {
                        // Signal index is unknown OR not associated to a role
                        // We can only perform a brute force QML update i.e. notify that all properties have changed

                        // Notify a property change for each instance
                        for (int indexPosition : indexes)
                        {
                            // Create an index
                            QModelIndex modelIndex = index(indexPosition);

                            Q_EMIT dataChanged(modelIndex, modelIndex);
                        }
                    }
                }
                // Else: item is no more in our list (should not happen ?)
            }
        }
    }


    /**
     * @brief Callback called when an item is deleted outside of our list
     * @param item
     */
    void _onListItemExternalDeletion(QObject* deletedItem) Q_DECL_OVERRIDE
    {
        if (deletedItem != nullptr)
        {
            //qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: item deleted outside of our list model!\n==> Are you sure that you have coded your deletion code right?";

            // Try to find all indexes of our deleted item
            // NB: We can not directly used our remove(item) function because deletedItem is no
            //     more a CustomItemType object but a simple QObject
            QList<int> deletedItemIndexes;

            for (int index = 0; index < _list.count(); index++)
            {
                 CustomItemType* listItem = _list.at(index);

                 // NB: comparison of pointer addresses
                 if (listItem == deletedItem)
                 {
                     //NB: it's important to use prepend instead of append (see below)
                     deletedItemIndexes.prepend(index);
                 }
            }


            // Check the number of indexes
            int numberOfIndexes = deletedItemIndexes.count();
            if (numberOfIndexes > 0)
            {
                // Check if we have one or more occurrences of this item in our list
                if (numberOfIndexes == 1)
                {
                    // Easy case: we have a unique entry in our list
                    int invalidIndex = deletedItemIndexes.at(0);

                    beginRemoveRows(QModelIndex(), invalidIndex, invalidIndex);

                    // Remove item at the invalid index
                    _list.takeAt(invalidIndex);

                    endRemoveRows();


                    // Update properties (count, etc.)
                    _listUpdated();
                }
                else
                {
                    // Complex case: we have multiple entries in our list

                    qWarning() << "I2CustomItemListModel<" << CustomItemType::staticMetaObject.className() << "> warning: removing" << numberOfIndexes << "occurrences of a deleted item";

                    // NB: Costly update because we need to notify a global update of our list
                    beginResetModel();

                    // Remove all occurrences in our list
                    // NB: deletedItemIndexes has been built with prepend to ensure that higher index positions
                    //     are at the beginning of our list.
                    //     Thus the following indexes are still valid when we remove an item from our list.
                    for (int index = 0; index < deletedItemIndexes.count(); index++)
                    {
                        int invalidIndex = deletedItemIndexes.at(index);
                        _list.takeAt(invalidIndex);
                    }

                    endResetModel();


                    // Update properties (count, etc.)
                    _listUpdated();
                }
            }
            // Else: Should not happen
        }
    }


private:

    /**
     * @brief Subscribe to signals of a specific item
     * @param item
     */
    void _subscribeToItem(CustomItemType *item)
    {
        if (item != nullptr)
        {
            // Subscribe to each notify signal associated to a property of our item class
            for (auto notifySignalIndex: _propertyNotifySignals)
            {
                QMetaMethod notifySignal = item->metaObject()->method(notifySignalIndex);
                // NB: Qt::UniqueConnection to ensure that we only subscribe once to each item
                connect (item, notifySignal, this, _onListItemChangedCallback, Qt::UniqueConnection);
            }

            // Subscribe to deletion of our item
            // NB: Qt::UniqueConnection to ensure that we only subscribe once to each item
            connect(item, SIGNAL(destroyed(QObject*)), this, SLOT(_onListItemExternalDeletion(QObject*)),  Qt::UniqueConnection);
        }
    }


    /**
     * @brief Unsubscribe to signals of a specific item
     * @param item
     */
    void _unsubscribeToItem(CustomItemType *item)
    {
        if (item != nullptr)
        {
            // Remove all subscriptions to signals of our item
            disconnect(item, 0, this, 0);
        }
    }


    /**
     * @brief Called when our list is updated, to check if our count has changed
     */
    void _listUpdated()
    {
        if (_lastItemCount != _list.count())
        {
            _lastItemCount = _list.count();
            Q_EMIT countChanged();
        }
    }

private:
    // Flag indicating if our list allows duplicate items or not
    bool _allowDuplicateItems;

    // Last number of items
    int _lastItemCount;

    // List used to store our items
    QList<CustomItemType *> _list;

    // Hashtable associating a role value (int) to a property name
    QHash<int, QByteArray> _roleNames;

    // Hashtable associating a signal index to a role value
    QHash<int, int> _signalRoles;

    // List of notify signals of our item class
    QList<int> _propertyNotifySignals;

    // Callback used when an item has been updated (for optimization purpose only)
    QMetaMethod _onListItemChangedCallback;
};



#endif // _I2_CUSTOMITEMLISTMODEL_H_
