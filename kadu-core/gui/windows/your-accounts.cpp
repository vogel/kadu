/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "accounts/model/accounts-model.h"
#include "misc/misc.h"
#include "icons_manager.h"

#include "your-accounts.h"

YourAccounts::YourAccounts(QWidget*parent) :
		QWidget(parent)
{

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Your accounts"));

	createGui();

	loadWindowGeometry(this, "General", "YourAccountsWindowGeometry", 0, 50, 425, 500);
}

YourAccounts::~YourAccounts()
{
	saveWindowGeometry(this, "General", "YourAccountsWindowGeometry");
}

void YourAccounts::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QHBoxLayout *contentLayout = new QHBoxLayout(this);
	mainLayout->addItem(contentLayout);

	QVBoxLayout *sideLayout = new QVBoxLayout(this);
	contentLayout->addItem(sideLayout);

	QPushButton *newAccount = new QPushButton(tr("New account"), this);
	sideLayout->addWidget(newAccount);

	QListView *accountsView = new QListView(this);
	sideLayout->addWidget(accountsView);
	accountsView->setModel(new AccountsModel(accountsView));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
}
