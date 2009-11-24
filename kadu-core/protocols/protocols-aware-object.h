/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PROTOCOLS_AWARE_OBJECT
#define PROTOCOLS_AWARE_OBJECT

#include <QtCore/QList>

#include "aware-object.h"

class ProtocolFactory;

class KADUAPI ProtocolsAwareObject : public AwareObject<ProtocolsAwareObject>
{

protected:
	virtual void protocolRegistered(ProtocolFactory *protocolFactory) = 0;
	virtual void protocolUnregistered(ProtocolFactory *protocolFactory) = 0;

public:
	static void notifyProtocolRegistered(ProtocolFactory *protocolFactory);
	static void notifyProtocolUnregistered(ProtocolFactory *protocolFactory);

	void triggerAllProtocolsRegistered();
	void triggerAllProtocolsUnregistered();

};

#endif // PROTOCOLS_AWARE_OBJECT
