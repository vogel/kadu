/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BUDDIES_MODEL_PROXY
#define BUDDIES_MODEL_PROXY

#include <QtGui/QSortFilterProxyModel>

class AbstractBuddyFilter;
class AbstractContactFilter;

class BuddiesModelProxy : public QSortFilterProxyModel
{
	Q_OBJECT

	QList<AbstractBuddyFilter *> BuddyFilters;
	QList<AbstractContactFilter *> ContactFilters;

	bool BrokenStringCompare;
	bool SortByStatus;
	int compareNames(const QString &n1, const QString &n2) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit BuddiesModelProxy(QObject *parent = 0);
	virtual ~BuddiesModelProxy();

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	void addFilter(AbstractContactFilter *filter);
	void removeFilter(AbstractContactFilter *filter);

	void setSortByStatus(bool sortByStatus);

};

#endif // BUDDIES_MODEL_PROXY
