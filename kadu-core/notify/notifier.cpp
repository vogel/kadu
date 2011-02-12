/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "icons-manager.h"

#include "notifier.h"

Notifier::Notifier(const QString &name, const QString &description, const QString &iconPath, QObject *parent) :
		QObject(parent), Name(name), Description(description), IconPath(iconPath)
{
}

Notifier::~Notifier()
{
}

Notifier::CallbackCapacity Notifier::callbackCapacity()
{
	return CallbackNotSupported;
}

QIcon Notifier::icon() const
{
	return IconsManager::instance()->iconByPath(IconPath);
}
