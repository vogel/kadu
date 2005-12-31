/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "protocol.h"

Protocol::Protocol(const QString &id, QObject *parent, const char *name) : QObject(parent, name), id(id)
{
}

Protocol::~Protocol()
{
	delete CurrentStatus;
	delete NextStatus;
}

const QDateTime &Protocol::connectionTime() const
{
	return ConnectionTime;
}
