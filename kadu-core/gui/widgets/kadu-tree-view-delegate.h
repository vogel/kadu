/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_TREE_VIEW_DELEGATE_H
#define KADU_TREE_VIEW_DELEGATE_H

#include <QtWidgets/QItemDelegate>

#include "gui/widgets/talkable-delegate-configuration.h"

class KaduTreeViewDelegate : public QItemDelegate
{
	Q_OBJECT

	TalkableDelegateConfiguration Configuration;

	bool ShowIdentityNameIfMany;

private slots:
	void updateShowIdentityName();

protected:
	QStyleOptionViewItemV4 getOptions(const QModelIndex &index, const QStyleOptionViewItem &option) const;

public:
	explicit KaduTreeViewDelegate(TalkableTreeView *parent = 0);
	virtual ~KaduTreeViewDelegate();

	void setShowIdentityNameIfMany(bool showIdentityNameIfMany);
	void setUseConfigurationColors(bool use);

	TalkableDelegateConfiguration & configuration() { return Configuration; }

	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // KADU_TREE_VIEW_DELEGATE_H
