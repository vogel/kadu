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
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/hot-key.h"
#include "icons/kadu-icon.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocols-manager.h"

#include "buddies-list-view-delegate.h"
#include "buddies-list-view-menu-manager.h"

#include "buddies-list-view.h"
#include "tool-tip-class-manager.h"

BuddiesListView::BuddiesListView(QWidget *parent) :
		QTreeView(parent), Delegate(0), ProxyModel(new BuddiesModelProxy(this)),
		BackgroundImageMode(BackgroundNone), BackgroundTemporaryFile(0), ContextMenuEnabled(false)
{
	setAnimated(BackgroundImageMode == BackgroundNone);
#ifndef Q_WS_MAEMO_5
	/* Disable as we use kinetic scrolling by default */
	setDragEnabled(true);
#endif
	setItemsExpandable(true);
	setExpandsOnDoubleClick(false);
	setHeaderHidden(true);
	setMouseTracking(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
#ifndef Q_WS_MAEMO_5
	setUniformRowHeights(false);
#endif
	setWordWrap(true);

	Delegate = new BuddiesListViewDelegate(this);
	setItemDelegate(Delegate);

	HideUnloadedFilter = new ContactNoUnloadedAccountFilter(this);
	HideUnloadedFilter->setEnabled(true);

	ToolTipTimeoutTimer.setSingleShot(true);

	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));

	configurationUpdated();
}

BuddiesListView::~BuddiesListView()
{
}

void BuddiesListView::setModel(QAbstractItemModel *model)
{
	AbstractBuddiesModel *buddiesModel = dynamic_cast<AbstractBuddiesModel *>(model);
	if (!buddiesModel)
		return;

	ProxyModel->setSourceModel(model);

	Delegate->setModel(ProxyModel);
	QTreeView::setModel(ProxyModel);

	ProxyModel->addFilter(HideUnloadedFilter);
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

void BuddiesListView::selectBuddy(Buddy buddy)
{
	setCurrentIndex(ProxyModel->indexForValue(buddy));
}

Contact BuddiesListView::currentContact() const
{
	return contactAt(currentIndex());
}

ContactSet BuddiesListView::selectedContacts() const
{
	ContactSet result;

	QModelIndexList selectionList = selectedIndexes();
	foreach (const QModelIndex &selection, selectionList)
	{
		Contact contact = contactAt(selection);
		if (contact)
			result.insert(contact);
	}

	return result;
}

Buddy BuddiesListView::currentBuddy() const
{
	return buddyAt(currentIndex());
}

BuddySet BuddiesListView::selectedBuddies() const
{
	BuddySet result;

	QModelIndexList selectionList = selectedIndexes();
	foreach (const QModelIndex &selection, selectionList)
	{
		Buddy buddy = buddyAt(selection);
		if (buddy)
			result.insert(buddy);
	}

	return result;
}

BuddyOrContact BuddiesListView::buddyOrContactAt(const QModelIndex &index) const
{
	switch (index.data(ItemTypeRole).toInt())
	{
		case BuddyRole:
			return buddyAt(index);
		case ContactRole:
			return contactAt(index);
	}

	return BuddyOrContact();
}

Buddy BuddiesListView::buddyAt(const QModelIndex &index) const
{
	const AbstractBuddiesModel *model = dynamic_cast<const AbstractBuddiesModel *>(index.model());
	if (!model)
		return Buddy::null;

	return model->buddyAt(index);
}

Contact BuddiesListView::contactAt(const QModelIndex &index) const
{
	const AbstractBuddiesModel *model = dynamic_cast<const AbstractBuddiesModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contactAt(index);
}

Chat BuddiesListView::chatForIndex(const QModelIndex &index) const
{
	if (!index.isValid())
		return Chat::null;

	Contact con = contactAt(index);
	if (con.isNull())
		return Chat::null;

	return ChatManager::instance()->findChat(ContactSet(con));
}

Chat BuddiesListView::chatByPendingMessages(const QModelIndex &index) const
{
	if (index.data(ItemTypeRole) == BuddyRole)
		return PendingMessagesManager::instance()->chatForBuddy(buddyAt(index));
	else
		return PendingMessagesManager::instance()->chatForContact(contactAt(index));
}

// TODO 0.10.0: This method is too big. Review and split
Chat BuddiesListView::currentChat() const
{
	BuddySet buddies;
	Contact contact;
	ContactSet contacts;
	Account account;

	QModelIndexList selectionList = selectedIndexes();
	if (selectionList.count() == 1)
	{
		Chat chat = chatByPendingMessages(selectionList.at(0));
		if (chat)
			return chat;
	}

	foreach (const QModelIndex &selection, selectionList)
	{
		if (!account)
		{
			if (selection.data(ItemTypeRole) == BuddyRole)
				buddies.insert(buddyAt(selection));
			else
			{
				contact = contactAt(selection);
				if (!contact)
					return Chat::null;

				contacts.insert(contact);

				account = contact.contactAccount();

				foreach (const Buddy &buddy, buddies)
				{
					contact = BuddyPreferredManager::instance()->preferredContact(buddy, account);
					if (!contact)
						return Chat::null;

					contacts.insert(contact);
				}
			}
		}
		else
		{
			if (selection.data(ItemTypeRole) == BuddyRole)
			{
				contact = BuddyPreferredManager::instance()->preferredContact(buddyAt(selection), account);
				if (!contact)
					return Chat::null;

				contacts.insert(contact);
			}
			else
			{
				contact = contactAt(selection);
				if (!contact)
					return Chat::null;

				if (contact.contactAccount() == account)
					contacts.insert(contact);
				else
					return Chat::null;
			}
		}
	}

	if (!account)
		return ChatManager::instance()->findChat(buddies, true);
	else
	    return ChatManager::instance()->findChat(contacts, true);
}

void BuddiesListView::triggerActivate(const QModelIndex& index)
{
	// we need to fetch these 2 object first
	// because afer calling buddyActivated or chatActivate index can became invalid
	// because of changing filters and stuff
	Chat chat = currentChat();
	Buddy buddy = buddyAt(index);

	if (buddy)
		emit buddyActivated(buddy);
	if (chat)
		emit chatActivated(chat);
}

void BuddiesListView::setContextMenuEnabled(bool enabled)
{
	ContextMenuEnabled = enabled;
}

void BuddiesListView::configurationUpdated()
{
	bool showExpandingControl = config_file.readBoolEntry("Look", "ShowExpandingControl", false);

	if (rootIsDecorated() && !showExpandingControl)
		collapseAll();
	setRootIsDecorated(showExpandingControl);
}

void BuddiesListView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!ContextMenuEnabled)
		return;

	Buddy buddy = buddyAt(indexAt(event->pos()));
	if (buddy.isNull())
		return;

	QScopedPointer<QMenu> menu(BuddiesListViewMenuManager::instance()->menu(this, this, buddy.contacts()));
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
		KaduWindowActions::deleteUserActionActivated(this);
	else if (HotKey::shortCut(event, "ShortCuts", "kadu_persinfo"))
		KaduWindowActions::editUserActionActivated(this);
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

