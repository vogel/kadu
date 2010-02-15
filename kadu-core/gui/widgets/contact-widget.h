/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CONTACT_WIDGET_H
#define CONTACT_WIDGET_H

#include <QtGui/QDialog>
#include <QtGui/QLineEdit>

#include "contacts/contact.h"
#include "gui/widgets/configuration/configuration-widget.h"

#include "exports.h"

class ConfigurationContactDataManager;

class KADUAPI ContactWidget : public ConfigurationWidget
{
	Q_OBJECT

	Contact Data;
	void createGui();

public:
	explicit ContactWidget(ConfigurationContactDataManager *dataManager, Contact contact, QWidget *parent = 0);
	virtual ~ContactWidget();

};

#endif // CONTACT_WIDGET_H
