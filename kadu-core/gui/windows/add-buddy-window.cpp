/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "accounts/filter/protocol-filter.h"
#include "accounts/filter/writeable-contacts-list-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-preferred-manager.h"
#include "buddies/buddy.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "buddies/model/groups-model.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "core/core.h"
#include "gui/widgets/accounts-combo-box.h"
#include "gui/widgets/groups-combo-box.h"
#include "gui/widgets/select-talkable-combo-box.h"
#include "icons/kadu-icon.h"
#include "identities/identity.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"
#include "protocols/roster.h"
#include "protocols/services/roster-service.h"
#include "talkable/filter/exclude-buddy-talkable-filter.h"
#include "protocols/services/subscription-service.h"
#include "url-handlers/url-handler-manager.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent, const Buddy &buddy, bool forceBuddyAccount) :
		QDialog(parent, Qt::Window), DesktopAwareObject(this), UserNameLabel(0), UserNameEdit(0),
		MobileAccountAction(0), EmailAccountAction(0), AccountCombo(0),
		GroupCombo(0), DisplayNameEdit(0), MergeBuddy(0), SelectBuddy(0), AskForAuthorization(0),
		AllowToSeeMeCheck(0), ErrorLabel(0), AddContactButton(0), MyBuddy(buddy),
		ForceBuddyAccount(forceBuddyAccount)
{
	setWindowRole("kadu-add-buddy");
	setWindowTitle(tr("Add buddy"));
	setAttribute(Qt::WA_DeleteOnClose);

	if (MyBuddy)
	{
		MyAccount = BuddyPreferredManager::instance()->preferredAccount(MyBuddy);
		if (!MyAccount)
			MyBuddy = Buddy::null;
	}

	createGui();
	if (!MyBuddy)
		addFakeAccountsToComboBox();
}

AddBuddyWindow::~AddBuddyWindow()
{
	saveWindowGeometry(this, "General", "AddBuddyWindowGeometry");
}

void AddBuddyWindow::createGui()
{
	loadWindowGeometry(this, "General", "AddBuddyWindowGeometry", 0, 50, 425, 430);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *mainWidget = new QWidget(this);
	mainLayout->addWidget(mainWidget);
	mainLayout->addStretch(100);

	Layout = new QFormLayout(mainWidget);

	AccountCombo = new AccountsComboBox(MyBuddy.isNull(), AccountsComboBox::NotVisibleWithOneRowSourceModel, this);
	AccountCombo->setIncludeIdInDisplay(true);
	AccountCombo->addFilter(new WriteableContactsListFilter(AccountCombo));

	if (MyBuddy)
	{
		AccountCombo->setCurrentAccount(MyAccount);

		ProtocolFilter *protocolFilter = new ProtocolFilter(AccountCombo);
		protocolFilter->setProtocolName(MyAccount.protocolName());
		AccountCombo->addFilter(protocolFilter);
	}

	if (MyBuddy && ForceBuddyAccount)
	{
		// NOTE: keep "%1 (%2)" consistent with AccountsModel::data() for DisplayRole, when IncludeIdInDisplay is true
		// TODO 0.10: remove such code duplication
		QString accountLabel = QString("<b>%1 (%2)</b>").arg(MyAccount.accountIdentity().name()).arg(MyAccount.id());
		Layout->addRow(tr("Account:"), new QLabel(accountLabel));
		AccountCombo->hide();
	}
	else
		Layout->addRow(tr("Account:"), AccountCombo);

	UserNameEdit = new QLineEdit(this);
	UserNameLabel = new QLabel(this);

	if (MyBuddy)
	{
		UserNameEdit->setText(MyBuddy.id(MyAccount));
		UserNameEdit->hide();

		Layout->addRow(UserNameLabel, new QLabel(QString("<b>%1</b>").arg(MyBuddy.id(MyAccount)), this));
	}
	else
		Layout->addRow(UserNameLabel, UserNameEdit);

	MergeBuddy = new QCheckBox(tr("Merge with existing buddy"), this);
	Layout->addRow(0, MergeBuddy);

	DisplayNameEdit = new QLineEdit(this);
	Layout->addRow(tr("Visible name:"), DisplayNameEdit);

	if (MyBuddy)
		DisplayNameEdit->setText(MyBuddy.display());

	connect(DisplayNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));

	NonMergeWidgets.append(DisplayNameEdit);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this buddy"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 2);
	hintLabel->setFont(hintLabelFont);
	Layout->addRow(0, hintLabel);

	NonMergeWidgets.append(hintLabel);

	GroupCombo = new GroupsComboBox(this);
	Layout->addRow(tr("Add in group:"), GroupCombo);

	NonMergeWidgets.append(GroupCombo);

	SelectBuddy = new SelectTalkableComboBox(this);
	SelectBuddy->addBeforeAction(new QAction(tr(" - Select buddy - "), SelectBuddy));

	BuddyListModel *buddyListModel = new BuddyListModel(SelectBuddy);
	new BuddyManagerAdapter(buddyListModel);
	SelectBuddy->setBaseModel(buddyListModel);
	SelectBuddy->setEnabled(false);
	SelectBuddy->setVisible(false);
	SelectBuddy->addFilter(new ExcludeBuddyTalkableFilter(Core::instance()->myself(), SelectBuddy));
	Layout->addRow(tr("Merge with:"), SelectBuddy);

	MergeWidgets.append(SelectBuddy);

	AskForAuthorization = new QCheckBox(tr("Ask contact for authorization"), this);
	Layout->addRow(0, AskForAuthorization);

	AllowToSeeMeCheck = new QCheckBox(tr("Allow buddy to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	Layout->addRow(0, AllowToSeeMeCheck);

	NonMergeWidgets.append(AllowToSeeMeCheck);

	ErrorLabel = new QLabel(this);
	QFont labelFont = ErrorLabel->font();
	labelFont.setBold(true);
	ErrorLabel->setFont(labelFont);
	Layout->addRow(0, ErrorLabel);

	QDialogButtonBox *buttons = new QDialogButtonBox(mainWidget);
	mainLayout->addWidget(buttons);

	AddContactButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Add buddy"), this);
	AddContactButton->setDefault(true);
	connect(AddContactButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddContactButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountChanged()));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGui()));
	connect(AccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAddContactEnabled()));
	connect(MergeBuddy, SIGNAL(toggled(bool)), this, SLOT(mergeToggled(bool)));
	connect(MergeBuddy, SIGNAL(toggled(bool)), this, SLOT(setAddContactEnabled()));
	connect(SelectBuddy, SIGNAL(currentIndexChanged(int)), this, SLOT(setAddContactEnabled()));

	mergeToggled(false);

	setAddContactEnabled();
	accountChanged();
	updateGui();

	setFixedHeight(height());

	if (MyBuddy)
		DisplayNameEdit->setFocus();
	else if (AccountCombo->currentAccount())
		UserNameEdit->setFocus();
	else
		AccountCombo->setFocus();
}