void BuddiesListView::resizeEvent(QResizeEvent *event)
{
	QTreeView::resizeEvent(event);
	if (BackgroundImageMode == BackgroundStretched)
		updateBackground();

	scheduleDelayedItemsLayout();
}

void BuddiesListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	QTreeView::currentChanged(current, previous);

	if (!current.isValid())
	{
		emit currentChanged(BuddyOrContact());
		return;
	}

	BuddyOrContact buddyOrContact = buddyOrContactAt(current);
	if (BuddyOrContact::ItemNone != buddyOrContact.type())
		emit currentChanged(buddyOrContact);
}

void BuddiesListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	emit buddySelectionChanged();
}

void BuddiesListView::doubleClickedSlot(const QModelIndex &index)
{
	if (index.isValid())
		triggerActivate(index);
}

void BuddiesListView::setBackground(const QString &backgroundColor, const QString &alternateColor, const QString &file, BackgroundMode mode)
{
	BackgroundColor = backgroundColor;
	AlternateBackgroundColor = alternateColor;
	setAnimated(mode == BackgroundNone);
	BackgroundImageMode = mode;
	BackgroundImageFile = file;
	updateBackground();
}

void BuddiesListView::updateBackground()
{
	// TODO fix image "Stretched" + update on resize event - write image into resource tree
	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	if (config_file.readBoolEntry("Look", "AlignUserboxIconsTop"))
	{
		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-closed", "16x16").fullPath() + "); margin-top: 4px; image-position: top }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-open", "16x16").fullPath() + "); image-position: top; margin-top: 8px }");
	}
	else
	{
 		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-closed", "16x16").fullPath() + ") }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + KaduIcon("kadu_icons/stylesheet-branch-open", "16x16").fullPath() + ") }");
	}

	style.append("QTreeView { background-color: transparent;");

	QString viewportStyle(QString("QWidget { background-color: %1;").arg(BackgroundColor));

	if (BackgroundImageMode == BackgroundNone)
	{
		setAlternatingRowColors(true);
		style.append(QString("alternate-background-color: %1;").arg(AlternateBackgroundColor));
	}
	else
	{
		setAlternatingRowColors(false);

		if (BackgroundImageMode != BackgroundTiled && BackgroundImageMode != BackgroundTiledAndCentered)
			viewportStyle.append("background-repeat: no-repeat;");

		if (BackgroundImageMode == BackgroundCentered || BackgroundImageMode == BackgroundTiledAndCentered)
			viewportStyle.append("background-position: center;");

		if (BackgroundImageMode == BackgroundStretched)
		{
			// style.append("background-size: 100% 100%;"); will work in 4.6 maybe?
			QImage image(BackgroundImageFile);
			if (!image.isNull())
			{
				delete BackgroundTemporaryFile;
				BackgroundTemporaryFile = new QTemporaryFile(QDir::tempPath() + "/kadu_background_XXXXXX.png", this);

				if (BackgroundTemporaryFile->open())
				{
					QImage stretched = image.scaled(viewport()->width(), viewport()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
					if (stretched.save(BackgroundTemporaryFile, "PNG"))
						viewportStyle.append(QString("background-image: url(%1);").arg(BackgroundTemporaryFile->fileName()));
					BackgroundTemporaryFile->close();
				}
			}
		}
		else
			viewportStyle.append(QString("background-image: url(%1);").arg(BackgroundImageFile));

		viewportStyle.append("background-attachment: fixed;");
	}

	style.append("}");
	viewportStyle.append("}");

	setStyleSheet(style);
	viewport()->setStyleSheet(viewportStyle);
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

BuddySet BuddiesListView::buddies()
{
	return selectedBuddies();
}

ContactSet BuddiesListView::contacts()
{
	return selectedContacts();
}

Chat BuddiesListView::chat()
{
	return currentChat();
}

StatusContainer * BuddiesListView::statusContainer()
{
	return currentChat().chatAccount().statusContainer();
}

bool BuddiesListView::hasContactSelected()
{
	QModelIndexList selectionList = selectedIndexes();
	foreach (const QModelIndex &selection, selectionList)
		if (ContactRole == selection.data(ItemTypeRole).toInt())
			return true;

	return false;
}

void BuddiesListView::hideEvent(QHideEvent *event)
{
	toolTipHide(false);
	QTreeView::hideEvent(event);
}
