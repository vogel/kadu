/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "avatars/avatar-manager.h"
#include "avatars/avatar.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "gui/widgets/buddy-avatar-widget.h"
#include "gui/widgets/buddy-contacts-table.h"
#include "gui/widgets/composite-configuration-value-state-notifier.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/widgets/simple-configuration-value-state-notifier.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocol.h"

#include "buddy-general-configuration-widget.h"

BuddyGeneralConfigurationWidget::BuddyGeneralConfigurationWidget(const Buddy &buddy, QWidget *parent) :
		QWidget(parent),
		ValueStateNotifier(new CompositeConfigurationValueStateNotifier(this)),
		SimpleValueStateNotifier(new SimpleConfigurationValueStateNotifier(this)),
		MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();

	ValueStateNotifier->addConfigurationValueStateNotifier(SimpleValueStateNotifier);

	updateStateNotifier();
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
	connect(DisplayEdit, SIGNAL(textChanged(QString)), this, SLOT(updateStateNotifier()));
	DisplayEdit->setText(MyBuddy.display());
	nameLayout->addWidget(DisplayEdit);
	if (1 == MyBuddy.contacts().count())
	{
		Protocol *protocolHandler = MyBuddy.contacts().at(0).contactAccount().protocolHandler();
		if (protocolHandler && protocolHandler->contactsListReadOnly())
		{
			DisplayEdit->setReadOnly(true);
			DisplayEdit->setToolTip(tr("Protocol used by this buddy's contact does not allow changing buddy's name client-side"));
		}
	}

	AvatarWidget = new BuddyAvatarWidget(MyBuddy, nameWidget);
	nameLayout->addWidget(AvatarWidget);

	QGroupBox *contactsBox = new QGroupBox(tr("Buddy contacts"), this);
	QVBoxLayout *contactsLayout = new QVBoxLayout(contactsBox);
	ContactsTable = new BuddyContactsTable(MyBuddy, contactsBox);
	ValueStateNotifier->addConfigurationValueStateNotifier(ContactsTable->valueStateNotifier());
	contactsLayout->addWidget(ContactsTable);

	PreferHigherStatusCheck = new QCheckBox(tr("Prefer the most available contact"), contactsBox);
	PreferHigherStatusCheck->setToolTip(tr(
			"<p>When enabled and one of this buddy's contacts has higher status (i.e., more available) "
			"than the others, that contact will be considered preferred regardless of its priority</p>"));
	PreferHigherStatusCheck->setChecked(MyBuddy.preferHigherStatuses());
	contactsLayout->addWidget(PreferHigherStatusCheck);

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

const ConfigurationValueStateNotifier * BuddyGeneralConfigurationWidget::valueStateNotifier() const
{
	return ValueStateNotifier;
}

bool BuddyGeneralConfigurationWidget::isValid() const
{
	QString display = DisplayEdit->text();
	if (display.isEmpty())
		return false;

	Buddy buddy = BuddyManager::instance()->byDisplay(display, ActionReturnNull);
	if (buddy && buddy != MyBuddy)
		return false;

	return true;
}

void BuddyGeneralConfigurationWidget::updateStateNotifier()
{
	SimpleValueStateNotifier->setState(isValid() ? StateChangedDataValid : StateChangedDataInvalid);
}

void BuddyGeneralConfigurationWidget::save()
{
	ContactsTable->save(); // first update contacts

	MyBuddy.setDisplay(DisplayEdit->text());
	MyBuddy.setHomePhone(PhoneEdit->text());
	MyBuddy.setMobile(MobileEdit->text());
	MyBuddy.setEmail(EmailEdit->text());
	MyBuddy.setWebsite(WebsiteEdit->text());
	MyBuddy.setPreferHigherStatuses(PreferHigherStatusCheck->isChecked());

	QPixmap avatar = AvatarWidget->avatarPixmap();
	if (!AvatarWidget->buddyAvatar() || avatar.isNull())
		removeBuddyAvatar();
	else
		setBuddyAvatar(avatar);
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

#include "moc_buddy-general-configuration-widget.cpp"
