 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

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
	QGridLayout *layout = new QGridLayout(this);

	QLabel *nickNameLabel = new QLabel(tr("Nick"), this);
	QLabel *firstNameLabel = new QLabel(tr("First name"), this);
	QLabel *lastNameLabel = new QLabel(tr("Last name"), this);
	QLabel *sexLabel = new QLabel(tr("Sex"), this);
	QLabel *familyNameLabel = new QLabel(tr("Family name"), this);
	QLabel *birthYearLabel = new QLabel(tr("Birth year"), this);
	QLabel *cityLabel = new QLabel(tr("City"), this);
	QLabel *familyCityLabel = new QLabel(tr("Family city"), this);

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

	layout->addWidget(nickNameLabel, 0, 0);
	layout->addWidget(NickName, 1, 0);
	layout->addWidget(firstNameLabel, 0, 1);
	layout->addWidget(FirstName, 1, 1);
	layout->addWidget(lastNameLabel, 0, 2);
	layout->addWidget(LastName, 1, 2);
	layout->addWidget(sexLabel, 2, 0);
	layout->addWidget(Sex, 3, 0);
	layout->addWidget(familyNameLabel, 2, 1);
	layout->addWidget(FamilyName, 3, 1);
	layout->addWidget(birthYearLabel, 4, 0);
	layout->addWidget(BirthYear, 5, 0);
	layout->addWidget(cityLabel, 6, 0);
	layout->addWidget(City, 7, 0);
	layout->addWidget(familyCityLabel, 6, 1);
	layout->addWidget(FamilyCity, 7, 1);
	
	layout->setRowStretch(8, 100);
	
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
	buddy.setGender((BuddyShared::BuddyGender)Sex->currentIndex());

	Service->updatePersonalInfo(buddy);
}
