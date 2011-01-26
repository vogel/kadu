/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AVATAR_JOB_RUNNER_H
#define AVATAR_JOB_RUNNER_H

#include <QtCore/QObject>

#include "exports.h"

class Account;
class AvatarService;
class Contact;

class KADUAPI AvatarJobRunner : public QObject
{
	Q_OBJECT

	AvatarService * avatarService(const Account &account);
	AvatarService * avatarService(const Contact &contact);

private slots:
	void avatarFetched(Contact contact, bool ok);

public:
	explicit AvatarJobRunner(QObject *parent);
	virtual ~AvatarJobRunner();

	void runJob(const Contact &contact);

signals:
	void jobFinished(bool ok);

};

#include "contacts/contact.h" // for MOC

#endif // AVATAR_JOB_RUNNER_H
