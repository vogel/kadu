/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>
#include <QtGui/QFormLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "gui/widgets/buddy-avatar-widget.h"
#include "gui/widgets/buddy-contacts-table.h"
#include "gui/windows/message-dialog.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "buddy-general-configuration-widget.h"

BuddyGeneralConfigurationWidget::BuddyGeneralConfigurationWidget(Buddy &buddy, QWidget *parent)
		: QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyGeneralConfigurationWidget::~BuddyGeneralConfigurationWidget()
{
}

void BuddyGeneralConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *nameWidget = new QWidget(this);
	layout->addWidget(nameWidget);

	QHBoxLayout *nameLayout = new QHBoxLayout(nameWidget);

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ':', nameWidget);
	nameLayout->addWidget(numberLabel);

	DisplayEdit = new QLineEdit(nameWidget);
	connect(DisplayEdit, SIGNAL(textChanged(QString)), this, SIGNAL(validChanged()));
	DisplayEdit->setText(MyBuddy.display());
	nameLayout->addWidget(DisplayEdit);

	AvatarWidget = new BuddyAvatarWidget(MyBuddy, nameWidget);
	nameLayout->addWidget(AvatarWidget);

	QGroupBox *contactsBox = new QGroupBox(tr("Buddy contacts"));
	QVBoxLayout *contactsLayout = new QVBoxLayout(contactsBox);
	ContactsTable = new BuddyContactsTable(MyBuddy, contactsBox);
	connect(ContactsTable, SIGNAL(validChanged()), this, SIGNAL(validChanged()));
	contactsLayout ->addWidget(ContactsTable);

	layout->addWidget(contactsBox);

	QGroupBox *communicationBox = new QGroupBox(tr("Communication Information"));
	QFormLayout *communicationLayout = new QFormLayout(communicationBox);

	PhoneEdit = new QLineEdit(this);
	PhoneEdit->setText(MyBuddy.homePhone());
	communicationLayout->addRow(new QLabel(tr("Phone") + ':'), PhoneEdit);

	MobileEdit = new QLineEdit(this);
	MobileEdit->setText(MyBuddy.mobile());
	communicationLayout->addRow(new QLabel(tr("Mobile") + ':'), MobileEdit);

	EmailEdit = new QLineEdit(this);
	EmailEdit->setText(MyBuddy.email());
	communicationLayout->addRow(new QLabel(tr("E-Mail") + ':'), EmailEdit);

	WebsiteEdit = new QLineEdit(this);
	WebsiteEdit->setText(MyBuddy.website());
	communicationLayout->addRow(new QLabel(tr("Website") + ':'), WebsiteEdit);

	layout->addWidget(communicationBox);
	layout->addStretch(100);
}

bool BuddyGeneralConfigurationWidget::isValid()
{
	QString display = DisplayEdit->text();
	if (display.isEmpty())
		return false;

	Buddy buddy = BuddyManager::instance()->byDisplay(display, ActionReturnNull);
	if (buddy && buddy != MyBuddy)
		return false;

	return ContactsTable->isValid();
}

void BuddyGeneralConfigurationWidget::save()
{
	MyBuddy.setDisplay(DisplayEdit->text());
	MyBuddy.setHomePhone(PhoneEdit->text());
	MyBuddy.setMobile(MobileEdit->text());
	MyBuddy.setEmail(EmailEdit->text());
	MyBuddy.setWebsite(WebsiteEdit->text());

	QPixmap avatar = AvatarWidget->avatarPixmap();
	if (!AvatarWidget->buddyAvatar() || avatar.isNull())
		removeBuddyAvatar();
	else
		setBuddyAvatar(avatar);

	ContactsTable->save();
}

void BuddyGeneralConfigurationWidget::removeBuddyAvatar()
{
	Avatar buddyAvatar = MyBuddy.buddyAvatar();
	if (buddyAvatar.isNull())
		return;

	buddyAvatar.setPixmap(QPixmap());
	AvatarManager::instance()->removeItem(buddyAvatar);
	MyBuddy.setBuddyAvatar(Avatar::null);
}

void BuddyGeneralConfigurationWidget::setBuddyAvatar(const QPixmap& avatar)
{
	Avatar buddyAvatar = AvatarManager::instance()->byBuddy(MyBuddy, ActionCreateAndAdd);
	buddyAvatar.setPixmap(avatar);
}
