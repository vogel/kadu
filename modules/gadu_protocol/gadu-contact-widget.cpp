/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

#include "gadu-contact-details.h"
#include "gadu-contact-data-manager.h"
#include "misc/misc.h"

#include "gadu-contact-widget.h"

GaduContactWidget::GaduContactWidget(Contact contact, QWidget *parent) :
		ContactWidget(new GaduContactDataManager(contact, parent),
			contact, parent),
		Data(contact)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

GaduContactWidget::~GaduContactWidget()
{
}

void GaduContactWidget::createGui()
{
	appendUiFile(dataPath("kadu/modules/configuration/gadu_contact.ui"));
}
