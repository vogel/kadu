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

#include "status/storable-status-container.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Account;
class IdentityShared;
class StatusConfigurationHolder;

class IdentityStatusContainer : public StorableStatusContainer
{
	Q_OBJECT

public:
	explicit IdentityStatusContainer(IdentityShared *identityShared);
	virtual ~IdentityStatusContainer();

	virtual QString statusContainerName() override;

	virtual Status status() override;
	virtual void setStatus(Status status, StatusChangeSource source) override;
	virtual bool isStatusSettingInProgress() override;
	virtual int maxDescriptionLength() override;

	virtual KaduIcon statusIcon() override;
	virtual KaduIcon statusIcon(const Status &status) override;

	virtual QList<StatusType> supportedStatusTypes() override;

	void addAccount(const Account &account);
	void removeAccount(const Account &account);

private:
	QPointer<StatusConfigurationHolder> m_statusConfigurationHolder;

	IdentityShared *m_identityShared;
	Status m_lastSetStatus;

private slots:
	INJEQT_SET void setStatusConfigurationHolder(StatusConfigurationHolder *statusConfigurationHolder);

};
