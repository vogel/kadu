/*
 * %kadu copyright begin%
 * Copyright 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "contacts/model/buddy-contact-model.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-personal-info-service.h"

#include "gadu-contact-personal-info-widget.h"

GaduContactPersonalInfoWidget::GaduContactPersonalInfoWidget(const Contact &contact, QWidget *parent) :
		QWidget(parent), MyContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();

	ContactPersonalInfoService *service = contact.contactAccount().protocolHandler()->contactPersonalInfoService();
	if (!service)
	{
		reset();
		return;
	}

	connect(service, SIGNAL(personalInfoAvailable(Buddy)), this, SLOT(personalInfoAvailable(Buddy)));
	service->fetchPersonalInfo(contact);
}

GaduContactPersonalInfoWidget::~GaduContactPersonalInfoWidget()
{
}

void GaduContactPersonalInfoWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QGroupBox *infoWidget = new QGroupBox(this);
	QFormLayout *infoLayout = new QFormLayout(infoWidget);

	FirstNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("First Name") + ':', infoWidget), FirstNameText);

	LastNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Last Name") + ':', infoWidget), LastNameText);

	NicknameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Nickname") + ':', infoWidget), NicknameText);

	GenderText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Gender") + ':', infoWidget), GenderText);

	BirthdateText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Birthdate") + ':', infoWidget), BirthdateText);

	CityText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("City") + ':', infoWidget), CityText);

	StateProvinceText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("State/Province") + ':', infoWidget), StateProvinceText);

	layout->addWidget(infoWidget);
	layout->addStretch(100);
}

void GaduContactPersonalInfoWidget::reset()
{
	FirstNameText->clear();
	LastNameText->clear();
	NicknameText->clear();
	GenderText->clear();
	BirthdateText->clear();
	CityText->clear();
	StateProvinceText->clear();
}

void GaduContactPersonalInfoWidget::personalInfoAvailable(Buddy buddy)
{
	if (buddy.contacts().isEmpty())
		return;

	Contact contact = buddy.contacts().at(0);

	if (MyContact.id() != contact.id())
		return;

	FirstNameText->setText(buddy.firstName());
	LastNameText->setText(buddy.lastName());
	NicknameText->setText(buddy.nickName());

	switch (buddy.gender())
	{
		case GenderFemale:
			GenderText->setText(tr("Female"));
			break;
		case GenderMale:
			GenderText->setText(tr("Male"));
			break;
		case GenderUnknown:
			GenderText->clear();
			break;
	}

	if (0 != buddy.birthYear())
		BirthdateText->setText(QString::number(buddy.birthYear()));
	else
		BirthdateText->clear();

	CityText->setText(buddy.city());
	StateProvinceText->clear(); // do not have any info, do we need this control anyway?
}

#include "moc_gadu-contact-personal-info-widget.cpp"
