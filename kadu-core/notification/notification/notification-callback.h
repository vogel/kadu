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

#pragma once

#include "exports.h"

#include <QtCore/QString>
#include <functional>

class Notification;

class KADUAPI NotificationCallback
{

public:
	NotificationCallback();
	explicit NotificationCallback(QString name, QString title, std::function<void(Notification *)> callback);

	QString name() const;
	QString title() const;
	std::function<void(Notification *)> callback() const;

	void call(Notification *notification) const;

private:
	QString m_name;
	QString m_title;
	std::function<void(Notification *)> m_callback;

};

bool operator == (const NotificationCallback &x, const NotificationCallback &y);
