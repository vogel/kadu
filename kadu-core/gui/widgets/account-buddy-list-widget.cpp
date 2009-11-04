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

#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "buddies/model/buddies-model.h"
#include "buddies/model/buddies-model-proxy.h"
#include "buddies/filter/account-buddy-filter.h"
#include "gui/windows/message-box.h"

#include "debug.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-list-service.h"

#include "account-buddy-list-widget.h"

AccountBuddyListWidget::AccountBuddyListWidget(Account account, QWidget *parent) : QWidget(parent), CurrentAccount(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	BuddiesWidget = new BuddiesListView(0, this);
	BuddiesModelProxy *model = new BuddiesModelProxy(this);
	model->setSourceModel(new BuddiesModel(BuddyManager::instance(), this));
	BuddiesWidget->setModel(model);
	BuddiesWidget->setMinimumSize(QSize(30, 30));

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

	layout->addWidget(BuddiesWidget);
	layout->addWidget(buttons);

	AccountBuddyFilter *filter = new AccountBuddyFilter(CurrentAccount, this);
	filter->setEnabled(true);
	BuddiesWidget->addFilter(filter);

	ContactListService *manager = CurrentAccount.protocolHandler()->contactListService();
	if (!manager)
		return;
	connect(manager, SIGNAL(contactListExported(bool)), this, SLOT(contactListExported(bool)));
	connect(manager, SIGNAL(contactListImported(bool, BuddyList)),
		this, SLOT(buddiesListImported(bool, BuddyList)));
}

void AccountBuddyListWidget::startImportTransfer()
{
	kdebugf();

	if (!CurrentAccount.protocolHandler()->isConnected())
	{
		MessageBox::msg(tr("Cannot import user list from server in offline mode"), false, "Critical", this);
		return;
	}

	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;
	ImportButton->setEnabled(false);
	service->importContactList();

	kdebugf2();
}

void AccountBuddyListWidget::startExportTransfer()
{
	kdebugf();

	if (!CurrentAccount.protocolHandler()->isConnected())
	{
		MessageBox::msg(tr("Cannot export user list to server in offline mode"), false, "Critical", this);
		kdebugf2();
		return;
	}

	ContactListService *service = CurrentAccount.protocolHandler()->contactListService();
	if (!service)
		return;

	ExportButton->setEnabled(false);
	Clear = false;
	service->exportContactList();

	kdebugf2();
}

void AccountBuddyListWidget::buddiesListImported(bool ok, BuddyList buddies)
{
	kdebugf();

	ImportButton->setEnabled(true);
	BuddyList beforeImportList = BuddyManager::instance()->buddies(CurrentAccount, true);
	if (!ok)
		return;

	foreach (Buddy buddy, buddies)
	{
		Buddy c = BuddyManager::instance()->byId(CurrentAccount, buddy.accountData(CurrentAccount)->id());
		foreach (Buddy b, beforeImportList)
			if (b.accountData(CurrentAccount) && b.accountData(CurrentAccount)->id() == c.accountData(CurrentAccount)->id())
				beforeImportList.removeOne(b);
		c.setFirstName(buddy.firstName());
		c.setLastName(buddy.lastName());
		c.setNickName(buddy.nickName());
		c.setMobile(buddy.mobile());
		c.setGroups(buddy.groups());
		c.setEmail(buddy.email());
		c.setDisplay(buddy.display());
		c.setHomePhone(buddy.homePhone());
		if (c.isAnonymous())
			BuddyManager::instance()->addBuddy(c);
	}

	if (!beforeImportList.isEmpty())
	{
		QStringList contactsList;
		foreach (Buddy c, beforeImportList)
			contactsList.append(c.display());
		if (MessageBox::ask(tr("Following contacts from your list were not found on server: %0.\nDo you want to remove them from contacts list?").arg(contactsList.join(", "))))
			foreach (Buddy c, beforeImportList)
				BuddyManager::instance()->removeBuddy(c);
	}

	MessageBox::msg(tr("Your contact list has been successfully imported from server"), false, "Infromation", this);

	kdebugf2();
}

void AccountBuddyListWidget::buddiesListExported(bool ok)
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
