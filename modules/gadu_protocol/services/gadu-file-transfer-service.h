/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_FILE_TRANSFER_SERVICE_H
#define GADU_FILE_TRANSFER_SERVICE_H

#include <libgadu.h>

#include "protocols/services/file-transfer-service.h"

class GaduProtocol;

class GaduFileTransferService : public FileTransferService
{
	Q_OBJECT

	GaduProtocol *Protocol;

public:
	GaduFileTransferService(GaduProtocol *protocol);

	virtual FileTransfer * loadFileTransferFromStorage(StoragePoint *storage);
	virtual FileTransfer * createOutgoingFileTransfer(Buddy contact);

};

#endif // GADU_FILE_TRANSFER_SERVICE_H

// kate: indent-mode cstyle; replace-tabs off; tab-width 4; 
