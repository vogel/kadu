/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "accounts/account-shared.h"

#include "debug.h"
#include "gadu-account-details.h"

#include "gadu-personal-info-widget.h"

GaduPersonalInfoWidget::GaduPersonalInfoWidget(Account account, QWidget* parent) :
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

GaduPersonalInfoWidget::~GaduPersonalInfoWidget()
{
}

void GaduPersonalInfoWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	NickName = new QLineEdit(this);
	connect(NickName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	FirstName = new QLineEdit(this);
	connect(FirstName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	LastName = new QLineEdit(this);
	connect(LastName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	Sex = new QComboBox(this);
	connect(Sex, SIGNAL(currentIndexChanged(int)), this, SIGNAL(dataChanged()));
	Sex->addItem(tr("Unknown Gender"));
	Sex->addItem(tr("Female"));
	Sex->addItem(tr("Male"));

	FamilyName = new QLineEdit(this);
	connect(FamilyName, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	BirthYear = new QLineEdit(this);
	connect(BirthYear, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));
	BirthYear->setInputMask("d000");

	City = new QLineEdit(this);
	connect(City, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	FamilyCity = new QLineEdit(this);
	connect(FamilyCity, SIGNAL(textChanged(QString)), this, SIGNAL(dataChanged()));

	layout->addRow(tr("Nick"), NickName);
	layout->addRow(tr("First name"), FirstName);
	layout->addRow(tr("Last name"), LastName);
	layout->addRow(tr("Sex"), Sex);
	layout->addRow(tr("Family name"), FamilyName);
	layout->addRow(tr("Birth year"), BirthYear);
	layout->addRow(tr("City"), City);
	layout->addRow(tr("Family city"), FamilyCity);
}

void GaduPersonalInfoWidget::personalInfoAvailable(Buddy buddy)
{
	kdebugmf (KDEBUG_INFO,"personal info available\n");
	MyBuddy = buddy;
	fillForm();
}

void GaduPersonalInfoWidget::fillForm()
{
 	NickName->setText(MyBuddy.nickName());
	FirstName->setText(MyBuddy.firstName());
	LastName->setText(MyBuddy.lastName());
	Sex->setCurrentIndex((int)MyBuddy.gender());
	FamilyName->setText(MyBuddy.familyName());
	BirthYear->setText(QString::number(MyBuddy.birthYear()));
	City->setText(MyBuddy.city());
	FamilyCity->setText(MyBuddy.familyCity());
}

bool GaduPersonalInfoWidget::isModified()
{
	return NickName->text() != MyBuddy.nickName()
	|| FirstName->text() != MyBuddy.firstName()
	|| LastName->text() != MyBuddy.lastName()
	|| Sex->currentIndex() != (int)MyBuddy.gender()
	|| FamilyName->text() != MyBuddy.familyName()
	|| BirthYear->text() != QString::number(MyBuddy.birthYear())
	|| City->text() != MyBuddy.city()
	|| FamilyCity->text() != MyBuddy.familyCity();
}

void GaduPersonalInfoWidget::apply()
{
	Buddy buddy = Buddy::create();

	buddy.setNickName((*NickName).text());
	buddy.setFirstName((*FirstName).text());
	buddy.setLastName((*LastName).text());
	buddy.setFamilyName((*FamilyName).text());
	buddy.setBirthYear((*BirthYear).text().toUShort());
	buddy.setCity((*City).text());
	buddy.setFamilyCity((*FamilyCity).text());
	buddy.setGender((BuddyGender)Sex->currentIndex());

	Service->updatePersonalInfo(buddy);
	MyBuddy = buddy;
}

void GaduPersonalInfoWidget::cancel()
{
	fillForm();
}
