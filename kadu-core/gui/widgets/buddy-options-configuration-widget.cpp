/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <QtGui/QCheckBox>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "notify/notification-manager.h"
#include "protocols/protocol.h"

#include "buddy-options-configuration-widget.h"

BuddyOptionsConfigurationWidget::BuddyOptionsConfigurationWidget(const Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	updateOfflineTo();
	configurationUpdated();

	connect(MyBuddy, SIGNAL(contactAdded(Contact)), this, SLOT(updateOfflineTo()));
	connect(MyBuddy, SIGNAL(contactRemoved(Contact)), this, SLOT(updateOfflineTo()));
}

BuddyOptionsConfigurationWidget::~BuddyOptionsConfigurationWidget()
{
}

void BuddyOptionsConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	OfflineToCheckBox = new QCheckBox(tr("Allow buddy to see when I'm available"), this);
	OfflineToCheckBox->setChecked(!MyBuddy.isOfflineTo());
	connect(OfflineToCheckBox, SIGNAL(clicked(bool)), this, SLOT(offlineToToggled(bool)));
	layout->addWidget(OfflineToCheckBox);

	BlockCheckBox = new QCheckBox(tr("Block buddy"), this);
	BlockCheckBox->setChecked(MyBuddy.isBlocked());
	layout->addWidget(BlockCheckBox);

	NotifyCheckBox = new QCheckBox(tr("Notify when buddy's status changes"), this);
	layout->addWidget(NotifyCheckBox);

	HideDescriptionCheckBox = new QCheckBox(tr("Hide description"), this);
	layout->addWidget(HideDescriptionCheckBox);

	layout->addStretch(100);

	if (MyBuddy)
	{
		HideDescriptionCheckBox->setChecked(MyBuddy.property("kadu:HideDescription", false).toBool());
		NotifyCheckBox->setChecked(MyBuddy.property("notify:Notify", true).toBool());
	}
}

void BuddyOptionsConfigurationWidget::save()
{
	MyBuddy.setBlocked(BlockCheckBox->isChecked());
	MyBuddy.setOfflineTo(!OfflineToCheckBox->isChecked());

	if (MyBuddy)
	{
		if (!HideDescriptionCheckBox->isChecked())
			MyBuddy.removeProperty("kadu:HideDescription");
		else
			MyBuddy.addProperty("kadu:HideDescription", true, CustomProperties::Storable);

		if (NotifyCheckBox->isChecked())
			MyBuddy.removeProperty("notify:Notify");
		else
			MyBuddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}
}

void BuddyOptionsConfigurationWidget::configurationUpdated()
{
}

void BuddyOptionsConfigurationWidget::updateOfflineTo()
{
	if (MyBuddy.isOfflineTo())
	{
		OfflineToCheckBox->setEnabled(true);
		return;
	}

	OfflineToCheckBox->setEnabled(false);
	foreach (const Contact &contact, MyBuddy.contacts())
	{
		if (!contact.contactAccount().protocolHandler())
			continue;

		if (contact.contactAccount().protocolHandler()->supportsPrivateStatus())
		{
			OfflineToCheckBox->setEnabled(true);
			return;
		}
	}
}

void BuddyOptionsConfigurationWidget::offlineToToggled(bool toggled)
{
	if (toggled == true)
		return;

	foreach (const Contact &contact, MyBuddy.contacts())
	{
		if (!contact.contactAccount().protocolHandler())
			continue;

		if (contact.contactAccount().protocolHandler()->supportsPrivateStatus() && !contact.contactAccount().privateStatus())
		{
			if (QMessageBox::question(this, "Private status", "You need to turn on 'private status' to check "
									  "this option. Turning on 'private status' means that from now on you "
									  "will be seen as offline for all people that are not on your buddy list."
									  "\nWould you like to set 'private status' now?",
									  QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
				== QMessageBox::Yes)
			{
				foreach (const Contact &con, MyBuddy.contacts())
					con.contactAccount().setPrivateStatus(true);
			}

			return;
		}
	}
}

#include "moc_buddy-options-configuration-widget.cpp"
