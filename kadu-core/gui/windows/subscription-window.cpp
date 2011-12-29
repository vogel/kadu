/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
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
#include <QtGui/QApplication>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "buddies/model/groups-model.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/groups-combo-box.h"
#include "gui/widgets/select-buddy-combo-box.h"
#include "icons/icons-manager.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"

#include "subscription-window.h"

void SubscriptionWindow::getSubscription(Contact contact, QObject *receiver, const char *slot)
{
	SubscriptionWindow *window = new SubscriptionWindow(contact);
	connect(window, SIGNAL(requestAccepted(Contact, bool)), receiver, slot);

	window->exec();
}

SubscriptionWindow::SubscriptionWindow(Contact contact, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), CurrentContact(contact)
{
	setWindowRole("kadu-subscription");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("New Contact Request"));

	// It'd be too unsafe to not add this contact to the manager now and rely later on addItem()
	// as the contact might be added in the meantime. See bug #2222.
	Contact knownContact = ContactManager::instance()->byId(CurrentContact.contactAccount(), CurrentContact.id(), ActionReturnNull);
	if (knownContact)
		CurrentContact = knownContact;
	else
	{
		CurrentContact.setDirty(false);
		ContactManager::instance()->addItem(CurrentContact);
	}

	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(2, 4);

	QLabel *messageLabel = new QLabel(tr("<b>%1</b> wants to be able to chat with you.").arg(CurrentContact.id()), this);

	QLabel *groupLabel = new QLabel(tr("Add in Group") + ':', this);

	GroupCombo = new GroupsComboBox(this);

	QLabel *visibleNameLabel = new QLabel(tr("Visible Name") + ':', this);

	QLabel *hintLabel = new QLabel(tr("Enter a name for this contact."), this);
	QFont hintLabelFont = hintLabel->font();
	hintLabelFont.setItalic(true);
	hintLabelFont.setPointSize(hintLabelFont.pointSize() - 2);
	hintLabel->setFont(hintLabelFont);

	VisibleName = new QLineEdit(this);

	MergeContact = new QCheckBox(tr("Merge with an existing contact"), this);

	QWidget *selectContactWidget = new QWidget(this);
	QHBoxLayout *selectContactLayout = new QHBoxLayout(selectContactWidget);
	selectContactLayout->addSpacing(20);
	SelectContact = new SelectBuddyComboBox(selectContactWidget);
	SelectContact->setEnabled(false);
	selectContactLayout->addWidget(SelectContact);

	connect(MergeContact, SIGNAL(toggled(bool)), SelectContact, SLOT(setEnabled(bool)));
	connect(MergeContact, SIGNAL(toggled(bool)), VisibleName, SLOT(setDisabled(bool)));

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Allow"), this);
	okButton->setDefault(true);
	buttons->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Ignore"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(rejected()));

	layout->addWidget(messageLabel, 0, 0, 1, 3);
	layout->addWidget(visibleNameLabel, 1, 0, 1, 1);
	layout->addWidget(VisibleName, 1, 1, 1, 1);
	layout->addWidget(hintLabel, 2, 1, 1, 1);
	layout->addWidget(groupLabel, 3, 0, 1, 1);
	layout->addWidget(GroupCombo, 3, 1, 1, 1);
	layout->addWidget(MergeContact, 4, 1, 1, 3);
	layout->addWidget(selectContactWidget, 5, 1, 1, 3);

	layout->addWidget(buttons, 6, 1, 1, 3);

	VisibleName->setFocus();
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::accepted()
{
	//Giving somebody a status subscription does not force us to add them to our contact list.
	if (VisibleName->text().isEmpty() && !MergeContact->isChecked())
	{
		emit requestAccepted(CurrentContact, true);
		close();
	}

	Buddy buddy;
	if (!MergeContact->isChecked())
	{
		buddy = BuddyManager::instance()->byContact(CurrentContact, ActionCreateAndAdd);
		buddy.setAnonymous(false);
		buddy.setDisplay(VisibleName->text());
	}
	else
	{
		buddy = SelectContact->currentBuddy();
		if (!buddy)
			return;

		CurrentContact.setOwnerBuddy(buddy);
	}

	Group group = GroupCombo->currentGroup();
	if (group)
		buddy.addToGroup(group);

	emit requestAccepted(CurrentContact, true);
	close();
}

void SubscriptionWindow::rejected()
{
  	emit requestAccepted(CurrentContact, false);
	close();
}
