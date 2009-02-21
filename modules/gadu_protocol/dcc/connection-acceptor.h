/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTION_ACCEPTOR_H
#define CONNECTION_ACCEPTOR_H

class ConnectionAcceptor
{
public:
	virtual bool acceptConnection(unsigned int uin, unsigned int peerUin, unsigned int peerAddr) = 0;

};

#endif // CONNECTION_ACCEPTOR_H
