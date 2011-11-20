/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "contacts/contact-manager.h"
#include "contacts/contact.h"
#include "contacts/model/buddy-contact-model.h"
#include "misc/misc.h"
#include "model/roles.h"
#include "os/generic/url-opener.h"
#include "protocols/protocol.h"
#include "protocols/services/contact-personal-info-service.h"

#include "jabber-contact-personal-info-widget.h"

JabberContactPersonalInfoWidget::JabberContactPersonalInfoWidget(Contact &contact, QWidget *parent) :
		QWidget(parent), MyContact(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	reset();

	ContactPersonalInfoService *service = contact.contactAccount().protocolHandler()->contactPersonalInfoService();
	if (!service)
		return;

	connect(service, SIGNAL(personalInfoAvailable(Buddy)), this, SLOT(personalInfoAvailable(Buddy)));

	Buddy b = Buddy::create();
	Contact c = Contact::create();
	c.setId(contact.id());
	c.setOwnerBuddy(b);

	service->fetchPersonalInfo(c);
}

JabberContactPersonalInfoWidget::~JabberContactPersonalInfoWidget()
{
}

void JabberContactPersonalInfoWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QGroupBox *infoWidget = new QGroupBox(this);
	QFormLayout *infoLayout = new QFormLayout(infoWidget);

	FullNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Full Name") + ':', infoWidget), FullNameText);

	FamilyNameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Family Name") + ':', infoWidget), FamilyNameText);

	NicknameText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Nickname") + ':', infoWidget), NicknameText);

	BirthdateText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Birthdate") + ':', infoWidget), BirthdateText);

	CityText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("City") + ':', infoWidget), CityText);

	EmailText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("E-Mail") + ':', infoWidget), EmailText);
	connect(EmailText, SIGNAL(linkActivated(const QString &)), this, SLOT(urlClicked(const QString &)));

	WebsiteText = new QLabel(this);
	infoLayout->addRow(new QLabel(tr("Website") + ':', infoWidget), WebsiteText);
	connect(WebsiteText, SIGNAL(linkActivated(const QString &)), this, SLOT(urlClicked(const QString &)));

	layout->addWidget(infoWidget);
	layout->addStretch(100);
}

void JabberContactPersonalInfoWidget::reset()
{
	FullNameText->clear();
	FamilyNameText->clear();
	NicknameText->clear();
	BirthdateText->clear();
	CityText->clear();
	EmailText->clear();
	WebsiteText->clear();
}

void JabberContactPersonalInfoWidget::personalInfoAvailable(Buddy buddy)
{
	FullNameText->setText(buddy.firstName());
	FamilyNameText->setText(buddy.familyName());
	NicknameText->setText(buddy.nickName());

	if (0 != buddy.birthYear())
		BirthdateText->setText(QString::number(buddy.birthYear()));
	else
		BirthdateText->clear();

	CityText->setText(buddy.city());
	EmailText->setText(QString("<a href=\"mailto:%1\">%1</a>").arg(buddy.email()));
	WebsiteText->setText(QString("<a href=\"%1\">%1</a>").arg(buddy.website()));
}

void JabberContactPersonalInfoWidget::urlClicked(const QString &link)
{
	UrlOpener::openUrl(link.toUtf8());
}
