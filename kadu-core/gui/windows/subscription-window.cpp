/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "gui/windows/add-buddy-window.h"
#include "icons/icons-manager.h"
#include "model/roles.h"
#include "protocols/services/roster/roster-entry.h"

#include "subscription-window.h"

void SubscriptionWindow::getSubscription(Contact contact, QObject *receiver, const char *slot)
{
	SubscriptionWindow *window = new SubscriptionWindow(contact);
	connect(window, SIGNAL(requestConsidered(Contact, bool)), receiver, slot);

	window->exec();
}

SubscriptionWindow::SubscriptionWindow(Contact contact, QWidget *parent) :
		QDialog(parent), DesktopAwareObject(this), CurrentContact(contact)
{
	setWindowRole("kadu-subscription");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("Ask For Sharing Status"));
	resize(500, 120);

	// It'd be too unsafe to not add this contact to the manager now and rely later on addItem()
	// as the contact might be added in the meantime. See bug #2222.
	Contact knownContact = ContactManager::instance()->byId(CurrentContact.contactAccount(), CurrentContact.id(), ActionReturnNull);
	if (knownContact)
		CurrentContact = knownContact;
	else if (CurrentContact)
		CurrentContact.rosterEntry()->setState(RosterEntrySynchronized);

	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnStretch(2, 4);

	QLabel *messageLabel = new QLabel(tr("User <b>%1</b> wants to add you to his contact list.").arg(CurrentContact.id()), this);

	QLabel *finalQuestionLabel = new QLabel(tr("Do you want this person to see your status?"), this);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *shareAndAdd = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Allow and add buddy..."), this);
	if (knownContact && !knownContact.isAnonymous())
	{
		shareAndAdd->setVisible(false);
	}
	else
	{
		shareAndAdd->setDefault(true);
		buttons->addButton(shareAndAdd, QDialogButtonBox::AcceptRole);
	}

	QPushButton *share = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Allow"), this);
	share->setDefault(true);
	buttons->addButton(share, QDialogButtonBox::AcceptRole);

	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Ignore"), this);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(shareAndAdd, SIGNAL(clicked(bool)), this, SLOT(accepted()));
	connect(share, SIGNAL(clicked(bool)), this, SLOT(allowed()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(rejected()));

	layout->addWidget(messageLabel, 0, 0, 1, 3);
	layout->addWidget(finalQuestionLabel, 1, 0, 1, 1);
	layout->addWidget(buttons, 2, 0, 1, 3);

	shareAndAdd->setFocus();
}

SubscriptionWindow::~SubscriptionWindow()
{
}

void SubscriptionWindow::accepted()
{
	Buddy buddy = BuddyManager::instance()->byContact(CurrentContact, ActionCreate);
	buddy.setAnonymous(true);
	(new AddBuddyWindow(0, buddy))->show();
	allowed();
}

void SubscriptionWindow::allowed()
{
	emit requestConsidered(CurrentContact, true);
	close();
}

void SubscriptionWindow::rejected()
{
	emit requestConsidered(CurrentContact, false);
	close();
}

#include "moc_subscription-window.cpp"
