/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>

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
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(4, 20);
	layout->setColumnMinimumWidth(5, 100);
	layout->setColumnMinimumWidth(8, 20);
	layout->setColumnStretch(3, 10);
	layout->setColumnStretch(6, 2);

	int row = 0;
	layout->setRowStretch(row++, 1); 
	
	QLabel *numberLabel = new QLabel(tr("Visible Name") + ":", this);
	layout->addWidget(numberLabel, 2, 2, 1, 1);
	DisplayEdit = new QLineEdit(this);
	DisplayEdit->setText(MyBuddy.display());
	layout->addWidget(DisplayEdit, 2, 3, 1, 1);

	QWidget *photoWidget = new QWidget;
	QVBoxLayout *photoLayout = new QVBoxLayout(photoWidget);
	photoLayout->setSpacing(2);

	QLabel *photoLabel = new QLabel(this);
	QPixmap photoPixmap = QPixmap(MyBuddy.contacts().count() > 0 ? MyBuddy.contacts().at(0).contactAvatar().pixmap() : QPixmap());
	photoLabel->setPixmap(photoPixmap);
	photoLayout->addWidget(photoLabel);

	QPushButton *changePhotoButton = new QPushButton(tr("Change Icon..."));
	photoLayout->addWidget(changePhotoButton);

	layout->addWidget(photoWidget, 1, 6, 3, 1);

	QGroupBox *contactsBox = new QGroupBox(tr("Buddy contacts"));
	QVBoxLayout *contactsLayout = new QVBoxLayout(contactsBox);
	contactsLayout ->addWidget(new BuddyContactsTable(MyBuddy, contactsBox));

	layout->addWidget(contactsBox, 4, 2, 2, 6);

	QGroupBox *communicationBox = new QGroupBox(tr("Communication Information"));
	QGridLayout *communicationLayout = new QGridLayout(communicationBox);
	communicationLayout->setColumnStretch(0, 1);
	communicationLayout->setColumnStretch(1, 3);
	communicationLayout->setColumnStretch(2, 3);

	row = 0;

	QHBoxLayout *phoneLayout = new QHBoxLayout;
	QLabel *phoneLabel = new QLabel(tr("Phone") + ":");
	PhoneEdit = new QLineEdit(this);
	PhoneEdit->setText(MyBuddy.homePhone());
	communicationLayout->addWidget(phoneLabel, row, 0, 1, 1);
	communicationLayout->addWidget(PhoneEdit, row++, 1, 1, 1);

	QHBoxLayout *mobileLayout = new QHBoxLayout;
	QLabel *mobileLabel = new QLabel(tr("Mobile") + ":");
	MobileEdit = new QLineEdit(this);
	MobileEdit->setText(MyBuddy.mobile());
	communicationLayout->addWidget(mobileLabel, row, 0, 1, 1);
	communicationLayout->addWidget(MobileEdit, row++, 1, 1, 1);

	QHBoxLayout *emailLayout = new QHBoxLayout;
	QLabel *emailLabel = new QLabel(tr("E-Mail") + ":");
	EmailEdit = new QLineEdit(this);
	EmailEdit->setText(MyBuddy.email());
	communicationLayout->addWidget(emailLabel, row, 0, 1, 1);
	communicationLayout->addWidget(EmailEdit, row++, 1, 1, 1);

	QHBoxLayout *websiteLayout = new QHBoxLayout;
	QLabel *websiteLabel = new QLabel(tr("Website") + ":");
	WebsiteEdit = new QLineEdit(this);
	WebsiteEdit->setText(MyBuddy.website());
	communicationLayout->addWidget(websiteLabel, row, 0, 1, 1);
	communicationLayout->addWidget(WebsiteEdit, row++, 1, 1, 1);

	layout->addWidget(communicationBox, 6, 2, 2, 6);
	layout->setRowStretch(8, 100);
}

void BuddyGeneralConfigurationWidget::unmergeContact()
{
	//TODO 0.6.6 how to get contact ID here?
	if (MessageDialog::ask(qApp->translate("MergedContactProperties", "Are you sure you want to remove the contact <contact name> from the merged contact %1?")./*arg().*/arg(MyBuddy.display())))
		emit doUnmergeContact();
}

void BuddyGeneralConfigurationWidget::saveConfiguration()
{
	MyBuddy.setDisplay(DisplayEdit->text());
	MyBuddy.setHomePhone(PhoneEdit->text());
	MyBuddy.setMobile(MobileEdit->text());
	MyBuddy.setEmail(EmailEdit->text());
	MyBuddy.setWebsite(WebsiteEdit->text());
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

