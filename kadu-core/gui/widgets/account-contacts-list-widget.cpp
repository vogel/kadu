/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "contacts/contact-manager.h"
#include "contacts/account-data/contact-account-data.h"
#include "contacts/model/contacts-model.h"
#include "contacts/model/contacts-model-proxy.h"
#include "contacts/filter/account-contact-filter.h"
#include "gui/windows/message-box.h"

#include "debug.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-list-service.h"

#include "account-contacts-list-widget.h"

AccountContactsListWidget::AccountContactsListWidget(Account *account, QWidget *parent) : QWidget(parent), CurrentAccount(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	ContactsWidget = new ContactsListView(0, this);
	ContactsModelProxy *model = new ContactsModelProxy(this);
	model->setSourceModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->setModel(model);
	ContactsWidget->setMinimumSize(QSize(30, 30));

	QWidget *buttons = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setSpacing(5);

	ImportButton = new QPushButton(tr("Import contacts"), buttons);
	//importButton->setMinimumWidth(ContactsWidget->minimumWidth());
	connect(ImportButton, SIGNAL(clicked()), this, SLOT(startImportTransfer()));
	buttonsLayout->addWidget(ImportButton);	

	ExportButton = new QPushButton(tr("Export contacts"), buttons);
	//exportButton->setMinimumWidth(ContactsWidget->minimumWidth());
	connect(ExportButton, SIGNAL(clicked()), this, SLOT(startExportTransfer()));
	buttonsLayout->addWidget(ExportButton);

	layout->addWidget(ContactsWidget);
	layout->addWidget(buttons);

	AccountContactFilter *filter = new AccountContactFilter(CurrentAccount, this);
	filter->setEnabled(true);
	ContactsWidget->addFilter(filter);

	ContactListService *manager = CurrentAccount->protocol()->contactListService();
	if (!manager)
		return;
	connect(manager, SIGNAL(contactListExported(bool)), this, SLOT(contactListExported(bool)));
	connect(manager, SIGNAL(contactListImported(bool, ContactList)),
		this, SLOT(contactListImported(bool, ContactList)));
}

void AccountContactsListWidget::startImportTransfer()
{
	kdebugf();

	if (!CurrentAccount->protocol()->isConnected())
	{
		MessageBox::msg(tr("Cannot import user list from server in offline mode"), false, "Critical", this);
		return;
	}

	ContactListService *service = CurrentAccount->protocol()->contactListService();
	if (!service)
		return;
	ImportButton->setEnabled(false);
	service->importContactList();

	kdebugf2();
}

void AccountContactsListWidget::startExportTransfer()
{
	kdebugf();

	if (!CurrentAccount->protocol()->isConnected())
	{
		MessageBox::msg(tr("Cannot export user list to server in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}
	ContactListService *service = CurrentAccount->protocol()->contactListService();
	if (!service)
		return;
	ExportButton->setEnabled(false);
	Clear = false;
	service->exportContactList();

	kdebugf2();
}

void AccountContactsListWidget::contactListImported(bool ok, ContactList contacts)
{
	kdebugf();

	ImportButton->setEnabled(true);
	ContactList beforeImportList = ContactManager::instance()->contacts(CurrentAccount, true);
	if (!ok)
		return;

	foreach (Contact contact, contacts)
	{
		Contact c = ContactManager::instance()->byId(CurrentAccount, contact.accountData(CurrentAccount)->id());
		foreach (Contact b, beforeImportList)
			if (b.accountData(CurrentAccount) && b.accountData(CurrentAccount)->id() == c.accountData(CurrentAccount)->id())
				beforeImportList.removeOne(b);
		c.setFirstName(contact.firstName());
		c.setLastName(contact.lastName());
		c.setNickName(contact.nickName());
		c.setMobile(contact.mobile());
		c.setGroups(contact.groups());
		c.setEmail(contact.email());
		c.setDisplay(contact.display());
		c.setHomePhone(contact.homePhone());
		if (c.isAnonymous())
			ContactManager::instance()->addContact(c);
	}

	if (!beforeImportList.isEmpty())
	{
		QStringList contactsList;
		foreach (Contact c, beforeImportList)
			contactsList.append(c.display());
		if (MessageBox::ask(tr("Following contacts from your list were not found on server: %0.\nDo you want to remove them from contacts list?").arg(contactsList.join(", "))))
			foreach (Contact c, beforeImportList)
				ContactManager::instance()->removeContact(c);
	}

	MessageBox::msg(tr("Your contact list has been successfully imported from server"), false, "Infromation", this);

	kdebugf2();
}

void AccountContactsListWidget::contactListExported(bool ok)
{
	kdebugf();

	if (Clear)
		if (ok)
			MessageBox::msg(tr("Your contact list has been successfully deleted on server"), false, "Infromation", this);
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe delete userlist on server was unsuccessful"), false, "Critical", this);
	else
		if (ok)
			MessageBox::msg(tr("Your contact list has been successfully exported to server"), false, "Information", this);
		else
			MessageBox::msg(tr("The application encountered an internal error\nThe export was unsuccessful"), false, "Critical", this);

	ExportButton->setEnabled(true);

	kdebugf2();
}
