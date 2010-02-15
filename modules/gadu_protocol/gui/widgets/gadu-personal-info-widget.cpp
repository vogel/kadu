/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
		QWidget(parent)
{
	createGui();

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
	FirstName = new QLineEdit(this);
	LastName = new QLineEdit(this);
	Sex = new QComboBox(this);
	Sex->addItem(tr("Unknown Gender"));
	Sex->addItem(tr("Male"));
	Sex->addItem(tr("Female"));
	FamilyName = new QLineEdit(this);
	BirthYear = new QLineEdit(this);
	BirthYear->setInputMask("d000");
	City = new QLineEdit(this);
	FamilyCity = new QLineEdit(this);

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
	kdebugmf (KDEBUG_INFO,"personal info available");
	NickName->setText(buddy.nickName());
	FirstName->setText(buddy.firstName());
	LastName->setText(buddy.lastName());
	Sex->setCurrentIndex((int)buddy.gender());
	FamilyName->setText(buddy.familyName());
	BirthYear->setText(QString::number(buddy.birthYear()));
	City->setText(buddy.city());
	FamilyCity->setText(buddy.familyCity());
}

void GaduPersonalInfoWidget::applyData()
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
}
