/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/model/groups-model.h"
#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/groups-combo-box.h"
#include "icons-manager.h"
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
		QDialog(parent), CurrentContact(contact)
{
	setWindowRole("kadu-subscription");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("New Contact Request"));

	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(2, 4);

	QLabel *messageLabel = new QLabel(tr("%1 wants to be able to chat with you.").arg(contact.id()), this);
	QLabel *visibleNameLabel = new QLabel(tr("Visible Name") + ":", this);
	QLabel *enterNameLabel = new QLabel(tr("Enter a name for this contact."), this);
	VisibleName = new QLineEdit(this);
	QLabel *groupLabel = new QLabel(tr("Add in Group") + ":", this);

	GroupCombo = new GroupsComboBox(this);

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
	layout->addWidget(enterNameLabel, 2, 1, 1, 1);
	layout->addWidget(groupLabel, 3, 0, 1, 1);
	layout->addWidget(GroupCombo, 3, 1, 1, 1);
	layout->addWidget(buttons, 4, 1, 1, 3);
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::accepted()
{
	//Giving somebody a status subscription does not force us to add them to our contact list.
	if (VisibleName->text().isEmpty())
	{
		emit requestAccepted(CurrentContact, true);
		close();
	}

	ContactManager::instance()->addItem(CurrentContact);
	Buddy buddy = BuddyManager::instance()->byContact(CurrentContact, ActionCreateAndAdd);
	buddy.setAnonymous(false);
	buddy.setDisplay(VisibleName->text());

	Group group= GroupCombo->currentGroup();
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
