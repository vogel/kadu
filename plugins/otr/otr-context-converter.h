/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_CONTEXT_CONVERTER_H
#define OTR_CONTEXT_CONVERTER_H

extern "C" {
#	include <libotr/context.h>
}

#include <QtCore/QObject>

class Chat;
class Contact;

class OtrContextConverter : public QObject
{
	Q_OBJECT

public:
	Chat connectionContextToChat(ConnContext *context) const;
	Contact connectionContextToContact(ConnContext *context) const;

};

#endif // OTR_CONTEXT_CONVERTER_H
