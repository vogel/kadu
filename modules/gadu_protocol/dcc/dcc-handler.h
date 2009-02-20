/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DCC_HANDLER_H
#define DCC_HANDLER_H

class DccSocket;

class DccHandler
{
public:
	virtual ~DccHandler() {}

	virtual bool addSocket(DccSocket *socket) = 0;
	virtual void removeSocket(DccSocket *socket) = 0;

	virtual int dccType() = 0;

	virtual bool socketEvent(DccSocket *socket, bool &lock) = 0;

	virtual void connectionDone(DccSocket *socket) = 0;
	virtual void connectionError(DccSocket *socket) = 0;

	virtual void connectionAccepted(DccSocket *socket) = 0;
	virtual void connectionRejected(DccSocket *socket) = 0;

};

#endif // DCC_HANDLER_H
