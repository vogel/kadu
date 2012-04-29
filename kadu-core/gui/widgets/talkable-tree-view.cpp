/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#include <QtCore/QDir>
#include <QtCore/QModelIndex>
#include <QtCore/QScopedPointer>
#include <QtCore/QTemporaryFile>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QImage>
#include <QtGui/QMenu>

#include "accounts/account.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/buddy.h"
#include "chat/chat-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/main-configuration-holder.h"
#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/actions/base-action-context.h"
#include "gui/actions/delete-talkable-action.h"
#include "gui/actions/edit-talkable-action.h"
#include "gui/hot-key.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/filtered-tree-view.h"
#include "gui/widgets/talkable-delegate.h"
#include "gui/widgets/talkable-menu-manager.h"
#include "gui/widgets/tool-tip-class-manager.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/kadu-window-actions.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "model/model-chain.h"
#include "model/model-index-list-converter.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocol.h"
#include "protocols/protocols-manager.h"
#include "status/status-container-manager.h"

#include "talkable-tree-view.h"

TalkableTreeView::TalkableTreeView(QWidget *parent) :
		KaduTreeView(parent), Delegate(0), Chain(0), ContextMenuEnabled(false)
{
	Context = new BaseActionContext();
	connect(MainConfigurationHolder::instance(), SIGNAL(setStatusModeChanged()), this, SLOT(updateContext()));

	Delegate = new TalkableDelegate(this);
	setItemDelegate(Delegate);

	ToolTipTimeoutTimer.setSingleShot(true);

	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));

	updateContext();
}

TalkableTreeView::~TalkableTreeView()
{
	disconnect(MainConfigurationHolder::instance(), 0, this, 0);

	delete Context;
	Context = 0;
}

void TalkableTreeView::setChain(ModelChain *chain)
{
	Chain = chain;
	Delegate->setChain(Chain);

	QTreeView::setModel(Chain->lastModel());

	connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	        this, SLOT(updateContext()));

	// for TalkableProxyModel
	connect(model(), SIGNAL(invalidated()), this, SLOT(updateContext()));

	// maybe contact priorities changed?
	// fix for #2392
	connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateContext()));
}

ModelChain * TalkableTreeView::chain() const
{
	return Chain;
}

void TalkableTreeView::setShowIdentityNameIfMany(bool show)
{
	Delegate->setShowIdentityNameIfMany(show);
}

void TalkableTreeView::setUseConfigurationColors(bool use)
{
	Delegate->setUseConfigurationColors(use);
}

TalkableDelegateConfiguration & TalkableTreeView::delegateConfiguration()
{
	return Delegate->configuration();
}

Talkable TalkableTreeView::talkableAt(const QModelIndex &index) const
{
	switch (index.data(ItemTypeRole).toInt())
	{
		case BuddyRole:
			return Talkable(index.data(BuddyRole).value<Buddy>());
		case ContactRole:
			return Talkable(index.data(ContactRole).value<Contact>());
		case ChatRole:
			return Talkable(index.data(ChatRole).value<Chat>());
	}

	return Talkable();
}

void TalkableTreeView::triggerActivate(const QModelIndex& index)
{
	// Context->chat() can be different that Chat talkable on this index
	// if more than one non-chat items are selected at the same time
	const Talkable &talkable = Context->chat().isNull()
			? talkableAt(index)
			: Talkable(Context->chat());

	if (!talkable.isEmpty())
		emit talkableActivated(talkable);
}

void TalkableTreeView::setContextMenuEnabled(bool enabled)
{
	ContextMenuEnabled = enabled;
}

void TalkableTreeView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!ContextMenuEnabled)
		return;

	QScopedPointer<QMenu> menu(TalkableMenuManager::instance()->menu(this, Context));
	menu->exec(event->globalPos());
}

