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


#ifndef _I2_ENUMLISTMODEL_H_
#define _I2_ENUMLISTMODEL_H_

#include <QAbstractListModel>
#include <QObject>
#include <QtQml>
#include <QDebug>
#include <QMetaProperty>
#include <QQmlEngine>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2EnumListItemData class is used to store infos (integer value, name) about each value of an enum
 */
class I2QUICK_EXPORT I2EnumListItemData : public QObject
{
    Q_OBJECT

    // Value of our enum
    Q_PROPERTY (int value READ value CONSTANT)

    // Pretty name of our enum i.e. the name that will be used to display our enum value in a textfield
    Q_PROPERTY (QString name READ name CONSTANT)

    // Key of our enum i.e. the identifier of our enum value as declared in C++
    Q_PROPERTY (QString key READ key CONSTANT)

public:
    /**
     * @brief Constructor
     * @param key
     * @param value
     * @param name
     * @param parent
     */
    explicit I2EnumListItemData (QString key, int value, QString name, QObject* parent = nullptr);


   /**
     * @brief Value of our enum
     * @return
     */
    int value() const {
        return _value;
    }

    /**
     * @brief Pretty name of our enum i.e. the name that will be used to display our enum value in a textfield
     * @return
     */
    QString name() const {
        return _name;
    }

    /**
     * @brief Key of our enum i.e. the identifier of our enum value as declared in C++
     * @return
     */
    QString key() const {
        return _key;
    }

protected:
    // Key of our enum value
    QString _key;

    // Enum value
    int _value;

    // Pretty name of our enum value
    QString _name;
};

QML_DECLARE_TYPE(I2EnumListItemData)



/**
 * @brief AbstractI2EnumListModel defines the base class of our template for enum list models
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 */
class I2QUICK_EXPORT AbstractI2EnumListModel : public QAbstractListModel
{
    Q_OBJECT

