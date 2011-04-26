/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "accounts/account.h"
#include "protocols/services/personal-info-service.h"
#include "protocols/protocol.h"

#include "jabber-personal-info-widget.h"

JabberPersonalInfoWidget::JabberPersonalInfoWidget(Account account, QWidget* parent) :
		QWidget(parent), MyBuddy(Buddy::create())
{
	createGui();
	fillForm();

	if (account.isNull() || !account.protocolHandler())
		return;

	Service = account.protocolHandler()->personalInfoService();
	if (!Service)
		return;

	connect(Service, SIGNAL(personalInfoAvailable(Buddy)), this, SLOT(personalInfoAvailable(Buddy)));
	Service->fetchPersonalInfo();
}

JabberPersonalInfoWidget::~JabberPersonalInfoWidget()
{
}

void JabberPersonalInfoWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	FullName = new QLineEdit(this);
	connect(FullName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	NickName = new QLineEdit(this);
	connect(NickName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	FamilyName = new QLineEdit(this);
	connect(FamilyName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	BirthYear = new QLineEdit(this);
	connect(BirthYear, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));
	BirthYear->setInputMask("d000");

	City = new QLineEdit(this);
	connect(City, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	Email = new QLineEdit(this);
	connect(Email, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	Website = new QLineEdit(this);
	connect(Website, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	layout->addRow(tr("Full name"), FullName);
	layout->addRow(tr("Nick"), NickName);
	layout->addRow(tr("Family name"), FamilyName);
	layout->addRow(tr("Birth year"), BirthYear);
	layout->addRow(tr("City"), City);
	layout->addRow(tr("E-Mail"), Email);
	layout->addRow(tr("Website"), Website);
}

void JabberPersonalInfoWidget::personalInfoAvailable(Buddy buddy)
{
	MyBuddy = buddy;
	fillForm();
}

void JabberPersonalInfoWidget::fillForm()
{
	NickName->setText(MyBuddy.nickName());
	FullName->setText(MyBuddy.firstName());
	FamilyName->setText(MyBuddy.familyName());
	BirthYear->setText(QString::number(MyBuddy.birthYear()));
	City->setText(MyBuddy.city());
	Email->setText(MyBuddy.email());
	Website->setText(MyBuddy.website());
}

bool JabberPersonalInfoWidget::isModified()
{
	return NickName->text() != MyBuddy.nickName()
	|| FullName->text() != MyBuddy.firstName()
	|| FamilyName->text() != MyBuddy.familyName()
	|| BirthYear->text() != QString::number(MyBuddy.birthYear())
	|| City->text() != MyBuddy.city()
	|| Email->text() != MyBuddy.email()
	|| Website->text() != MyBuddy.website();
}

void JabberPersonalInfoWidget::apply()
{
	Buddy buddy = Buddy::create();

	buddy.setNickName((*NickName).text());
	buddy.setFirstName((*FullName).text());
	buddy.setFamilyName((*FamilyName).text());
	buddy.setBirthYear((*BirthYear).text().toUShort());
	buddy.setCity((*City).text());
	buddy.setEmail((*Email).text());
	buddy.setWebsite((*Website).text());

	Service->updatePersonalInfo(buddy);
	MyBuddy = buddy;
}

void JabberPersonalInfoWidget::cancel()
{
	fillForm();
}
