/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#ifndef JABBER_FILE_TRANSFER_SERVICE_H
#define JABBER_FILE_TRANSFER_SERVICE_H

#include "file-transfer/jabber-file-transfer-handler.h"
#include "protocols/services/file-transfer-service.h"

namespace XMPP
{
	class S5BServer;
}

class JabberProtocol;

class JabberFileTransferService : public FileTransferService
{
	Q_OBJECT

	JabberProtocol *Protocol;

private slots:
	void loggedIn();
	void loggedOut();

	void incomingFileTransferSlot();

public:
	explicit JabberFileTransferService(JabberProtocol *protocol);
	virtual ~JabberFileTransferService();

	virtual FileTransferHandler * createFileTransferHandler(FileTransfer fileTransfer);

};

#endif // JABBER_FILE_TRANSFER_SERVICE_H
