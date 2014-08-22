/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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
#include "protocols/protocol.h"
#include "protocols/services/contact-personal-info-service.h"

#include "buddy-personal-info-configuration-widget.h"

BuddyPersonalInfoConfigurationWidget::BuddyPersonalInfoConfigurationWidget(const Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy), InfoWidget(0)
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
	Layout = new QVBoxLayout(this);

	QWidget *contactWidget = new QWidget(this);
	Layout->addWidget(contactWidget);

	QFormLayout *contactLayout = new QFormLayout(contactWidget);

	ContactIdCombo = new QComboBox(contactWidget);
	BuddyContactModel *contactModel = new BuddyContactModel(MyBuddy);
	contactModel->setIncludeIdentityInDisplay(true);
	ContactIdCombo->setModel(contactModel);
	connect(ContactIdCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(accountSelectionChanged(int)));

	contactLayout->addRow(new QLabel(tr("Buddy contact") + ':', contactWidget), ContactIdCombo);

	Layout->addStretch(100);
}

void BuddyPersonalInfoConfigurationWidget::accountSelectionChanged(int index)
{
  	Contact c = ContactIdCombo->model()->index(index, 0).data(ContactRole).value<Contact>();

	if (!c)
		return;

	if (InfoWidget)
	{
		InfoWidget->deleteLater();
		InfoWidget->hide();
	}

	if (!c.contactAccount().protocolHandler())
		return;

	InfoWidget = c.contactAccount().protocolHandler()->protocolFactory()->newContactPersonalInfoWidget(c, this);
	Layout->insertWidget(1, InfoWidget);
}

#include "moc_buddy-personal-info-configuration-widget.cpp"
