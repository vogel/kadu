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
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>

#include <QtGui/QLineEdit>

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"

#include "contact-general-configuration-widget.h"

ContactGeneralConfigurationWidget::ContactGeneralConfigurationWidget(Contact &contact, QWidget *parent)
	: QWidget(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactGeneralConfigurationWidget::~ContactGeneralConfigurationWidget()
{
}

void ContactGeneralConfigurationWidget::createGui()
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

	QLabel *tabLabel = new QLabel(tr("General Properties"), this);
	QFont tabLabelFont = tabLabel->font();
	tabLabelFont.setPointSize(tabLabelFont.pointSize() + 3);
	tabLabelFont.setWeight(QFont::Bold);
	tabLabel->setFont(tabLabelFont);
	layout->addWidget(tabLabel, 0, 1, 1, 4);

	QLabel *numberLabel = new QLabel(tr("Visible Name") + ":", this);
	layout->addWidget(numberLabel, 2, 2, 1, 1);
	QLineEdit *AccountId = new QLineEdit(this);
	AccountId->setText(CurrentContact.display());
	layout->addWidget(AccountId, 2, 3, 1, 1);

	QWidget *photoWidget = new QWidget;
	QVBoxLayout *photoLayout = new QVBoxLayout(photoWidget);
	photoLayout->setSpacing(2);

	QPushButton *photoButton = new QPushButton;
	photoButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	photoLayout->addWidget(photoButton);

	QPushButton *changePhotoButton = new QPushButton(tr("Change Icon..."));
	photoLayout->addWidget(changePhotoButton);

	layout->addWidget(photoWidget, 1, 6, 3, 1);

	QGroupBox *accountsBox = new QGroupBox(tr("Merged Contact Accounts"));
	QGridLayout *accountsLayout = new QGridLayout(accountsBox);
	accountsLayout->setColumnMinimumWidth(2, 20);
	accountsLayout->setColumnStretch(0, 1);
	accountsLayout->setColumnStretch(1, 1);
	accountsLayout->setColumnStretch(5, 2);
	accountsLayout->setColumnStretch(6, 2);
	row = 0;

	QLabel *defaultContactLabel = new QLabel(tr("Default Contact") + ":");
	QComboBox *defaultContact = new QComboBox(this);
	QLabel *defaultContactNoticeLabel = new QLabel(tr("Chat messages will be sent to this username when you select the name from the buddy list"));
	accountsLayout->addWidget(defaultContactLabel, row, 0, 1, 1);
	accountsLayout->addWidget(defaultContact, row++, 1, 1, 4);
	accountsLayout->addWidget(defaultContactNoticeLabel, row++, 1, 1, 5);

	QLabel *inLabel = new QLabel(tr("in"));
	QLineEdit *contactLineEdit = new QLineEdit(this);
	QComboBox *accountsCombo = new QComboBox(this);

	accountsLayout->addWidget(contactLineEdit, row, 0, 1, 3);
	accountsLayout->addWidget(inLabel, row, 3, 1, 1);
	accountsLayout->addWidget(accountsCombo, row++, 4, 1, 3);

	QPushButton *addContactButton = new QPushButton(tr("Add Contact..."), this);
	QPushButton *setOrderButton = new QPushButton(tr("Set Order..."), this);

	accountsLayout->addWidget(addContactButton, row, 0, 1, 1);
	accountsLayout->addWidget(setOrderButton, row, 1, 1, 1);

	layout->addWidget(accountsBox, 4, 2, 2, 6);

	QGroupBox *communicationBox = new QGroupBox(tr("Communication Information"));
	QGridLayout *communicationLayout = new QGridLayout(communicationBox);
	communicationLayout->setColumnStretch(0, 1);
	communicationLayout->setColumnStretch(1, 3);
	communicationLayout->setColumnStretch(2, 3);

	row = 0;

	QHBoxLayout *phoneLayout = new QHBoxLayout;
	QLabel *phoneLabel = new QLabel(tr("Phone") + ":");
	QLineEdit *phone = new QLineEdit(this);
	communicationLayout->addWidget(phoneLabel, row, 0, 1, 1);
	communicationLayout->addWidget(phone, row++, 1, 1, 1);

	QHBoxLayout *mobileLayout = new QHBoxLayout;
	QLabel *mobileLabel = new QLabel(tr("Mobile") + ":");
	QLineEdit *mobile = new QLineEdit(this);

	communicationLayout->addWidget(mobileLabel, row, 0, 1, 1);
	communicationLayout->addWidget(mobile, row++, 1, 1, 1);

	QHBoxLayout *emailLayout = new QHBoxLayout;
	QLabel *emailLabel = new QLabel(tr("E-Mail") + ":");
	QLineEdit *email = new QLineEdit(this);
	communicationLayout->addWidget(emailLabel, row, 0, 1, 1);
	communicationLayout->addWidget(email, row++, 1, 1, 1);

	QHBoxLayout *websiteLayout = new QHBoxLayout;
	QLabel *websiteLabel = new QLabel(tr("Website") + ":");
	QLineEdit *website = new QLineEdit(this);
	communicationLayout->addWidget(websiteLabel, row, 0, 1, 1);
	communicationLayout->addWidget(website, row++, 1, 1, 1);

	layout->addWidget(communicationBox, 6, 2, 2, 6);
	layout->setRowStretch(8, 100);
}
