/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-kadu-data.h"
#include "buddies/buddy-shared.h"
#include "configuration/configuration-file.h"
#include "notify/buddy-notify-data.h"
#include "notify/notification-manager.h"
#include "protocols/protocol.h"

#include "buddy-options-configuration-widget.h"

BuddyOptionsConfigurationWidget::BuddyOptionsConfigurationWidget(Buddy &buddy, QWidget *parent) :
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
	BuddyNotifyData *bnd = 0;
	if (MyBuddy.data())
		bnd = MyBuddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), false);
	if (bnd)
		NotifyCheckBox->setChecked(bnd->notify());

	layout->addWidget(NotifyCheckBox);

	HideDescriptionCheckBox = new QCheckBox(tr("Hide description"), this);
	BuddyKaduData *ckd = 0;
	if (MyBuddy.data())
		ckd = MyBuddy.data()->moduleStorableData<BuddyKaduData>("kadu", 0, false);
	if (ckd)
		HideDescriptionCheckBox->setChecked(ckd->hideDescription());

	layout->addWidget(HideDescriptionCheckBox);

	layout->addStretch(100);
}

void BuddyOptionsConfigurationWidget::save()
{
	MyBuddy.setBlocked(BlockCheckBox->isChecked());
	MyBuddy.setOfflineTo(!OfflineToCheckBox->isChecked());

	BuddyNotifyData *bnd = 0;
	BuddyKaduData *ckd = 0;
	if (MyBuddy.data())
	{
		bnd = MyBuddy.data()->moduleStorableData<BuddyNotifyData>("notify", NotificationManager::instance(), true);
		ckd = MyBuddy.data()->moduleStorableData<BuddyKaduData>("kadu", 0, true);
	}
	if (bnd)
	{
		bnd->setNotify(NotifyCheckBox->isChecked());
		bnd->store();
	}
	if (ckd)
	{
		ckd->setHideDescription(HideDescriptionCheckBox->isChecked());
		ckd->store();
	}
}

void BuddyOptionsConfigurationWidget::configurationUpdated()
{
	NotifyCheckBox->setEnabled(!config_file.readBoolEntry("Notify", "NotifyAboutAll"));
}

void BuddyOptionsConfigurationWidget::updateOfflineTo()
{
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
