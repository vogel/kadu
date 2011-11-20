/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef CHAT_EDIT_BOX_SIZE_MANAGER_H
#define CHAT_EDIT_BOX_SIZE_MANAGER_H

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"
#include "exports.h"

class KADUAPI ChatEditBoxSizeManager : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatEditBoxSizeManager)

	static ChatEditBoxSizeManager *Instance;

	int CommonHeight;

	ChatEditBoxSizeManager();
	virtual ~ChatEditBoxSizeManager();

protected:
	virtual void configurationUpdated();

public:
	static ChatEditBoxSizeManager * instance();

	void setCommonHeight(int height);
	int commonHeight() { return CommonHeight; }

	bool initialized();

signals:
	void commonHeightChanged(int height);

};

#endif // CHAT_EDIT_BOX_SIZE_MANAGER_H
