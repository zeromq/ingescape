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


#ifndef I2SORTFILTERPROXYMODEL_H
#define I2SORTFILTERPROXYMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

#include <QtQml>

#include "base/I2CustomItemListModel.h"
#include "base/I2CustomItemListSortFilterProxyModel.h"

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2SortFilterProxyModel class defines a base class for custom QSortFilterProxyModel
 * with ad-hoc filterAccepts and isLessThan functions
 */
class I2QUICK_EXPORT I2SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    // Count property, i.e. the number of items in our list, to mimic QQmlListProperty
    // => the number of items in our list after sorting and filtering data
    Q_PROPERTY (int count READ count NOTIFY countChanged)

    // Count property to get the number of items in our source model
    // => raw items count of our source model i.e. the number of items of our source model list
    Q_PROPERTY (int sourceModelCount READ sourceModelCount NOTIFY sourceModelCountChanged)

    // Sort order (ascending / descending)
    Q_PROPERTY (Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2SortFilterProxyModel(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~I2SortFilterProxyModel();


    /**
     * @brief Set our source model
     * @param sourceModel
     */
    void setSourceModel(QAbstractItemModel* sourceModel) Q_DECL_OVERRIDE;


    /**
     * @brief Returns the model's role names
     * @return
     */
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;


    /**
     * @brief Get the number of items in our list after sorting and filtering
     * @return
     */
    int count() const;


    /**
     * @brief Get the size of our source model list i.e. the raw count of items before filtering
     * @return
     */
    int sourceModelCount() const;


    /**
     * @brief Set the order to use for sorting
     * @param order
     */
    void setSortOrder(Qt::SortOrder order);


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
    I2SortFilterProxyModel& operator+= (QObject* item)
    {
        append(item);
        return (*this);
    }


    /**
     * @brief Append an item to our list
     * @param item
     * @return
     */
    I2SortFilterProxyModel& operator<< (QObject* item)
    {
        append(item);
        return (*this);
    }




// Function defined by QSortFilterProxyModel
public Q_SLOTS:
    /**
     * @brief Invalidates the current sorting and filtering
     */
    void invalidate();


// Function defined by QSortFilterProxyModel
protected:
    /**
     * @brief Invalidates the current filtering
     */
    void invalidateFilter();


// Functions that must be overridden in a derived class
protected:
    /**
     * @brief filterAccepts is used to check if an item should be included in our result list or not
     *
     * @param item
     * @param index
     *
     * @return True if the item should be included in the result, false otherwise (i.e. the item is removed from our list)
     */
    virtual bool filterAccepts(QObject* item, int index) const;


    /**
     * @brief isLessThan is used to compare items in order to sort them
     *
     * @param item1
     * @param indexItem1
     * @param item2
     * @param indexItem2
     *
     * @return True if item1 is smaller than item2, false otherwise
     */
    virtual bool isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const;


Q_SIGNALS:
    /**
     * @brief Called when our count property has changed
     */
    void countChanged();


    /**
     * @brief Called when the number of items in our source model has changed
     */
    void sourceModelCountChanged();


    /**
     * @brief Called our sort order has changed
     *
     * @param order
     */
    void sortOrderChanged(Qt::SortOrder order);


// List API
public:
    /**
     * @brief Append an item to our source list (end of our source model list)
     * @param item
     */
    Q_INVOKABLE void append(QObject *item);


    /**
     * @brief Prepend an item to our source list (beginning of our source model list)
     * @param item
     */
    Q_INVOKABLE void prepend(QObject *item);


    /**
     * @brief Remove a given item from our list
     * @param item
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    Q_INVOKABLE void remove(QObject *item);


    /**
     * @brief Remove item at the given position in our list after sorting and filtering data
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void remove(int index);


    /**
     * @brief Remove item at the given position in our source model list
     * @param index
     *
     * NB: item will be removed from our list BUT WILL NOT BE DELETED
     */
    void sourceModelRemove(int index);


    /**
     * @brief Check if a given item is in our list after sorting and filtering data
     * @param item
     * @return
     */
    Q_INVOKABLE bool contains(QObject *item) const;


    /**
     * @brief Check if a given item is in our source model list
     * @param item
     * @return
     */
    Q_INVOKABLE bool sourceModelContains(QObject *item) const;


    /**
     * @brief Get index of a given item in our list after sorting and filtering data
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE int indexOf(QObject *item) const;


    /**
     * @brief Get index of a given item in our source model list
     * @param item
     * @return Index of our item OR -1 if no item matched
     */
    Q_INVOKABLE int sourceModelIndexOf(QObject *item) const;


    /**
     * @brief Removes all items from the source list
     *
     * NB: items will be removed from our list BUT WILL NOT BE DELETED
     */
    void clear();


    /**
     * @brief Delete all items in our list
     */
    void deleteAllItems();


    /**
     * @brief Get item at a given position in our list after sorting and filtering data
     * @param index
     * @return
     */
    Q_INVOKABLE QObject* get(int index);


    /**
     * @brief Get item at a given position in our source model list
     * @param index
     * @return
     */
    Q_INVOKABLE QObject* sourceModelGet(int index);


    /**
     * @brief Get a QList representation of our resulting list i.e. the list obtained after sorting and filtering data
     * @return
     */
    const QList<QObject *> toList() const;


    /**
     * @brief Get a QList representation of our source model list model
     * @return
     */
    const QList<QObject *> sourceModelToList() const;


protected Q_SLOTS:
   /**
    * @brief Called when our source model triggers a countChanged signal
    */
   void _onSourceModelCountChanged();


   /**
    * @brief Called when our source model is destroyed
    */
   void _onSourceModelDestroyed(QObject *);


   /**
    * @brief Called when our count may have changed
    */
   void _onCountMayHaveChanged();


protected:

    /**
     * @brief filterAcceptsRow Function called to filter items of our source model
     * @param source_row
     * @param source_parent
     * @return Returns true if the item in the row indicated by the given source_row and source_parent should be included in the model; otherwise returns false.
     */
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const Q_DECL_OVERRIDE;


    /**
     * @brief lessThan Function called to compare two items of our source model in order to sort them
     * @param source_left
     * @param source_right
     * @return Returns true if the value of the item referred to by the given index source_left is less than the value of the item referred to by the given index source_right, otherwise returns false
     */
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const Q_DECL_OVERRIDE;


    /**
     * @brief Called when our filter is updated
     */
    void _filterUpdated();


    /**
     * @brief Check if a source model is valid
     * @param sourceModel
     * @return
     */
    bool _isValidSourceModel(QAbstractItemModel* sourceModel);


    /**
     * @brief Called to clean-up data
     */
    void _cleanUp();


protected:
    // Used to store our sourceModel if it is an AbstractI2CustomItemListModel
    AbstractI2CustomItemListModel* _sourceModelAsListModel;

    // Used to store our sourceModel if it is an AbstractI2CustomItemSortFilterProxyModel
    AbstractI2CustomItemSortFilterProxyModel* _sourceModelAsSortFilterProxyModel;

    // Used to store our sourceModel if it is an I2SortFilterProxyModel
    I2SortFilterProxyModel* _sourceModelAsI2SortFilterProxyModel;

    // Previous number of items (after filtering) in our sort-filter proxy model
    int _lastResultCount;
};


QML_DECLARE_TYPE(I2SortFilterProxyModel)

#endif // I2CPPSORTFILTERPROXYMODEL_H
