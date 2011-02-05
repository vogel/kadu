/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CONTACT_SET_CONFIGURATION_HELPER_H
#define CONTACT_SET_CONFIGURATION_HELPER_H

#include "contacts/contact-set.h"

namespace ContactSetConfigurationHelper
{
	ContactSet loadFromConfiguration(StorableObject *parent, const QString &nodeName);
	ContactSet loadFromConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode);
	void saveToConfiguration(StorableObject *parent, const QString &nodeName, const ContactSet &contactSet);
	void saveToConfiguration(XmlConfigFile *configurationStorage, QDomElement contactSetNode, const ContactSet &contactSet);
};

#endif // CONTACT_SET_CONFIGURATION_HELPER_H
