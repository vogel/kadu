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
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>

#include <QtGui/QLineEdit>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "misc/misc.h"

#include "contact-general-configuration-widget.h"

ContactGeneralConfigurationWidget::ContactGeneralConfigurationWidget(Contact &contact, QWidget *parent)
	: QWidget(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactGeneralConfigurationWidget::~ContactGeneralConfigurationWidget()
{
}

void ContactGeneralConfigurationWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnMinimumWidth(5, 100);
	layout->setColumnMinimumWidth(8, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;

	QLabel *tabLabel = new QLabel(tr("General Properties"), this);
	QFont tabLabelFont = tabLabel->font();
	tabLabelFont.setPointSize(tabLabelFont.pointSize() + 3);
	tabLabelFont.setWeight(QFont::Bold);
	tabLabel->setFont(tabLabelFont);
	layout->addWidget(tabLabel, 0, 1, 1, 4);

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ":", this);
	layout->addWidget(numberLabel, 2, 2, 1, 1);
	DisplayEdit = new QLineEdit(this);
	DisplayEdit->setText(CurrentContact.display());
	layout->addWidget(DisplayEdit, 2, 3, 1, 1);

	QWidget *photoWidget = new QWidget;
	QVBoxLayout *photoLayout = new QVBoxLayout(photoWidget);
	photoLayout->setSpacing(2);

	QLabel *photoLabel = new QLabel(this);
	QPixmap photoPixmap = QPixmap(CurrentContact.accountDatas().at(0)->avatar().pixmap());
	photoLabel->setPixmap(photoPixmap);
	photoLayout->addWidget(photoLabel);

	QPushButton *changePhotoButton = new QPushButton(tr("Change Icon..."));
	photoLayout->addWidget(changePhotoButton);

	layout->addWidget(photoWidget, 1, 6, 3, 1);

	QGroupBox *accountsBox = new QGroupBox(tr("Merged Contact Accounts"));
	AccountsLayout = new QGridLayout(accountsBox);
	AccountsLayout->setColumnMinimumWidth(2, 20);
	AccountsLayout->setColumnStretch(0, 1);
	AccountsLayout->setColumnStretch(1, 1);
	AccountsLayout->setColumnStretch(5, 2);
	AccountsLayout->setColumnStretch(6, 2);
	row = 0;

	QLabel *defaultContactLabel = new QLabel(tr("Default Contact") + ":");

	QComboBox *defaultContact = new QComboBox(this);
	QLabel *defaultContactNoticeLabel = new QLabel(tr("Chat messages will be sent to this username when you select the name from the buddy list"));
	AccountsLayout->addWidget(defaultContactLabel, row, 0, 1, 1);
	AccountsLayout->addWidget(defaultContact, row++, 1, 1, 4);
	AccountsLayout->addWidget(defaultContactNoticeLabel, row++, 1, 1, 5);

	QWidget *contactsWidget = new QWidget(this);
	ContactsLayout = new QGridLayout(contactsWidget);
	AccountsLayout->setColumnStretch(0, 2);
	AccountsLayout->setColumnStretch(2, 2);
	
	foreach (ContactAccountData *data, CurrentContact.accountDatas())
	{
		defaultContact->addItem(data->id());
		addAccountDataRow(data);
	}

	AccountsLayout->addWidget(contactsWidget, row++, 0, 1, 5);

	QPushButton *addContactButton = new QPushButton(tr("Add Contact..."), this);
	connect(addContactButton, SIGNAL(clicked()), this, SLOT(addAccountDataRow()));
	QPushButton *setOrderButton = new QPushButton(tr("Set Order..."), this);

	AccountsLayout->addWidget(addContactButton, row, 0, 1, 1);
	AccountsLayout->addWidget(setOrderButton, row, 1, 1, 1);

	layout->addWidget(accountsBox, 4, 2, 2, 6);

	QGroupBox *communicationBox = new QGroupBox(tr("Communication Information"));
	QGridLayout *communicationLayout = new QGridLayout(communicationBox);
	communicationLayout->setColumnStretch(0, 1);
	communicationLayout->setColumnStretch(1, 3);
	communicationLayout->setColumnStretch(2, 3);

	row = 0;

	QHBoxLayout *phoneLayout = new QHBoxLayout;
	QLabel *phoneLabel = new QLabel(tr("Phone") + ":");
	PhoneEdit = new QLineEdit(this);
	PhoneEdit->setText(CurrentContact.homePhone());
	communicationLayout->addWidget(phoneLabel, row, 0, 1, 1);
	communicationLayout->addWidget(PhoneEdit, row++, 1, 1, 1);

	QHBoxLayout *mobileLayout = new QHBoxLayout;
	QLabel *mobileLabel = new QLabel(tr("Mobile") + ":");
	MobileEdit = new QLineEdit(this);
	MobileEdit->setText(CurrentContact.mobile());
	communicationLayout->addWidget(mobileLabel, row, 0, 1, 1);
	communicationLayout->addWidget(MobileEdit, row++, 1, 1, 1);

	QHBoxLayout *emailLayout = new QHBoxLayout;
	QLabel *emailLabel = new QLabel(tr("E-Mail") + ":");
	EmailEdit = new QLineEdit(this);
	EmailEdit->setText(CurrentContact.email());
	communicationLayout->addWidget(emailLabel, row, 0, 1, 1);
	communicationLayout->addWidget(EmailEdit, row++, 1, 1, 1);

	QHBoxLayout *websiteLayout = new QHBoxLayout;
	QLabel *websiteLabel = new QLabel(tr("Website") + ":");
	WebsiteEdit = new QLineEdit(this);
	WebsiteEdit->setText(CurrentContact.website());
	communicationLayout->addWidget(websiteLabel, row, 0, 1, 1);
	communicationLayout->addWidget(WebsiteEdit, row++, 1, 1, 1);

	layout->addWidget(communicationBox, 6, 2, 2, 6);
	layout->setRowStretch(8, 100);
}

