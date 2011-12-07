/*
 * %kadu copyright begin%
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SELECT_BUDDY_POPUP_H
#define SELECT_BUDDY_POPUP_H

#include "talkable/talkable.h"

#include "gui/widgets/filtered-tree-view.h"

class QModelIndex;

class Buddy;
class TalkableFilter;
class TalkableProxyModel;
class TalkableTreeView;

class SelectBuddyPopup : public FilteredTreeView
{
	Q_OBJECT

	TalkableTreeView *View;
	TalkableProxyModel *ProxyModel;

private slots:
	void itemClicked(const QModelIndex &index);
	void talkableActivated(const Talkable &talkable);

public:
	explicit SelectBuddyPopup(QWidget *parent = 0);
	virtual ~SelectBuddyPopup();

	void show(Buddy buddy);

	void addFilter(TalkableFilter *filter);
	void removeFilter(TalkableFilter *filter);

signals:
	void buddySelected(Buddy buddy);

};

#include "buddies/buddy.h" // for MOC

#endif // SELECT_BUDDY_POPUP_H
