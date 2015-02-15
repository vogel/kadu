/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TALKABLE_DELEGATE_H
#define TALKABLE_DELEGATE_H

// for OS X
#undef check

#include <QtCore/QPointer>

#include "buddies/buddy.h"
#include "gui/widgets/kadu-tree-view-delegate.h"
#include "gui/widgets/talkable-delegate-configuration.h"
#include "message/message.h"

class Contact;
class ModelChain;

class TalkableDelegate : public KaduTreeViewDelegate
{
	Q_OBJECT

	QPointer<ModelChain> Chain;

private slots:
	void contactUpdated(const Contact &contact);
	void buddyUpdated(const Buddy &buddy);
	void messageStatusChanged(Message message);

public:
	explicit TalkableDelegate(TalkableTreeView *parent = 0);
	virtual ~TalkableDelegate();

	virtual void setChain(ModelChain *chain);

	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

};

#endif // TALKABLE_DELEGATE_H
