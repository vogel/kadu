/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
#include "configuration/main-configuration.h"
#include "contacts/filter/contact-no-unloaded-account-filter.h"
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/kadu-window-actions.h"
#include "gui/windows/main-window.h"
#include "gui/hot-key.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocols-manager.h"

#include "icons-manager.h"

#include "buddies-list-view-delegate.h"
#include "buddies-list-view-menu-manager.h"

#include "buddies-list-view.h"
#include "tool-tip-class-manager.h"

BuddiesListView::BuddiesListView(MainWindow *mainWindow, QWidget *parent) :
		QTreeView(parent), MyMainWindow(mainWindow), Delegate(0), Model(0),
		ProxyModel(new BuddiesModelProxy(this)), BackgroundTemporaryFile(0)
{
	setAlternatingRowColors(true);
	setAnimated(BackgroundImageMode == BackgroundNone);
#ifndef Q_WS_MAEMO_5
	/* Disable as we use kinetic scrolling by default */
	setDragEnabled(true);
#endif
	setExpandsOnDoubleClick(false);
	setHeaderHidden(true);
	setItemsExpandable(true);
	setMouseTracking(true);
	setRootIsDecorated(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
#ifndef Q_WS_MAEMO_5
	setUniformRowHeights(false);
#endif
	setWordWrap(true);

	Delegate = new BuddiesListViewDelegate(this);
	setItemDelegate(Delegate);

	HideUnloadedFilter = new ContactNoUnloadedAccountFilter(this);

	Delegate->setModel(ProxyModel);
	QTreeView::setModel(ProxyModel);

	ToolTipTimeoutTimer.setSingleShot(true);

	connect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));
	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));
	connect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(update()));
	connect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(update()));

	simpleModeChanged();
}

BuddiesListView::~BuddiesListView()
{
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageAdded(Message)), this, SLOT(update()));
	disconnect(PendingMessagesManager::instance(), SIGNAL(messageRemoved(Message)), this, SLOT(update()));
}

