/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef CHATS_PROXY_MODEL_H
#define CHATS_PROXY_MODEL_H

#include <QtGui/QSortFilterProxyModel>

class ChatFilter;
class ChatsModel;

class ChatsProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

	ChatsModel *SourceChatsModel;
	QList<ChatFilter *> Filters;

	bool BrokenStringCompare;
	bool SortByStatus;
	int compareNames(const QString &n1, const QString &n2) const;

private slots:
	void modelDestroyed();

protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public:
	explicit ChatsProxyModel(QObject *parent = 0);
	virtual ~ChatsProxyModel();

	virtual void setSourceModel(QAbstractItemModel *sourceModel);

	void addFilter(ChatFilter *filter);
	void removeFilter(ChatFilter *filter);

};

#endif // CHATS_PROXY_MODEL_H
