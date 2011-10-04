/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef STATUS_SETTER_H
#define STATUS_SETTER_H

#include "configuration/configuration-aware-object.h"
#include "status/status-container-aware-object.h"

class StatusSetter : private StatusContainerAwareObject, private ConfigurationAwareObject
{
	Q_DISABLE_COPY(StatusSetter)

	bool CoreInitialized;
	QString StartupStatus;
	QString StartupDescription;
	bool StartupLastDescription;
	bool OfflineToInvisible;

	static StatusSetter *Instance;

	void setDefaultStatus(StatusContainer *statusContainer);

	StatusSetter();
	virtual ~StatusSetter();

protected:
	void configurationUpdated();

	void statusContainerRegistered(StatusContainer *statusContainer);
	void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	static StatusSetter * instance();

	void coreInitialized();

	void setStatus(StatusContainer *statusContainer, Status status);
	Status manuallySetStatus(StatusContainer *statusContainer);

};

#endif // STATUS_SETTER_H
