/*
 * %kadu copyright begin%
 * Copyright 2011, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification/notifier.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class SoundConfigurationUiHandler;
class SoundManager;

class SoundNotifier : public Notifier
{
	Q_OBJECT

	static SoundNotifier *Instance;
	QPointer<SoundConfigurationUiHandler> m_soundConfigurationUiHandler;
	QPointer<SoundManager> m_soundManager;

public:
	Q_INVOKABLE explicit SoundNotifier(QObject *parent = nullptr);
	virtual ~SoundNotifier();

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr) override;
	virtual void notify(Notification *notification) override;

private slots:
	INJEQT_SETTER void setSoundConfigurationUiHandler(SoundConfigurationUiHandler *soundConfigurationUiHandler);
	INJEQT_SETTER void setSoundManager(SoundManager *soundManager);

};
