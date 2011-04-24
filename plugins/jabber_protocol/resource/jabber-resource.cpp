 /*
  * jabberresource.cpp
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

// Qt includes
#include <qtimer.h>

// Kopete includes
//#include "jabbercapabilitiesmanager.h"

#include "debug.h"

#include "jabber-resource.h"

class JabberResource::Private
{
public:
	Private( JabberProtocol *t_protocol, const XMPP::Jid &t_jid, const XMPP::Resource &t_resource )
	 : protocol(t_protocol), jid(t_jid), resource(t_resource), capsEnabled(false)
	{
		// Make sure the resource is always set.
		jid.withResource(resource.name());
	}

	JabberProtocol *protocol;
	XMPP::Jid jid;
	XMPP::Resource resource;

	QString clientName, clientSystem;
	XMPP::Features supportedFeatures;
	bool capsEnabled;
};

JabberResource::JabberResource ( JabberProtocol *protocol, const XMPP::Jid &jid, const XMPP::Resource &resource )
	: d( new Private(protocol, jid, resource) )
{
// //     JabberCapabilitiesManager *cm = protocol->protocol()->capabilitiesManager();
// //     if ( cm && cm->capabilitiesEnabled(jid) )
// //     {
// //         d->capsEnabled = cm->capabilitiesEnabled(jid);
// //     }

	if (protocol->isConnected())
	{
#if 0  //disabled because that flood the server, specially bad with the irc gateway
		QTimer::singleShot ( protocol->client()->getPenaltyTime () * 1000, this, SLOT ( slotGetTimedClientVersion () ) );
#endif
// 		/*if(!d->capsEnabled)
// 		{
// 			QTimer::singleShot ( protocol->client()->getPenaltyTime () * 1000, this, SLOT ( slotGetDiscoCapabilties () ) );
// 		}*/
	}
}

JabberResource::~JabberResource ()
{
	delete d;
}

const XMPP::Jid &JabberResource::jid () const
{
	return d->jid;
}

const XMPP::Resource &JabberResource::resource () const
{
	return d->resource;
}

void JabberResource::setResource ( const XMPP::Resource &resource )
{
	d->resource = resource;

	// Check if the caps are now available.
	///d->capsEnabled = d->protocol->protocol()->capabilitiesManager()->capabilitiesEnabled(d->jid);

	emit updated( this );
}

const QString &JabberResource::clientName () const
{
	return d->clientName;
}

const QString &JabberResource::clientSystem () const
{
	return d->clientSystem;
}

/*XMPP::Features JabberResource::features() const
{
	if(d->capsEnabled)
	{
		///return d->protocol->protocol()->capabilitiesManager()->features(d->jid);
	}
	else
	{
		return d->supportedFeatures;
	}
}*/

void JabberResource::slotGetTimedClientVersion ()
{
	if (d->protocol->isConnected())
	{
		QString jid = d->jid.full();
		kdebug("Requesting client version for %s\n", jid.toLocal8Bit().data());
		// request client version
		XMPP::JT_ClientVersion *task = new XMPP::JT_ClientVersion(d->protocol->client()->rootTask());
		// signal to ourselves when the vCard data arrived
		QObject::connect ( task, SIGNAL ( finished () ), this, SLOT ( slotGotClientVersion () ) );
		task->get ( d->jid );
		task->go ( true );
	}
}

void JabberResource::slotGotClientVersion ()
{
	XMPP::JT_ClientVersion *clientVersion = (XMPP::JT_ClientVersion *) sender ();

	if ( clientVersion->success () )
	{
		d->clientName = clientVersion->name () + ' ' + clientVersion->version ();
		d->clientSystem = clientVersion->os ();

		emit updated ( this );
	}
}

void JabberResource:: slotGetDiscoCapabilties ()
{
	if ( d->protocol->isConnected () )
	{
		QString jid = d->jid.full();
		kdebug("Requesting Client Features for %s\n", jid.toLocal8Bit().data());

		XMPP:: JT_DiscoInfo *task = new XMPP::JT_DiscoInfo ( d->protocol->client()->rootTask () );
		// Retrive features when service discovery is done.
		QObject::connect ( task, SIGNAL ( finished () ), this, SLOT (slotGotDiscoCapabilities () ) );
		task->get ( d->jid);
		task->go ( true );
	}
}

void JabberResource::slotGotDiscoCapabilities ()
{
	XMPP::JT_DiscoInfo *discoInfo = (XMPP::JT_DiscoInfo *) sender ();

	if ( discoInfo->success () )
	{
		d->supportedFeatures = discoInfo->item().features();

		emit updated ( this );
	}
}

