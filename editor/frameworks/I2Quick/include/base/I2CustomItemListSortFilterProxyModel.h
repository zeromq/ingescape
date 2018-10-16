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


#ifndef _I2_CUSTOMITEMLISTSORTFILTERPROXYMODEL_H_
#define _I2_CUSTOMITEMLISTSORTFILTERPROXYMODEL_H_

#include <QSortFilterProxyModel>

#include <QDebug>
#include <QElapsedTimer>

#include "base/I2CustomItemListModel.h"
#include "helpers/i2enumpropertyhelpers.h"

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The FilterPatternSyntax class defines custom enum values to interpret the meaning of the filter pattern
 */
I2_ENUM(FilterPatternSyntax, RegExp, Wildcard, FixedString)



/**
 * @brief AbstractI2CustomItemSortFilterProxyModel defines the base class of our template for custom sort-filter proxy models
 *
 * NB: we use a base class because "Template classes are not supported by Q_OBJECT". Thus, if we want
 *     to define QMl properties (Q_PROPERTY), signals or Q_INVOKABLE methods, we need a base class
 */
class I2QUICK_EXPORT AbstractI2CustomItemSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    // Count property, i.e. the number of items in our list, to mimic QQmlListProperty
    // => the number of items in our list after sorting and filtering data
    Q_PROPERTY (int count READ count NOTIFY countChanged)

    // Count property to get the number of items in our source model
    // => raw items count of our source model i.e. the number of items of our source model list
    Q_PROPERTY (int sourceModelCount READ sourceModelCount NOTIFY sourceModelCountChanged)

    // Indicates if our source model can contain duplicate items in our list
    Q_PROPERTY (bool sourceModelAllowDuplicateItems READ sourceModelAllowDuplicateItems WRITE setSourceModelAllowDuplicateItems NOTIFY sourceModelAllowDuplicateItemsChanged)


    // Sort order
    Q_PROPERTY (Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)

    // Name of the property used for sorting
    Q_PROPERTY (QString sortProperty READ sortProperty WRITE setSortProperty NOTIFY sortPropertyChanged)


    // Name of the property used for filtering
    Q_PROPERTY (QString filterProperty READ filterProperty WRITE setFilterProperty NOTIFY filterPropertyChanged)

    // Syntax used to interpret the meaning of the filter pattern. RegExp by default
    Q_PROPERTY (FilterPatternSyntax::Value filterPatternSyntax READ filterPatternSyntax WRITE setFilterPatternSyntax NOTIFY filterPatternSyntaxChanged)

    // Filter pattern used for filtering
    Q_PROPERTY (QString filterPattern READ filterPattern WRITE setFilterPattern NOTIFY filterPatternChanged)


public:
    /**
     * @brief Constructor
     *
     * @param parent
     */
    explicit AbstractI2CustomItemSortFilterProxyModel(QObject *parent = 0);


    /**
     * @brief Returns the model's role names
     * @return
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;


    /**
     * @brief Set the role that is used to query the source model's data when sorting items
     * @param role
     */
    virtual void setSortRole(int role) = 0;

    /**
     * @brief Set the order to use for sorting
     * @param order
     */
    virtual void setSortOrder(Qt::SortOrder order) = 0;

    /**
     * @brief Set the property that is used to query the source model's data when sorting items
     * @param propertyName
     */
    virtual void setSortProperty(const QString &propertyName) = 0;

    /**
     * @brief Get the property that is used to query the source model's data when sorting items
     * @return
     */
    virtual QString sortProperty() = 0;

    /**
     * @brief Set the role that is used to query the source model's data when filtering items
     * @param role
     */
    virtual void setFilterRole(int role) = 0;

    /**
     * @brief Set the property that is used to query the source model's data when filtering items
     * @param propertyName
     */
    virtual void setFilterProperty(const QString &propertyName) = 0;

    /**
     * @brief Get the property that is used to query the source model's data when filtering items
     * @return
     */
    virtual QString filterProperty() = 0;

    /**
     * @brief Set the pattern used to filter our list
     * @param filterPattern
     */
    virtual void setFilterPattern(const QString &filterPattern) = 0;

    /**
     * @brief Get the pattern used to filter our list
     * @return
     */
    virtual QString filterPattern() = 0;

    /**
     * @brief Set the syntax used to interpret the meaning of the filter pattern
     * @param patternSyntax
     */
    virtual void setFilterPatternSyntax(FilterPatternSyntax::Value patternSyntax) = 0;

    /**
     * @brief Get the syntax used to interpret the meaning of the filter pattern
     * @return
     */
    virtual FilterPatternSyntax::Value filterPatternSyntax() = 0;


    /**
     * @brief setSourceModelFromSortFilterList
     * @param sortFilterList
     */
    virtual void setSourceModelFromSortFilterList(AbstractI2CustomItemSortFilterProxyModel* sortFilterList) = 0;


