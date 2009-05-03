/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "accounts/model/accounts-model.h"
#include "contacts/model/contacts-model-base.h"
#include "protocols/model/protocols-model.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"
#include "icons-manager.h"

#include "your-accounts.h"

YourAccounts::YourAccounts(QWidget *parent) :
		QWidget(parent), CurrentNewAccountWidget(0)
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
	setLayout(mainLayout);

	ContentLayout = new QHBoxLayout(this);
	mainLayout->addItem(ContentLayout);

	QVBoxLayout *sideLayout = new QVBoxLayout(this);
	ContentLayout->addItem(sideLayout);
	ContentLayout->setStretchFactor(sideLayout, 1);

	QPushButton *newAccount = new QPushButton(tr("New account"), this);
	sideLayout->addWidget(newAccount);

	QListView *accountsView = new QListView(this);
	sideLayout->addWidget(accountsView);
	accountsView->setModel(new AccountsModel(accountsView));
	accountsView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	accountsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	accountsView->setIconSize(QSize(32, 32));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);
	mainLayout->addWidget(buttons);

	QPushButton *okButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(IconsManager::instance()->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	buttons->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	createNewAccountWidget();
}

void YourAccounts::createNewAccountWidget()
{
	NewAccountContainer = new QWidget(this);
	ContentLayout->addWidget(NewAccountContainer, 100);

	MainNewAccountLayout = new QVBoxLayout(NewAccountContainer);
	NewAccountContainer->setLayout(MainNewAccountLayout);

	QHBoxLayout *selectProtocolLayout = new QHBoxLayout(NewAccountContainer);
	MainNewAccountLayout->addItem(selectProtocolLayout);
	MainNewAccountLayout->setStretchFactor(selectProtocolLayout, 1);

	selectProtocolLayout->addWidget(new QLabel(tr("Select network"), NewAccountContainer));

	Protocols = new QComboBox(NewAccountContainer);
	Protocols->setModel(new ProtocolsModel(Protocols));
	selectProtocolLayout->addWidget(Protocols);

	connect(Protocols, SIGNAL(activated(int)), this, SLOT(protocolChanged(int)));
	protocolChanged(0);
}

void YourAccounts::protocolChanged(int protocolIndex)
{
	delete CurrentNewAccountWidget;
	CurrentNewAccountWidget = 0;

	if (protocolIndex < 0 || protocolIndex >= Protocols->count())
		return;

	ProtocolFactory *factory = ProtocolsManager::instance()->byName(Protocols->itemData(protocolIndex, ProtocolRole).toString());
	if (!factory)
		return;

	CurrentNewAccountWidget = factory->newCreateAccountWidget(NewAccountContainer);

	if (CurrentNewAccountWidget)
		MainNewAccountLayout->addWidget(CurrentNewAccountWidget, 100, Qt::AlignTop);
}
