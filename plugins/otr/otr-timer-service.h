/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_TIMER_SERVICE_H
#define OTR_TIMER_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class QTimer;

class OtrAppOpsService;
class OtrOpDataFactory;
class OtrUserStateService;

class OtrTimerService : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrAppOpsService> AppOpsService;
	QWeakPointer<OtrOpDataFactory> OpDataFactory;
	QWeakPointer<OtrUserStateService> UserStateService;

	QTimer *Timer;

	void timerControl(int intervalInSeconds);

private slots:
	void otrTimerTimeout();

public:
	static void wrapperOtrTimerControl(void *data, unsigned int interval);

	explicit OtrTimerService(QObject *parent = 0);
	virtual ~OtrTimerService();

	void setAppOpsService(OtrAppOpsService *appOpsService);
	void setOpDataFactory(OtrOpDataFactory *opDataFactory);
	void setUserStateService(OtrUserStateService *userStateService);

};

#endif // OTR_TIMER_SERVICE_H
