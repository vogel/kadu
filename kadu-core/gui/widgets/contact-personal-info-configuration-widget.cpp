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
#include <QtGui/QLineEdit>
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>

#include "accounts/account.h"
#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "contact-personal-info-configuration-widget.h"

ContactPersonalInfoConfigurationWidget::ContactPersonalInfoConfigurationWidget(Contact &contact, QWidget *parent)
	: QWidget(parent), CurrentContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

ContactPersonalInfoConfigurationWidget::~ContactPersonalInfoConfigurationWidget()
{
}

void ContactPersonalInfoConfigurationWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);
	layout->setColumnMinimumWidth(0, 10);
	layout->setColumnMinimumWidth(1, 10);
	layout->setColumnMinimumWidth(3, 10);
	layout->setColumnMinimumWidth(6, 20);

	int row = 0;

	QLabel *tabLabel = new QLabel(tr("Personal Information"), this);
	QFont tabLabelFont = tabLabel->font();
	tabLabelFont.setPointSize(tabLabelFont.pointSize() + 3);
	tabLabelFont.setWeight(QFont::Bold);
	tabLabel->setFont(tabLabelFont);
	layout->addWidget(tabLabel, row++, 1, 1, 4); 

	QLabel *usernameLabel = new QLabel(tr("Contact Username") + ":", this);
	layout->addWidget(usernameLabel, row, 3, 1, 1); 

	QComboBox *contactIdCombo = new QComboBox(this);
	foreach (ContactAccountData *data, CurrentContact.accountDatas())
		contactIdCombo->addItem(data->account()->protocol()->icon(), 
			    data->id(),
			    data->account()->uuid().toString()
		);
	layout->addWidget(contactIdCombo, row++, 4, 1, 1);

	QGroupBox *infoWidget = new QGroupBox(this);
	QGridLayout *infoLayout = new QGridLayout(infoWidget);
	infoLayout->setColumnMinimumWidth(0, 10);
	infoLayout->setColumnMinimumWidth(4, 10);
	infoLayout->setColumnStretch(1, 3);
	infoLayout->setColumnStretch(2, 3);
	infoLayout->setColumnStretch(3, 8);

	int subRow = 0;
	
	QLabel *firstNameLabel = new QLabel(tr("First Name") + ":",this);
	firstNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(firstNameLabel, subRow, 1, 1, 1);
	QLabel *firstNameText = new QLabel("Juzwa",this);
	infoLayout->addWidget(firstNameText, subRow++, 2, 1, 1);

	QLabel *lastNameLabel = new QLabel(tr("Last Name") + ":", this);
	lastNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(lastNameLabel, subRow, 1, 1, 1);
	QLabel *lastNameText = new QLabel("Burp",this);
	infoLayout->addWidget(lastNameText, subRow++, 2, 1, 1);

	QLabel *nicknameLabel = new QLabel(tr("Nickname") + ":", this);
	nicknameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(nicknameLabel, subRow, 1, 1, 1);
	QLabel *nicknameText = new QLabel("Burp",this);
	infoLayout->addWidget(nicknameText, subRow++, 2, 1, 1);

	QLabel *genderLabel = new QLabel(tr("Gender") + ":", this);
	genderLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(genderLabel, subRow, 1, 1, 1);
	QLabel *genderText = new QLabel("Male",this);
	infoLayout->addWidget(genderText, subRow++, 2, 1, 1);

	QLabel *birthdateLabel = new QLabel(tr("Birthdate") + ":",this);
	birthdateLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(birthdateLabel, subRow, 1, 1, 1);
	QLabel *birthdateText = new QLabel("19.08.1924", this);
	infoLayout->addWidget(birthdateText, subRow++, 2, 1, 1);

	QLabel *cityLabel = new QLabel(tr("City") + ":",this);
	cityLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(cityLabel, subRow, 1, 1, 1);
	QLabel *cityText = new QLabel("Wroc",this);
	infoLayout->addWidget(cityText, subRow++, 2, 1, 1);

	QLabel *stateProvinceLabel = new QLabel(tr("State/Province") + ":",this);
	stateProvinceLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(stateProvinceLabel, subRow, 1, 1, 1);
	QLabel *stateProvinceText = new QLabel("Poland",this);
	infoLayout->addWidget(stateProvinceText, subRow++, 2, 1, 1);

	infoLayout->setRowStretch(7, 20);

	QLabel *ipLabel = new QLabel(tr("IP Address") + ":",this);
	ipLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(ipLabel, subRow, 1, 1, 1);
	QLabel *ipText = new QLabel("127.0.0.1",this);
	infoLayout->addWidget(ipText, subRow++, 2, 1, 1);

	QLabel *portLabel = new QLabel(tr("Port") + ":",this);
	portLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(portLabel, subRow, 1, 1, 1);
	QLabel *portText = new QLabel("80",this);
	infoLayout->addWidget(portText, subRow++, 2, 1, 1);

	QLabel *dnsNameLabel = new QLabel(tr("DNS Name") + ":",this);
	dnsNameLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(dnsNameLabel, subRow, 1, 1, 1);
	QLabel *dnsNameText = new QLabel("192.168.1.1", this);
	infoLayout->addWidget(dnsNameText, subRow++, 2, 1, 1);

	QLabel *protocolVerLabel = new QLabel(tr("Protocol Version") + ":",this);
	protocolVerLabel->setAlignment(Qt::AlignRight);
	infoLayout->addWidget(protocolVerLabel, 11, 1, 1, 1);
	QLabel *protocolVerText = new QLabel("1500",this);
	infoLayout->addWidget(protocolVerText, 11, 2, 1, 1);

	infoLayout->setRowStretch(12, 100);

	layout->addWidget(infoWidget, row++, 2, 1, 4);

	layout->setRowStretch(row, 100);
}