//
// QList-like comfort API
//
public:
    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    AbstractI2CustomItemSortFilterProxyModel& operator+= (QObject* item)
    {
        append(item);
        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    AbstractI2CustomItemSortFilterProxyModel& operator<< (QObject* item)
    {
        append(item);
        return (*this);
    }



Q_SIGNALS:
    /**
     * @brief Called when the number of items after sorting and filtering data has changed
     */
    void countChanged();

    /**
     * @brief Called when the number of items in our source model has changed
     */
    void sourceModelCountChanged();

    /**
     * @brief Called when the allowDuplicateItems of our source model has changed
     * @param value
     */
    void sourceModelAllowDuplicateItemsChanged(bool value);

    /**
     * @brief Called when our sort order has changed
     * @param value
     */
    void sortOrderChanged(Qt::SortOrder value);

    /**
     * @brief Called when our sort property has changed
     * @param value
     */
    void sortPropertyChanged(const QString &value);

    /**
     * @brief Called when our filter property has changed
     * @param value
     */
    void filterPropertyChanged(const QString &value);

    /**
     * @brief Called when our filter pattern has changed
     * @param value
     */
    void filterPatternChanged(const QString &value);

    /**
     * @brief Called when our filter pattern syntax has changed
     * @param value
     */
    void filterPatternSyntaxChanged(FilterPatternSyntax::Value value);


 protected Q_SLOTS:
    /**
     * @brief Called when our source model triggers a countChanged signal
     */
    virtual void _onSourceModelCountChanged() = 0;

    /**
     * @brief Called when the allowDuplicateItemsChanged signal of our source model has been triggered
     * @param value
     */
    virtual void _onSourceModelAllowDuplicateItemsChanged(bool value) = 0;

    /**
     * @brief Called when our source model is destroyed
     */
    virtual void _onSourceModelDestroyed(QObject *) = 0;

    /**
     * @brief Called when an item of our source list has changed
     * @param source_top_left
     * @param source_bottom_right
     * @param roles
     */
    virtual void _onSourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QVector<int> &roles) = 0;


public: // List API

    /**
     * @brief Get the size of our list after sorting and filtering data
     * @return
     * NB: Equivalent to count()
     */
    virtual int size() const = 0;

    /**
     * @brief Get the size of our source model list i.e. the raw count of items before filtering
     * @return
     * NB: Equivalent to sourceModelCount()
     */
    virtual int sourceModelSize() const = 0;

    /**
     * @brief Get the size of our list after sorting and filtering data
     * @return
     * NB: equivalent to size()
     */
    virtual int count() const = 0;

    /**
     * @brief Get the size of our source model list i.e. the raw count of items before filtering
     * @return
     */
    virtual int sourceModelCount() const = 0;

    /**
     * @brief Check if our source list is empty after sorting and filtering data
     * @return
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Check if our source model list is empty
     * @return
     */
    virtual bool sourceModelIsEmpty() const = 0;


    /**
     * @brief Check if our source model allows duplicate items or not
     * @return
     */
    virtual bool sourceModelAllowDuplicateItems() const  = 0;

    /**
     * @brief Set if our source model allows duplicate items or not
     * @return
     */
    virtual void setSourceModelAllowDuplicateItems(bool value) = 0;


    /**
     * @brief Append an item to our source list (end of our source model list)
     * @param item
     */
    virtual void append(QObject *item) = 0;

    /**
     * @brief Prepend an item to our source list (beginning of our source model list)
     * @param item
     */
    virtual void prepend(QObject *item) = 0;

    /**
     * @brief Remove a given item from our list
     * @param item
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    virtual void remove(QObject *item) = 0;

    /**
     * @brief Remove item at the given position in our list after sorting and filtering data
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    virtual void remove(int index) = 0;

    /**
     * @brief Remove item at the given position in our source model list
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    virtual void sourceModelRemove(int index) = 0;

    /**
     * @brief Check if a given item is in our list after sorting and filtering data
     * @param item
     * @return
     */
    Q_INVOKABLE virtual bool contains(QObject *item) const = 0;

    /**
     * @brief Check if a given item is in our source model list
     * @param item
     * @return
     */
    Q_INVOKABLE virtual bool sourceModelContains(QObject *item) const = 0;

    /**
     * @brief Get index of a given item in our list after sorting and filtering data
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE virtual int indexOf(QObject *item) const = 0;

    /**
     * @brief Get index of a given item in our source model list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    virtual int sourceModelIndexOf(QObject *item) const = 0;

    /**
     * @brief Removes all items from the source list
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
    virtual QList<QObject *> toQObjectList() = 0;

    /**
     * @brief Get item at a given position in our list after sorting and filtering data
     * @param index
     * @return
     */
    Q_INVOKABLE virtual QObject* get(int index) = 0;

    /**
     * @brief Get item at a given position in our source model list
     * @param index
     * @return
     */
    Q_INVOKABLE virtual QObject* sourceModelGet(int index) = 0;

    /**
     * @brief Role value used to get a pointer to the Qt object stored in our list
     * @return
     */
    int QtObjectRole() const
    {
      return Qt::UserRole;
    }


