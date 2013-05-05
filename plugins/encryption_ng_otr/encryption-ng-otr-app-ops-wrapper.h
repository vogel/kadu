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

#ifndef ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H
#define ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H

#include <QtCore/QObject>

extern "C" {
#	include <libotr/proto.h>
#	include <libotr/message.h>
}

class EncryptionNgOtrOpData;

class EncryptionNgOtrAppOpsWrapper : public QObject
{
	Q_OBJECT

	friend OtrlPolicy kadu_enomf_policy(void *, ConnContext *);
	friend void kadu_enomf_create_privkey(void *, const char *, const char *);
	friend void kadu_enomf_inject_message(void *, const char *, const char *, const char *, const char *);

	OtrlMessageAppOps Ops;

	OtrlPolicy policy(EncryptionNgOtrOpData *ngOtrOpData);
	void createPrivateKey(EncryptionNgOtrOpData *ngOtrOpData);
	void injectMessage(EncryptionNgOtrOpData *ngOtrOpData, const QString &messageContent);

public:
	explicit EncryptionNgOtrAppOpsWrapper();
	virtual ~EncryptionNgOtrAppOpsWrapper();

	const OtrlMessageAppOps * ops() const;

};

#endif // ENCRYPTION_NG_OTR_APP_OPS_WRAPPER_H
