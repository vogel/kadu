/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/filter/id-validity-filter.h"
#include "accounts/filter/writeable-contacts-list-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/model/groups-model.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/accounts-combo-box.h"
#include "gui/widgets/groups-combo-box.h"
#include "gui/widgets/select-buddy-combo-box.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/services/roster-service.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "icons-manager.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent) :
		QDialog(parent, Qt::Window), MyBuddy(Buddy::null)
{
	setWindowRole("kadu-add-buddy");

	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	addMobileAccountToComboBox();

	connect(AccountCombo, SIGNAL(accountChanged(Account, Account)), this, SLOT(accountChanged(Account, Account)));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGui()));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAddContactEnabled()));

	setAddContactEnabled();
}

AddBuddyWindow::~AddBuddyWindow()
{
	saveWindowGeometry(this, "General", "AddBuddyWindowGeometry");
}

void AddBuddyWindow::createGui()
{
	loadWindowGeometry(this, "General", "AddBuddyWindowGeometry", 0, 50, 425, 430);

	QGridLayout *layout = new QGridLayout(this);

	UserNameLabel = new QLabel(tr("User ID:"), this);
	UserNameLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(UserNameLabel, 0, 0, Qt::AlignRight);
	UserNameEdit = new QLineEdit(this);
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAccountFilter()));

	layout->addWidget(UserNameEdit, 0, 1);
	layout->addWidget(new QLabel(tr("in"), this), 0, 2);

	AccountCombo = new AccountsComboBox(this);

	AccountComboIdFilter = new IdValidityFilter(AccountCombo);
	AccountCombo->addFilter(AccountComboIdFilter);

	WriteableContactsListFilter *writeableCotnactsListFilter = new WriteableContactsListFilter(AccountCombo);
	AccountCombo->addFilter(writeableCotnactsListFilter);

	AccountCombo->setModelColumn(1); // use long account name
	layout->addWidget(AccountCombo, 0, 3);

	layout->addWidget(new QLabel(tr("Add in group:"), this), 1, 0, Qt::AlignRight);
	GroupCombo = new GroupsComboBox(this);
	layout->addWidget(GroupCombo, 1, 1, 1, 3);

	layout->addWidget(new QLabel(tr("Visible name:"), this), 2, 0, Qt::AlignRight);
	DisplayNameEdit = new QLineEdit(this);
	connect(DisplayNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	layout->addWidget(DisplayNameEdit, 2, 1, 1, 1);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this contact"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 2);
	hintLabel->setFont(hintLabelFont);
	layout->addWidget(hintLabel, 3, 1, 1, 3);

	MergeContact = new QCheckBox(tr("Merge with an existing contact"), this);
	layout->addWidget(MergeContact, 4, 1, 1, 3);

	QWidget *selectContactWidget = new QWidget(this);
	QHBoxLayout *selectContactLayout = new QHBoxLayout(selectContactWidget);
	selectContactLayout->addSpacing(20);
	SelectContact = new SelectBuddyComboBox(selectContactWidget);
	SelectContact->setEnabled(false);
	selectContactLayout->addWidget(SelectContact);
	layout->addWidget(selectContactWidget, 5, 1, 1, 3);

	connect(MergeContact, SIGNAL(toggled(bool)), SelectContact, SLOT(setEnabled(bool)));
	connect(MergeContact, SIGNAL(toggled(bool)), DisplayNameEdit, SLOT(setDisabled(bool)));
	connect(MergeContact, SIGNAL(toggled(bool)), this, SLOT(setAddContactEnabled()));
	connect(SelectContact, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAddContactEnabled()));
	connect(SelectContact, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAccountFilter()));

	AskForAuthorization = new QCheckBox(tr("Ask contact for authorization"), this);
	AskForAuthorization->setEnabled(false);
	layout->addWidget(AskForAuthorization, 7, 1, 1, 3);

	// TODO 0.6.6: it is not used anywhere
	AllowToSeeMeCheck = new QCheckBox(tr("Allow contact to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	layout->addWidget(AllowToSeeMeCheck, 8, 1, 1, 3);

	layout->setRowMinimumHeight(6, 20);
	layout->setRowMinimumHeight(9, 20);
	layout->setRowStretch(9, 100);

	ErrorLabel = new QLabel();
	QFont labelFont = ErrorLabel->font();
	labelFont.setBold(true);
	ErrorLabel->setFont(labelFont);
	layout->addWidget(ErrorLabel, 10, 1, 1, 4);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons, 11, 0, 1, 4);

	AddContactButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Add contact"), this);
	AddContactButton->setDefault(true);
	connect(AddContactButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddContactButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

	layout->setColumnMinimumWidth(0, 150);
	layout->setColumnMinimumWidth(1, 200);
	setFixedHeight(layout->minimumSize().height());
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void AddBuddyWindow::addMobileAccountToComboBox()
{
	ActionsProxyModel *actionsModel = AccountCombo->actionsModel();

	MobileAccountAction = new QAction(IconsManager::instance()->iconByPath("phone"), tr("Mobile"), AccountCombo);
	actionsModel->addAfterAction(MobileAccountAction);
}

void AddBuddyWindow::displayErrorMessage(const QString &message)
{
	ErrorLabel->setText(message);
}

void AddBuddyWindow::setBuddy(Buddy buddy)
{
	MyBuddy = buddy;

	Account account = BuddyPreferredManager::instance()->preferredAccount(buddy);
	if (!account.isNull())
	{
		AccountCombo->setCurrentAccount(account);
		UserNameEdit->setText(buddy.id(account));
	}

	DisplayNameEdit->setText(buddy.display());
}

void AddBuddyWindow::setGroup(Group group)
{
	GroupCombo->setCurrentGroup(group);
}

bool AddBuddyWindow::isMobileAccount()
{
	return AccountCombo->data(ActionRole).value<QAction *>() == MobileAccountAction;
}

void AddBuddyWindow::accountChanged(Account account, Account lastAccount)
{
	if (lastAccount && lastAccount.protocolHandler())
	{
		disconnect(lastAccount.protocolHandler(), SIGNAL(connected(Account)), this, SLOT(setAddContactEnabled()));
		disconnect(lastAccount.protocolHandler(), SIGNAL(disconnected(Account)), this, SLOT(setAddContactEnabled()));
	}

	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
	{
		AskForAuthorization->setEnabled(false);
		AskForAuthorization->setChecked(false);
	}
	else
	{
		connect(account.protocolHandler(), SIGNAL(connected(Account)), this, SLOT(setAddContactEnabled()));
		connect(account.protocolHandler(), SIGNAL(disconnected(Account)), this, SLOT(setAddContactEnabled()));

		AskForAuthorization->setEnabled(true);
		AskForAuthorization->setChecked(true);
	}
}

void AddBuddyWindow::updateAccountGui()
{
	Account account = AccountCombo->currentAccount();
	if (account.isNull())
		UserNameLabel->setText(tr("Username:"));
	else
		UserNameLabel->setText(account.protocolHandler()->protocolFactory()->idLabel());

	MergeContact->setEnabled(true);
	AllowToSeeMeCheck->setEnabled(true);
}

void AddBuddyWindow::updateMobileGui()
{
	UserNameLabel->setText(tr("Mobile number:"));
	MergeContact->setChecked(false);
	MergeContact->setEnabled(false);
	SelectContact->setCurrentBuddy(Buddy::null);
	AllowToSeeMeCheck->setEnabled(false);
}

void AddBuddyWindow::updateGui()
{
	if (isMobileAccount())
		updateMobileGui();
	else
		updateAccountGui();
}

void AddBuddyWindow::validateData()
{
	AddContactButton->setEnabled(false);

	Account account = AccountCombo->currentAccount();
	if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->protocolFactory())
	{
		displayErrorMessage(tr("Account is not selected"));
		return;
	}

	if (account.protocolHandler()->rosterService() && !account.protocolHandler()->isConnected())
	{
		displayErrorMessage(tr("You must be connected to add contacts to this account"));
		return;
	}

	if (account.protocolHandler()->protocolFactory()->validateId(UserNameEdit->text()) != QValidator::Acceptable)
	{
		displayErrorMessage(tr("Entered username is invalid"));
		return;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), ActionReturnNull);
	if (contact && contact.ownerBuddy() && !contact.ownerBuddy().isAnonymous())
	{
		displayErrorMessage(tr("This contact is already available as <i>%1</i>").arg(contact.ownerBuddy().display()));
		return;
	}

	if (MergeContact->isChecked())
	{
		if (!SelectContact->currentBuddy())
		{
			displayErrorMessage(tr("Select contact to merge with"));
			return;
		}
	}
	else
	{
		if (BuddyManager::instance()->byDisplay(DisplayNameEdit->text(), ActionReturnNull))
		{
			displayErrorMessage(tr("Visible name is already used for another contact"));
			return;
		}
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::validateMobileData()
{
	static QRegExp mobileRegularExpression("[0-9]{3,12}");

	if (!mobileRegularExpression.exactMatch(UserNameEdit->text()))
	{
		displayErrorMessage(tr("Entered mobile number is invalid"));
		return;
	}

	if (MergeContact->isChecked())
	{
		displayErrorMessage(tr("Merging mobile number with buddy is not supported. Please use edit buddy window."));
		return;
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::setAddContactEnabled()
{
	if (isMobileAccount())
		validateMobileData();
	else
		validateData();
}

void AddBuddyWindow::setAccountFilter()
{
	AccountComboIdFilter->setId(UserNameEdit->text());
}

bool AddBuddyWindow::addContact()
{
	Account account = AccountCombo->currentAccount();
	if (account.isNull())
		return false;

	Buddy buddy;

	if (!MergeContact->isChecked())
	{
		if (MyBuddy.isNull())
		{
			buddy = Buddy::create();
			buddy.data()->setState(StorableObject::StateNew);
		}
		else
			buddy = MyBuddy;

		BuddyManager::instance()->addItem(buddy);

		buddy.setAnonymous(false);
		buddy.setOfflineTo(!AllowToSeeMeCheck->isChecked());

		QString display = DisplayNameEdit->text().isEmpty() ? UserNameEdit->text() : DisplayNameEdit->text();
		buddy.setDisplay(display);
	}
	else
	{
		buddy = SelectContact->currentBuddy();
		if (buddy.isNull())
			return false;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), ActionCreateAndAdd);
	// force reattach for gadu protocol, even if buddy == contact.ownerBuddy()
	contact.setOwnerBuddy(Buddy::null);
	contact.setOwnerBuddy(buddy);

	buddy.addToGroup(GroupCombo->currentGroup());

	if (AskForAuthorization->isChecked())
		askForAuthorization(contact);

	return true;
}

bool AddBuddyWindow::addMobile()
{
	Buddy buddy = Buddy::create();
	buddy.data()->setState(StorableObject::StateNew);
	buddy.setAnonymous(false);
	buddy.setMobile(UserNameEdit->text());
	buddy.setDisplay(DisplayNameEdit->text().isEmpty() ? UserNameEdit->text() : DisplayNameEdit->text());
	buddy.addToGroup(GroupCombo->currentGroup());

	BuddyManager::instance()->addItem(buddy);

	return true;
}

void AddBuddyWindow::accept()
{
	bool ok;

	if (isMobileAccount())
		ok = addMobile();
	else
		ok = addContact();

	if (ok)
		QDialog::accept();
}

void AddBuddyWindow::reject()
{
	QDialog::reject();
}

void AddBuddyWindow::askForAuthorization(Contact contact)
{
	Account account = AccountCombo->currentAccount();

	if (!account || !account.protocolHandler() || !account.protocolHandler()->rosterService())
		return;

	account.protocolHandler()->rosterService()->askForAuthorization(contact);
}