protected:
    FilterPatternSyntax::Value _filterPatternSyntax;
    QString _filterPattern;
};




/**
 * @brief The I2CustomItemSortFilterProxyModel template provides a custom QSortFilterProxyModel
 * for filtering and sorting AbstractI2CustomItemListModel instances
 *
 * NB: we implement our methods in our header file instead of using a .cpp file because we don't want to
 *     force programmers to explicitly instantiate our template for each type
 *     (template class I2CustomItemSortFilterProxyModel<MyType>;) before using it
 *     => it is required by some compilers such as Visutal Studio or clang to avoid linker errors
 *    (e.g. symbol(s) not found for architecture x86_64 with XCode/clang on OSX)
 */
template<class CustomItemType> class I2CustomItemSortFilterProxyModel : public AbstractI2CustomItemSortFilterProxyModel
{
public:
    /**
     * @brief Constructor
     *
     * @param sourceModel
     * @param parent
     */
    explicit I2CustomItemSortFilterProxyModel(QObject *parent = 0) : AbstractI2CustomItemSortFilterProxyModel(parent),
        _lastResultCount(0),
        _originalList(NULL)
    {
        // Enable the dynamic sort&filter which will automatically update
        // our list when items are appended/removed
        setDynamicSortFilter(true);

        // By default, sort and filter are set to be case insensitive
        setSortCaseSensitivity(Qt::CaseInsensitive);
        setFilterCaseSensitivity(Qt::CaseInsensitive);

        // Sort our model
        sort(0);
    }


    /**
      * @brief Destructor
      */
    ~I2CustomItemSortFilterProxyModel()
    {
        // Unsubscribe to our original list (source model) if needed
        if (_originalList != NULL)
        {
            // Unsubscribe to destruction
            disconnect(_originalList, SIGNAL(destroyed(QObject*)), this, SLOT(_onSourceModelDestroyed(QObject *)));

            // Unsubscribe to count changes i.e. the number of items
            disconnect(_originalList, SIGNAL(countChanged()), this, SLOT(_onSourceModelCountChanged()));

            // Unsubscribe to data changes i.e. the properties of our items
            disconnect(_originalList, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                    this, SLOT(_onSourceDataChanged(QModelIndex,QModelIndex,QVector<int>)));

            // Unsubscribe to its allowDuplicateItems property
            disconnect(_originalList, SIGNAL(allowDuplicateItemsChanged(bool)), this, SLOT(_onSourceModelAllowDuplicateItemsChanged(bool)));

            _originalList = NULL;
        }
    }


    /**
    * @brief setSourceModelFromSortFilterList
    * @param sortFilterList
    */
    void setSourceModelFromSortFilterList(AbstractI2CustomItemSortFilterProxyModel* sortFilterList) Q_DECL_OVERRIDE
    {
      if (sortFilterList != NULL)
      {
        setSourceModel(sortFilterList->sourceModel());
      }
      else
      {
        setSourceModel(NULL);
      }
    }


    /**
     * @brief Set our source model
     * @param sourceModel
     */
    void setSourceModel(QAbstractItemModel* sourceModel) Q_DECL_OVERRIDE
    {
        // Check if we have a valid source
        I2CustomItemListModel<CustomItemType>* validSourceModel = dynamic_cast<I2CustomItemListModel<CustomItemType> *>(sourceModel);
        if ((validSourceModel != NULL) || (sourceModel == NULL))
        {
            // Unsubscribe to previous source model
            if (_originalList != NULL)
            {
                // Unsubscribe to destruction
                disconnect(_originalList, SIGNAL(destroyed(QObject*)), this, SLOT(_onSourceModelDestroyed(QObject *)));

                // Unsubscribe to count changes i.e. the number of items
                disconnect(_originalList, SIGNAL(countChanged()), this, SLOT(_onSourceModelCountChanged()));

                // Unsubscribe to data changes i.e. the properties of our items
                disconnect(_originalList, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                        this, SLOT(_onSourceDataChanged(QModelIndex,QModelIndex,QVector<int>)));

                // Unsubscribe to its allowDuplicateItems property
                disconnect(_originalList, SIGNAL(allowDuplicateItemsChanged(bool)), this, SLOT(_onSourceModelAllowDuplicateItemsChanged(bool)));
            }

            // Set our new source model
            _originalList = validSourceModel;
            QSortFilterProxyModel::setSourceModel(sourceModel);

            // Subscribe to our new source model if needed
            if (_originalList != NULL)
            {
                // Subscribe to destruction
                connect(_originalList, SIGNAL(destroyed(QObject*)), this, SLOT(_onSourceModelDestroyed(QObject *)));

                // Subscribe to count changes i.e. the number of items
                connect(_originalList, SIGNAL(countChanged()), this, SLOT(_onSourceModelCountChanged()));

                // Subscribe to data changes i.e. the properties of our items
                connect(_originalList, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                        this, SLOT(_onSourceDataChanged(QModelIndex,QModelIndex,QVector<int>)));

                // Subscribe to its allowDuplicateItems property
                connect(_originalList, SIGNAL(allowDuplicateItemsChanged(bool)), this, SLOT(_onSourceModelAllowDuplicateItemsChanged(bool)));
            }

            // Notify related changes
            Q_EMIT sourceModelCountChanged();
            Q_EMIT countChanged();
        }
        else
        {
            qWarning() << "I2CustomItemSortFilterProxyModel warning: sourceModel " << sourceModel << " does not inherit I2CustomItemListModel class";
            _originalList = NULL;
        }
    }



    /**
     * @brief Set the role that is used to query the source model's data when sorting items
     * @param role
     */
    void setSortRole(int role) Q_DECL_OVERRIDE
    {
        if (role != sortRole())
        {
            QSortFilterProxyModel::setSortRole(role);
            if (_originalList != NULL)
            {
                Q_EMIT sortPropertyChanged(_originalList->nameForRole(role));
            }
            else
            {
                Q_EMIT sortPropertyChanged("");
            }
        }
    }


    /**
     * @brief Set the order to use for sorting
     * @param order
     */
    void setSortOrder(Qt::SortOrder order) Q_DECL_OVERRIDE
    {
        if (sortOrder() != order)
        {
            sort(0, order);
            Q_EMIT sortOrderChanged(order);
        }
    }


    /**
     * @brief Set the property that is used to query the source model's data when sorting items
     * @param propertyName
     */
    void setSortProperty(const QString &propertyName) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            // try to find the role associated to this property name
            int newRole = _originalList->roleForName(propertyName.toUtf8());
            if (newRole == -1)
            {
                qWarning() << "I2CustomItemSortFilterProxyModel warning: unknown property " << propertyName;
            }

            setSortRole(newRole);
        }
        else
        {
            qWarning() << "I2CustomItemSortFilterProxyModel warning: can not assign sort property (" << propertyName << ") without a source model";
        }
    }


    /**
     * @brief Get the property that is used to query the source model's data when sorting items
     * @return
     */
    QString sortProperty() Q_DECL_OVERRIDE
    {
        QString result;

        if (_originalList)
        {
            result = _originalList->nameForRole(sortRole());
        }
        else
        {
            qWarning() << "I2CustomItemSortFilterProxyModel warning: can not get sort property for sort role (" << sortRole() << ") without a source model";
        }

        return result;
    }


    /**
     * @brief Set the role that is used to query the source model's data when filtering items
     * @param role
     */
    void setFilterRole(int role) Q_DECL_OVERRIDE
    {
        if (role != filterRole())
        {
            QSortFilterProxyModel::setFilterRole(role);
            if (_originalList != NULL)
            {
                Q_EMIT filterPropertyChanged(_originalList->nameForRole(role));
            }
            else
            {
                Q_EMIT filterPropertyChanged("");
            }
        }
    }


    /**
     * @brief Set the property that is used to query the source model's data when filtering items
     * @param propertyName
     */
    void setFilterProperty(const QString &propertyName) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            int newRole = _originalList->roleForName(propertyName.toUtf8());
            if (newRole == -1)
            {
                qWarning() << "I2CustomItemSortFilterProxyModel warning: unknown property " << propertyName;
            }
            setFilterRole(newRole);
        }
        else
        {
            qWarning() << "I2CustomItemSortFilterProxyModel warning: can not assign filter property (" << propertyName << ") without a source model";
        }
    }


    /**
     * @brief Get the property that is used to query the source model's data when filtering items
     * @return
     */
    QString filterProperty() Q_DECL_OVERRIDE
    {
        QString result;

        if (_originalList != NULL)
        {
            result = _originalList->nameForRole(filterRole());
        }
        else
        {
            qWarning() << "I2CustomItemSortFilterProxyModel warning: can not get sort property for filter role (" << filterRole() << ") without a source model";
        }

        return result;
    }


    /**
     * @brief Apply a given filter pattern
     * @param pattern The pattern used to filter our list
     * @param patternSyntax Syntax used to interpret the meaning of the filter pattern
     */
    void _applyFilterPattern(QString pattern, FilterPatternSyntax::Value patternSyntax)
    {

        switch (patternSyntax) {
            case FilterPatternSyntax::RegExp:
                {
                    QSortFilterProxyModel::setFilterRegExp(pattern);
                }
                break;

            case FilterPatternSyntax::Wildcard:
                {
                    QSortFilterProxyModel::setFilterWildcard(pattern);
                }
                break;

            case FilterPatternSyntax::FixedString:
                {
                    QSortFilterProxyModel::setFilterFixedString(pattern);
                }
                break;

            default:
                break;
        }

        _filterUpdated();
    }


    /**
     * @brief Set the pattern used to filter our list
     * @param filterPattern
     */
    void setFilterPattern(const QString &filterPattern) Q_DECL_OVERRIDE
    {
        if (_filterPattern != filterPattern)
        {
            _filterPattern = filterPattern;

            _applyFilterPattern(_filterPattern, _filterPatternSyntax);

            filterPatternChanged(filterPattern);
        }
    }


    /**
     * @brief Get the pattern used to filter our list
     * @return
     */
    QString filterPattern() Q_DECL_OVERRIDE
    {
        return _filterPattern;
    }


    /**
     * @brief Set the syntax used to interpret the meaning of the filter pattern
     * @param patternSyntax
     */
    void setFilterPatternSyntax(FilterPatternSyntax::Value patternSyntax) Q_DECL_OVERRIDE
    {
        if (_filterPatternSyntax != patternSyntax)
        {
            _filterPatternSyntax = patternSyntax;

            _applyFilterPattern(_filterPattern, _filterPatternSyntax);

            filterPatternSyntaxChanged(patternSyntax);
        }
    }


    /**
     * @brief Get the syntax used to interpret the meaning of the filter pattern
     * @return
     */
    FilterPatternSyntax::Value filterPatternSyntax() Q_DECL_OVERRIDE
    {
        return _filterPatternSyntax;
    }


    /**
      * @brief Sets the fixed string used to filter the contents of the source model to the given pattern
      *
      * @param pattern
      */
    void setFilterFixedString(const QString& pattern)
    {
        _filterPatternSyntax = FilterPatternSyntax::FixedString;
        _filterPattern = pattern;
        filterPatternSyntaxChanged(_filterPatternSyntax);
        filterPatternChanged(_filterPattern);

        // Filter our list
        QSortFilterProxyModel::setFilterFixedString(pattern);

        _filterUpdated();
    }


    /**
      * @brief Sets the wildcard expression used to filter the contents of the source model to the given pattern.
      *
      * @param pattern
      */
    void setFilterWildcard(const QString& pattern)
    {
        _filterPatternSyntax = FilterPatternSyntax::Wildcard;
        _filterPattern = pattern;
        filterPatternSyntaxChanged(_filterPatternSyntax);
        filterPatternChanged(_filterPattern);

        // Filter our list
        QSortFilterProxyModel::setFilterWildcard(pattern);

        _filterUpdated();
    }


    /**
     * @brief Set the regexp used to filter our list
     *        If no QRegExp is set, everything in the source model will be accepted
     *
     * @param regExp
     */
    void setFilterRegExp(const QRegExp& regExp)
    {
        _filterPatternSyntax = FilterPatternSyntax::RegExp;
        _filterPattern = regExp.pattern();
        filterPatternSyntaxChanged(_filterPatternSyntax);
        filterPatternChanged(_filterPattern);

        // Filter our list
        QSortFilterProxyModel::setFilterRegExp(regExp);

        _filterUpdated();
    }


    /**
     * @brief Set the regexp used to filter our list
     *        If an empty string is set, everything in the source model will be accepted
     *
     * @param pattern
     */
    void setFilterRegExp(const QString& pattern)
    {
        _filterPatternSyntax = FilterPatternSyntax::RegExp;
        _filterPattern = pattern;
        filterPatternSyntaxChanged(_filterPatternSyntax);
        filterPatternChanged(_filterPattern);

        // Filter our list
        QSortFilterProxyModel::setFilterRegExp(pattern);

        _filterUpdated();
    }


