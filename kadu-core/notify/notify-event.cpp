/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify-event.h"

NotifyEvent::NotifyEvent()
{
}

NotifyEvent::NotifyEvent(QString name, QString description) :
		m_name{std::move(name)},
		m_description{std::move(description)}
{
	auto index = m_name.indexOf("/");
	m_category = (index > 0) ? m_name.left(index) : QString();
}

QString NotifyEvent::name() const
{
	return m_name;
}

QString NotifyEvent::category() const
{
	return m_category;
}

QString NotifyEvent::description() const
{
	return m_description;
}

bool operator == (const NotifyEvent &x, const NotifyEvent &y)
{
	return x.name() == y.name();
}
