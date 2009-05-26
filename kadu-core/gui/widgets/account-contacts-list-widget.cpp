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
#include "contacts/model/contacts-model.h"
#include "contacts/model/contacts-model-proxy.h"
#include "contacts/model/filter/account-contact-filter.h"

#include "account-contacts-list-widget.h"

AccountContactsListWidget::AccountContactsListWidget(Account *account, QWidget *parent) : QWidget(parent), CurrentAccount(account)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(5);

	ContactsWidget = new ContactsListWidget(0, this);
	ContactsModelProxy *model = new ContactsModelProxy(this);
	model->setSourceModel(new ContactsModel(ContactManager::instance(), this));
	ContactsWidget->setModel(model);
	ContactsWidget->setMinimumSize(QSize(30, 30));

	QWidget *buttons = new QWidget(this);
	QHBoxLayout *buttonsLayout = new QHBoxLayout(buttons);
	buttonsLayout->setContentsMargins(0, 0, 0, 0);
	buttonsLayout->setSpacing(5);

	QPushButton *importButton = new QPushButton(tr("Import contacts"), buttons);
	//importButton->setMinimumWidth(ContactsWidget->minimumWidth());
	//connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));
	buttonsLayout->addWidget(importButton);	

	QPushButton *exportButton = new QPushButton(tr("Export contacts"), buttons);
	//exportButton->setMinimumWidth(ContactsWidget->minimumWidth());
	//connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));
	buttonsLayout->addWidget(exportButton);

	layout->addWidget(ContactsWidget);
	layout->addWidget(buttons);

	AccountContactFilter *filter = new AccountContactFilter(CurrentAccount, this);
	filter->setEnabled(true);
	ContactsWidget->addFilter(filter);
}