void AddBuddyWindow::addFakeAccountsToComboBox()
{
	MobileAccountAction = new QAction(KaduIcon("phone").icon(), tr("Mobile"), AccountCombo);
	AccountCombo->addAfterAction(MobileAccountAction);

	EmailAccountAction = new QAction(KaduIcon("mail-message-new").icon(), tr("E-mail"), AccountCombo);
	AccountCombo->addAfterAction(EmailAccountAction);
}

void AddBuddyWindow::displayErrorMessage(const QString &message)
{
	ErrorLabel->setText(message);
}

void AddBuddyWindow::setGroup(Group group)
{
	GroupCombo->setCurrentGroup(group);
}

bool AddBuddyWindow::isMobileAccount()
{
	return (MobileAccountAction && AccountCombo->currentAction() == MobileAccountAction);
}

bool AddBuddyWindow::isEmailAccount()
{
	return (EmailAccountAction && AccountCombo->currentAction() == EmailAccountAction);
}

void AddBuddyWindow::accountChanged()
{
	Account account = AccountCombo->currentAccount();

	if (LastSelectedAccount && LastSelectedAccount.protocolHandler())
		disconnect(LastSelectedAccount.protocolHandler(), 0, this, 0);

	if (!account || !account.protocolHandler() || !account.protocolHandler()->subscriptionService())
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

	LastSelectedAccount = account;
}

void AddBuddyWindow::updateAccountGui()
{
	if (UserNameLabel)
	{
		Account account = AccountCombo->currentAccount();
		if (account.isNull())
			UserNameLabel->setText(tr("User ID:"));
		else
			UserNameLabel->setText(account.protocolHandler()->protocolFactory()->idLabel());
	}

	MergeBuddy->setEnabled(true);
	AllowToSeeMeCheck->setEnabled(true);
}

void AddBuddyWindow::updateMobileGui()
{
	UserNameLabel->setText(tr("Mobile number:"));
	MergeBuddy->setChecked(false);
	MergeBuddy->setEnabled(false);
	AllowToSeeMeCheck->setEnabled(false);
}

void AddBuddyWindow::updateEmailGui()
{
	UserNameLabel->setText(tr("E-mail address:"));
	MergeBuddy->setChecked(false);
	MergeBuddy->setEnabled(false);
	AllowToSeeMeCheck->setEnabled(false);
}

