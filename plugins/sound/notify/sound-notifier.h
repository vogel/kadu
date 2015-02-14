/*
 * %kadu copyright begin%
 * Copyright 2008, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#include "notify/notifier.h"

#include <QtCore/QPointer>

class SoundConfigurationUiHandler;
class SoundManager;

class SoundNotifier : public Notifier
{
	Q_OBJECT

	static SoundNotifier *Instance;
	QPointer<SoundConfigurationUiHandler> m_configurationUiHandler;
	QPointer<SoundManager> m_manager;

public:
	explicit SoundNotifier(QObject *parent = nullptr);
	virtual ~SoundNotifier();

	void setConfigurationUiHandler(SoundConfigurationUiHandler *configurationUiHandler);
	void setManager(SoundManager *manager);

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr) override;
	virtual void notify(Notification *notification) override;

};
