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

#pragma once

#include "widgets/kadu-tree-view.h"

#include "actions/action-context-provider.h"
#include "actions/action-context.h"
#include "buddies/buddy.h"
#include "chat/chat.h"
#include "contacts/contact.h"
#include "talkable/talkable.h"
#include "exports.h"

#include <QtCore/QDateTime>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

class QContextMenuEvent;
class QModelIndex;

class BaseActionContext;
class Buddy;
class BuddySet;
class BuddiesModelProxy;
class ContactSet;
class DeleteTalkableAction;
class EditTalkableAction;
class InjectedFactory;
class MenuInventory;
class ModelChain;
class StatusConfigurationHolder;
class StatusContainerManager;
class TalkableDelegate;
class TalkableDelegateConfiguration;
class ToolTipManager;

class KADUAPI TalkableTreeView : public KaduTreeView, public ActionContextProvider
{
	Q_OBJECT

public:
	explicit TalkableTreeView(QWidget *parent = nullptr);
	virtual ~TalkableTreeView();

	virtual void setChain(ModelChain *chain);
	virtual ModelChain * chain() const;

	void setShowIdentityNameIfMany(bool show);
	void setUseConfigurationColors(bool use);

	TalkableDelegateConfiguration * delegateConfiguration();

	void updateLayout() { updateGeometries(); }

	void setContextMenuEnabled(bool enabled);

	// ActionContextProvider implementation
	virtual ActionContext * actionContext() override;

signals:
	void talkableActivated(const Talkable &talkable);
	void currentChanged(const Talkable &talkable);

protected:
	virtual void contextMenuEvent(QContextMenuEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;

	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;

	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;

	virtual void hideEvent(QHideEvent *event) override;

	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private:
	friend class TalkableDelegateConfiguration;

	QPointer<DeleteTalkableAction> m_deleteTalkableAction;
	QPointer<EditTalkableAction> m_editTalkableAction;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MenuInventory> m_menuInventory;
	QPointer<StatusContainerManager> m_statusContainerManager;
	QPointer<StatusConfigurationHolder> m_statusConfigurationHolder;
	QPointer<ToolTipManager> m_toolTipManager;

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
	INJEQT_SET void setDeleteTalkableAction(DeleteTalkableAction *deleteTalkableAction);
	INJEQT_SET void setEditTalkableAction(EditTalkableAction *editTalkableAction);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMenuInventory(MenuInventory *menuInventory);
	INJEQT_SET void setStatusContainerManager(StatusContainerManager *statusContainerManager);
	INJEQT_SET void setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder);
	INJEQT_SET void setToolTipManager(ToolTipManager *toolTipManager);
	INJEQT_INIT void init();

	void doubleClickedSlot(const QModelIndex &index);

	// Tool tips
	void toolTipTimeout();
	void toolTipRestart(QPoint pos);
	void toolTipHide(bool waitForAnother = true);

	// Actions
	void updateContext();

};
