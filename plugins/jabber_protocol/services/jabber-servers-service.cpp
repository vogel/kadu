/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http:www.gnu.org/licenses/>.
 */

#include "jabber-servers-service.h"

#include <QtCore/QStringList>

JabberServersService::JabberServersService(QObject *parent) :
				QObject{parent}
{
}

JabberServersService::~JabberServersService()
{
}

QStringList JabberServersService::knownServers()
{
	return QStringList{}
		<< "adastra.re"
		<< "alpha-labs.net"
		<< "apocalysto.ru"
		<< "bigowl.net"
		<< "blah.im"
		<< "cabbar.org"
		<< "chatme.biz"
		<< "chatme.community"
		<< "chatme.education"
		<< "chatme.im"
		<< "chatme.sexy"
		<< "chatme.singles"
		<< "chatme.social"
		<< "chatme.xyz"
		<< "coderollers.com"
		<< "creep.im"
		<< "crypt.am"
		<< "crypt.mn"
		<< "default.rs"
		<< "dotchat.me"
		<< "e-utp.net"
		<< "einfachjabber.de"
		<< "foxba.se"
		<< "freamware.net"
		<< "fysh.in"
		<< "gabbler.de"
		<< "gajim.org"
		<< "gnuhost.eu"
		<< "gojabber.org"
		<< "hot-chilli.eu"
		<< "hot-chilli.net"
		<< "igniterealtime.org"
		<< "im.hot-chilli.eu"
		<< "im.hot-chilli.net"
		<< "im.it-native.de"
		<< "inbox.im"
		<< "injabber.info"
		<< "is-a-furry.org"
		<< "jabb3r.net"
		<< "jabbeng.in"
		<< "jabber-br.org"
		<< "jabber-hosting.de"
		<< "jabber.at"
		<< "jabber.ccc.de"
		<< "jabber.co.cm"
		<< "jabber.cz"
		<< "jabber.de"
		<< "jabber.fourecks.de"
		<< "jabber.hot-chilli.eu"
		<< "jabber.hot-chilli.net"
		<< "jabber.i-pobox.net"
		<< "jabber.kernel-error.de"
		<< "jabber.minus273.org"
		<< "jabber.no-sense.net"
		<< "jabber.ru.com"
		<< "jabber.rueckgr.at"
		<< "jabber.se"
		<< "jabber.smash-net.org"
		<< "jabber.theforest.us"
		<< "jabber.web.id"
		<< "jabber.wien"
		<< "jabber.zone"
		<< "jabberafrica.org"
		<< "jabberforum.de"
		<< "jabberon.net"
		<< "jabberon.ru"
		<< "jabberwiki.de"
		<< "jabbim.com"
		<< "jabbim.cz"
		<< "jabbim.pl"
		<< "jabbim.sk"
		<< "jabjab.de"
		<< "jappix.com"
		<< "jid.su"
		<< "jix.im"
		<< "jodo.im"
		<< "jsmart.web.id"
		<< "kdex.de"
		<< "lethyro.net"
		<< "limun.org"
		<< "linuxlovers.at"
		<< "lsd-25.ru"
		<< "mijabber.es"
		<< "miscastonline.com"
		<< "neko.im"
		<< "njs.netlab.cz"
		<< "ohai.su"
		<< "p-h.im"
		<< "planetjabber.de"
		<< "pod.so"
		<< "radiodd.de"
		<< "richim.org"
		<< "rosolina.estate"
		<< "rows.io"
		<< "securejabber.me"
		<< "see.ph"
		<< "slang.cool"
		<< "sternenschweif.de"
		<< "suchat.org"
		<< "talk.planetjabber.de"
		<< "twattle.net"
		<< "uplink.io"
		<< "webchat.domains"
		<< "wtfismyip.com"
		<< "wusz.org"
		<< "xmpp-hosting.de"
		<< "xmpp.cm"
		<< "xmpp.guru"
		<< "xmpp.jp"
		<< "xmpp.kz"
		<< "xmpp.ninja"
		<< "xmpp.pro"
		<< "xmpp.ru.net"
		<< "xmpp.su"
		<< "xmpp.technology"
		<< "xmpp.tips"
		<< "xmpp.zone"
		<< "xmppcomm.com"
		<< "xmppcomm.net"
		<< "xmppnet.de";
}

QStringList JabberServersService::knownNoXDataServers()
{
	return QStringList{}
		<< "apocalysto.ru"
		<< "bigowl.net"
		<< "blah.im"
		<< "cabbar.org"
		<< "creep.im"
		<< "crypt.am"
		<< "crypt.mn"
		<< "e-utp.net"
		<< "foxba.se"
		<< "freamware.net"
		<< "gajim.org"
		<< "gnuhost.eu"
		<< "injabber.info"
		<< "jabbeng.in"
		<< "jabber.co.cm"
		<< "jabber.no-sense.net"
		<< "jabber.ru.com"
		<< "jabber.rueckgr.at"
		<< "jabber.theforest.us"
		<< "jabberafrica.org"
		<< "jappix.com"
		<< "jid.su"
		<< "jsmart.web.id"
		<< "neko.im"
		<< "p-h.im"
		<< "twattle.net"
		<< "uplink.io"
		<< "wtfismyip.com"
		<< "xmpp.cm"
		<< "xmpp.jp"
		<< "xmpp.kz"
		<< "xmpp.ninja"
		<< "xmpp.pro"
		<< "xmpp.su"
		<< "xmppcomm.com"
		<< "xmppcomm.net";
}

#include "moc_jabber-servers-service.cpp"
