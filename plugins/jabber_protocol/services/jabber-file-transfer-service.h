/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_FILE_TRANSFER_SERVICE_H
#define JABBER_FILE_TRANSFER_SERVICE_H

#include "file-transfer/file-transfer-handler.h"
#include "protocols/services/file-transfer-service.h"

namespace XMPP
{
	class S5BServer;

	class JabberProtocol;
}

class JabberFileTransferService : public FileTransferService
{
	Q_OBJECT

	XMPP::JabberProtocol *Protocol;

private slots:
	void loggedIn();
	void loggedOut();

	void incomingFileTransferSlot();

public:
	explicit JabberFileTransferService(XMPP::JabberProtocol *protocol);
	virtual ~JabberFileTransferService();

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer) override;
	virtual FileTransferCanSendResult canSend(Contact contact) override;

};

#endif // JABBER_FILE_TRANSFER_SERVICE_H
