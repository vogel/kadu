/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_FACTORY_H
#define GADU_PROTOCOL_FACTORY_H

#include "protocol_factory.h"

class GaduProtocolFactory : public ProtocolFactory
{
public:
	virtual Protocol * newInstance();
	virtual QString name() { return "gadu"; }

};

#endif // GADU_PROTOCOL_FACTORY_H
