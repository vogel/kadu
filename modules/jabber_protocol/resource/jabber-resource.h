 /*
  * jabberresource.h
  *
  * Copyright (c) 2005-2006 by Michaël Larouche <larouche@kde.org>
  * Copyright (c) 2004 by Till Gerken <till@tantalo.net>
  *
  * Kopete    (c) 2001-2006 by the Kopete developers  <kopete-devel@kde.org>
  *
  * *************************************************************************
  * *                                                                       *
  * * This program is free software; you can redistribute it and/or modify  *
  * * it under the terms of the GNU General Public License as published by  *
  * * the Free Software Foundation; either version 2 of the License, or     *
  * * (at your option) any later version.                                   *
  * *                                                                       *
  * *************************************************************************
  */

#ifndef JABBER_RESOURCE_H
#define JABBER_RESOURCE_H

/**
 * Container class for a contact's resource
 */

#include <QtCore/QObject>

// libiris includes
#include <im.h>
#include <xmpp_tasks.h>

#include "jabber-protocol.h"

namespace XMPP
{
class Resource;
class Jid;
class Features;
}

class JabberProtocol;

class JabberResource : public QObject
{
Q_OBJECT

public:
	/**
	 * Create a new Jabber resource.
	 */
	JabberResource (JabberProtocol *protocol, const XMPP::Jid &jid, const XMPP::Resource &resource);
	~JabberResource();

	const XMPP::Jid &jid() const;
	const XMPP::Resource &resource() const;

	void setResource ( const XMPP::Resource &resource );

	/**
	 * Return the client name for this resource.
	 * @return the client name
	 */
	const QString &clientName () const;
	/**
	 * Return the client system for this resource.
	 * @return the client system.
	 */
	const QString &clientSystem () const;

	/**
	 * Get the available features for this resource.
	 */
	/*XMPP::Features features() const;*/

signals:
	void updated ( JabberResource * );

private slots:
	void slotGetTimedClientVersion ();
	void slotGotClientVersion ();
	void slotGetDiscoCapabilties ();
	void slotGotDiscoCapabilities ();

private:
	class Private;
	Private * const d;
};

#endif
