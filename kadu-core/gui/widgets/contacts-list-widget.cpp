/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QModelIndex>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>

#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "contacts/model/contacts-model.h"
#include "contacts/model/contacts-model-proxy.h"

#include "action.h"
#include "userbox.h"

#include "contacts-list-widget-delegate.h"
#include "contacts-list-widget-menu-manager.h"

#include "contacts-list-widget.h"

ContactsListWidget::ContactsListWidget(KaduMainWindow *mainWindow, QWidget *parent)
	: QListView(parent), MainWindow(mainWindow)
{
	// all these tree are needed to make this view updating layout properly
	setLayoutMode(Batched);
	setResizeMode(Adjust);
	setWordWrap(true);

	ContactsModel *model = new ContactsModel(ContactManager::instance(), this);
	ContactsModelProxy *proxyModel = new ContactsModelProxy(this);
	proxyModel->setSourceModel(model);
	proxyModel->invalidate();

	Delegate = new ContactsListWidgetDelegate(proxyModel, this);

	setModel(proxyModel);
	setItemDelegate(Delegate);
}

ContactsListWidget::~ContactsListWidget()
{
	if (Delegate)
	{
		delete Delegate;
		Delegate = 0;
	}
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

	menu->exec(event->globalPos());
}