void TalkableTreeView::keyPressEvent(QKeyEvent *event)
{
	// TODO 0.10.0: add proper shortcuts handling
	if (HotKey::shortCut(event, "ShortCuts", "kadu_deleteuser"))
		Core::instance()->kaduWindow()->kaduWindowActions()->deleteTalkable()->trigger(Context);
	else if (HotKey::shortCut(event, "ShortCuts", "kadu_persinfo"))
		Core::instance()->kaduWindow()->kaduWindowActions()->editTalkable()->trigger(Context);
	else
	{
		switch (event->key())
		{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				triggerActivate(currentIndex());
				break;
			default:
				KaduTreeView::keyPressEvent(event);
		}
	}

	toolTipHide(false);
}

void TalkableTreeView::wheelEvent(QWheelEvent *event)
{
	QTreeView::wheelEvent(event);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(QPoint(0, 0), size()).contains(event->pos()))
		toolTipRestart(event->pos());
	else
		toolTipHide(false);
}

void TalkableTreeView::leaveEvent(QEvent *event)
{
	QTreeView::leaveEvent(event);
	toolTipHide(false);
}

void TalkableTreeView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);

	// TODO 0.10.0: remove once #1802 is fixed
	if (!indexAt(event->pos()).isValid())
		setCurrentIndex(QModelIndex());

	toolTipHide();
}

void TalkableTreeView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	toolTipRestart(event->pos());
}

void TalkableTreeView::mouseMoveEvent(QMouseEvent *event)
{
	QTreeView::mouseMoveEvent(event);
	toolTipRestart(event->pos());
}

StatusContainer * TalkableTreeView::statusContainerForChat(const Chat &chat) const
{
	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		return chat.chatAccount().accountIdentity().data();
	else if (MainConfigurationHolder::instance()->isSetStatusPerAccount())
		return chat.chatAccount().statusContainer();
	else
		return StatusContainerManager::instance();
}

void TalkableTreeView::setCurrentTalkable(const Talkable &talkable)
{
	if (CurrentTalkable == talkable)
		return;

	CurrentTalkable = talkable;
	emit currentChanged(CurrentTalkable);
}

void TalkableTreeView::updateContext()
{
	// cuurent index is part of context
	setCurrentTalkable(talkableAt(currentIndex()));

	ModelIndexListConverter converter(selectedIndexes());

	Context->changeNotifier()->block();

	Context->setRoles(converter.roles());
	Context->setBuddies(converter.buddies());
	Context->setContacts(converter.contacts());
	Context->setChat(converter.chat());
	Context->setStatusContainer(statusContainerForChat(converter.chat()));

	Context->changeNotifier()->unblock();
}

ActionContext * TalkableTreeView::actionContext()
{
	return Context;
}

void TalkableTreeView::doubleClickedSlot(const QModelIndex &index)
{
	if (index.isValid())
		triggerActivate(index);
}

// Tool Tips

void TalkableTreeView::toolTipTimeout()
{
	if (Talkable::ItemNone != ToolTipItem.type())
	{
		ToolTipClassManager::instance()->showToolTip(QCursor::pos(), ToolTipItem);
		ToolTipTimeoutTimer.stop();
	}
}

#define TOOL_TIP_TIMEOUT 1000

void TalkableTreeView::toolTipRestart(QPoint pos)
{
	Talkable item = talkableAt(indexAt(pos));

	if (Talkable::ItemNone != item.type())
	{
		if (item != ToolTipItem)
			toolTipHide();
		ToolTipItem = item;
	}
	else
	{
		toolTipHide();
		ToolTipItem = Talkable();
	}

	ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
}

void TalkableTreeView::toolTipHide(bool waitForAnother)
{
	ToolTipClassManager::instance()->hideToolTip();

	if (waitForAnother)
		ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
	else
		ToolTipTimeoutTimer.stop();
}

void TalkableTreeView::hideEvent(QHideEvent *event)
{
	toolTipHide(false);
	QTreeView::hideEvent(event);
}

void TalkableTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	QTreeView::currentChanged(current, previous);

	setCurrentTalkable(talkableAt(current));
}
