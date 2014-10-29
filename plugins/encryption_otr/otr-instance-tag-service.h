/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2011, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

extern "C" {
#	include <libotr/proto.h>
}

class Account;

class OtrPathService;
class OtrUserStateService;

class OtrInstanceTagService : public QObject
{
	Q_OBJECT

public:
	static void wrapperOtrCreateInstanceTag(void *data, const char *accountName, const char *protocol);

	Q_INVOKABLE OtrInstanceTagService();
	virtual ~OtrInstanceTagService();

	void readInstanceTags();
	void writeInstanceTags();
	void createInstanceTag(const Account &account);

private slots:
	INJEQT_SETTER void setPathService(OtrPathService *pathService);
	INJEQT_SETTER void setUserStateService(OtrUserStateService *userStateService);

private:
	QPointer<OtrPathService> PathService;
	QPointer<OtrUserStateService> UserStateService;

	QString instanceTagsFileName() const;

};
