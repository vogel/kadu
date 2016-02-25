/*
 * %kadu copyright begin%
 * Copyright 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "hint-manager.h"

#include "hints-widget-positioner.h"
#include "hints-widget.h"

#include "notification/notification.h"

#include <QtCore/QTimer>

HintManager::HintManager(QObject *parent) :
		QObject{parent},
		Notifier{"Hints", "Hints", KaduIcon{"kadu_icons/notify-hints"}}
{
}

HintManager::~HintManager()
{
}

void HintManager::setHintsWidget(HintsWidget *hintsWidget)
{
	m_hintsWidget = hintsWidget;
}

NotifierConfigurationWidget * HintManager::createConfigurationWidget(QWidget *parent)
{
	Q_UNUSED(parent);

	return nullptr;
}

void HintManager::notify(const Notification &notification)
{
	m_hintsWidget->addNotification(notification);
}

#include "moc_hint-manager.cpp"
