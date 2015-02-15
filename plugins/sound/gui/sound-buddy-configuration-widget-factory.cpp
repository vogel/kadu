/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sound-buddy-configuration-widget-factory.h"

#include "gui/sound-buddy-configuration-widget.h"
#include "sound-manager.h"

SoundBuddyConfigurationWidgetFactory::SoundBuddyConfigurationWidgetFactory(QObject *parent) :
		QObject{parent}
{
}

SoundBuddyConfigurationWidgetFactory::~SoundBuddyConfigurationWidgetFactory()
{
}

void SoundBuddyConfigurationWidgetFactory::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

BuddyConfigurationWidget * SoundBuddyConfigurationWidgetFactory::createWidget(const Buddy &buddy, QWidget *parent)
{
	return new SoundBuddyConfigurationWidget{buddy, m_soundManager, parent};
}
