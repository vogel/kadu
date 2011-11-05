/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Przemysław Rudy (prudy1@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "buddies/buddy-list-mime-data-helper.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy-set.h"
#include "buddies/model/buddies-model-proxy.h"
#include "chat/chat-manager.h"
#include "chat/message/pending-messages-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/main-configuration-holder.h"
#include "contacts/filter/contact-no-unloaded-account-filter.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/actions/base-action-data-source.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/hot-key.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "model/model-index-list-converter.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocols-manager.h"
#include "status/status-container-manager.h"

#include "buddies-list-view-delegate.h"
#include "buddies-list-view-menu-manager.h"

#include "buddies-list-view.h"
#include "tool-tip-class-manager.h"

BuddiesListView::BuddiesListView(QWidget *parent) :
		KaduTreeView(parent), Delegate(0), ProxyModel(new BuddiesModelProxy(this)), ContextMenuEnabled(false)
{
	ActionData = new BaseActionDataSource();
	connect(MainConfigurationHolder::instance(), SIGNAL(setStatusModeChanged()), this, SLOT(updateActionData()));

	Delegate = new BuddiesListViewDelegate(this);
	setItemDelegate(Delegate);

	HideUnloadedFilter = new ContactNoUnloadedAccountFilter(this);
	HideUnloadedFilter->setEnabled(true);

	ToolTipTimeoutTimer.setSingleShot(true);

	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));
}

BuddiesListView::~BuddiesListView()
{
	delete ActionData;
	ActionData = 0;
}

void BuddiesListView::setModel(QAbstractItemModel *model)
{
	ProxyModel->setSourceModel(model);

	Delegate->setModel(ProxyModel);
	QTreeView::setModel(ProxyModel);

	ProxyModel->addFilter(HideUnloadedFilter);

	connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
	        this, SLOT(updateActionData()));
}

void BuddiesListView::addFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void BuddiesListView::removeFilter(AbstractBuddyFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

void BuddiesListView::setShowAccountName(bool show)
{
	Delegate->setShowAccountName(show);
}

void BuddiesListView::useConfigurationColors(bool use)
{
	Delegate->useConfigurationColors(use);
}

BuddiesListViewDelegateConfiguration & BuddiesListView::delegateConfiguration()
{
	return Delegate->configuration();
}

BuddyOrContact BuddiesListView::buddyOrContactAt(const QModelIndex &index) const
{
	switch (index.data(ItemTypeRole).toInt())
	{
		case BuddyRole:
			return index.data(BuddyRole).value<Buddy>();
		case ContactRole:
			return index.data(ContactRole).value<Contact>();
	}

	return BuddyOrContact();
}

Chat BuddiesListView::chatForIndex(const QModelIndex &index) const
{
	if (!index.isValid())
		return Chat::null;

	const Contact &contact = index.data(ContactRole).value<Contact>();
	if (!contact)
		return Chat::null;

	return ChatManager::instance()->findChat(ContactSet(contact));
}

void BuddiesListView::triggerActivate(const QModelIndex& index)
{
	// we need to fetch these 2 object first
	// because afer calling buddyActivated or chatActivate index can became invalid
	// because of changing filters and stuff
	const Chat &chat = ActionData->chat();
	const Buddy &buddy = index.data(BuddyRole).value<Buddy>();

	if (buddy)
		emit buddyActivated(buddy);
	if (chat)
		emit chatActivated(chat);
}

void BuddiesListView::setContextMenuEnabled(bool enabled)
{
	ContextMenuEnabled = enabled;
}

void BuddiesListView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!ContextMenuEnabled)
		return;

	Buddy buddy = indexAt(event->pos()).data(BuddyRole).value<Buddy>();
	if (buddy.isNull())
		return;

	QScopedPointer<QMenu> menu(BuddiesListViewMenuManager::instance()->menu(this, ActionData, buddy.contacts()));
	menu->exec(event->globalPos());
}

bool BuddiesListView::shouldEventGoToFilter(QKeyEvent *event)
{
	return !event->text().isEmpty() && event->text().at(0).isPrint();
}

