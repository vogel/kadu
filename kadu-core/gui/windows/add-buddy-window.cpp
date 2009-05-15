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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QWhatsThis>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/contact.h"
#include "contacts/group.h"
#include "contacts/group-manager.h"
#include "icons-manager.h"
#include "protocols/protocol.h"

#include "add-buddy-window.h"

AddBuddyWindow::AddBuddyWindow(QWidget *parent)
	: QWidget(parent, Qt::Window), showMoreOptions(false), contactAccount(0)
{
	createGui();

	continueButton->setEnabled(false);
	chooseAccountWidget->setVisible(false);
	stepTwoWidget->setVisible(false);
	addBuddyButton->setVisible(false);
}

AddBuddyWindow::AddBuddyWindow(Contact contact, QWidget *parent)
	: QWidget(parent, Qt::Window), showMoreOptions(false), contactAccount(contact.prefferedAccount())
{
	createGui();

	continueButton->setVisible(false);
	chooseAccountWidget->setVisible(false);
	stepTwoWidget->setVisible(true);
	addBuddyButton->setVisible(true);

	disconnect(buddyUid, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(clearUid()));
	buddyUid->setText(contact.id(contactAccount));
	buddyUid->setReadOnly(true);
}

void AddBuddyWindow::createGui()
{
	setWindowTitle(tr("Add buddy"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QLabel *icon = new QLabel;
	icon->setPixmap(IconsManager::instance()->loadPixmap("AddUserWindowIcon"));

	QLabel *name = new QLabel;
	name->setText("<font size=\"+2\">" + tr("Add Buddy") + "</font>");
	name->setWordWrap(true);
	name->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));

	QWidget *topWidget = new QWidget;
	QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
	topLayout->addWidget(icon);
	topLayout->addWidget(name);

	QFrame *frame = new QFrame;
	frame->setFrameShape(QFrame::HLine);
//
	QLabel *info = new QLabel;
	info->setText(tr("Enter instant messanger: ")
		      + "<b>adress,  number</b> or <a href=\"e-mail\">e-mail adresses</a> of the person you wish<br />"
		      "to add to your buddy list and click Continue");
	connect(info, SIGNAL(linkActivated(const QString &)), this, SLOT(helpLinkClicked()));

	buddyUid = new QLineEdit("number or adress (like username@gmail.com)");
	buddyUid->setToolTip(tr("Type adress or number of your buddy here and click Continue\n"));
			//	"You can also type two or more buddies to form a group conversation\n"
			//	"Separate every buddy using a coma (ex: joe@gmail.com,kate@jabber.org)"));
	connect(buddyUid, SIGNAL(textChanged(QString)), this, SLOT(buddyIdTextChanged(QString)));
	connect(buddyUid, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(clearUid()));

	continueButton = new QPushButton(tr("Continue"));
	connect(continueButton, SIGNAL(clicked()), this, SLOT(stepOneFinished()));

	QHBoxLayout *stepOneLayout = new QHBoxLayout;
	stepOneLayout->addWidget(buddyUid);
	stepOneLayout->addWidget(continueButton);
//
	QLabel *chooseAccountLabel = new QLabel;
	chooseAccountLabel->setText(tr("To which of your accounts you want to add this buddy:"));

	chooseAccountCombo = new QComboBox;
	chooseAccountCombo->addItem(tr("Choose account..."));
	chooseAccountCombo->setToolTip(tr("Choose account..."));
	connect(chooseAccountCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountSelected(int)));

	chooseAccountWidget = new QWidget;
	QHBoxLayout *chooseAccountLayout = new QHBoxLayout(chooseAccountWidget);
	chooseAccountLayout->addWidget(chooseAccountLabel);
	chooseAccountLayout->addWidget(chooseAccountCombo);
//
	QLabel *chooseNameLabel = new QLabel;
	chooseNameLabel->setText(tr("Choose visible name for this contact:"));

	chooseNameCombo = new QComboBox;
	chooseNameCombo->setEditable(true);
//	chooseNameCombo->addItem(tr("First Name and Last Name or Nickname"));

	QHBoxLayout *chooseNameLayout = new QHBoxLayout;
	chooseNameLayout->addWidget(chooseNameLabel);
	chooseNameLayout->addWidget(chooseNameCombo);
//
	QLabel *addToGroupLabel = new QLabel;
	addToGroupLabel->setText(tr("Add buddy to the group:"));

	addToGroupCombo = new QComboBox;
	QStringList items(tr("All"));
	foreach (const Group *group, GroupManager::instance()->groups())
		items << group->name();
	addToGroupCombo->addItems(items);
	addToGroupCombo->insertSeparator(items.count());
	addToGroupCombo->addItem(tr("Create new group..."));

	QHBoxLayout *addToGroupLayout = new QHBoxLayout;
	addToGroupLayout->addWidget(addToGroupLabel);
	addToGroupLayout->addWidget(addToGroupCombo);
