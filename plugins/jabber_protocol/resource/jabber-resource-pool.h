 /*
  * jabberresourcepool.h
  *
  * Copyright (c) 2004 by Till Gerken <till@tantalo.net>
  * Copyright (c) 2006 by Michaël Larouche <larouche@kde.org>
  *
  * Kopete    (c) by the Kopete developers  <kopete-devel@kde.org>
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

#ifndef JABBER_RESOURCE_POOL_H
#define JABBER_RESOURCE_POOL_H

#include <QtCore/QList>
#include <QtCore/QObject>

#include <im.h>

class JabberResource;
class JabberProtocol;

/**
 * @author Till Gerken <till@tantalo.net>
 * @author Michaël Larouche <larouche@kde.org>
 */
class JabberResourcePool : public QObject
{
	Q_OBJECT

	QList<JabberResource *> pool;
	QList<JabberResource *> lockList;

	/**
	 * Pointer to the JabberProtocol instance.
	 */
	JabberProtocol *protocol;

	/**
	 * Return a usable JabberResource for a given JID.
	 */
	JabberResource * bestJabberResource(const XMPP::Jid &jid, bool honourLock = true);

private slots:
	void slotResourceDestroyed(QObject *sender);
	
public:
	static XMPP::Resource EmptyResource;

	typedef QList<JabberResource*> ResourceList;

	/**
	 * Default constructor
	 */
	explicit JabberResourcePool(JabberProtocol *protocol);

	/**
	 * Default destructor
	 */
	virtual ~JabberResourcePool();

	/**
	 * Add a resource to the pool
	 */
	void addResource(const XMPP::Jid &jid, const XMPP::Resource &resource);

	/**
	 * Remove a resource from the pool
	 */
	void removeResource(const XMPP::Jid &jid, const XMPP::Resource &resource);

	/**
	 * Remove all resources for a given address from the pool
	 */
	void removeAllResources(const XMPP::Jid &jid);

	/**
	 * Remove all resources from the pool
	 */
	void clear();

	/**
	 * Lock to a certain resource
	 */
	void lockToResource(const XMPP::Jid &jid, const XMPP::Resource &resource);

	/**
	 * Remove a resource lock
	 */
	void removeLock(const XMPP::Jid &jid);

	/**
	 * Return the JabberResource instance for the locked resource, if any.
	 */
	 JabberResource * lockedJabberResource(const XMPP::Jid &jid);

	/**
	 * Return currently locked resource, if any
	 */
	const XMPP::Resource & lockedResource (const XMPP::Jid &jid);

	/**
	 * Return usable resource for a given JID
	 * Matches by userHost(), honors locks for a JID by default
	 */
	const XMPP::Resource & bestResource(const XMPP::Jid &jid, bool honourLock = true);

};

#endif // JABBER_RESOURCE_POOL_H
