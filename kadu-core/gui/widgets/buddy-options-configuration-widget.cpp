/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
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
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnStretch(3, 10);

	int row = 0;

	layout->setRowStretch(row++, 1);       

	BlockCheckBox = new QCheckBox(tr("Block contact"), this);
	BlockCheckBox->setChecked(MyBuddy.isBlocked());
	layout->addWidget(BlockCheckBox, row++, 2, 1, 2);      

	OfflineToCheckBox = new QCheckBox(tr("Always appear as offline to contact"), this);
	OfflineToCheckBox->setChecked(MyBuddy.isOfflineTo());
	layout->addWidget(OfflineToCheckBox, row++, 2, 1, 2);        

	NotifyCheckBox = new QCheckBox(tr("Notify when contact's status changes"), this);
	ContactNotifyData *cnd = 0;
	if (MyBuddy.data())
		cnd = MyBuddy.data()->moduleData<ContactNotifyData>("notify");
	if (cnd)
		NotifyCheckBox->setChecked(cnd->notify());

	layout->addWidget(NotifyCheckBox, row++, 2, 1, 2);

	layout->setRowStretch(row, 100);
}

void BuddyOptionsConfigurationWidget::saveConfiguration()
{

	MyBuddy.setBlocked(BlockCheckBox->isChecked());
	MyBuddy.setOfflineTo(OfflineToCheckBox->isChecked());

	ContactNotifyData *cnd = 0;
	if (MyBuddy.data())
		cnd = MyBuddy.data()->moduleData<ContactNotifyData>("notify");
	if (cnd)
	{
		cnd->setNotify(NotifyCheckBox->isChecked());
		cnd->store();
	}
}
