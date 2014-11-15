/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "buddies/buddy.h"
#include "contacts/contact.h"
#include "protocols/services/account-service.h"
#include "roster/roster-task.h"

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QQueue>

class Protocol;

/**
 * @addtogroup Protocol
 * @{
 */

/**
 * @class RosterService
 * @author Rafał 'Vogel' Malinowski
 * @short Roster protocol service allows adding, removing and updating contacts on remote roster.
 *
 * This service allows adding, removing and updating contacts on remote roster. Every added contact is watched
 * for changes and updated automatically, until it is removed.
 *
 * If an action cannot be executed immediately it is stored as @link RosterTask @endlink object for later execution.
 * List of tasks is available by calling tasks() getter and can be update by addTasks(). This allows for storing
 * and restoring this list between program invocations. Only one @link RosterTask @endlink for each contact id can be on
 * the list at a time. This service is responsible of choosing which task should be left on the list and which one should
 * be removed in case when second one is added for given id.
 *
 * When receiving updates from the remote roster, list of tasks is checked for id of contact from remote roster. Requests for
 * contact removal or update from remote roster will be ignored if an update or addition task is on the task list. This service
 * assumes that its changes are more important that these of remote roster. Update requests are ignored if Detached flag
 * of @link RosterEntry @endlink of given contact is set to true.
 *
 * At the beginning of roster initialization all contacts of service's account that do not have any tasks are marked as deleted by
 * remote roster. During initialization deletion mark is removed from contacts that have data on remote roster. After
 * initialization the rest of contacts is removed from local roster. In this case Detached flag of @link RosterEntry @endlink
 * does not count as it is only used for detaching from data synchronization.
 */
class KADUAPI RosterService : public AccountService
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of RosterService bound to given Account.
	 * @param account to bound this service to
	 * @param contacts initial list of account contacts that should be stored in Roster
	 */
	explicit RosterService(Account account, const QVector<Contact> &contacts, QObject *parent = nullptr);
	virtual ~RosterService();

	/**
	 * @short Set protocol to use by this service.
	 * @todo Switch to ConnectionService?
	 * @author Rafał 'Vogel' Malinowski
	 * @param protocol protocol to bound this service to
	 */
	void setProtocol(Protocol *protocol);

	/**
	 * @short Return true if protocol supports concept of eoster tasks.
	 * 
	 * GaduGadu does not support this - all roster changes are done in one go, and it may be impossible
	 * to split them in tasks. In Gadu if we have the same version of contact list as server then it can
	 * be safely updated in one go. If not - server version is merged with local one. That means local
	 * deletion may be reverted.
	 * 
	 * XMPP fully supports roster task.
	 */
	virtual bool supportsTasks() const = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return list of current non-executed roster tasks.
	 * @return list of current non-executed roster tasks
	 */
	QVector<RosterTask> tasks();

	/**
	 * @short Add tasks for later execution.
	 * @param tasks list of tasks
	 * @see addTask(const RosterTask &)
	 */
	void addTasks(const QVector<RosterTask> &tasks);

	const QVector<Contact> & contacts() const;

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new contact to roster.
	 * @param contact new contact
	 *
	 * If given contact is valid and belongs to current account, then new roster task for adding item to roster is created.
	 * If it is possible, task is executed immediately. If not, it is stored for later execution.
	 */
	void addContact(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Remove contact from roster.
	 * @param contact contact to remove
	 *
	 * If given contact is valid and belongs to current account, then new roster task for deleting item from roster is created.
	 * If it is possible, task is executed immediately. If not, it is stored for later execution.
	 */
	void removeContact(const Contact &contact);

signals:
	/**
	 * @short Signal emitted when new contact is added to roster
	 * @param contact added contact
	 * 
	 * Signal is not emitted for contacts added as initial in constructor.
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactAdded(Contact contact);

	/**
	 * @short Signal emitted when contact is removed from roster
	 * @param contact removed contact
	 *
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactRemoved(Contact contact);

	/**
	 * @short Signal emitted when contact is updated on roster
	 * @param contact updated contact
	 * 
	 * Signal is emitted for detached and attached contacts.
	 */
	void contactUpdated(Contact contact);

	/**
	 * @short Signal emitted when any task is added to list of tasks.
	 */
	void taskAdded();

protected:
	virtual bool canPerformLocalUpdate() const;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return type of task for given id.
	 * @param id id of contact to check task for
	 * @return type of task for given id
	 *
	 * If no task for given id is found then RosterTaskType::None is returned.
	 */
	RosterTaskType taskType(const QString &id);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Execute given roster task.
	 * @param task to execute
	 */
	virtual void executeTask(const RosterTask &task) = 0;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Execute all stored RosterTasks.
	 *
	 * This method executes all stored tasks. List of not-executed tasks will be empty after this call.
	 */
	void executeAllTasks();

	/**
	 * @short Resets all synchronizing contacts to desynchronized.
	 * 
	 * Should be used just after connection has been made. In case when synchronization operation was interrupted
	 * and roster entry can be lest in synchronizing state. This method fixes that by resetting it to desynchronized.
	 */
	void resetSynchronizingToDesynchronized();

	/**
	 * @return List of tasks required to update @p contacts
	 * 
	 * For each contact that has a RosterEntry and is in desynchronized state a new RosterTask with Update mode is returned.
	 */
	static QVector<RosterTask> updateTasksForContacts(const QVector<Contact> &contacts);

	/**
	 * @return List of tasks required to update known contacts.
	 * 
	 * For each known contact that has a RosterEntry and is in desynchronized state a new RosterTask with Update mode is returned.
	 */
	QVector<RosterTask> updateTasksForContacts() const;

	Protocol * protocol() const;

	bool containsTask(const QString &id) const;

protected slots:
	/**
	 * @short Add new synchronized contact to be watched.
	 * @param contact new contact
	 *
	 * If given contact is valid and belongs to current account, then roster service will watch for changes of
	 * its synchronization state. After adding its synchronization state is set to Synchronized.
	 */
	void addSynchronizedContact(const Contact &contact);

private:
	QPointer<Protocol> m_protocol;

	QVector<Contact> m_contacts;
	QQueue<RosterTask> m_tasks;
	QMap<QString, RosterTask> m_idToTask;

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Add new task for later execution.
	 * @param task new task
	 *
	 * If existing task for given id is available then this service decides which one to use and which one to ignore.
	 * If existing task is deletion, then it is always replaced. Addition task can be only replaced by deletion task. Update task
	 * can be replaced by any non-update task.
	 */
	void addTask(const RosterTask &task);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Return true if task of second type should replace task of first type on list of task to execute.
	 * @return true if task of second type should replace task of first type on list of task to execute
	 */
	bool shouldReplaceTask(RosterTaskType taskType, RosterTaskType replacementType);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Connects to data emitted by given contact when its roster data changes.
	 * @param contact contact to connect to
	 */
	void connectContact(const Contact &contact);

	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Disconnects data emitted by given contact when its roster data changes.
	 * @param contact contact to disconnect from
	 */
	void disconnectContact(const Contact &contact);

private slots:
	/**
	 * @enum RosterState
	 * @author Rafał 'Vogel' Malinowski
	 * @short Slot called when data of contact or contact's owner buddy changed.
	 *
	 * This slot can only by called for contacts that were previously added to roster using addContact() methods
	 * and were not removed.
	 */
	void contactDirtinessChanged();

};

/**
 * @}
 */
