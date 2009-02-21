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

class GaduFileTransfer : public FileTransfer
{
public:
	void setFileTransferNotifiers(DccSocketNotifiers *) {}

};

#endif // GADU_FILE_TRANSFER_H