public: // List API

    /**
     * @brief Get the size of our list after sorting and filtering data
     * @return
     * NB: Equivalent to count()
     */
    int size() const Q_DECL_OVERRIDE
    {
      return rowCount();
    }


    /**
     * @brief Get the size of our source model list i.e. the raw count of items before filtering
     * @return
     * NB: Equivalent to sourceModelCount()
     */
    int sourceModelSize() const Q_DECL_OVERRIDE
    {
        int size = 0;
        if (_originalList != NULL)
        {
            size = _originalList->size();
        }
        return size;
    }


    /**
     * @brief Get the size of our list after sorting and filtering data
     * @return
     * NB: equivalent to size()
     */
    int count() const Q_DECL_OVERRIDE
    {
        return rowCount();
    }


    /**
     * @brief Get the size of our source model list i.e. the raw count of items before filtering
     * @return
     */
    int sourceModelCount() const Q_DECL_OVERRIDE
    {
        return sourceModelSize();
    }

    /**
     * @brief Check if our source list is empty after sorting and filtering data
     * @return
     */
    bool isEmpty() const Q_DECL_OVERRIDE
    {
      return (count() == 0);
    }


    /**
     * @brief Check if our source model list is empty
     * @return
     */
    bool sourceModelIsEmpty() const Q_DECL_OVERRIDE
    {
        return (sourceModelSize() == 0);
    }


    /**
     * @brief Check if our source model allows duplicate items or not
     * @return
     */
    bool sourceModelAllowDuplicateItems() const Q_DECL_OVERRIDE
    {
        bool result = true;

        if (_originalList != NULL)
        {
            result = _originalList->allowDuplicateItems();
        }

        return result;
    }


    /**
     * @brief Set if our source model allows duplicate items or not
     * @return
     */
    void setSourceModelAllowDuplicateItems(bool value) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->setAllowDuplicateItems(value);
        }
    }



    /**
     * @brief Append an item to our source list (end of our source model list)
     * @param item
     */
    void append(QObject* item) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->append(item);
        }
    }


    /**
     * @brief Prepend an item to our source list (beginning of our source model list)
     * @param item
     */
    void prepend(QObject* item) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->prepend(item);
        }
    }


    /**
     * @brief Remove a given item from our list
     * @param item
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void remove(QObject* item) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->remove(item);
        }
    }


    /**
     * @brief Remove item at the given position in our list after sorting and filtering data
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void remove(int itemIndex) Q_DECL_OVERRIDE
    {
       if ((_originalList != NULL) && (itemIndex >= 0) && (itemIndex < count()))
       {
         // Convert index from resulting list to source list
         QModelIndex itemModelIndex = index(itemIndex, 0);
         QModelIndex mappedItemModelIndex = mapToSource(itemModelIndex);

         _originalList->remove(mappedItemModelIndex.row());
       }
    }


    /**
     * @brief Remove item at the given position in our source model list
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void sourceModelRemove(int index) Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->remove(index);
        }
    }


    /**
     * @brief Check if a given item is in our list after sorting and filtering data
     * @param item
     * @return
     */
    Q_INVOKABLE bool contains(QObject *item) const Q_DECL_OVERRIDE
    {
      bool result = false;

      if (item != NULL)
      {
        CustomItemType* customItem = qobject_cast<CustomItemType *>(item);
        if (customItem != NULL)
        {
          QList<CustomItemType *> resultsList = toList();
          result = resultsList.contains(customItem);
        }
      }

      return result;
    }


    /**
     * @brief Check if a given item is in our source model list
     * @param item
     * @return
     */
    Q_INVOKABLE bool sourceModelContains(QObject* item) const Q_DECL_OVERRIDE
    {
        bool result = false;
        if (_originalList != NULL)
        {
            result = _originalList->contains(item);
        }
        return result;
    }


    /**
     * @brief Get index of a given item in our list after sorting and filtering data
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE int indexOf(QObject *item) const Q_DECL_OVERRIDE
    {
      int index = -1;

      if (item != NULL)
      {
        CustomItemType* customItem = qobject_cast<CustomItemType *>(item);
        if (customItem != NULL)
        {
          QList<CustomItemType *> resultsList = toList();
          index = resultsList.indexOf(customItem);
        }
      }

      return index;
    }


    /**
     * @brief Get index of a given item in our source model list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    int sourceModelIndexOf(QObject* item) const Q_DECL_OVERRIDE
    {
        int index = -1;
        if (_originalList != NULL)
        {
            index = _originalList->indexOf(item);
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
        if (_originalList != NULL)
        {
            _originalList->clear();
        }
    }


    /**
     * @brief Delete all items in our list
     */
    void deleteAllItems() Q_DECL_OVERRIDE
    {
        if (_originalList != NULL)
        {
            _originalList->deleteAllItems();
        }
    }


    /**
     * @brief Get a generic QList, i.e. a QList of QObjects, representation of our list model
     * @return A QList of QObjects
     */
    QList<QObject *> toQObjectList() Q_DECL_OVERRIDE
    {
        QList<QObject *> objectList;

        for (int index = 0; index < count(); index++)
        {
            QObject* item = get(index);
            if (item != NULL)
            {
                objectList.append(item);
            }
        }

        return objectList;
    }


    /**
     * @brief Get item at a given position in our list after sorting and filtering data
     * @param index
     * @return
     */
    Q_INVOKABLE QObject* get(int itemIndex) Q_DECL_OVERRIDE
    {
      QObject* item = NULL;

      if ((_originalList != NULL) && (itemIndex >= 0) && (itemIndex < count()))
      {
        // Convert index from resulting list to source list
        QModelIndex itemModelIndex = index(itemIndex, 0);
        QModelIndex mappedItemModelIndex = mapToSource(itemModelIndex);

        item = _originalList->get(mappedItemModelIndex.row());
      }

      return item;
    }


    /**
     * @brief Get item at a given position in our source model list
     * @param index
     * @return
     */
    QObject *sourceModelGet(int index) Q_DECL_OVERRIDE
    {
        return sourceModelAt(index);
    }


