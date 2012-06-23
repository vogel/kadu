/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <xmpp_tasks.h>

#include "services/jabber-vcard-fetch-callback.h"
#include "services/jabber-vcard-uploader.h"

#include "jabber-vcard-service.h"

namespace XMPP
{

JabberVCardService::JabberVCardService(QObject *parent) :
		QObject(parent)
{
}

JabberVCardService::~JabberVCardService()
{
}

void JabberVCardService::setXmppClient(Client *xmppClient)
{
	XmppClient = xmppClient;
}

Client * JabberVCardService::xmppClient() const
{
	return XmppClient.data();
}

void JabberVCardService::fetch(const XMPP::Jid &jid, JabberVCardFetchCallback *callback)
{
	if (!callback)
		return;

	if (!XmppClient || !XmppClient.data()->rootTask())
	{
		callback->vCardFetched(false, VCard());
		return;
	}

	JT_VCard *task = new JT_VCard(XmppClient.data()->rootTask());
	FetchCallbacks.insert(task, callback);

	connect(task, SIGNAL(finished()), this, SLOT(fetched()));
	task->get(jid);
	task->go(true);
}

void JabberVCardService::fetched()
{
	JT_VCard *task = qobject_cast<JT_VCard *>(sender());
	Q_ASSERT(task);
	Q_ASSERT(FetchCallbacks.contains(task));

	JabberVCardFetchCallback *callback = FetchCallbacks.value(task);
	FetchCallbacks.remove(task);

	if (task->success())
		callback->vCardFetched(true, task->vcard());
	else
		callback->vCardFetched(false, VCard());
}

JabberVCardUploader * JabberVCardService::createVCardUploader()
{
	if (!XmppClient)
		return 0;

	return new JabberVCardUploader(XmppClient.data(), this);
}

}