void BuddiesListView::setModel(AbstractBuddiesModel *model)
{
	Model = model;

	ProxyModel->setSourceModel(dynamic_cast<QAbstractItemModel *>(model));
	ProxyModel->addFilter(HideUnloadedFilter);
	ProxyModel->sort(1);
	ProxyModel->sort(0); // something is wrong with sorting in my Qt version
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
	QModelIndex index = Model->indexForValue(buddy);
	index = ProxyModel->mapFromSource(index);

	setCurrentIndex(index);
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

// TODO 0.8.0: This method is too big. Review and split
Chat BuddiesListView::currentChat() const
{
	BuddySet buddies;
	Contact contact;
	ContactSet contacts;
	Account account;

	QModelIndexList selectionList = selectedIndexes();
	if (selectionList.count() == 1)
	{
		Chat chat = chatByPendingMessages(selectionList[0]);
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
	Chat chat = currentChat();
	if (chat)
		emit chatActivated(chat);

	Buddy buddy = buddyAt(index);
	if (buddy)
		emit buddyActivated(buddy);
}

void BuddiesListView::contextMenuEvent(QContextMenuEvent *event)
{
	if (!MyMainWindow)
		return;

	Buddy buddy = buddyAt(indexAt(event->pos()));
	if (buddy.isNull())
		return;

	QMenu *menu = BuddiesListViewMenuManager::instance()->menu(this, this, buddy.contacts());
	menu->exec(event->globalPos());
	delete menu;
}

bool BuddiesListView::shouldEventGoToFilter(QKeyEvent *event)
{
	return !event->text().isEmpty() && event->text().at(0).isPrint();
}

void BuddiesListView::keyPressEvent(QKeyEvent *event)
{
	// TODO 0.6.7: add proper shortcuts handling
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

	// TODO 0.6.7: remove once #1802 is fixed
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
	doItemsLayout();
	if (BackgroundImageMode == BackgroundStretched)
		updateBackground();
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

void BuddiesListView::simpleModeChanged()
{
	if (MainConfiguration::instance()->simpleMode() && !config_file.readBoolEntry("General", "ExpandingInSimpleMode", false))
	{
		collapseAll();
		setItemsExpandable(false);
		setRootIsDecorated(false);
	}
	else
	{
		setItemsExpandable(true);
		setRootIsDecorated(true);
	}
}

void BuddiesListView::doubleClickedSlot(const QModelIndex &index)
{
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
	// TODO 0.6.6 fix image "Stretched" + update on resize event - write image into resource tree
	QString style;
	style.append("QTreeView::branch:has-siblings:!adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-siblings:adjoins-item { border-image: none; image: none }");
	style.append("QTreeView::branch:has-childres:!has-siblings:adjoins-item { border-image: none; image: none }");
	if (config_file.readBoolEntry("Look", "AlignUserboxIconsTop"))
	{
		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + IconsManager::instance()->iconPath("kadu_icons/stylesheet-branch-closed", "16x16") + "); margin-top: 4px; image-position: top }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + IconsManager::instance()->iconPath("kadu_icons/stylesheet-branch-open", "16x16") + "); image-position: top; margin-top: 8px }");
	}
	else
	{
 		style.append("QTreeView::branch:has-children:!has-siblings:closed, QTreeView::branch:closed:has-children:has-siblings "
		     "{ border-image: none; image: url(" + IconsManager::instance()->iconPath("kadu_icons/stylesheet-branch-closed", "16x16") + ") }");
		style.append("QTreeView::branch:open:has-children:!has-siblings, QTreeView::branch:open:has-children:has-siblings "
			"{ border-image: none; image: url(" + IconsManager::instance()->iconPath("kadu_icons/stylesheet-branch-open", "16x16") + ") }");
	}

	style.append("QFrame {");

	style.append(QString(" background-color: %1;").arg(BackgroundColor));

	if (BackgroundImageMode == BackgroundNone)
	{
		style.append(QString("alternate-background-color: %1; }").arg(AlternateBackgroundColor));
		setAlternatingRowColors(true);
		setStyleSheet(style);

		return;
	}

	setAlternatingRowColors(false);

	if (BackgroundImageMode != BackgroundTiled && BackgroundImageMode != BackgroundTiledAndCentered)
		style.append(" background-repeat: no-repeat;");
	if (BackgroundImageMode == BackgroundCentered || BackgroundImageMode == BackgroundTiledAndCentered)
		style.append("background-position: center;");
	if (BackgroundImageMode == BackgroundStretched)
	{
		// style.append("background-size: 100% 100%;"); will work in 4.6 maybe?
		QImage image(BackgroundImageFile);
		if (image.isNull())
		{
			setStyleSheet(QString());
			return;
		}

		if (BackgroundTemporaryFile)
			delete BackgroundTemporaryFile;
			BackgroundTemporaryFile = new QTemporaryFile(QDir::tempPath() + "/kadu_background_XXXXXX.png", this);

		if (BackgroundTemporaryFile->open())
		{
			QImage stretched = image.scaled(viewport()->width(), viewport()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
			if (stretched.save(BackgroundTemporaryFile, "PNG"))
				style.append(QString("background-image: url(%1);").arg(BackgroundTemporaryFile->fileName()));
			BackgroundTemporaryFile->close();
		}
	}
	else
		style.append(QString("background-image: url(%1);").arg(BackgroundImageFile));
	style.append("background-attachment:fixed;}");

	setStyleSheet(style);
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
/*
QImage *UserBox::backgroundImage = 0;

class ULEComparer
{
	public:
		inline bool operator()(const Contact &e1, const Contact &e2) const;
		QList<UserBox::CmpFuncDesc> CmpFunctions;
		ULEComparer() : CmpFunctions() {}
};

inline bool ULEComparer::operator()(const Contact &e1, const Contact &e2) const
{
	int ret = 0;
	foreach(const UserBox::CmpFuncDesc &f, CmpFunctions)
	{
		ret = f.func(e1, e2);
//		kdebugm(KDEBUG_WARNING, "%s %s %d\n", qPrintable(e1.altNick()), qPrintable(e2.altNick()), ret);
		if (ret)
			break;
	}
	return ret < 0;
}*/

// CreateNotifier UserBox::createNotifier;


// 	showDescriptionAction = new ActionDescription(
// 		ActionDescription::TypeUserList, "descriptionsAction",
// 		this, SLOT(showDescriptionsActionActivated(QAction *, bool)),
// 		"ShowDescription", tr("Hide descriptions"),
// 		true, tr("Show descriptions")
// 	);
// 	connect(showDescriptionAction, SIGNAL(actionCreated(KaduAction *)), this, SLOT(setDescriptionsActionState()));

// 	setDescriptionsActionState();

// 	addCompareFunction("Status", tr("Statuses"), compareStatus);
// 	if (brokenStringCompare)
// 		addCompareFunction("AltNick", tr("Nicks, case insensitive"), compareAltNickCaseInsensitive);
// 	else
// 		addCompareFunction("AltNick", tr("Nicks"), compareAltNick);

// 	connect(&pending, SIGNAL(messageFromUserAdded(Contact)), this, SLOT(messageFromUserAdded(Contact)));
// 	connect(&pending, SIGNAL(messageFromUserDeleted(Contact)), this, SLOT(messageFromUserAdded(Contact)));

// void UserBox::showDescriptionsActionActivated(QActiogn *sender, bool toggle)
// {
// 	config_file.writeEntry("Look", "ShowDesc", !toggle);
// 	KaduListBoxPixmap::setShowDesc(!toggle);
// 	UserBox::refreshAllLater();
// 	setDescriptionsActionState();
// }

// void UserBox::setDescriptionsActionState()
// {
// 	foreach (KaduAction *action, showDescriptionAction->actions())
// 		action->setChecked(!KaduListBoxPixmap::ShowDesc);
// }

// void UserBox::messageFromUserAdded(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

// void UserBox::messageFromUserDeleted(Contact elem)
// {
// 	if (visibleUsers()->contains(UserListElement::fromContact(elem, AccountManager::instance()->defaultAccount())))
// 		refreshLater();
// }

/*
void UserBox::addCompareFunction(const QString &id, const QString &trDescription,
			int (*cmp)(const Contact &, const Contact &))
{
	comparer->CmpFunctions.append(CmpFuncDesc(id, trDescription, cmp));
	refreshLater();
}

void UserBox::removeCompareFunction(const QString &id)
{
	foreach(const CmpFuncDesc &c, comparer->CmpFunctions)
		if (c.id == id)
		{
			comparer->CmpFunctions.remove(c);
			refreshLater();
			break;
		}
}*/

// bool UserBox::moveUpCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == 0)
//  				break;
//  			d = *c;
//  			--c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c += 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }

// bool UserBox::moveDownCompareFunction(const QString &id)
// {
// 	kdebugf();
// 	CmpFuncDesc d;
// 	int pos = 0;
// 	int cnt = comparer->CmpFunctions.count();
// 	bool found = false;
// 	QList<CmpFuncDesc>::iterator c;
// 	for (c = comparer->CmpFunctions.begin(); c != comparer->CmpFunctions.end(); ++c)
// 	{
//  		if ((*c).id == id)
//  		{
//  			found = true;
//  			if (pos == cnt - 1)
//  				break;
//  			d = *c;
//  			++c;
//  			c = comparer->CmpFunctions.insert(c, d);
//  			c -= 2;
//  			comparer->CmpFunctions.remove(c);
//  			refreshLater();
//  			break;
//  		}
//  		++pos;
// 	}
// 	kdebugf2();
// 	return found;
// }

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

bool BuddiesListView::hasContactSelected()
{
	QModelIndexList selectionList = selectedIndexes();
	foreach (const QModelIndex &selection, selectionList)
		if (ContactRole == selection.data(ItemTypeRole).toInt())
			return true;

	return false;
}
