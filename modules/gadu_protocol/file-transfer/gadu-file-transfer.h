/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_FILE_TRANSFER_H
#define GADU_FILE_TRANSFER_H

#include "file-transfer/file-transfer.h"

class DccSocketNotifiers;
class GaduProtocol;

class GaduFileTransfer : public FileTransfer
{
	DccSocketNotifiers *SocketNotifiers;
	bool WaitingForSocketNotifiers;

protected:
	virtual void updateFileInfo();

public:
	GaduFileTransfer(Account *account);
	GaduFileTransfer(Account *account, Contact peer, FileTransferType transferType);
	virtual ~GaduFileTransfer();

	void setFileTransferNotifiers(DccSocketNotifiers *);
	void socketNotAvailable();

	virtual void send();
	virtual void stop();
	virtual void pause();
	virtual void restore();

	virtual bool accept(const QFile &file);
	virtual void reject();

};

#endif // GADU_FILE_TRANSFER_H
