/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
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

#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "contacts/model/buddy-contact-model.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol.h"

#include "buddy-personal-info-configuration-widget.h"

BuddyPersonalInfoConfigurationWidget::BuddyPersonalInfoConfigurationWidget(Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	accountSelectionChanged(0);
}

BuddyPersonalInfoConfigurationWidget::~BuddyPersonalInfoConfigurationWidget()
{
}

void BuddyPersonalInfoConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QWidget *contactWidget = new QWidget(this);
	layout->addWidget(contactWidget);

	QFormLayout *contactLayout = new QFormLayout(contactWidget);

	ContactIdCombo = new QComboBox(this);
	ContactIdCombo->setModel(new BuddyContactModel(MyBuddy));
	ContactIdCombo->setModelColumn(1); // use long name
	connect(ContactIdCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountSelectionChanged(int)));

	contactLayout->addRow(new QLabel(tr("Buddy contact") + ":", contactWidget), ContactIdCombo);

	QGroupBox *infoWidget = new QGroupBox(this);
	QFormLayout *infoLayout = new QFormLayout(infoWidget);

	FirstNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("First Name") + ":", infoWidget), FirstNameText);

	LastNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Last Name") + ":", infoWidget), LastNameText);

	NicknameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Nickname") + ":", infoWidget), NicknameText);

	GenderText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Gender") + ":", infoWidget), GenderText);

	BirthdateText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Birthdate") + ":", infoWidget), BirthdateText);

	CityText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("City") + ":", infoWidget), CityText);

	StateProvinceText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("State/Province") + ":", infoWidget), StateProvinceText);

	IpText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("IP Address") + ":", infoWidget), IpText);

	PortText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Port") + ":", infoWidget), PortText);

	DnsNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("DNS Name") + ":", infoWidget), DnsNameText);

	ProtocolVerText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Protocol Version") + ":", infoWidget), ProtocolVerText);

	layout->addWidget(infoWidget);
	layout->addStretch(100);
}

void BuddyPersonalInfoConfigurationWidget::accountSelectionChanged(int index)
{
	Contact contact = qvariant_cast<Contact>(ContactIdCombo->model()->index(index, 0).data(ContactRole));
	if (!contact)
	{
		FirstNameText->setText(QString::null);
		LastNameText->setText(QString::null);
		NicknameText->setText(QString::null);
		GenderText->setText(QString::null);
		BirthdateText->setText(QString::null);
		CityText->setText(QString::null);
		StateProvinceText->setText(QString::null);
		IpText->setText(QString::null);
		PortText->setText(QString::null);
		DnsNameText->setText(QString::null);
		ProtocolVerText->setText(QString::null);
	}
	else
	{
		//TODO 0.6.6 proper values
		FirstNameText->setText(MyBuddy.firstName());
		LastNameText->setText(MyBuddy.lastName());
		NicknameText->setText(MyBuddy.nickName());
		GenderText->setText(MyBuddy.firstName());
		BirthdateText->setText(MyBuddy.firstName());
		CityText->setText(MyBuddy.city());
		StateProvinceText->setText(MyBuddy.firstName());
		IpText->setText(contact.address().toString());
		PortText->setText(QString::number(contact.port()));
		DnsNameText->setText(contact.dnsName());
		ProtocolVerText->setText(contact.protocolVersion());
	}
}
