/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/filter/id-regular-expression-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "buddies/buddy.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/model/groups-model.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/accounts-combo-box.h"
#include "gui/widgets/select-buddy-combobox.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "icons-manager.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent) :
		QDialog(parent, Qt::Window), MyBuddy(Buddy::null)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	setAddContactEnabled();
	setValidateRegularExpression();
}

AddBuddyWindow::~AddBuddyWindow()
{
	saveWindowGeometry(this, "General", "AddBuddyWindowGeometry");
}

void AddBuddyWindow::createGui()
{
	loadWindowGeometry(this, "General", "AddBuddyWindowGeometry", 0, 50, 425, 430);

	QGridLayout *layout = new QGridLayout(this);

	UserNameLabel = new QLabel(tr("Username:"), this);
	UserNameLabel->setAlignment(Qt::AlignRight);
	layout->addWidget(UserNameLabel, 0, 0, Qt::AlignRight);
	UserNameEdit = new QLineEdit(this);
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAccountFilter()));

	UserNameValidator = new QRegExpValidator(UserNameEdit);
	UserNameEdit->setValidator(UserNameValidator);
	layout->addWidget(UserNameEdit, 0, 1);
	layout->addWidget(new QLabel(tr("in"), this), 0, 2);

	AccountCombo = new AccountsComboBox(true, this);
	connect(AccountCombo, SIGNAL(activated(int)), this, SLOT(setUsernameLabel()));
	connect(AccountCombo, SIGNAL(activated(int)), this, SLOT(setAddContactEnabled()));
	connect(AccountCombo, SIGNAL(activated(int)), this, SLOT(setValidateRegularExpression()));

	AccountComboIdFilter = new IdRegularExpressionFilter(AccountCombo);
	AccountCombo->addFilter(AccountComboIdFilter);
	AccountCombo->setModelColumn(1); // use long account name
	layout->addWidget(AccountCombo, 0, 3);

	layout->addWidget(new QLabel(tr("Add in group:"), this), 1, 0, Qt::AlignRight);
	GroupCombo = new QComboBox(this);
	GroupCombo->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	connect(GroupCombo, SIGNAL(activated(int)), this, SLOT(groupChanged(int)));
	GroupsModel *groupComboModel = new GroupsModel(GroupCombo);

	QSortFilterProxyModel *sortModel = new QSortFilterProxyModel(GroupCombo);
	sortModel->setSourceModel(groupComboModel);
	sortModel->setDynamicSortFilter(true);
	sortModel->sort(1);
	sortModel->sort(0);

	ActionsProxyModel::ModelActionList groupsModelBeforeActions;
	groupsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select group - "), ""));
	ActionsProxyModel::ModelActionList groupsModelAfterActions;
	groupsModelAfterActions.append(qMakePair<QString, QString>(tr("Create a new group..."), "createNewGroup"));

	ActionsProxyModel *groupsProxyModel = new ActionsProxyModel(groupsModelBeforeActions,
			groupsModelAfterActions, GroupCombo);
	groupsProxyModel->setSourceModel(sortModel);

	GroupCombo->setModel(groupsProxyModel);
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
	SelectContact = new SelectBuddyCombobox(selectContactWidget);
	SelectContact->setEnabled(false);
	selectContactLayout->addWidget(SelectContact);
	layout->addWidget(selectContactWidget, 5, 1, 1, 3);

	connect(MergeContact, SIGNAL(toggled(bool)), SelectContact, SLOT(setEnabled(bool)));
	connect(MergeContact, SIGNAL(toggled(bool)), DisplayNameEdit, SLOT(setDisabled(bool)));
	connect(MergeContact, SIGNAL(toggled(bool)), this, SLOT(setAddContactEnabled()));
	connect(SelectContact, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAddContactEnabled()));
	connect(SelectContact, SIGNAL(buddyChanged(Buddy)), this, SLOT(setAccountFilter()));

	// TODO 0.6.6: it is not used anywhere
	AllowToSeeMeCheck = new QCheckBox(tr("Allow contact to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	layout->addWidget(AllowToSeeMeCheck, 7, 1, 1, 3);

	layout->setRowMinimumHeight(6, 20);
	layout->setRowMinimumHeight(8, 20);
	layout->setRowStretch(8, 100);
	
	ErrorLabel = new QLabel();
	QFont labelFont = ErrorLabel->font();
	labelFont.setBold(true);
	ErrorLabel->setFont(labelFont);
	layout->addWidget(ErrorLabel, 9, 1, 1, 4);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons, 10, 0, 1, 4);

	AddContactButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Add contact"), this);
	AddContactButton->setDefault(true);
	connect(AddContactButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddContactButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

	layout->setColumnMinimumWidth(0, 150);
	layout->setColumnMinimumWidth(1, 200);
// 	TODO: NOW, does not work
// 	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
// 	setMaximumHeight(layout->minimumSize().height());
// 	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

void AddBuddyWindow::displayErrorMessage(const QString &message)
{
	ErrorLabel->setText(message);
}

Account AddBuddyWindow::selectedAccount()
{
	QModelIndex index = AccountCombo->model()->index(AccountCombo->currentIndex(), 0);
	return index.data(AccountRole).value<Account>();
}

void AddBuddyWindow::setBuddy(Buddy buddy)
{
	MyBuddy = buddy;

	Account account = buddy.prefferedAccount();
	if (!account.isNull())
	{
		AccountCombo->setCurrentAccount(account);
		UserNameEdit->setText(buddy.id(account));
	}

	DisplayNameEdit->setText(buddy.display());
}

void AddBuddyWindow::setUsernameLabel()
{
	Account account = selectedAccount();
	if (account.isNull())
		UserNameLabel->setText(tr("Username:"));
	else
		UserNameLabel->setText(account.protocolHandler()->protocolFactory()->idLabel());
}

void AddBuddyWindow::setAddContactEnabled()
{
	Account account = selectedAccount();
	if (account.isNull() || !account.protocolHandler() || !account.protocolHandler()->protocolFactory())
	{
		AddContactButton->setEnabled(false);
		displayErrorMessage(tr("Account is not selected"));
		return;
	}

	if (!account.protocolHandler()->protocolFactory()->idRegularExpression().exactMatch(UserNameEdit->text()))
	{
		AddContactButton->setEnabled(false);
		displayErrorMessage(tr("Entered username is invalid"));
		return;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), false);
	if (contact && contact.ownerBuddy() && !contact.ownerBuddy().isAnonymous())
	{
		AddContactButton->setEnabled(false);
		displayErrorMessage(tr("This contact is already available as <i>%1</i>").arg(contact.ownerBuddy().display()));
		return;
	}

	if (MergeContact->isChecked())
	{
		if (!SelectContact->buddy())
		{
			AddContactButton->setEnabled(false);
			displayErrorMessage(tr("Select contact to merge with"));
			return;
		}
	}
	else
	{
		if (DisplayNameEdit->text().isEmpty())
		{
			AddContactButton->setEnabled(false);
			displayErrorMessage(tr("Visible name can not be empty"));
			return;
		}

		if (BuddyManager::instance()->byDisplay(DisplayNameEdit->text()))
		{
			AddContactButton->setEnabled(false);
			displayErrorMessage(tr("Visible name is already used for another contact"));
			return;
		}
	}

	AddContactButton->setEnabled(true);
	displayErrorMessage(QString::null);
}

void AddBuddyWindow::setValidateRegularExpression()
{
	Account account = selectedAccount();
	if (!account.isNull() && account.protocolHandler())
	{
		UserNameValidator->setRegExp(account.protocolHandler()->protocolFactory()->idRegularExpression());
		return;
	}

	QStringList regularExpressions;

	foreach (Account account, AccountManager::instance()->items())
	{
		QRegExp regularExpression = account.protocolHandler()->protocolFactory()->idRegularExpression();
		if (!regularExpression.isEmpty())
			regularExpressions.append(regularExpression.pattern());
	}

	regularExpressions.removeDuplicates();
	UserNameValidator->setRegExp(QRegExp(QString("(%1)").arg(regularExpressions.join("|"))));
}

void AddBuddyWindow::setAccountFilter()
{
	AccountComboIdFilter->setId(UserNameEdit->text());
}

void AddBuddyWindow::groupChanged(int index)
{
	QModelIndex modelIndex = GroupCombo->model()->index(index, 0, QModelIndex());
	QString action = modelIndex.data(ActionRole).toString();

	if (action.isEmpty())
		return;
	bool ok;

	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
			tr("Please enter the name for the new group:"), QLineEdit::Normal,
			QString::null, &ok);

	if (!ok || newGroupName.isEmpty() || !GroupManager::instance()->acceptableGroupName(newGroupName))
	{
		GroupCombo->setCurrentIndex(0);
		return;
	}

	GroupManager::instance()->byName(newGroupName);
	GroupCombo->setCurrentIndex(GroupCombo->findText(newGroupName));
}

void AddBuddyWindow::accept()
{
	Account account = selectedAccount();
	if (account.isNull())
		return;

	Buddy buddy;

	if (!MergeContact->isChecked())
	{
		if (MyBuddy.isNull())
			buddy = BuddyManager::instance()->byId(account, UserNameEdit->text(), true);
		else
			buddy = MyBuddy;

		buddy.setAnonymous(false);
		buddy.setDisplay(DisplayNameEdit->text());
	}
	else
	{
		buddy = SelectContact->buddy();
		if (buddy.isNull())
			return;
	}

	Contact contact = ContactManager::instance()->byId(account, UserNameEdit->text(), true);
	contact.setOwnerBuddy(buddy);

	QDialog::accept();
}

void AddBuddyWindow::reject()
{
	QDialog::reject();
}
