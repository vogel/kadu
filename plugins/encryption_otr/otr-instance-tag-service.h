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

#ifndef OTR_INSTANCE_TAG_SERVICE_H
#define OTR_INSTANCE_TAG_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

extern "C" {
#	include <libotr/proto.h>
}

class Account;

class OtrPathService;
class OtrUserStateService;

class OtrInstanceTagService : public QObject
{
	Q_OBJECT

	QPointer<OtrPathService> PathService;
	QPointer<OtrUserStateService> UserStateService;

public:
	static void wrapperOtrCreateInstanceTag(void *data, const char *accountName, const char *protocol);

	explicit OtrInstanceTagService(QObject *parent = 0);
	virtual ~OtrInstanceTagService();

	void setPathService(OtrPathService *pathService);
	void setUserStateService(OtrUserStateService *userStateService);

	void readInstanceTags();
	void writeInstanceTags();
	void createInstanceTag(const Account &account);

};

#endif // OTR_INSTANCE_TAG_SERVICE_H
