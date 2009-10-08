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

#include "configuration/contact-account-data-manager.h"
#include "contacts/contact-account-data.h"
#include "misc/misc.h"

#include "contact-personal-info-configuration-widget.h"

ContactPersonalInfoConfigurationWidget::ContactPersonalInfoConfigurationWidget(QWidget *parent)
	: QWidget(parent)
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
	layout->setColumnStretch(3, 10);

	int row = 0;

	QLabel *tabLabel = new QLabel(tr("Personal Information"), this);
	layout->addWidget(tabLabel, row++, 0, 1, 4); 

	QLabel *usernameLabel = new QLabel(tr("Contact Username") + ":", this);
	layout->addWidget(usernameLabel, row, 1, 1, 1); 

	QComboBox *contactIdCombo = new QComboBox(this);
	layout->addWidget(contactIdCombo, row++, 2, 1, 1);

	QWidget *infoWidget = new QWidget(this);
	QGridLayout *infoLayout = new QGridLayout(infoWidget);
	infoLayout->setColumnStretch(1, 3);
	infoLayout->setColumnStretch(2, 3);
	infoLayout->setColumnStretch(3, 10);
	
	QLabel *firstNameLabel = new QLabel(tr("First Name") + ":",this);
	infoLayout->addWidget(firstNameLabel, 0, 1, 1, 1);
	QLabel *firstNameText = new QLabel("Juzwa",this);
	infoLayout->addWidget(firstNameText, 0, 2, 1, 1);

	QLabel *lastNameLabel = new QLabel(tr("Last Name") + ":", this);
	infoLayout->addWidget(lastNameLabel, 1, 1, 1, 1);
	QLabel *lastNameText = new QLabel("Burp",this);
	infoLayout->addWidget(lastNameText, 1, 2, 1, 1);

	QLabel *nicknameLabel = new QLabel(tr("Nickname") + ":", this);
	infoLayout->addWidget(nicknameLabel, 2, 1, 1, 1);
	QLabel *nicknameText = new QLabel("Burp",this);
	infoLayout->addWidget(nicknameText, 2, 2, 1, 1);

	QLabel *genderLabel = new QLabel(tr("Gender") + ":", this);
	infoLayout->addWidget(genderLabel, 3, 1, 1, 1);
	QLabel *genderText = new QLabel("Male",this);
	infoLayout->addWidget(genderText, 3, 2, 1, 1);

	QLabel *birthdateLabel = new QLabel(tr("Birthdate") + ":",this);
	infoLayout->addWidget(birthdateLabel, 4, 1, 1, 1);
	QLabel *birthdateText = new QLabel("19.08.1924", this);
	infoLayout->addWidget(birthdateText, 4, 2, 1, 1);

	QLabel *cityLabel = new QLabel(tr("City") + ":",this);
	infoLayout->addWidget(cityLabel, 5, 1, 1, 1);
	QLabel *cityText = new QLabel("Wroc",this);
	infoLayout->addWidget(cityText, 5, 2, 1, 1);

	QLabel *stateProvinceLabel = new QLabel(tr("State/Province") + ":",this);
	infoLayout->addWidget(stateProvinceLabel, 6, 1, 1, 1);
	QLabel *stateProvinceText = new QLabel("Poland",this);
	infoLayout->addWidget(stateProvinceText, 6, 2, 1, 1);

	infoLayout->setRowStretch(7, 20);

	QLabel *ipLabel = new QLabel(tr("IP Address") + ":",this);
	infoLayout->addWidget(ipLabel, 8, 1, 1, 1);
	QLabel *ipText = new QLabel("127.0.0.1",this);
	infoLayout->addWidget(ipText, 8, 2, 1, 1);

	QLabel *portLabel = new QLabel(tr("Port") + ":",this);
	infoLayout->addWidget(portLabel, 9, 1, 1, 1);
	QLabel *portText = new QLabel("80",this);
	infoLayout->addWidget(portText, 9, 2, 1, 1);

	QLabel *dnsNameLabel = new QLabel(tr("DNS Name") + ":",this);
	infoLayout->addWidget(dnsNameLabel, 10, 1, 1, 1);
	QLabel *dnsNameText = new QLabel("192.168.1.1",this);
	infoLayout->addWidget(dnsNameText, 10, 2, 1, 1);

	QLabel *protocolVerLabel = new QLabel(tr("Protocol Version") + ":",this);
	infoLayout->addWidget(protocolVerLabel, 11, 1, 1, 1);
	QLabel *protocolVerText = new QLabel("1500",this);
	infoLayout->addWidget(protocolVerText, 11, 2, 1, 1);


	layout->addWidget(infoWidget, row++, 0, 1, 6);

	layout->setRowStretch(row, 100);
}
