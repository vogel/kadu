/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QTimer>

#include "status/status-container.h"

#include "status-icon.h"

StatusIcon::StatusIcon(StatusContainer *statusContainer, QObject *parent) :
		QObject(parent), MyStatusContainer(statusContainer), BlinkTimer(0), BlinkOffline(true)
{
	statusUpdated();
	connect(MyStatusContainer, SIGNAL(statusUpdated(StatusContainer *)), this, SLOT(statusUpdated(StatusContainer *)));
}

StatusIcon::~StatusIcon()
{
}

void StatusIcon::enableBlink()
{
	if (BlinkTimer)
		return;

	BlinkTimer = new QTimer(this);
	connect(BlinkTimer, SIGNAL(timeout()), this, SLOT(blink()));
	BlinkTimer->start(500);
}

void StatusIcon::disableBlink()
{
	if (!BlinkTimer)
		return;

	delete BlinkTimer;
	BlinkTimer = 0;

	setIcon(MyStatusContainer->statusIcon());
}

void StatusIcon::blink()
{
	if (!MyStatusContainer->isStatusSettingInProgress())
	{
		disableBlink();
		return;
	}

	BlinkOffline = !BlinkOffline;

	if (BlinkOffline)
		setIcon(MyStatusContainer->statusIcon(StatusTypeOffline));
	else
		setIcon(MyStatusContainer->statusIcon());
}

void StatusIcon::updateStatus()
{
	if (!MyStatusContainer->isStatusSettingInProgress())
		setIcon(MyStatusContainer->statusIcon());
	else
		enableBlink();
}

void StatusIcon::statusUpdated(StatusContainer *container)
{
	Q_UNUSED(container)

	updateStatus();
}

void StatusIcon::configurationUpdated()
{
	updateStatus();
}

void StatusIcon::setIcon(const KaduIcon &icon)
{
	Icon = icon;
	emit iconUpdated(Icon);
}

#include "moc_status-icon.cpp"
