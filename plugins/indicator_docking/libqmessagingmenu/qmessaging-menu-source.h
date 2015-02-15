/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <QtCore/QObject>

class QMessagingMenuApp;

class QMessagingMenuSource : public QObject
{
	Q_OBJECT

public:
	explicit QMessagingMenuSource(const QString &id, const QString &label, const QString &icon, QMessagingMenuApp *menuApp);
	explicit QMessagingMenuSource(const QString &id, const QString &label, const QString &icon, int count, QMessagingMenuApp *menuApp);
	virtual ~QMessagingMenuSource();

	void setLabel(const QString &label);
	void setIcon(const QString &icon);
	void setCount(int count);

	void drawAttention();
	void removeAttention();

private:
	QMessagingMenuApp *m_menuApp;
	QString m_id;

};
