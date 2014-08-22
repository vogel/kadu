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

#ifndef OTR_TRUST_LEVEL_SERVICE_H
#define OTR_TRUST_LEVEL_SERVICE_H

extern "C" {
#	include <libotr/context.h>
}

#include <QtCore/QObject>
#include <QtCore/QPointer>

class Contact;

class OtrContextConverter;
class OtrUserStateService;

class OtrTrustLevelService : public QObject
{
	Q_OBJECT

	QPointer<OtrContextConverter> ContextConverter;
	QPointer<OtrUserStateService> UserStateService;

public:
	static void wrapperOtrUpdateContextList(void *data);

	enum TrustLevel
	{
		TrustLevelUnknown,
		TrustLevelNotPrivate,
		TrustLevelUnverified,
		TrustLevelPrivate
	};

	explicit OtrTrustLevelService(QObject *parent = 0);
	virtual ~OtrTrustLevelService();

	void setContextConverter(OtrContextConverter *contextConverter);
	void setUserStateService(OtrUserStateService *userStateService);

	void storeTrustLevelToContact(const Contact &contact, TrustLevel level) const;
	TrustLevel loadTrustLevelFromContact(const Contact &contact) const;

	TrustLevel trustLevelFromContext(ConnContext *context) const;

public slots:
	void updateTrustLevels();

signals:
	void trustLevelsUpdated();

};

#endif // OTR_TRUST_LEVEL_SERVICE_H
