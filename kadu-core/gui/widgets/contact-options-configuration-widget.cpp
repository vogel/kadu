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

#include "buddies/account-data/contact-account-data.h"
#include "notify/contact-notify-data.h"

#include "contact-options-configuration-widget.h"

ContactOptionsConfigurationWidget::ContactOptionsConfigurationWidget(Buddy &contact, QWidget *parent)
	: QWidget(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactOptionsConfigurationWidget::~ContactOptionsConfigurationWidget()
{
}

void ContactOptionsConfigurationWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *tabLabel = new QLabel(tr("Options"), this);
	QFont tabLabelFont = tabLabel->font();
	tabLabelFont.setPointSize(tabLabelFont.pointSize() + 3);
	tabLabelFont.setWeight(QFont::Bold);
	tabLabel->setFont(tabLabelFont);
	layout->addWidget(tabLabel, row++, 1, 1, 4);

	layout->setRowStretch(row++, 1);       

	BlockCheckBox = new QCheckBox(tr("Block contact"), this);
	BlockCheckBox->setChecked(CurrentContact.isBlocked(CurrentContact.prefferedAccount()));
	layout->addWidget(BlockCheckBox, row++, 2, 1, 2);      

	OfflineToCheckBox = new QCheckBox(tr("Always appear as offline to contact"), this);
	OfflineToCheckBox->setChecked(CurrentContact.isOfflineTo(CurrentContact.prefferedAccount()));
	layout->addWidget(OfflineToCheckBox, row++, 2, 1, 2);        

	NotifyCheckBox = new QCheckBox(tr("Notify when contact's status changes"), this);
	ContactNotifyData *cnd = CurrentContact.moduleData<ContactNotifyData>();
	if (cnd)
	{
		NotifyCheckBox->setChecked(cnd->notify());
		delete cnd;
	}
	layout->addWidget(NotifyCheckBox, row++, 2, 1, 2);   

	layout->setRowStretch(row, 100);                            
}

void ContactOptionsConfigurationWidget::saveConfiguration()
{

	CurrentContact.accountData(CurrentContact.prefferedAccount())->setBlocked(BlockCheckBox->isChecked());
	CurrentContact.setOfflineTo(CurrentContact.prefferedAccount(), OfflineToCheckBox->isChecked());
	ContactNotifyData *cnd = CurrentContact.moduleData<ContactNotifyData>();
	if (cnd)
	{
		cnd->setNotify(NotifyCheckBox->isChecked());
		cnd->storeConfiguration();
		delete cnd;
	}
}