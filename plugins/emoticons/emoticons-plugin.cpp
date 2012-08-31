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

#include "emoticons-manager.h"

#include "emoticons-plugin.h"

EmoticonsPlugin::EmoticonsPlugin(QObject *parent) :
		QObject(parent)
{
}

EmoticonsPlugin::~EmoticonsPlugin()
{
}

int EmoticonsPlugin::init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	Manager.reset(new EmoticonsManager());

	return 0;
}

void EmoticonsPlugin::done()
{
	Manager.reset(0);
}

Q_EXPORT_PLUGIN2(emoticons, EmoticonsPlugin)
