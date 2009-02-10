/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QModelIndex>
#include <QtGui/QApplication>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"

#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "contacts/contact-list-mime-data-helper.h"
#include "contacts/contact-manager.h"

#include "contacts/model/contacts-model-proxy.h"

#include "protocols/protocol_factory.h"
#include "protocols/protocol-menu-manager.h"
#include "protocols/protocols_manager.h"

#include "action.h"
#include "icons_manager.h"
#include "userbox.h"

#include "contacts-list-widget-delegate.h"
#include "contacts-list-widget-menu-manager.h"

#include "contacts-list-widget.h"
#include "tool-tip-class-manager.h"

ContactsListWidget::ContactsListWidget(KaduMainWindow *mainWindow, QWidget *parent)
	: QListView(parent), MainWindow(mainWindow), ProxyModel(new ContactsModelProxy(this)), Delegate(0)
{
	// all these tree are needed to make this view updating layout properly
	setLayoutMode(Batched);
	setResizeMode(Adjust);
	setWordWrap(true);

	setDragEnabled(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	Delegate = new ContactsListWidgetDelegate(this);
	setItemDelegate(Delegate);

	Delegate->setModel(ProxyModel);
	QAbstractItemView::setModel(ProxyModel);

	connect(&ToolTipTimeoutTimer, SIGNAL(timeout()), this, SLOT(toolTipTimeout()));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(doubleClickedSlot(const QModelIndex &)));
}

ContactsListWidget::~ContactsListWidget()
{
	if (ProxyModel->sourceModel())
	{
		delete ProxyModel->sourceModel();
		ProxyModel->setSourceModel(0);
	}

	if (Delegate)
	{
		delete Delegate;
		Delegate = 0;
	}
}

void ContactsListWidget::setModel(AbstractContactsModel *model)
{
	if (ProxyModel->sourceModel())
	{
		delete ProxyModel->sourceModel();
		ProxyModel->setSourceModel(0);
	}

	ProxyModel->setSourceModel(dynamic_cast<QAbstractItemModel *>(model));
	ProxyModel->invalidate();
}

void ContactsListWidget::addFilter(AbstractContactFilter *filter)
{
	ProxyModel->addFilter(filter);
}

void ContactsListWidget::removeFilter(AbstractContactFilter *filter)
{
	ProxyModel->removeFilter(filter);
}

ContactList ContactsListWidget::selectedContacts() const
{
	ContactList result;

	QModelIndexList selectionList = selectedIndexes();
	foreach (QModelIndex selection, selectionList)
		result.append(contact(selection));

	return result;
}

Contact ContactsListWidget::contact(const QModelIndex &index) const
{
	const AbstractContactsModel *model = dynamic_cast<const AbstractContactsModel *>(index.model());
	if (!model)
		return Contact::null;

	return model->contact(index);
}

void ContactsListWidget::triggerActivate(const QModelIndex& index)
{
	if (!index.isValid())
		return;
	Contact con = contact(index);
	if (!con.isNull())
		emit contactActivated(con);
}

void ContactsListWidget::contextMenuEvent(QContextMenuEvent *event)
{
	Contact con = contact(indexAt(event->pos()));
	if (con.isNull())
		return;

	QMenu *menu = new QMenu(this);

	foreach (ActionDescription *actionDescription, ContactsListWidgetMenuManager::instance()->contactsListActions())
	{
		if (actionDescription)
		{
			KaduAction *action = actionDescription->createAction(MainWindow);
			menu->addAction(action);
			action->checkState();
		}
		else
			menu->addSeparator();
	}

	QMenu *management = menu->addMenu(tr("User management"));

	foreach (ActionDescription *actionDescription, ContactsListWidgetMenuManager::instance()->managementActions())
		if (actionDescription)
		{
			KaduAction *action = actionDescription->createAction(MainWindow);
			management->addAction(action);
			action->checkState();
		}
		else
			management->addSeparator();

	foreach (Account * account, con.accounts())
	{
		if (!account || !account->protocol())
			continue;

		ProtocolFactory *protocolFactory = account->protocol()->protocolFactory();

		if (!protocolFactory || !protocolFactory->getProtocolMenuManager())
			continue;

		QMenu *account_menu = menu->addMenu(account->name());
		if (!protocolFactory->iconName().isEmpty())
			account_menu->setIcon(icons_manager->loadIcon(protocolFactory->iconName()));

		if (protocolFactory->getProtocolMenuManager()->protocolActions(account, con).size() == 0)
			continue;

		foreach (ActionDescription *actionDescription, protocolFactory->getProtocolMenuManager()->protocolActions(account, con))
			if (actionDescription)
			{
				KaduAction *action = actionDescription->createAction(MainWindow);
				account_menu->addAction(action);
				action->checkState();
			}
			else
				account_menu->addSeparator();
	}

	menu->exec(event->globalPos());
}

void ContactsListWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Return:
		case Qt::Key_Enter:
			triggerActivate(currentIndex());
			break;
		default:
			QAbstractItemView::keyPressEvent(event);
	}

	toolTipHide(false);
}

void ContactsListWidget::wheelEvent(QWheelEvent *event)
{
	QAbstractScrollArea::wheelEvent(event);

	// if event source (e->globalPos()) is inside this widget (QRect(...))
	if (QRect(QPoint(0, 0), size()).contains(event->pos()))
		toolTipRestart();
	else
		toolTipHide(false);
}

void ContactsListWidget::leaveEvent(QEvent *event)
{
	QWidget::leaveEvent(event);
	toolTipHide();
}

void ContactsListWidget::mousePressEvent(QMouseEvent *event)
{
	QAbstractItemView::mousePressEvent(event);
	toolTipHide();
}

void ContactsListWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QListView::mouseReleaseEvent(event);
	toolTipRestart();
}

void ContactsListWidget::mouseMoveEvent(QMouseEvent *event)
{
	QListView::mouseMoveEvent(event);
	toolTipRestart();
}

void ContactsListWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	QListView::currentChanged(current, previous);

	if (!current.isValid())
		return;
	Contact con = contact(current);
	if (!con.isNull())
		emit currentContactChanged(con);
}

void ContactsListWidget::doubleClickedSlot(const QModelIndex &index)
{
	triggerActivate(index);
}

void ContactsListWidget::setBackground(const QString &file, BackgroundMode mode)
{
	BackgroundImageMode = mode;
	BackgroundImageFile = file;
	updateBackground();
}

void ContactsListWidget::updateBackground()
{
	// TODO 0.6.6 fix image "Stretched" + update on resize event - write image into resource tree

	QString style;

	if (BackgroundImageMode == BackgroundNone)
	{
		setStyleSheet(style);
		return;
	}

	QImage *backgroundImage = 0;

	if (BackgroundImageMode == BackgroundStretched)
	{
		/*if (!file.isEmpty() && QFile::exists(file))
			backgroundImage = new QImage(file);
		QImage stretchedImage = backgroundImage->smoothScale(
			ContactsWidget->viewport()->width(), ContactsWidget->viewport()->height());
		QFile file2(":/backgroundImage.png");
		file2.open(QIODevice::WriteOnly);
		stretchedImage.save(&file2, "PNG");
		file = ":/backgroundImage.png";*/
	}

	style = QString("QFrame { background-image: url(%1);").arg(BackgroundImageFile);
	if (BackgroundImageMode != BackgroundTiled && BackgroundImageMode != BackgroundTiledAndCentered)
		style.append(" background-repeat: no-repeat;");
	if (BackgroundImageMode == BackgroundCentered || BackgroundImageMode == BackgroundTiledAndCentered)
		style.append("background-position: center;");
	style.append("background-attachment:fixed;}");
	setStyleSheet(style);
}

// Tool Tips

void ContactsListWidget::toolTipTimeout()
{
	if (!ToolTipContact.isNull())
	{
		ToolTipClassManager::instance()->showToolTip(QCursor().pos(), ToolTipContact);
		ToolTipTimeoutTimer.stop();
	}
}

#define TOOL_TIP_TIMEOUT 1000

void ContactsListWidget::toolTipRestart()
{
	Contact con = contact(currentIndex());

	if (!con.isNull())
	{
		if (con != ToolTipContact)
			toolTipHide();
		ToolTipContact = con;
	}
	else
	{
		toolTipHide();
		ToolTipContact = Contact::null;
	}

	ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
}

void ContactsListWidget::toolTipHide(bool waitForAnother)
{
	ToolTipClassManager::instance()->hideToolTip();

	if (waitForAnother)
		ToolTipTimeoutTimer.start(TOOL_TIP_TIMEOUT);
	else
		ToolTipTimeoutTimer.stop();
}
