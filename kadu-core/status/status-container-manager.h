/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_CONTAINER_MANAGER_H
#define STATUS_CONTAINER_MANAGER_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-aware-object.h"
#include "status/status-container.h"

#include <stdio.h>

class KADUAPI StatusContainerManager : public StatusContainer, public ConfigurationAwareObject, private AccountsAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(StatusContainerManager)

	static StatusContainerManager *Instance;

	StatusContainerManager();
	virtual ~StatusContainerManager();

	void init();

	QList<StatusContainer *> StatusContainers;

	QString StartupStatus;
	QString StartupDescription;
	bool StartupLastDescription;
	bool OfflineToInvisible;
	bool PrivateStatus;

	bool DisconnectWithCurrentDescription;
	QString DisconnectDescription;

protected:
	virtual void accountRegistered(Account *account);
	virtual void accountUnregistered(Account *account);
	virtual void configurationUpdated();

public:
	static StatusContainerManager * instance();

	unsigned int count() const { return StatusContainers.count(); }
	const QList<StatusContainer *> statusContainers() const { printf("count: %d\n", count()); return StatusContainers; }

	void registerStatusContainer(StatusContainer *statusContainer);
	void unregisterStatusContainer(StatusContainer *statusContainer);

	//StatusContainer Implementation
	virtual QString statusContainerName();

	virtual void setStatus(Status newStatus);
	virtual const Status & status();

	virtual QString statusName();
	virtual QPixmap statusPixmap();
	virtual QPixmap statusPixmap(Status status);
	virtual QPixmap statusPixmap(const QString &statusType);

	virtual QList<StatusType *> supportedStatusTypes();

	virtual int maxDescriptionLength();

	virtual QString statusNamePrefix() { return QString(tr("All") + " "); }

	virtual void setDefaultStatus(const QString &startupStatus, bool offlineToInvisible,
				      const QString &startupDescription, bool StartupLastDescription) {}
	virtual void disconnectAndStoreLastStatus(bool disconnectWithCurrentDescription,
						  const QString &disconnectDescription) {}

	virtual void setPrivateStatus(bool isPrivate);


signals:
	void statusContainerAboutToBeRegistered(StatusContainer *);
	void statusContainerRegistered(StatusContainer *);
	void statusContainerAboutToBeUnregistered(StatusContainer *);
	void statusContainerUnregistered(StatusContainer *);

};

#endif // STATUS_CONTAINER_MANAGER_H
