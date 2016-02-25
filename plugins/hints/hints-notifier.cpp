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

#include "hints-notifier.h"

#include "hints-widget.h"

HintsNotifier::HintsNotifier(QObject *parent) :
		QObject{parent},
		Notifier{"Hints", "Hints", KaduIcon{"kadu_icons/notify-hints"}}
{
}

HintsNotifier::~HintsNotifier()
{
}

void HintsNotifier::setHintsWidget(HintsWidget *hintsWidget)
{
	m_hintsWidget = hintsWidget;
}

NotifierConfigurationWidget * HintsNotifier::createConfigurationWidget(QWidget *parent)
{
	Q_UNUSED(parent);

	return nullptr;
}

void HintsNotifier::notify(const Notification &notification)
{
	m_hintsWidget->addNotification(notification);
}

#include "moc_hints-notifier.cpp"