public: // Extra list API

    /**
     * @brief Append multiple items to our list
     * @param items
     */
    void append(const QList<CustomItemType *> &items)
    {
        if (_originalList != NULL)
        {
            _originalList->appendRows(items);
        }
    }


    /**
     * @brief Get item at a given position in our source model list
     * @param index
     * @return
     */
    CustomItemType* sourceModelAt(int index)
    {
        CustomItemType* result = NULL;
        if (_originalList != NULL)
        {
            result = _originalList->at(index);
        }
        return result;
    }

    /**
     * @brief Get item at a given position in our resulting list i.e. the list obtained after sorting and filtering data
     * @param itemIndex
     * @return
     */
    CustomItemType* at(int itemIndex)
    {
      CustomItemType* item = NULL;

      if ((_originalList != NULL) && (itemIndex >= 0) && (itemIndex < count()))
      {
        // Convert index from resulting list to source list
        QModelIndex itemModelIndex = index(itemIndex, 0);
        QModelIndex mappedItemModelIndex = mapToSource(itemModelIndex);

        item = _originalList->at(mappedItemModelIndex.row());
      }

      return item;
    }


    /**
     * @brief Removes the item at index position in our source model list and returns it
     * @param index
     * @return
     */
    CustomItemType* sourceModelTakeAt(int index)
    {
        CustomItemType* result = NULL;
        if (_originalList != NULL)
        {
            result = _originalList->takeAt(index);
        }
        return result;
    }

    /**
     * @brief Removes the item at index position in our resulting list and returns it
     * @param index
     * @return
     */
    CustomItemType* takeAt(int itemIndex)
    {
      CustomItemType* item = NULL;

      if ((_originalList != NULL) && (itemIndex >= 0) && (itemIndex < count()))
      {
        // Convert index from resulting list to source list
        QModelIndex itemModelIndex = index(itemIndex, 0);
        QModelIndex mappedItemModelIndex = mapToSource(itemModelIndex);

        item = _originalList->takeAt(mappedItemModelIndex.row());
      }

      return item;
    }


    /**
     * @brief Get a QList representation of our source model list model
     * @return
     */
    const QList<CustomItemType *> sourceModelToList() const
    {
        QList<CustomItemType *> result;

        if ((_originalList != NULL) && (_originalList->count() != 0))
        {
            result = _originalList->toList();
        }

        return result;
    }


    /**
     * @brief Get a QList representation of our resulting list i.e. the list obtained after sorting and filtering data
     * @return
     */
    const QList<CustomItemType *> toList() const
    {
        QList<CustomItemType *> result;

        if ((rowCount() != 0) && (_originalList != NULL))
        {
          for (int itemIndex = 0; itemIndex < rowCount(); itemIndex++)
          {
            // Convert local index to source index
            QModelIndex itemModelIndex = index(itemIndex, 0);
            QModelIndex mappedItemModelIndex = mapToSource(itemModelIndex);

            CustomItemType* item = _originalList->at(mappedItemModelIndex.row());
            if (item != NULL)
            {
              result.append(item);
            }
          }
        }

        return result;
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
    I2CustomItemSortFilterProxyModel& operator+= (CustomItemType* item)
    {
        if (_originalList != NULL)
        {
            _originalList->appendRow(item);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: can not add an item without a sourceModel";
        }

        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    I2CustomItemSortFilterProxyModel& operator<< (CustomItemType* item)
    {
        if (_originalList != NULL)
        {
            _originalList->appendRow(item);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: can not add an item without a sourceModel";
        }
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemSortFilterProxyModel& operator+= (const QList<CustomItemType*> &list)
    {
        if (_originalList != NULL)
        {
            _originalList->append(list);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: can not add a list of items without a sourceModel";
        }
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemSortFilterProxyModel& operator<< (const QList<CustomItemType*> &list)
    {
        if (_originalList != NULL)
        {
            _originalList->append(list);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "warning: can not add a list of items without a sourceModel";
        }
    }


public:
    /**
     * @brief Called when our source model triggers a countChanged signal
     */
    void _onSourceModelCountChanged() Q_DECL_OVERRIDE
    {
        _filterUpdated();
        Q_EMIT sourceModelCountChanged();
    }


    /**
     * @brief Called when our source model is destroyed
     */
    void _onSourceModelDestroyed(QObject* sender) Q_DECL_OVERRIDE
    {
        if (sender != NULL)
        {
            // Remove references to our source model
            _originalList = NULL;
            QSortFilterProxyModel::setSourceModel(NULL);

            // Notify changes
            sourceModelCountChanged();
            countChanged();
        }
    }


    /**
     * @brief Called when an item of our source list has changed
     * @param source_top_left
     * @param source_bottom_right
     * @param roles
     */
    void _onSourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QVector<int> &roles) Q_DECL_OVERRIDE
    {
        Q_UNUSED(source_top_left)
        Q_UNUSED(source_bottom_right)
        Q_UNUSED(roles)

        _filterUpdated();
    }


    /**
     * @brief Called when the allowDuplicateItemsChanged signal of our source model has been triggered
     * @param value
     */
    void _onSourceModelAllowDuplicateItemsChanged(bool value) Q_DECL_OVERRIDE
    {
        Q_EMIT sourceModelAllowDuplicateItemsChanged(value);
    }


private:

    /**
     * @brief Called when our filter is updated
     */
    void _filterUpdated()
    {
        int newRowCount = rowCount();

        if (_lastResultCount != newRowCount)
        {
            _lastResultCount = newRowCount;
            Q_EMIT countChanged();
        }
    }


protected:
    /**
     * @brief Clean-up reference to our original list
     */
    void _cleanUpOriginalListReference()
    {
        // Unsubscribe to our original list (source model) if needed
        if (_originalList != NULL)
        {
            // Unsubscribe to destruction
            disconnect(_originalList, SIGNAL(destroyed(QObject*)), this, SLOT(_onSourceModelDestroyed(QObject *)));

            // Unsubscribe to count changes i.e. the number of items
            disconnect(_originalList, SIGNAL(countChanged()), this, SLOT(_onSourceModelCountChanged()));

            // Unsubscribe to data changes i.e. the properties of our items
            disconnect(_originalList, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                    this, SLOT(_onSourceDataChanged(QModelIndex,QModelIndex,QVector<int>)));

            // Unsubscribe to its allowDuplicateItems property
            disconnect(_originalList, SIGNAL(allowDuplicateItemsChanged(bool)), this, SLOT(_onSourceModelAllowDuplicateItemsChanged(bool)));

            _originalList = NULL;
        }
    }

protected:
    /// Last result count after sorting and filtering data
    int _lastResultCount;

    /// Our source model
    I2CustomItemListModel<CustomItemType>* _originalList;
};



/**
 * @brief The I2CustomItemSortFilterListModel template provides a list of custom items that is sortable
 *  and filterable. It is a I2CustomItemSortFilterProxyModel that owns its I2CustomItemListModel
 *
 * TODO: Find a way to define methods in our .cpp file: on OSX, clang triggers "symbols not found" errors
 *       when we try to define methods in our .cpp file
 */
template<class CustomItemType> class I2CustomItemSortFilterListModel : public I2CustomItemSortFilterProxyModel<CustomItemType>
{
public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2CustomItemSortFilterListModel(QObject *parent = 0) : I2CustomItemSortFilterProxyModel<CustomItemType>(parent)
    {
        // Use it as our source model
        I2CustomItemSortFilterProxyModel<CustomItemType>::setSourceModel(&_list);
    }


    /**
      * @brief Destructor
      */
    ~I2CustomItemSortFilterListModel()
    {
        // Clean-up reference to our original list
        I2CustomItemSortFilterProxyModel<CustomItemType>::_cleanUpOriginalListReference();

        // Clear our list
        _list.clear();
    }


    /**
     * @brief Set our source model
     * @param sourceModel
     */
    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE
    {
        Q_UNUSED(sourceModel)
        qWarning() << "I2CustomItemSortFilterListModel warning: the sourceModel can not be modified. It is defined at creation";
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
    I2CustomItemSortFilterListModel& operator+= (CustomItemType* item)
    {
        _list.appendRow(item);
        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    I2CustomItemSortFilterListModel& operator<< (CustomItemType* item)
    {
        _list.appendRow(item);
        return (*this);
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemSortFilterListModel& operator+= (const QList<CustomItemType*> &list)
    {
        _list.append(list);
        return (*this);
    }


    /**
     * @brief Append a list of items to our list
     * @param list
     * @return
     */
    I2CustomItemSortFilterListModel& operator<< (const QList<CustomItemType*> &list)
    {
        _list.append(list);
        return (*this);
    }



protected:
    I2CustomItemListModel<CustomItemType> _list;
};


#endif // _I2_CUSTOMITEMLISTSORTFILTERPROXYMODEL_H_
