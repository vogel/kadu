/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

JabberPersonalInfoWidget::~JabberPersonalInfoWidget()
{
}


void JabberPersonalInfoWidget::createGui()
{
	QFormLayout *layout = new QFormLayout(this);

	FullName = new QLineEdit(this);
	NickName = new QLineEdit(this);
	FamilyName = new QLineEdit(this);
	BirthYear = new QLineEdit(this);
	City = new QLineEdit(this);
	Email = new QLineEdit(this);
	Website = new QLineEdit(this);

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
	NickName->setText(buddy.nickName());
	FullName->setText(buddy.firstName());
	FamilyName->setText(buddy.familyName());
	BirthYear->setText(QString::number(buddy.birthYear()));
	City->setText(buddy.city());
	Email->setText(buddy.email());
	Website->setText(buddy.website());
}