    // count property to mimic QQmlListProperty
    Q_PROPERTY (int count READ count NOTIFY countChanged)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit AbstractI2EnumListModel (QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AbstractI2EnumListModel();


public: // Helpers to switch between role and property name

    /**
     * @brief Returns the model's role names
     * @return
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;


    /**
     * @brief Get the role associated to a given property name
     * @param name
     * @return
     */
    int roleForName(const QByteArray &name);


    /**
     * @brief Get the property name associated to a given role
     * @param role
     * @return
     */
    QByteArray nameForRole(int role);


public: // List API

    /**
     * @brief Get the size of our list
     * @return
     * NB: Equivalent to count()
     */
    Q_INVOKABLE int size() const;


    /**
     * @brief Get the size of our list
     * @return
     * NB: equivalent to size()
     */
    Q_INVOKABLE int count() const;


    /**
     * @brief Check if our list is empty
     * @return
     */
    Q_INVOKABLE bool isEmpty() const;


    /**
     * @brief Get item at a given position
     * @param index
     * @return
     */
    Q_INVOKABLE I2EnumListItemData* get(int index);


    /**
     * @brief Get the enum value at a given position
     * @param index
     * @return
     */
    Q_INVOKABLE int getValue(int index);


    /**
     * @brief Get the enum key at a given position
     * @param index
     * @return
     */
    Q_INVOKABLE QString getKey(int index);


    /**
     * @brief Check if our list contains a given enum value
     * @param enumValue
     * @return
     */
    Q_INVOKABLE bool containsEnumValue(int enumValue);


    /**
     * @brief Get index of a given enum value in our list
     * @param enumValue
     * @return -1 if our list does not contain this enum value
     */
    Q_INVOKABLE int indexOfEnumValue(int enumValue);


    /**
     * @brief Append a given enum value to our list
     * @param enumValue
     */
    Q_INVOKABLE virtual void appendEnumValue(int enumValue) = 0;


    /**
     * @brief Remove a given enum value from our list
     * @param enumValue
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE virtual void removeEnumValue(int enumValue) = 0;


    /**
     * @brief Delete a given enum value from our list
     * @param enumValue
     */
    Q_INVOKABLE virtual void deleteEnumValue(int enumValue) = 0;


    /**
     * @brief Append a list of enum values to our list
     * @param enumValues
     */
    Q_INVOKABLE virtual void appendEnumValues(QList<int> enumValues) = 0;


    /**
     * @brief Check if our list contains a given enum key
     * @param enumKey
     * @return
     */
    Q_INVOKABLE virtual bool containsEnumKey(QString enumKey) = 0;


    /**
     * @brief Get index of a given enum value in our list
     * @param enumKey
     * @return -1 if our list does not contain this enum key
     */
    Q_INVOKABLE virtual int indexOfEnumKey(QString enumKey) = 0;


    /**
     * @brief Append a given enum key to our list
     * @param enumValue
     */
    Q_INVOKABLE virtual void appendEnumKey(QString enumKey) = 0;


    /**
     * @brief Remove a given enum value from our list
     * @param enumKey
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE virtual void removeEnumKey(QString enumKey) = 0;


    /**
     * @brief Delete a given enum value from our list
     * @param enumKey
     */
    Q_INVOKABLE virtual void deleteEnumKey(QString enumKey) = 0;


    /**
     * @brief Append a list of enum keys to our list
     * @param enumValues
    */
    Q_INVOKABLE virtual void appendEnumKeys(QList<QString> enumKeys) = 0;


    /**
     * @brief Fill our list with all values of our enum
     */
    virtual void fillWithAllEnumValues() = 0;


    /**
     * @brief Delete all items in our list
     */
    void deleteAllItems();


    /**
     * @brief Returns an STL-style iterator pointing to the first item in the list
     * @return
     */
    typename QList<I2EnumListItemData *>::const_iterator begin() const
    {
        return _list.begin();
    }


    /**
     * @brief Returns an STL-style iterator pointing to the imaginary item after the last item in the list
     * @return
     */
    typename QList<I2EnumListItemData *>::const_iterator end() const
    {
        return _list.end();
    }


    /**
     * @brief Returns a const STL-style iterator pointing to the first item in the list
     * @return
     */
    typename QList<I2EnumListItemData *>::const_iterator constBegin() const
    {
        return _list.constBegin();
    }


    /**
     * @brief Returns a const STL-style iterator pointing to the imaginary item after the last item in the list
     * @return
     */
    typename QList<I2EnumListItemData *>::const_iterator constEnd() const
    {
        return _list.constEnd();
    }


    /**
     * @brief Get a QList representation of our list model
     * @return A casted QList
     */
    const QList<I2EnumListItemData *> toList() const
    {
        return QList<I2EnumListItemData *>(_list);
    }


    /**
     * @brief Get a generic QList, i.e. a QList of int values, representation of our list model
     * @return A QList of int values
     */
    QList<int> toEnumValuesList() const
    {
        QList<int> result;

        for (I2EnumListItemData* item : _list)
        {
            result.append(item->value());
        }

        return result;
    }


    /**
     * @brief Role value used to get a pointer to the Qt object stored in our list
     * @return
     */
    int QtObjectRole() const
    {
        return Qt::UserRole;
    }


public: // Enum list specific API

  /**
   * @brief Get the item in our list associated to a given enum value
   * @param enumValue
   * @return
   */
  Q_INVOKABLE I2EnumListItemData* getItemWithValue(int enumValue);


  /**
   * @brief Get the item in our list associated to a given enum key
   * @param enumKey
   * @return
   */
  Q_INVOKABLE I2EnumListItemData* getItemWithKey(QString enumKey);


Q_SIGNALS:
    /**
     * @brief Triggers when the number of items in our list changes
     */
    void countChanged ();


protected Q_SLOTS:
    /**
     * @brief Callback called when an item is deleted outside of our list
     * @param item
     */
    void _onListItemExternalDeletion(QObject *item);


protected:
    /**
     * @brief Subscribe to signals of a specific item
     * @param item
     */
    void _subscribeToItem(I2EnumListItemData *item);


    /**
     * @brief Unsubscribe to signals of a specific item
     * @param item
     */
    void _unsubscribeToItem(I2EnumListItemData *item);


    /**
     * @brief Called when our list is updated, to check if our count has changed
     */
    void _listUpdated();


protected:
    // Last number of items in our list
    int _lastItemCount;

    // List used to store our items
    QList<I2EnumListItemData *> _list;

    // Roles for each property of our item class
    QHash<int, QByteArray> _roleNames;
};




/**
 * @brief I2EnumListModel defines a template to create a list model for enums
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 *
 * TODO: Find a way to define methods in our .cpp file: on OSX, clang triggers "symbols not found" errors
 *       when we try to define methods in our .cpp file
 */
template<class CustomEnumType> class I2EnumListModel : public AbstractI2EnumListModel
{
public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2EnumListModel(QObject *parent = nullptr): AbstractI2EnumListModel(parent)
    {
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

        QMetaObject enumListItemMetaObject = I2EnumListItemData::staticMetaObject;
        int propertyCount = enumListItemMetaObject.propertyCount ();
        int propertyRoleStartIndex = QtObjectRole() + 1;
        for (int index = 0; index < propertyCount; index++)
        {
            // Get name of our property
            QMetaProperty metaProperty = enumListItemMetaObject.property(index);
            QByteArray propertyName = QByteArray (metaProperty.name());

            // Check if we can add it to our list of roles
            if (!qmlReservedRoleNames.contains(propertyName))
            {
                // NB: Roles must be greater than QtObjectRole()
                int role = propertyRoleStartIndex + index;
                _roleNames.insert(role, propertyName);
            }
        }


        //
        // Check if we have a valid enum class
        //
        _enumClassMetaObject = CustomEnumType::staticMetaObject;

        // Check if this enum class has been generated with our property helper
        int enumeratorIndex = _enumClassMetaObject.indexOfEnumerator("Value");
        if (enumeratorIndex != -1)
        {
          _enumType =  _enumClassMetaObject.enumerator(enumeratorIndex);

          // Check if our enum class defines a 'enumToString' method
          int toStringIndex = _enumClassMetaObject.indexOfMethod("enumToString(int)");
          _isValidEnumClass = (toStringIndex != -1);

          if (!_isValidEnumClass)
          {
            qWarning () << "I2EnumListModel warning: class " << _enumClassMetaObject.className()
                        << " does not define a enumToString(int) method, i.e. it is not an enum class"
                        << " generated with a property helper";
          }
        }
        else
        {
          _isValidEnumClass = false;

          qWarning () << "I2EnumListModel warning: class " << _enumClassMetaObject.className()
                      << " does not define an enum named 'Value', i.e. it is not an enum class"
                      << " generated with a property helper";
        }
    }


    /**
      * Destructor
      */
    ~I2EnumListModel()
    {
        // Delete all items
        deleteAllItems();
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
            I2EnumListItemData *item = _list.at(index.row());
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
                  qDebug() << "I2EnumListModel warning in data(): invalid role" << role
                            << "for item at index " << index.row() << "(" << item << ")";
                }
              }
              //Else: role is Qt::DisplayRole
            }
            else
            {
              qDebug() << "I2EnumListModel warning in data(): item at index " << index.row() << " is undefined";
            }
        }
        // Else: We don't have an item at this index, we will return an empty value
        else
        {
          qDebug() << "I2EnumListModel warning in data(): invalid index " << index.row();
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
        Q_UNUSED(index);
        Q_UNUSED(value);
        Q_UNUSED(role);

        // Enum items are read-only, we can not edit their properties
        return false;
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
    void appendRow(I2EnumListItemData *item)
    {
        if (item != nullptr)
        {
            // Insert item in our list
            beginInsertRows (QModelIndex(), rowCount(), rowCount());
            _subscribeToItem(item);
            _list.append(item);
            endInsertRows ();

            // Update count
            _listUpdated();
        }
    }


    /**
     * @brief Append multiple items to our list
     * @param items
     */
    void appendRows(const QList<I2EnumListItemData *> &items)
    {
        // Check if we have at least one item to add
        if (items.size() != 0)
        {
            // Insert items in our list
            beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
            for(auto item: items)
            {
                _subscribeToItem(item);
                _list.append(item);
            }
            endInsertRows();

            // Update count
            _listUpdated();
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
              // Remove item
              beginRemoveRows(QModelIndex(), row, row);
              I2EnumListItemData* item = _list.takeAt(row);
              if (item != nullptr)
              {
                  _unsubscribeToItem(item);
              }
              endRemoveRows();

              // update count
              _listUpdated();

              result = true;
          }
          // Else: invalid index
          else
          {
              qWarning() << "I2EnumListModel<" << _enumClassMetaObject.className() << "> warning: can not remove item at" << row << "(invalid index)";
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

          // Check if we can remove items
          if ((count > 0) && (row >= 0) && ((row+count) <= _list.size()))
          {
              beginRemoveRows(QModelIndex(), row, row+count-1);
              for(int index = 0; index < count; index++)
              {
                  I2EnumListItemData* item = _list.takeAt(row);
                  if (item != nullptr)
                  {
                      _unsubscribeToItem(item);
                  }
              }
              endRemoveRows();

              // update count
              _listUpdated();

              result = true;
          }
          else
          {
              qWarning() << "I2EnumListModel<" << _enumClassMetaObject.className() << "> warning: can not remove items in range ["
                         << row << "," << (row + count-1) << "] (invalid range)";
          }

          return result;
    }


public: // List API

    /**
     * @brief Append a given enum value to our list
     * @param enumValue
     */
    void appendEnumValue(int enumValue) Q_DECL_OVERRIDE
    {
        // Check if we don't already have this value
        if (!containsEnumValue(enumValue))
        {
              // Check if we have a valid enum value
              const char *key = _enumType.valueToKey(enumValue);
              if (key != nullptr)
              {
                CustomEnumType myTempEnumObject;
                QString currentEnumItemKey = QString::fromLatin1(key);
                QString currentEnumItemName = myTempEnumObject.enumToString(enumValue);

                I2EnumListItemData* item = new I2EnumListItemData(currentEnumItemKey, enumValue, currentEnumItemName);
                appendRow(item);
              }
              else
              {
                qWarning() << "I2EnumListModel warning: enum" << _enumClassMetaObject.className()
                           << " does not define value '" << enumValue << "'";
              }
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum value" << enumValue << "is already in our list";
        }
    }


    /**
     * @brief Remove a given enum value from our list
     * @param enumValue
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void removeEnumValue(int enumValue) Q_DECL_OVERRIDE
    {
        int index = indexOfEnumValue(enumValue);
        if (index != -1)
        {
            // Remove our item
            removeRow(index);
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum value" << enumValue << "can not be removed because it is not in our list";
        }
    }


    /**
     * @brief Delete a given enum value from our list
     * @param enumValue
     */
    void deleteEnumValue(int enumValue) Q_DECL_OVERRIDE
    {
        int index = indexOfEnumValue(enumValue);
        if (index != -1)
        {
            // Save our item before removing it from our list
            I2EnumListItemData* item = _list.at(index);

            // Remove our item
            removeRow(index);

            // Delete our item
            if (item != nullptr)
            {
                delete item;
            }
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum value" << enumValue << "can not be deleted because it is not in our list";
        }
    }


    /**
     * @brief Append a list of enum values to our list
     * @param enumValues
     */
    void appendEnumValues(QList<int> enumValues) Q_DECL_OVERRIDE
    {
        QList<I2EnumListItemData *> items;
        CustomEnumType myTempEnumObject;

        for (int enumValue : enumValues)
        {
            // Check if we don't already have this value
            if (!containsEnumValue(enumValue))
            {
                  // Check if we have a valid enum value
                  const char *key = _enumType.valueToKey(enumValue);
                  if (key != nullptr)
                  {
                    QString enumKey = QString::fromLatin1(key);
                    QString enumName = myTempEnumObject.enumToString(enumValue);

                    I2EnumListItemData* item = new I2EnumListItemData(enumKey, enumValue, enumName);
                    items.append(item);
                  }
                  else
                  {
                    qWarning() << "I2EnumListModel warning: enum " << _enumClassMetaObject.className()
                               << " does not define value '" << enumValue << "'";
                  }
            }
            // Else: enum value is already in our list => nothing to do
        }

        if (!items.isEmpty())
        {
          appendRows(items);
        }
    }


    /**
     * @brief Check if our list contains a given enum key
     * @param enumKey
     * @return
     */
    bool containsEnumKey(QString enumKey) Q_DECL_OVERRIDE
    {
        return (indexOfEnumKey(enumKey) != -1);
    }


    /**
     * @brief Get index of a given enum key in our list
     * @param enumKey
     * @return -1 if our list does not contain this enum key
     */
    int indexOfEnumKey(QString enumKey) Q_DECL_OVERRIDE
    {
        int result = -1;

        int index = 0;
        while ((index < _list.count()) && (result == -1))
        {
            I2EnumListItemData* item = _list.at(index);
            if ((item != nullptr) && (item->key() == enumKey))
            {
                result = index;
            }

            index++;
        }

        return result;
    }


    /**
     * @brief Append a given enum key to our list
     * @param enumValue
     */
    void appendEnumKey(QString enumKey) Q_DECL_OVERRIDE
    {
        // Check if we don't already have this key
        if (!containsEnumKey(enumKey))
        {
              // Check if we have a valid enum key
              bool ok;
              int enumValue = _enumType.keyToValue(enumKey.toLatin1(), &ok);
              if (ok)
              {
                    CustomEnumType myTempEnumObject;
                    QString currentEnumItemName = myTempEnumObject.enumToString(enumValue);

                    I2EnumListItemData* item = new I2EnumListItemData(enumKey, enumValue, currentEnumItemName);
                    appendRow(item);
              }
              else
              {
                    qWarning() << "I2EnumListModel warning: enum " << _enumClassMetaObject.className()
                               << " does not define key '" << enumKey << "'";
              }
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum" << enumKey << "is already in our list";
        }
    }


    /**
     * @brief Remove a given enum key from our list
     * @param enumKey
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void removeEnumKey(QString enumKey) Q_DECL_OVERRIDE
    {
        int index = indexOfEnumKey(enumKey);
        if (index != -1)
        {
            // Remove our item
            removeRow(index);
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum key" << enumKey << "can not be removed because it is not in our list";
        }
    }


    /**
     * @brief Delete a given enum key from our list
     * @param enumKey
     */
    void deleteEnumKey(QString enumKey) Q_DECL_OVERRIDE
    {
        int index = indexOfEnumKey(enumKey);
        if (index != -1)
        {
            // Save our item before removing it from our list
            I2EnumListItemData* item = _list.at(index);

            // Remove our item
            removeRow(index);

            // Delete our item
            if (item != nullptr)
            {
                delete item;
            }
        }
        else
        {
            qWarning() << "I2EnumListModel warning: enum key" << enumKey << "can not be deleted because it is not in our list";
        }
    }



    /**
     * @brief Append a list of enum keys to our list
     * @param enumValues
     */
    void appendEnumKeys(QList<QString> enumKeys) Q_DECL_OVERRIDE
    {
      QList<I2EnumListItemData *> items;
      CustomEnumType myTempEnumObject;

      for (QString enumKey : enumKeys)
      {
          // Check if we don't already have this key
          if (!containsEnumKey(enumKey))
          {
                // Check if we have a valid enum key
                bool ok;
                int enumValue = _enumType.keyToValue(enumKey.toLatin1(), &ok);
                if (ok)
                {
                  QString currentEnumItemName = myTempEnumObject.enumToString(enumValue);

                  I2EnumListItemData* item = new I2EnumListItemData(enumKey, enumValue, currentEnumItemName);
                  appendRow(item);
                }
                else
                {
                  qWarning() << "I2EnumListModel warning: enum " << _enumClassMetaObject.className()
                             << " does not define key '" << enumKey << "'";
                }
          }
          else
          {
              qWarning() << "I2EnumListModel warning: enum" << enumKey << "is already in our list";
          }
      }

      if (!items.isEmpty())
      {
        appendRows(items);
      }
    }



    /**
     * @brief Fill our list with all values of our enum
     */
    void fillWithAllEnumValues() Q_DECL_OVERRIDE
    {
         // Delete all items to avoid duplicates if needed
         if (_list.count() > 0)
         {
             qWarning() << "I2EnumListModel<" << _enumClassMetaObject.className() << "> warning: list is not empty, deleteAllItems() should have been called before fillWithAllEnumValues()";
             deleteAllItems();
         }


          //
          // Fill our list with all values of our enum class
          //
          if (_isValidEnumClass)
          {
              CustomEnumType myTempEnumObject;

              // Check if we have at least one value
              if (_enumType.keyCount() > 0)
              {
                   beginInsertRows(QModelIndex(), rowCount(), rowCount() + _enumType.keyCount() - 1);

                   for(int index = 0; index < _enumType.keyCount(); ++index)
                   {
                       // Get the key of our current enum item
                       QString currentEnumItemKey = QString::fromLatin1(_enumType.key(index));

                       // Get the value of our current enum item
                       int currentEnumItemValue = _enumType.value(index);

                       // get the name of our curent enum item
                       QString currentEnumItemName = myTempEnumObject.enumToString(currentEnumItemValue);

                       // Save value
                       I2EnumListItemData* enumListItem = new I2EnumListItemData(currentEnumItemKey, currentEnumItemValue, currentEnumItemName);
                       _list.append(enumListItem);
                   }

                   endInsertRows();

                   // Update properties (count, etc.)
                   _listUpdated();
              }
              // Else: no value defined in our enum (should not happen)
          }
          else
          {
            qWarning () << "I2EnumListModel warning: class " << _enumClassMetaObject.className()
                        << " does not define an enum named 'Value', i.e. it is not an enum class"
                        << " generated with an I2Quick property helper";
          }
    }


protected:
    // Flag indicating if our list is associated to a valid enum class
    bool _isValidEnumClass;

    // Our enum class
    QMetaObject _enumClassMetaObject;

    // Our enum type
    QMetaEnum _enumType;
};



#endif // _I2_ENUMLISTMODEL_H_
