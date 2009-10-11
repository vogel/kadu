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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "accounts/filter/id-regular-expression-filter.h"
#include "accounts/model/accounts-model.h"
#include "accounts/model/accounts-proxy-model.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/group-manager.h"
#include "contacts/model/groups-model.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent) :
		QDialog(parent, Qt::Window), MyContact(ContactData::TypeNull)
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

	layout->addWidget(new QLabel(tr("Username:"), this), 0, 0, Qt::AlignRight);
	UserNameEdit = new QLineEdit(this);
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	connect(UserNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAccountFilter()));

	UserNameValidator = new QRegExpValidator(UserNameEdit);
	UserNameEdit->setValidator(UserNameValidator);
	layout->addWidget(UserNameEdit, 0, 1);
	layout->addWidget(new QLabel(tr("in"), this), 0, 2);

	AccountCombo = new QComboBox(this);
	connect(AccountCombo, SIGNAL(activated(int)), this, SLOT(setAddContactEnabled()));
	connect(AccountCombo, SIGNAL(activated(int)), this, SLOT(setValidateRegularExpression()));

	AccountComboModel = new AccountsModel(AccountCombo);
	AccountComboProxyModel = new AccountsProxyModel(AccountCombo);
	AccountComboProxyModel->setSourceModel(AccountComboModel);
	AccountComboFilter = new IdRegularExpressionFilter(AccountCombo);
	AccountComboProxyModel->addFilter(AccountComboFilter);

	ActionsProxyModel::ModelActionList accountsModelBeforeActions;
	accountsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select account - "), ""));
	ActionsProxyModel *accountsProxyModel = new ActionsProxyModel(accountsModelBeforeActions,
			ActionsProxyModel::ModelActionList(), AccountCombo);
	accountsProxyModel->setSourceModel(AccountComboProxyModel);

	AccountCombo->setModel(accountsProxyModel);
	AccountCombo->setModelColumn(1); // use long account name
	layout->addWidget(AccountCombo, 0, 3);

	layout->addWidget(new QLabel(tr("Add in group:"), this), 1, 0, Qt::AlignRight);
	GroupCombo = new QComboBox(this);
	connect(GroupCombo, SIGNAL(activated(int)), this, SLOT(groupChanged(int)));
	GroupsModel *groupComboModel = new GroupsModel(GroupCombo);

	ActionsProxyModel::ModelActionList groupsModelBeforeActions;
	groupsModelBeforeActions.append(qMakePair<QString, QString>(tr(" - Select group - "), ""));
	ActionsProxyModel::ModelActionList groupsModelAfterActions;
	groupsModelAfterActions.append(qMakePair<QString, QString>(tr("Create a new group..."), "createNewGroup"));

	ActionsProxyModel *groupsProxyModel = new ActionsProxyModel(groupsModelBeforeActions,
			groupsModelAfterActions, GroupCombo);
	groupsProxyModel->setSourceModel(groupComboModel);

	GroupCombo->setModel(groupsProxyModel);
	layout->addWidget(GroupCombo, 1, 1, 1, 3);

	layout->addWidget(new QLabel(tr("Visible name:"), this), 2, 0, Qt::AlignRight);
	DisplayNameEdit = new QLineEdit(this);
	connect(DisplayNameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(setAddContactEnabled()));
	layout->addWidget(DisplayNameEdit, 2, 1, 1, 3);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this contact"));
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 1);
	hintLabel->setFont(hintLabelFont);
	layout->addWidget(hintLabel, 3, 1, 1, 3);

	AllowToSeeMeCheck = new QCheckBox(tr("Allow contact to see me when I'm available"), this);
	AllowToSeeMeCheck->setChecked(true);
	layout->addWidget(AllowToSeeMeCheck, 5, 1, 1, 3);

	layout->setRowMinimumHeight(4, 20);
	layout->setRowMinimumHeight(6, 20);
	layout->setRowStretch(6, 100);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons, 7, 0, 1, 4);

	AddContactButton = new QPushButton(tr("Add contact"), this);
	AddContactButton->setDefault(true);
	connect(AddContactButton, SIGNAL(clicked(bool)), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(tr("Cancel"), this);
	connect(cancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

	buttons->addButton(AddContactButton, QDialogButtonBox::AcceptRole);
	buttons->addButton(cancel, QDialogButtonBox::DestructiveRole);

// 	TODO: NOW, does not work
// 	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
// 	setMaximumHeight(layout->minimumSize().height());
// 	layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

Account * AddBuddyWindow::selectedAccount()
{
	QModelIndex index = AccountCombo->model()->index(AccountCombo->currentIndex(), 0);
	return index.data(AccountRole).value<Account *>();
}

void AddBuddyWindow::setContact(Contact contact)
{
	MyContact = contact;

	Account *account = contact.prefferedAccount();
	if (account)
	{
		AccountCombo->setCurrentIndex(AccountComboModel->accountIndex(account));
		UserNameEdit->setText(contact.id(account));
	}

	DisplayNameEdit->setText(contact.display());
}

void AddBuddyWindow::setAddContactEnabled()
{
	Account *account = selectedAccount();
	if (!account)
	{
		AddContactButton->setEnabled(false);
		return;
	}

	if (!account->protocol()->protocolFactory()->idRegularExpression().exactMatch(UserNameEdit->text()))
	{
		AddContactButton->setEnabled(false);
		return;
	}

	if (DisplayNameEdit->text().isEmpty())
	{
		AddContactButton->setEnabled(false);
		return;
	}

	AddContactButton->setEnabled((ContactManager::instance()->byDisplay(DisplayNameEdit->text()).isNull()));
}

void AddBuddyWindow::setValidateRegularExpression()
{
	Account *account = selectedAccount();
	if (account)
	{
		UserNameValidator->setRegExp(account->protocol()->protocolFactory()->idRegularExpression());
		return;
	}

	QStringList regularExpressions;
	
	foreach (Account *account, AccountManager::instance()->accounts())
	{
		QRegExp regularExpression = account->protocol()->protocolFactory()->idRegularExpression();
		if (!regularExpression.isEmpty())
			regularExpressions.append(regularExpression.pattern());
	}
	
	regularExpressions.removeDuplicates();
	UserNameValidator->setRegExp(QRegExp(QString("(%1)").arg(regularExpressions.join("|"))));
}

void AddBuddyWindow::setAccountFilter()
{
	AccountComboFilter->setId(UserNameEdit->text());
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

	if (ok && !newGroupName.isEmpty() && GroupManager::instance()->acceptableGroupName(newGroupName))
		GroupManager::instance()->byName(newGroupName);
	else
		GroupCombo->setCurrentIndex(0);
}

void AddBuddyWindow::accept()
{
	Account *account = selectedAccount();
	if (MyContact.isNull())
		MyContact = ContactManager::instance()->byId(account, UserNameEdit->text());

	MyContact.setType(ContactData::TypeNormal);
	MyContact.setDisplay(DisplayNameEdit->text());

	QDialog::accept();
}

void AddBuddyWindow::reject()
{
	QDialog::reject();
}
