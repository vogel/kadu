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

#ifndef OTR_OP_DATA_H
#define OTR_OP_DATA_H

#include "chat/chat.h"

class OtrAppOpsWrapper;
class OtrNotifier;
class OtrPrivateKeyService;

class OtrOpData
{
	OtrAppOpsWrapper *AppOpsWrapper;
	OtrNotifier *Notifier;
	OtrPrivateKeyService *PrivateKeyService;
	Chat MyChat;
	QString MySender;

public:
	explicit OtrOpData();
	OtrOpData(const OtrOpData &copyMe);
	virtual ~OtrOpData();

	OtrOpData & operator = (const OtrOpData &copyMe);

	void setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper);
	OtrAppOpsWrapper * appOpsWrapper() const;

	void setNotifier(OtrNotifier *notifier);
	OtrNotifier * notifier() const;

	void setPrivateKeyService(OtrPrivateKeyService *privateKeyService);
	OtrPrivateKeyService * privateKeyService() const;

	void setChat(const Chat &chat);
	Chat chat() const;

	void setSender(const QString &sender);
	QString sender() const;

};

#endif // OTR_OP_DATA_H