void ContactGeneralConfigurationWidget::addAccountDataRow(ContactAccountData *data)
{
	int row = ContactsLayout->rowCount();
	QLineEdit *contactLineEdit = new QLineEdit(this);
	QLabel *inLabel = new QLabel(tr("in"));
	QComboBox *accountsCombo = new QComboBox(this);

	ContactsIds.append(contactLineEdit);
	ContactsAccounts.append(accountsCombo);

	if (!data)
		accountsCombo->addItem("-" + tr("Select a Network") + "-");
	foreach (Account *account, AccountManager::instance()->accounts())
	{
		accountsCombo->addItem(
		    account->protocol()->icon(), 
		    account->protocol()->protocolFactory()->displayName() + " (" + account->id() + ")",
		    account->uuid().toString()
		);
	}
	if (data)
		accountsCombo->setCurrentIndex(accountsCombo->findData(data->account()->uuid().toString()));

	ContactsLayout->addWidget(contactLineEdit, row, 0, 1, 1);
	ContactsLayout->addWidget(inLabel, row, 1, 1, 1);
	ContactsLayout->addWidget(accountsCombo, row++, 2, 1, 1);

	if (data)
		contactLineEdit->setText(data->id());
}

void ContactGeneralConfigurationWidget::saveConfiguration()
{
	CurrentContact.setDisplay(DisplayEdit->text());
	CurrentContact.setHomePhone(PhoneEdit->text());
	CurrentContact.setMobile(MobileEdit->text());
	CurrentContact.setEmail(EmailEdit->text());
	CurrentContact.setWebsite(WebsiteEdit->text());

	for (int i = 0; i < ContactsAccounts.count(); i++)
	{
		if (ContactsAccounts.at(i)->itemData(ContactsAccounts.at(i)->currentIndex()).toString().isEmpty())
			break;
		Account *account = AccountManager::instance()->byUuid(QUuid(ContactsAccounts.at(i)->itemData(ContactsAccounts.at(i)->currentIndex()).toString()));
		if (CurrentContact.hasStoredAccountData(account))
		{
			if (!ContactsIds.at(i)->text().isEmpty()/* && account->protocol()->validateId(ContactsIds.at(i)->text())*/)
				CurrentContact.accountData(account)->setId(ContactsIds.at(i)->text());
			else
				CurrentContact.removeAccountData(account);
		}
		else
		{
			ContactAccountData *data = new ContactAccountData(CurrentContact, account, ContactsIds.at(i)->text(), false);
			CurrentContact.addAccountData(data);
		}
	}
}
