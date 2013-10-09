/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2011, 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_SET_UP_ACTION_DESCRIPTION_H
#define ENCRYPTION_SET_UP_ACTION_DESCRIPTION_H

#include "gui/actions/action-description.h"

class Chat;

class EncryptionSetUpActionDescription : public ActionDescription
{
	Q_OBJECT

private slots:
	void canEncryptChanged(const Chat &chat);

protected:
	virtual void actionTriggered(QAction *sender, bool toggled);
	virtual QMenu * menuForAction(Action *action);
	virtual void updateActionState(Action *action);

public:
	explicit EncryptionSetUpActionDescription(QObject *parent);
	virtual ~EncryptionSetUpActionDescription();

	virtual QToolButton::ToolButtonPopupMode buttonPopupMode() const;

};

#endif // ENCRYPTION_SET_UP_ACTION_DESCRIPTION_H
