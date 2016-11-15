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

#include "gui/actions/action-description.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Contact;
class Myself;
class SubscriptionService;

class SubscriptionAction : public ActionDescription
{
	Q_OBJECT

public:
	explicit SubscriptionAction(QObject *parent = nullptr);
	virtual ~SubscriptionAction();

protected:
	virtual void actionTriggered(QAction *sender, bool toggled) override;
	virtual void updateActionState(Action *action) override;
	virtual void execute(SubscriptionService *subscriptionService, const Contact &contact) = 0;

	Contact contactFromAction(QAction *action);
	SubscriptionService * subscriptionServiceFromContact(const Contact &contact);

private:
	QPointer<Myself> m_myself;

private slots:
	INJEQT_SET void setMyself(Myself *myself);

};
