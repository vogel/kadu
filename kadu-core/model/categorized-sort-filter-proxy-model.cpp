/**
  * This file is part of the KDE project
  * Copyright (C) 2007 Rafael Fernández López <ereslibre@kde.org>
  * Copyright (C) 2007 John Tapsell <tapsell@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include "categorized-sort-filter-proxy-model.h"

#include <limits.h>

#include <QtCore/QItemSelection>
#include <QtCore/QSize>
#include <QtCore/QStringList>

// #include <kstringhandler.h>

CategorizedSortFilterProxyModel::CategorizedSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new Private())

{
}

CategorizedSortFilterProxyModel::~CategorizedSortFilterProxyModel()
{
    delete d;
}

void CategorizedSortFilterProxyModel::sort(int column, Qt::SortOrder order)
{
    d->sortColumn = column;
    d->sortOrder = order;

    QSortFilterProxyModel::sort(column, order);
}

bool CategorizedSortFilterProxyModel::isCategorizedModel() const
{
    return d->categorizedModel;
}

void CategorizedSortFilterProxyModel::setCategorizedModel(bool categorizedModel)
{
    if (categorizedModel == d->categorizedModel)
    {
        return;
    }

    d->categorizedModel = categorizedModel;

    invalidate();
}

int CategorizedSortFilterProxyModel::sortColumn() const
{
    return d->sortColumn;
}

Qt::SortOrder CategorizedSortFilterProxyModel::sortOrder() const
{
    return d->sortOrder;
}

void CategorizedSortFilterProxyModel::setSortCategoriesByNaturalComparison(bool sortCategoriesByNaturalComparison)
{
    if (sortCategoriesByNaturalComparison == d->sortCategoriesByNaturalComparison)
    {
        return;
    }

    d->sortCategoriesByNaturalComparison = sortCategoriesByNaturalComparison;

    invalidate();
}

bool CategorizedSortFilterProxyModel::sortCategoriesByNaturalComparison() const
{
    return d->sortCategoriesByNaturalComparison;
}


bool CategorizedSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (d->categorizedModel)
    {
        int compare = compareCategories(left, right);

        if (compare > 0) // left is greater than right
        {
            return false;
        }
        else if (compare < 0) // left is less than right
        {
            return true;
        }
    }

    return subSortLessThan(left, right);
}

bool CategorizedSortFilterProxyModel::subSortLessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}

int CategorizedSortFilterProxyModel::compareCategories(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant l = (left.model() ? left.model()->data(left, CategorySortRole) : QVariant());
    QVariant r = (right.model() ? right.model()->data(right, CategorySortRole) : QVariant());

    Q_ASSERT(l.isValid());
    Q_ASSERT(r.isValid());
    Q_ASSERT(l.type() == r.type());

    if (l.type() == QVariant::String)
    {
        QString lstr = l.toString();
        QString rstr = r.toString();

//         if (d->sortCategoriesByNaturalComparison)
//         {
//             return KStringHandler::naturalCompare(lstr, rstr);
//         }
//         else
        {
            if (lstr < rstr)
            {
                return -1;
            }

            if (lstr > rstr)
            {
                return 1;
            }

            return 0;
        }
    }

    qlonglong lint = l.toLongLong();
    qlonglong rint = r.toLongLong();

    if (lint < rint)
    {
        return -1;
    }

    if (lint > rint)
    {
        return 1;
    }

    return 0;
}
