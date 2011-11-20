/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "core/core.h"

#include "trusted-certificates-manager.h"

TrustedCertificatesManager * TrustedCertificatesManager::Instance = 0;

TrustedCertificatesManager * TrustedCertificatesManager::instance()
{
	if (0 == Instance)
		Instance = new TrustedCertificatesManager();
	return Instance;
}

void TrustedCertificatesManager::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

TrustedCertificatesManager::TrustedCertificatesManager()
{
	ConfigurationManager::instance()->registerStorableObject(this);
	load();
}

TrustedCertificatesManager::~TrustedCertificatesManager()
{
	ConfigurationManager::instance()->unregisterStorableObject(this);
}

StorableObject * TrustedCertificatesManager::storageParent()
{
	return 0;
}

QString TrustedCertificatesManager::storageNodeName()
{
	return QLatin1String("JabberTrustedCertificates");
}

QString TrustedCertificatesManager::storageItemNodeName()
{
	return QLatin1String("JabberTrustedCertificate");
}

void TrustedCertificatesManager::addTrustedCertificate(const QString &certificate)
{
	if (certificate.isEmpty())
		return;

	if (StringList.contains(certificate))
		removeTrustedCertificate(certificate);

	emit certificateAboutToBeAdded(certificate);
	StringList.prepend(certificate);
	emit certificateAdded(certificate);
}

void TrustedCertificatesManager::removeTrustedCertificate(const QString& certificate)
{
	if (!StringList.contains(certificate))
		return;

	emit certificateAboutToBeRemoved(certificate);
	StringList.removeAll(certificate);
	emit certificateRemoved(certificate);
}

bool TrustedCertificatesManager::isTrusted(const QString &certificate)
{
	foreach (const QString &cert, StringList)
		if (cert == certificate)
			return true;
	return false;
}
