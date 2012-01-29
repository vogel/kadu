/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TRUSTED_CERTIFICATES_MANAGER_H
#define TRUSTED_CERTIFICATES_MANAGER_H

#include "storage/storable-string-list.h"

class TrustedCertificatesManager : public QObject, public StorableStringList
{
	Q_OBJECT
	Q_DISABLE_COPY(TrustedCertificatesManager)

	static TrustedCertificatesManager *Instance;
	QStringList TemporaryList;

	TrustedCertificatesManager();
	virtual ~TrustedCertificatesManager();

public:
	static TrustedCertificatesManager * instance();
	static void destroyInstance();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	void addTrustedCertificate(const QString &certificate, bool persist = true);
	void removeTrustedCertificate(const QString &certificate);
	bool isTrusted(const QString &certificate);

signals:
	void certificateAboutToBeAdded(const QString &certificate);
	void certificateAdded(const QString &certificate);
	void certificateAboutToBeRemoved(const QString &certificate);
	void certificateRemoved(const QString &certificate);

};

#endif // TRUSTED_CERTIFICATES_MANAGER_H
