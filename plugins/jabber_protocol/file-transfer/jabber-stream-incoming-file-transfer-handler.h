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

#include "file-transfer/stream-incoming-file-transfer-handler.h"

#include <QtCore/QPointer>
#include <qxmpp/QXmppTransferManager.h>

class JabberStreamIncomingFileTransferHandler : public StreamIncomingFileTransferHandler
{
	Q_OBJECT

public:
	explicit JabberStreamIncomingFileTransferHandler(FileTransfer fileTransfer);
	virtual ~JabberStreamIncomingFileTransferHandler();

	void setTransferJob(QXmppTransferJob *transferJob);

	virtual void accept(QIODevice *destination);
	virtual void reject();

signals:
	void statusChanged();

private:
	QPointer<QXmppTransferJob> m_transferJob;

	bool m_inProgress;
	QPointer<QIODevice> m_destination;

	void cleanup(FileTransferStatus status);

private slots:
	void progress(qint64 progress, qint64 total);
	void stateChanged(QXmppTransferJob::State state);
	void error(QXmppTransferJob::Error error);

};
