/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-shared.h"
#include "notify/contact-notify-data.h"

#include "buddy-options-configuration-widget.h"


BuddyOptionsConfigurationWidget::BuddyOptionsConfigurationWidget(Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyOptionsConfigurationWidget::~BuddyOptionsConfigurationWidget()
{
}

void BuddyOptionsConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	OfflineToCheckBox = new QCheckBox(tr("Allow contact to see when I'm available"), this);
	OfflineToCheckBox->setChecked(!MyBuddy.isOfflineTo());
	layout->addWidget(OfflineToCheckBox);

	BlockCheckBox = new QCheckBox(tr("Ignore contact"), this);
	BlockCheckBox->setChecked(MyBuddy.isBlocked());
	layout->addWidget(BlockCheckBox);

	NotifyCheckBox = new QCheckBox(tr("Notify when contact's status changes"), this);
	ContactNotifyData *cnd = 0;
	if (MyBuddy.data())
		cnd = MyBuddy.data()->moduleStorableData<ContactNotifyData>("notify", false);
	if (cnd)
		NotifyCheckBox->setChecked(cnd->notify());

	layout->addWidget(NotifyCheckBox);
	layout->addStretch(100);
}

void BuddyOptionsConfigurationWidget::save()
{
	MyBuddy.setBlocked(BlockCheckBox->isChecked());
	MyBuddy.setOfflineTo(!OfflineToCheckBox->isChecked());

	ContactNotifyData *cnd = 0;
	if (MyBuddy.data())
		cnd = MyBuddy.data()->moduleStorableData<ContactNotifyData>("notify", true);;
	if (cnd)
	{
		cnd->setNotify(NotifyCheckBox->isChecked());
		cnd->store();
	}
}
