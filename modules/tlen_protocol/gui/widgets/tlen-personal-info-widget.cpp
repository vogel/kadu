/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "buddies/buddy-shared.h"
#include "protocols/services/personal-info-service.h"
#include "debug.h"

#include "tlen-account-details.h"
#include "tlen-contact-details.h"

#include "tlen-personal-info-widget.h"

TlenPersonalInfoWidget::TlenPersonalInfoWidget(Account account, QWidget* parent) :
		QWidget(parent), TAccount(account), FetchOk(false)
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

TlenPersonalInfoWidget::~TlenPersonalInfoWidget()
{
}


void TlenPersonalInfoWidget::createGui()
{
	QGridLayout *layout = new QGridLayout(this);

	QLabel *nickNameLabel = new QLabel(tr("Nick"), this);
	QLabel *firstNameLabel = new QLabel(tr("First name"), this);
	QLabel *lastNameLabel = new QLabel(tr("Last name"), this);
	QLabel *sexLabel = new QLabel(tr("Sex"), this);
	QLabel *birthYearLabel = new QLabel(tr("Birth year"), this);
	QLabel *cityLabel = new QLabel(tr("City"), this);
	QLabel *lookingForLabel = new QLabel(tr("Looking for"), this);
	QLabel *jobLabel = new QLabel(tr("Job"), this);
	QLabel *todayPlansLabel = new QLabel(tr("Today plans"), this);
	QLabel *showStatusLabel = new QLabel(tr("Show status"), this);
	QLabel *haveMicLabel = new QLabel(tr("Have mic"), this);
	QLabel *haveCamLabel = new QLabel(tr("Have cam"), this);
	QLabel *emailLabel = new QLabel(tr("e-mail"), this);

	NickName = new QLineEdit(this);
	FirstName = new QLineEdit(this);
	LastName = new QLineEdit(this);
	Sex = new QComboBox(this);
	Sex->addItem(tr("Unknown gender"));
	Sex->addItem(tr("Male"));
	Sex->addItem(tr("Female"));
	BirthYear = new QLineEdit(this);
	BirthYear->setInputMask("d000");
	City = new QLineEdit(this);
	LookingFor = new QComboBox(this);
	LookingFor->addItem(tr("Unknown"));
	LookingFor->addItem(tr("1"));
	LookingFor->addItem(tr("2"));
	LookingFor->addItem(tr("3"));
	LookingFor->addItem(tr("4"));
	Job = new QComboBox(this);
	Job->addItem(tr("Unknown"));
	Job->addItem(tr("1"));
	Job->addItem(tr("2"));
	Job->addItem(tr("3"));
	Job->addItem(tr("4"));
	TodayPlans = new QComboBox(this);
	TodayPlans->addItem(tr("Unknown"));
	TodayPlans->addItem(tr("1"));
	TodayPlans->addItem(tr("2"));
	TodayPlans->addItem(tr("3"));
	TodayPlans->addItem(tr("4"));
	ShowStatus = new QCheckBox(this);
	HaveMic = new QCheckBox(this);
	HaveCam = new QCheckBox(this);
	EMail = new QLineEdit(this);

	layout->addWidget(nickNameLabel, 0, 0);
	layout->addWidget(NickName, 1, 0);
	layout->addWidget(firstNameLabel, 0, 1);
	layout->addWidget(FirstName, 1, 1);
	layout->addWidget(lastNameLabel, 0, 2);
	layout->addWidget(LastName, 1, 2);

	layout->addWidget(sexLabel, 2, 0);
	layout->addWidget(Sex, 3, 0);
	layout->addWidget(birthYearLabel, 2, 1);
	layout->addWidget(BirthYear, 3, 1);
	layout->addWidget(cityLabel, 2, 2);
	layout->addWidget(City, 3, 2);

	layout->addWidget(lookingForLabel, 4, 0);
	layout->addWidget(LookingFor, 5, 0);
	layout->addWidget(jobLabel, 4, 1);
	layout->addWidget(Job, 5, 1);
	layout->addWidget(todayPlansLabel, 4, 2);
	layout->addWidget(TodayPlans, 5, 2);

	layout->addWidget(showStatusLabel, 6, 0);
	layout->addWidget(ShowStatus, 7, 0);
	layout->addWidget(haveMicLabel, 6, 1);
	layout->addWidget(HaveMic, 7, 1);
	layout->addWidget(haveCamLabel, 6, 2);
	layout->addWidget(HaveCam, 7, 2);
	layout->addWidget(emailLabel, 8, 1);
	layout->addWidget(EMail, 9, 1);

	layout->setRowStretch(11, 100);

}

void TlenPersonalInfoWidget::personalInfoAvailable(Buddy buddy)
{
	kdebugmf (KDEBUG_INFO,"personal info available");
	NickName->setText(buddy.nickName());
	FirstName->setText(buddy.firstName());
	LastName->setText(buddy.lastName());
	Sex->setCurrentIndex((int)buddy.gender());
	BirthYear->setText(QString::number(buddy.birthYear()));
	City->setText(buddy.city());
	EMail->setText(buddy.email());

	// TODO 0.6.6: dangerous - no details means not this account?
	FetchOk = false;

	// always one contact
	Contact contact = buddy.contacts()[0];
	if (contact.isNull())
		return;

	TlenContactDetails *tlenDetails = dynamic_cast<TlenContactDetails *>(contact.details());
	if (!tlenDetails)
		return;

	LookingFor->setCurrentIndex((int) tlenDetails->lookingFor());
	Job->setCurrentIndex((int) tlenDetails->job());
	TodayPlans->setCurrentIndex((int) tlenDetails->todayPlans());

	ShowStatus->setChecked(tlenDetails->showStatus());
	HaveMic->setChecked(tlenDetails->haveMic());
	HaveCam->setChecked(tlenDetails->haveCam());
	FetchOk = true;
}

void TlenPersonalInfoWidget::applyData()
{
	if (!FetchOk)
		return;

	Buddy buddy = Buddy::create();

	buddy.setNickName((*NickName).text());
	buddy.setFirstName((*FirstName).text());
	buddy.setLastName((*LastName).text());
	buddy.setBirthYear((*BirthYear).text().toUShort());
	buddy.setCity((*City).text());
	buddy.setGender((BuddyGender)Sex->currentIndex());
	buddy.setEmail((*EMail).text());

	Contact contact = Contact::create();
	contact.setContactAccount(TAccount);
	contact.setOwnerBuddy(buddy);
	contact.setId(TAccount.id());

	TlenContactDetails *tlenDetails = new TlenContactDetails(contact);
	tlenDetails->setState(StorableObject::StateNew);
	contact.setDetails(tlenDetails);

	tlenDetails->setLookingFor(LookingFor->currentIndex());
	tlenDetails->setJob(Job->currentIndex());
	tlenDetails->setTodayPlans(TodayPlans->currentIndex());
	tlenDetails->setShowStatus(ShowStatus->isChecked());
	tlenDetails->setHaveMic(HaveMic->isChecked());
	tlenDetails->setHaveCam(HaveCam->isChecked());

	Service->updatePersonalInfo(buddy);
}
