/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class QTimer;

class OtrAppOpsService;
class OtrOpDataFactory;
class OtrUserStateService;

class OtrTimerService : public QObject
{
	Q_OBJECT

private slots:
	void otrTimerTimeout();

public:
	static void wrapperOtrTimerControl(void *data, unsigned int interval);

	Q_INVOKABLE OtrTimerService();
	virtual ~OtrTimerService();

private slots:
	INJEQT_SETTER void setAppOpsService(OtrAppOpsService *appOpsService);
	INJEQT_SETTER void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrAppOpsService> AppOpsService;
	QPointer<OtrOpDataFactory> OpDataFactory;
	QPointer<OtrUserStateService> UserStateService;

	QTimer *Timer;

	void timerControl(unsigned int intervalInSeconds);

};