void BuddiesListView::keyPressEvent(QKeyEvent *event)
{
	// TODO 0.10.0: add proper shortcuts handling
	if (HotKey::shortCut(event, "ShortCuts", "kadu_deleteuser"))
		KaduWindowActions::deleteUserActionActivated(ActionData);
	else if (HotKey::shortCut(event, "ShortCuts", "kadu_persinfo"))
		KaduWindowActions::editUserActionActivated(ActionData);
	else
		switch (event->key())
		{
			case Qt::Key_Return:
			case Qt::Key_Enter:
				triggerActivate(currentIndex());
				break;
			default:
				if (shouldEventGoToFilter(event))
					event->ignore();
				else
					QTreeView::keyPressEvent(event);
		}

	toolTipHide(false);
}

void BuddiesListView::wheelEvent(QWheelEvent *event)
{
	QTreeView::wheelEvent(event);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(QPoint(0, 0), size()).contains(event->pos()))
		toolTipRestart(event->pos());
	else
		toolTipHide(false);
}

void BuddiesListView::leaveEvent(QEvent *event)
{
	QTreeView::leaveEvent(event);
	toolTipHide(false);
}

void BuddiesListView::mousePressEvent(QMouseEvent *event)
{
	QTreeView::mousePressEvent(event);

	// TODO 0.10.0: remove once #1802 is fixed
	if (!indexAt(event->pos()).isValid())
		setCurrentIndex(QModelIndex());

	toolTipHide();
}

void BuddiesListView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	toolTipRestart(event->pos());
}

void BuddiesListView::mouseMoveEvent(QMouseEvent *event)
{
	QTreeView::mouseMoveEvent(event);
	toolTipRestart(event->pos());
}

void BuddiesListView::updateActionData()
{
	ModelIndexListConverter converter(selectedIndexes());

	ActionData->blockChangedSignal();

	ActionData->setBuddies(converter.buddySet());
	ActionData->setContacts(converter.contactSet());

	const Chat &chat = converter.chat();
	ActionData->setChat(chat);

	ActionData->setHasContactSelected(false);
	QModelIndexList selectionList = selectedIndexes();
	foreach (const QModelIndex &selection, selectionList)
		if (ContactRole == selection.data(ItemTypeRole).toInt())
		{
			ActionData->setHasContactSelected(true);
			break;
		}

	if (MainConfigurationHolder::instance()->isSetStatusPerIdentity())
		ActionData->setStatusContainer(chat.chatAccount().accountIdentity().data());
	else if (MainConfigurationHolder::instance()->isSetStatusPerAccount())
		ActionData->setStatusContainer(chat.chatAccount().statusContainer());
	else
		ActionData->setStatusContainer(StatusContainerManager::instance());

	ActionData->unblockChangedSignal();
}

ActionDataSource * BuddiesListView::actionDataSource()
{
	return ActionData;
}

void BuddiesListView::doubleClickedSlot(const QModelIndex &index)
{
	if (index.isValid())
		triggerActivate(index);
}

// Tool Tips

void BuddiesListView::toolTipTimeout()
{
	if (BuddyOrContact::ItemNone != ToolTipItem.type())
	{
		ToolTipClassManager::instance()->showToolTip(QCursor::pos(), ToolTipItem);
		ToolTipTimeoutTimer.stop();
	}
}

#define TOOL_TIP_TIMEOUT 1000

void BuddiesListView::toolTipRestart(QPoint pos)
{
	BuddyOrContact item = buddyOrContactAt(indexAt(pos));

	if (BuddyOrContact::ItemNone != item.type())
	{
		if (item != ToolTipItem)
			toolTipHide();
		ToolTipItem = item;
	}
	else
	{
		toolTipHide();
		ToolTipItem = BuddyOrContact();
	}

	ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
}

void BuddiesListView::toolTipHide(bool waitForAnother)
{
	ToolTipClassManager::instance()->hideToolTip();

	if (waitForAnother)
		ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
	else
		ToolTipTimeoutTimer.stop();
}

void BuddiesListView::hideEvent(QHideEvent *event)
{
	toolTipHide(false);
	QTreeView::hideEvent(event);
}
