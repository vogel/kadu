/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

#include <QtGui/QLineEdit>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/avatar.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-shared.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "configuration/configuration-contact-data-manager.h"
#include "gui/windows/message-dialog.h"
#include "icons-manager.h"
#include "misc/misc.h"
#include "model/actions-proxy-model.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "buddy-general-configuration-widget.h"
#include "buddy-contacts-table.h"

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

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ":", nameWidget);
	nameLayout->addWidget(numberLabel);

	DisplayEdit = new QLineEdit(nameWidget);
	connect(DisplayEdit, SIGNAL(textChanged(QString)), this, SIGNAL(validChanged()));
	DisplayEdit->setText(MyBuddy.display());
	nameLayout->addWidget(DisplayEdit);

	QWidget *photoWidget = new QWidget(nameWidget);
	QVBoxLayout *photoLayout = new QVBoxLayout(photoWidget);
	photoLayout->setSpacing(2);

	QLabel *photoLabel = new QLabel(photoWidget);
	QPixmap photoPixmap = QPixmap(MyBuddy.contacts().count() > 0
			? MyBuddy.contacts().at(0).contactAvatar().pixmap()
			: QPixmap()).scaled(64, 64, Qt::KeepAspectRatio);
	photoLabel->setPixmap(photoPixmap);
	photoLabel->setFixedSize(QSize(70, 70));
	photoLayout->addWidget(photoLabel, 0, Qt::AlignCenter);

	QPushButton *changePhotoButton = new QPushButton(tr("Change Icon..."));
	photoLayout->addWidget(changePhotoButton);

	nameLayout->addWidget(photoWidget);

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
	communicationLayout->addRow(new QLabel(tr("Phone") + ":"), PhoneEdit);

	MobileEdit = new QLineEdit(this);
	MobileEdit->setText(MyBuddy.mobile());
	communicationLayout->addRow(new QLabel(tr("Mobile") + ":"), MobileEdit);

	EmailEdit = new QLineEdit(this);
	EmailEdit->setText(MyBuddy.email());
	communicationLayout->addRow(new QLabel(tr("E-Mail") + ":"), EmailEdit);

	WebsiteEdit = new QLineEdit(this);
	WebsiteEdit->setText(MyBuddy.website());
	communicationLayout->addRow(new QLabel(tr("Website") + ":"), WebsiteEdit);

	layout->addWidget(communicationBox);
	layout->addStretch(100);
}

bool BuddyGeneralConfigurationWidget::isValid()
{
	QString display = DisplayEdit->text();
	if (display.isEmpty())
		return false;

	Buddy buddy = BuddyManager::instance()->byDisplay(display);
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

	ContactsTable->save();
}

void BuddyGeneralConfigurationWidget::showOrderDialog()
{
	OrderDialog = new QDialog(this);
	OrderDialog->setAttribute(Qt::WA_DeleteOnClose);
	OrderDialog->setWindowTitle(tr("Set Order"));
	OrderDialog->resize(300, 200);

	QGridLayout *layout = new QGridLayout(OrderDialog);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnMinimumWidth(5, 100);
	layout->setColumnMinimumWidth(8, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;

	QTreeView *orderView = new QTreeView(OrderDialog);
	layout->addWidget(orderView, row++, 1, 1, 1);

	QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal, OrderDialog);

	QPushButton *cancelButton = new QPushButton(IconsManager::instance()->loadIcon("CloseWindowButton"), tr("Cancel"), OrderDialog);
	buttons->addButton(cancelButton, QDialogButtonBox::RejectRole);
	QPushButton *saveButton = new QPushButton(IconsManager::instance()->loadIcon("OkWindowButton"), tr("Save"), OrderDialog);
	buttons->addButton(saveButton, QDialogButtonBox::AcceptRole);

	connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(updateOrderAndClose()));
	connect(cancelButton, SIGNAL(clicked(bool)), OrderDialog, SLOT(close()));

	layout->addWidget(buttons, row, 1, 1, 1);

	OrderDialog->show();
}

void BuddyGeneralConfigurationWidget::updateOrder()
{
	MyBuddy.blockUpdatedSignal();

// 	ContactTab->saveConfiguration();
// 	GroupsTab->saveConfiguration(); 
// 	OptionsTab->saveConfiguration(); 

	MyBuddy.unblockUpdatedSignal();
}

void BuddyGeneralConfigurationWidget::updateOrderAndClose()
{
	updateOrder();
	OrderDialog->close();
}