void AddBuddyWindow::updateGui()
{
	if (isMobileAccount())
		updateMobileGui();
	else if (isEmailAccount())
		updateEmailGui();
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
		if (!UserNameEdit->text().isEmpty())
			displayErrorMessage(tr("Entered user identification is invalid"));
		else
			displayErrorMessage(tr("No user identification entered"));
		return;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), ActionReturnNull);
	if (!contact.isAnonymous())
	{
		displayErrorMessage(tr("This contact is already available as <i>%1</i>").arg(contact.display(true)));
		return;
	}

	if (MergeBuddy->isChecked())
	{
		if (!SelectBuddy->currentTalkable().isValidBuddy())
		{
			displayErrorMessage(tr("Select buddy to merge with"));
			return;
		}
	}
	else
	{
		if (DisplayNameEdit->text().isEmpty())
		{
			displayErrorMessage(tr("Enter visible name"));
			return;
		}
		else
		{
			Buddy existingBuddy = BuddyManager::instance()->byDisplay(DisplayNameEdit->text(), ActionReturnNull);
			if (existingBuddy && existingBuddy != MyBuddy)
			{
				displayErrorMessage(tr("Visible name is already used for another buddy"));
				return;
			}
		}
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::validateMobileData()
{
	Q_ASSERT(!MergeBuddy->isChecked());

	static QRegExp mobileRegularExpression("[0-9]{3,12}");

	if (!mobileRegularExpression.exactMatch(UserNameEdit->text()))
	{
		if (!UserNameEdit->text().isEmpty())
			displayErrorMessage(tr("Entered mobile number is invalid"));
		else
			displayErrorMessage(tr("No mobile number entered"));
		return;
	}

	if (DisplayNameEdit->text().isEmpty())
	{
		displayErrorMessage(tr("Enter visible name"));
		return;
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::validateEmailData()
{
	Q_ASSERT(!MergeBuddy->isChecked());

	if (!UrlHandlerManager::instance()->mailRegExp().exactMatch(UserNameEdit->text()))
	{
		if (!UserNameEdit->text().isEmpty())
			displayErrorMessage(tr("Entered e-mail is invalid"));
		else
			displayErrorMessage(tr("No e-mail entered"));
		return;
	}

	if (DisplayNameEdit->text().isEmpty())
	{
		displayErrorMessage(tr("Enter visible name"));
		return;
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString());
}

void AddBuddyWindow::setAddContactEnabled()
{
	if (isMobileAccount())
		validateMobileData();
	else if (isEmailAccount())
		validateEmailData();
	else
		validateData();
}

void AddBuddyWindow::mergeToggled(bool toggled)
{
	setUpdatesEnabled(false);

	foreach (QWidget *widget, NonMergeWidgets)
	{
		widget->setVisible(!toggled);
		widget->setEnabled(!toggled);

		QWidget *label = Layout->labelForField(widget);
		if (label)
			label->setVisible(!toggled);
	}

	foreach (QWidget *widget, MergeWidgets)
	{
		widget->setVisible(toggled);
		widget->setEnabled(toggled);

		QWidget *label = Layout->labelForField(widget);
		if (label)
			label->setVisible(toggled);
	}

	if (toggled)
		AddContactButton->setText(tr("Merge with buddy"));
	else
		AddContactButton->setText(tr("Add buddy"));

	setUpdatesEnabled(true);
}

bool AddBuddyWindow::addContact()
{
	Account account = AccountCombo->currentAccount();
	if (account.isNull())
		return false;

	Buddy buddy;

	if (!MergeBuddy->isChecked())
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
		buddy.addToGroup(GroupCombo->currentGroup());
	}
	else
	{
		buddy = SelectBuddy->currentTalkable().toBuddy();
		if (buddy.isNull())
			return false;
	}

	NotFoundAction action = MyBuddy.isNull()
			? ActionCreateAndAdd
			: ActionReturnNull;

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), action);

	if (contact)
	{
		contact.setOwnerBuddy(buddy);

		Roster::instance()->addContact(contact);

		if (!buddy.isOfflineTo())
			sendAuthorization(contact);

		if (AskForAuthorization->isChecked())
			askForAuthorization(contact);
	}

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

bool AddBuddyWindow::addEmail()
{
	Buddy buddy = Buddy::create();
	buddy.data()->setState(StorableObject::StateNew);
	buddy.setAnonymous(false);
	buddy.setEmail(UserNameEdit->text());
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
	else if (isEmailAccount())
		ok = addEmail();
	else
		ok = addContact();

	if (ok)
		QDialog::accept();
}

void AddBuddyWindow::askForAuthorization(const Contact &contact)
{
	Account account = AccountCombo->currentAccount();

	if (!account || !account.protocolHandler() || !account.protocolHandler()->subscriptionService())
		return;

	account.protocolHandler()->subscriptionService()->requestSubscription(contact);
}

void AddBuddyWindow::sendAuthorization(const Contact &contact)
{
	Account account = AccountCombo->currentAccount();

	if (!account || !account.protocolHandler() || !account.protocolHandler()->subscriptionService())
		return;

	account.protocolHandler()->subscriptionService()->resendSubscription(contact);
}
