/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-aware-object.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;

class KADUAPI NotificationConfiguration : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit NotificationConfiguration(QObject *parent = nullptr);
	virtual ~NotificationConfiguration();

	QString notifyConfigurationKey(const QString &eventType) const;

	bool notifyIgnoreOnConnection() const { return m_notifyIgnoreOnConnection; }
	bool ignoreOnlineToOnline() const { return m_ignoreOnlineToOnline; }
	bool newMessageOnlyIfInactive() const { return m_newMessageOnlyIfInactive; }
	bool silentModeWhenFullscreen() const { return m_silentModeWhenFullscreen; }
	void setSilentMode(bool silentMode);
	bool silentMode() const { return m_silentMode; }
	bool silentModeWhenDnD() const { return m_silentModeWhenDnD; }

signals:
	void notificationConfigurationUpdated();

protected:
	virtual void configurationUpdated() override;

private:
	QPointer<Configuration> m_configuration;

	bool m_newMessageOnlyIfInactive;
	bool m_notifyIgnoreOnConnection;
	bool m_ignoreOnlineToOnline;
	bool m_silentMode;
	bool m_silentModeWhenDnD;
	bool m_silentModeWhenFullscreen;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_INIT void init();

};
