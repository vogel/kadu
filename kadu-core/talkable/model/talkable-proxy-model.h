/*
 * %kadu copyright begin%
 * %kadu copyright end%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef TALKABLE_PROXY_MODEL_H
#define TALKABLE_PROXY_MODEL_H

#include <QtGui/QSortFilterProxyModel>

class AbstractBuddyFilter;
class Buddy;
class Chat;
class Contact;
class TalkableFilter;

class TalkableProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	QList<TalkableFilter *> TalkableFilters;
	QList<AbstractBuddyFilter *> BuddyFilters;

	bool BrokenStringCompare;
	bool SortByStatus;
	int compareNames(const QString &n1, const QString &n2) const;

	bool lessThan(const Chat &left, const Chat &right) const;
	bool lessThan(const Buddy &left, const Buddy &right) const;
	bool lessThan(const Contact &left, const Contact &right) const;

	bool accept(const Chat &chat) const;
	bool accept(const Buddy &buddy) const;
	bool accept(const Contact &contact) const;

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit TalkableProxyModel(QObject *parent = 0);
	virtual ~TalkableProxyModel();

	void addFilter(TalkableFilter *filter);
	void removeFilter(TalkableFilter *filter);

	void addFilter(AbstractBuddyFilter *filter);
	void removeFilter(AbstractBuddyFilter *filter);

	void setSortByStatus(bool sortByStatus);

};

#endif // TALKABLE_PROXY_MODEL_H
