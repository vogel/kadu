/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TALKABLE_TREE_VIEW_H
#define TALKABLE_TREE_VIEW_H

#include <QtCore/QDateTime>
#include <QtCore/QTimer>

#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "gui/actions/action-context-provider.h"
#include "gui/actions/action-context.h"
#include "talkable/talkable.h"
#include "exports.h"

#include "gui/widgets/kadu-tree-view.h"

class QContextMenuEvent;
class QModelIndex;

class BaseActionContext;
class Buddy;
class BuddySet;
class BuddiesModelProxy;
class ContactSet;
class ModelChain;
class TalkableDelegate;
class TalkableDelegateConfiguration;

class KADUAPI TalkableTreeView : public KaduTreeView, public ActionContextProvider
{
	Q_OBJECT

	friend class TalkableDelegateConfiguration;

	TalkableDelegate *Delegate;
	ModelChain *Chain;

	BaseActionContext *Context;
	Talkable CurrentTalkable;

	Talkable talkableAt(const QModelIndex &index) const;
	void triggerActivate(const QModelIndex &index);

	// Tool tips
	Talkable ToolTipItem;
	QTimer ToolTipTimeoutTimer;

	bool ContextMenuEnabled;

	StatusContainer * statusContainerForChat(const Chat &chat) const;

	void setCurrentTalkable(const Talkable &talkable);

private slots:
	void doubleClickedSlot(const QModelIndex &index);

	// Tool tips
	void toolTipTimeout();
	void toolTipRestart(QPoint pos);
	void toolTipHide(bool waitForAnother = true);

	// Actions
	void updateContext();

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

	virtual void wheelEvent(QWheelEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

	virtual void hideEvent(QHideEvent *event);

	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public:
	explicit TalkableTreeView(QWidget *parent = 0);
	virtual ~TalkableTreeView();

	virtual void setChain(ModelChain *chain);
	virtual ModelChain * chain() const;

	void setShowIdentityNameIfMany(bool show);
	void setUseConfigurationColors(bool use);

	TalkableDelegateConfiguration & delegateConfiguration();

	void updateLayout() { updateGeometries(); }

	void setContextMenuEnabled(bool enabled);

	// ActionContextProvider implementation
	virtual ActionContext * actionContext() override;

signals:
	void talkableActivated(const Talkable &talkable);
	void currentChanged(const Talkable &talkable);

};

#endif // TALKABLE_TREE_VIEW_H
