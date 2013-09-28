/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef OTR_FINGERPRINT_SERVICE_H
#define OTR_FINGERPRINT_SERVICE_H

#include <QtCore/QObject>
#include <QtCore/QWeakPointer>

class Account;
class Contact;
class OtrContextConverter;
class OtrUserStateService;

class OtrFingerprintService : public QObject
{
	Q_OBJECT

	QWeakPointer<OtrContextConverter> ContextConverter;
	QWeakPointer<OtrUserStateService> UserStateService;

	QString fingerprintsStoreFileName() const;

public:
	static void wrapperOtrWriteFingerprints(void *data);

	enum Trust
	{
		TrustNotVerified,
		TrustVerified
	};

	explicit OtrFingerprintService(QObject *parent = 0);
	virtual ~OtrFingerprintService();

	void setContextConverter(OtrContextConverter *contextConverter);
	void setUserStateService(OtrUserStateService *userStateService);

	void readFingerprints() const;
	void writeFingerprints() const;

	void setContactFingerprintTrust(const Contact &contact, Trust trust) const;
	Trust contactFingerprintTrust(const Contact &contact) const;

	QString extractAccountFingerprint(const Account &account) const;
	QString extractContactFingerprint(const Contact &contact) const;

signals:
	void fingerprintsUpdated() const;

};

#endif // OTR_FINGERPRINT_SERVICE_H
