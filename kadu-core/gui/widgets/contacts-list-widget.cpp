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

#include "contacts/contact.h"
#include "contacts/contact-list.h"
#include "contacts/contact-manager.h"
#include "contacts/model/contacts-model.h"

#include "action.h"
#include "userbox.h"

#include "contacts-list-widget-delegate.h"

#include "contacts-list-widget.h"

ContactsListWidget::ContactsListWidget(KaduMainWindow *mainWindow, QWidget *parent)
	: QListView(parent), MainWindow(mainWindow)
{
	Delegate = new ContactsListWidgetDelegate(this);

	setModel(new ContactsModel(ContactManager::instance(), this));
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
	const ContactsModel *model = dynamic_cast<const ContactsModel *>(index.model());
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

	foreach (ActionDescription *actionDescription, UserBox::UserBoxActions)
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

	foreach (ActionDescription *actionDescription, UserBox::ManagementActions)
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
