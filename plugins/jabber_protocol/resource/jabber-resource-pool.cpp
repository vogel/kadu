 /*
  * jabberresourcepool.cpp
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

#include <QtCore/QList>

#include "debug.h"

#include "jabber-protocol.h"
#include "jabber-resource-pool.h"
#include "jabber-resource.h"

/**
 * This resource will be returned if no other resource
 * for a given JID can be found. It's an empty offline
 * resource.
 */
XMPP::Resource JabberResourcePool::EmptyResource(QString(), XMPP::Status(QString(), QString(), 0, false));

JabberResourcePool::JabberResourcePool(XMPP::JabberProtocol *protocol) :
	QObject(protocol), protocol(protocol)
{
}

JabberResourcePool::~JabberResourcePool()
{
	qDeleteAll(pool);
}

void JabberResourcePool::slotResourceDestroyed(QObject *sender)
{
	kdebug("Resource has been destroyed, collecting the pieces.\n");

	// remove this resource from the lock list if it existed
	lockList.removeAll(static_cast<JabberResource *>(sender));
}

void JabberResourcePool::addResource(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebugf();
	// see if the resource already exists
	foreach (JabberResource *mResource, pool)
	{
		if ((mResource->jid().bare().toLower() == jid.bare().toLower()) && (mResource->resource().name().toLower() == resource.name().toLower()))
		{
			kdebug("Updating existing resource %s for %s\n", resource.name().toUtf8().constData(), jid.bare().toUtf8().constData());

			// It exists, update it. Don't do a "lazy" update by deleting
			// it here and readding it with new parameters later on,
			// any possible lockings to this resource will get lost.
			mResource->setResource(resource);

			return;
		}
	}

	kdebug("Adding new resource %s for %s\n", resource.name().toUtf8().constData(), jid.bare().toUtf8().constData());

	// create new resource instance and add it to the dictionary
	JabberResource *newResource = new JabberResource(jid, resource);

	connect(newResource, SIGNAL(destroyed(QObject *)), this, SLOT(slotResourceDestroyed(QObject *)));

	pool.append(newResource);

	kdebugf2();
}

void JabberResourcePool::removeResource(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebug("Removing resource %s for %s\n", resource.name().toUtf8().constData(), jid.bare().toUtf8().constData());

	foreach (JabberResource *mResource, pool)
	{
		if ((mResource->jid().bare().toLower() == jid.bare().toLower()) && (mResource->resource().name().toLower() == resource.name().toLower()))
		{
			delete pool.takeAt(pool.indexOf(mResource));
			return;
		}
	}
}

void JabberResourcePool::removeAllResources(const XMPP::Jid &jid)
{
	kdebug("Removing all resources for %s\n", jid.bare().toUtf8().constData());

	foreach (JabberResource *mResource, pool)
	{
		if (mResource->jid().bare().toLower() == jid.bare().toLower())
		{
			// only remove preselected resource in case there is one
			if (jid.resource().isEmpty() ||(jid.resource().toLower() == mResource->resource().name().toLower()))
			{
				kdebug("Removing resource %s / %s\n", jid.bare().toUtf8().constData(), mResource->resource().name().toUtf8().constData());

				delete pool.takeAt(pool.indexOf(mResource));
			}
		}
	}
}

void JabberResourcePool::clear()
{
	kdebug("Clearing the resource pool.\n");

	qDeleteAll(pool);
	pool.clear();
}

void JabberResourcePool::lockToResource(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebug("Locking %s to %s\n", jid.full().toUtf8().constData(), resource.name().toUtf8().constData());
	// remove all existing locks first

	removeLock(jid);

	// find the resource in our dictionary that matches
	foreach (JabberResource *mResource, pool)
	{
		if ((mResource->jid().bare().toLower() == jid.full().toLower()) && (mResource->resource().name().toLower() == resource.name().toLower()))
		{
			lockList.append(mResource);
			return;
		}
	}
}

void JabberResourcePool::removeLock(const XMPP::Jid &jid)
{
	kdebug("Removing resource lock for %s\n", jid.bare().toUtf8().constData());

	// find the resource in our dictionary that matches
	foreach (JabberResource *mResource, pool)
	{
		if ((mResource->jid().bare().toLower() == jid.bare().toLower()))
		{
			lockList.removeAll (mResource);
		}
	}
}

JabberResource *JabberResourcePool::lockedJabberResource( const XMPP::Jid &jid)
{
	// check if the JID already carries a resource, then we will have to use that one
	if (!jid.resource().isEmpty())
	{
		// we are subscribed to a JID, find the according resource in the pool
		foreach (JabberResource *mResource, pool)
		{
			if ((mResource->jid().bare().toLower() == jid.bare().toLower()) &&(mResource->resource().name() == jid.resource()))
			{
				return mResource;
			}
		}

		kdebug("WARNING: No resource found in pool, returning as offline.\n");

		return 0L;
	}

	// see if we have a locked resource
	foreach (JabberResource *mResource, lockList)
	{
		if (mResource->jid().bare().toLower() == jid.bare().toLower())
		{
			kdebug("Current lock for %s is %s\n", jid.bare().toUtf8().constData(), mResource->resource().name().toUtf8().constData());
			return mResource;
		}
	}

	kdebug("No lock available for %s\n", jid.bare().toUtf8().constData());

	return 0L;
}


JabberResource *JabberResourcePool::bestJabberResource(const XMPP::Jid &jid, bool honourLock)
{
	kdebug("Determining best resource for %s\n", jid.full().toUtf8().constData());

	if (honourLock)
	{
		// if we are locked to a certain resource, always return that one
		JabberResource *mResource = lockedJabberResource(jid);
		if (mResource)
		{
			kdebug("We have a locked resource %s for %s\n", mResource->resource().name().toUtf8().constData(), jid.full().toUtf8().constData());
			return mResource;
		}
	}

	JabberResource *bestResource = 0L;
	JabberResource *currentResource = 0L;

	foreach (currentResource, pool)
	{
		// make sure we are only looking up resources for the specified JID
		if (currentResource->jid().bare().toLower() != jid.bare().toLower())
		{
			continue;
		}

		// take first resource if no resource has been chosen yet
		if (!bestResource)
		{
			kdebug("Taking %s as first available resource.\n", currentResource->resource().name().toUtf8().constData());

			bestResource = currentResource;
			continue;
		}

		if (currentResource->resource().priority() > bestResource->resource().priority())
		{
			kdebug("Using %s due to better priority.\n", currentResource->resource().name().toUtf8().constData());
			// got a better match by priority
			bestResource = currentResource;
		}
		else
		{
			if (currentResource->resource().priority() == bestResource->resource().priority())
			{
				if(currentResource->resource().status().timeStamp() > bestResource->resource().status().timeStamp())
				{
					kdebug("Using %s due to better timestamp.\n", currentResource->resource().name().toUtf8().constData());

					// got a better match by timestamp (priorities are equal)
					bestResource = currentResource;
				}
			}
		}
	}

	return (bestResource) ? bestResource : 0L;
}

const XMPP::Resource & JabberResourcePool::bestResource(const XMPP::Jid &jid, bool honourLock)
{
	JabberResource *bestResource = bestJabberResource(jid, honourLock);

	return bestResource ? bestResource->resource() : EmptyResource;
}

#include "moc_jabber-resource-pool.cpp"
