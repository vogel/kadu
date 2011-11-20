/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef SEND_PUBLIC_KEY_ACTION_DESCRIPTION_H
#define SEND_PUBLIC_KEY_ACTION_DESCRIPTION_H

#include "gui/actions/action-description.h"

class Contact;

class SendPublicKeyActionDescription : public ActionDescription
{
	Q_OBJECT

	void sendPublicKey(const Contact &contact);

protected:
	virtual void actionTriggered(QAction *sender, bool toggled);
	virtual void updateActionState(Action *action);

public:
	explicit SendPublicKeyActionDescription(QObject *parent);
	virtual ~SendPublicKeyActionDescription();

};

#endif // SEND_PUBLIC_KEY_ACTION_DESCRIPTION_H
