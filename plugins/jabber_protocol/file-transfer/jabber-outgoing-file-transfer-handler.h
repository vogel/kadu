/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "file-transfer/outgoing-file-transfer-handler.h"

#include <QtCore/QPointer>
#include <qxmpp/QXmppTransferManager.h>

class JabberResourceService;

class JabberOutgoingFileTransferHandler : public OutgoingFileTransferHandler
{
	Q_OBJECT

public:
	explicit JabberOutgoingFileTransferHandler(QXmppTransferManager *transferManager, FileTransfer fileTransfer);
	virtual ~JabberOutgoingFileTransferHandler();

	void setResourceService(JabberResourceService *resourceService);

	void setJTransfer(FileTransfer *jTransfer);

	virtual void send(QIODevice *source);
	virtual void stop();

signals:
	void statusChanged();

private:
	QPointer<QXmppTransferManager> m_transferManager;
	QPointer<JabberResourceService> m_resourceService;

	QPointer<QXmppTransferJob> m_transferJob;

	bool m_inProgress;
	QPointer<QIODevice> m_source;

	void cleanup(FileTransferStatus status);

private slots:
	void progress(qint64 progress, qint64 total);
	void stateChanged(QXmppTransferJob::State state);
	void error(QXmppTransferJob::Error error);

};
