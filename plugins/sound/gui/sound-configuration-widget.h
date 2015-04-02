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

#include "gui/widgets/configuration/notifier-configuration-widget.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>

class QLabel;

class SoundManager;
class SoundSelectFile;

class SoundConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

public:
	explicit SoundConfigurationWidget(SoundManager *manager, QWidget *parent = nullptr);
	virtual ~SoundConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);

public slots:
	void themeChanged(int index);

signals:
	void soundFileEdited();

private:
	QPointer<SoundManager> m_manager;

	QMap<QString, QString> m_soundFiles;
	QString m_currentNotificationEvent;

	SoundSelectFile *m_soundSelectFile;

};