//
	QLabel *moreOptionsLabel = new QLabel;
	moreOptionsLabel->setText(tr("More options:"));

	expandMoreOptionsButton = new QPushButton(">");
	expandMoreOptionsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	connect(expandMoreOptionsButton, SIGNAL(clicked()), this, SLOT(showMoreOptionsChanged()));

	QHBoxLayout *moreOptionsLayout = new QHBoxLayout;
	moreOptionsLayout->addWidget(moreOptionsLabel);
	moreOptionsLayout->addWidget(expandMoreOptionsButton);
	moreOptionsLayout->setAlignment(expandMoreOptionsButton, Qt::AlignLeft);
	moreOptionsLayout->insertStretch(-1);

	offlineToCheckBox = new QCheckBox(tr("Allow to see when I'm available"));
	offlineToCheckBox->setVisible(showMoreOptions);
	offlineToCheckBox->setChecked(true);
//
	stepTwoWidget = new QWidget;
	QVBoxLayout *stepTwoLayout = new QVBoxLayout(stepTwoWidget);
	stepTwoLayout->addLayout(chooseNameLayout);
	stepTwoLayout->addLayout(addToGroupLayout);
	stepTwoLayout->addLayout(moreOptionsLayout);
	stepTwoLayout->addWidget(offlineToCheckBox);
//
	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	addBuddyButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Add Buddy"), this);
	buttons_layout->addButton(addBuddyButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(addBuddyButton, SIGNAL(clicked()), this, SLOT(addBuddy()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	main_layout->setSizeConstraint(QLayout::SetFixedSize);
	main_layout->addWidget(topWidget);
	main_layout->addWidget(frame);
	main_layout->addWidget(info);
	main_layout->addLayout(stepOneLayout);
	main_layout->addWidget(chooseAccountWidget);
	main_layout->addWidget(stepTwoWidget);
	main_layout->addWidget(buttons_layout);
}

void AddBuddyWindow::helpLinkClicked()
{
	QWhatsThis::showText(QCursor::pos(), tr("Some e-mail services provide chat functionality\n"
						"integrated with e-mail adresses\n"
						"For example you can add GMail adress as\n"
						"contact in a form: username@gmail.com"), this);
}

void AddBuddyWindow::clearUid()
{
	disconnect(buddyUid, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(clearUid()));
	buddyUid->clear();
}

void AddBuddyWindow::buddyIdTextChanged(QString id)
{
	bool isValid = false;
	foreach (Account *account, AccountManager::instance()->accounts())
		if (account && account->protocol() && account->protocol()->validateUserID(id))
		{
			isValid = true;
			break;
		}

	continueButton->setEnabled(isValid);
}

void AddBuddyWindow::stepOneFinished()
{
	continueButton->setVisible(false);
	buddyUid->setReadOnly(true);

	QString id = buddyUid->text();
	QStringList accounts;
	foreach (Account *account, AccountManager::instance()->accounts())
		if (account && account->protocol() && account->protocol()->validateUserID(id))
		{
			accounts << account->name();
			contactAccount = account;
		}

	if (accounts.count() > 1)
	{
		chooseAccountWidget->setVisible(true);
		chooseAccountCombo->addItems(accounts);
	}
	else
	{
		stepTwoWidget->setVisible(true);
		addBuddyButton->setVisible(true);
	}
}

void AddBuddyWindow::accountSelected(int index)
{
	QString accountName = chooseAccountCombo->currentText();
	foreach (Account *account, AccountManager::instance()->accounts())
		if (account && account->name() == accountName)
		{
			contactAccount = account;
			break;
		}

	chooseAccountCombo->setEnabled(false);
	stepTwoWidget->setVisible(true);
	addBuddyButton->setVisible(true);
}

void AddBuddyWindow::showMoreOptionsChanged()
{
	showMoreOptions = !showMoreOptions;
	expandMoreOptionsButton->setText(showMoreOptions ? "v" : ">");
	offlineToCheckBox->setVisible(showMoreOptions);
}

void AddBuddyWindow::addBuddy()
{
	Contact contact = contactAccount->getContactById(buddyUid->text());
	if (contact.isAnonymous())
	{
		contact.setType(ContactData::TypeNormal);
		contact.setDisplay(chooseNameCombo->currentText().isEmpty() ? buddyUid->text() : chooseNameCombo->currentText());
		contact.setOfflineTo(contactAccount, !offlineToCheckBox->isChecked());
		if (addToGroupCombo->currentIndex() != 0 && addToGroupCombo->currentIndex() != addToGroupCombo->count() - 1)
		{
			QList<Group *> group;
			group.append(GroupManager::instance()->byName(addToGroupCombo->currentText()));
			contact.setGroups(group);
		}
	}
	close();
}
