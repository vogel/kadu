/*
 * %kadu copyright begin%
 * Copyright 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/misc.h"

#include "jabber-protocol.h"

#include "jabber-server-change-password.h"

JabberServerChangePassword::JabberServerChangePassword(Account account, const QString &password, const QString &newPassword) :
	QObject(), Result(false), MyAccount(account), Password(password), NewPassword(newPassword)
{
}

void JabberServerChangePassword::performAction()
{
  	JabberProtocol *jabberProtocol = qobject_cast<JabberProtocol *>(MyAccount.protocolHandler());
	if (!jabberProtocol || !jabberProtocol->isConnected())
		emit finished(this);

	XMPP::JT_Register *task = new XMPP::JT_Register(jabberProtocol->client()->client()->rootTask());
	QObject::connect(task, SIGNAL(finished()),
			 this, SLOT(actionFinished()));
	XMPP::Jid j = MyAccount.id();
	task->reg(j.node(), NewPassword);
	task->go(true);
}

void JabberServerChangePassword::actionFinished()
{
	XMPP::JT_Register *task = (XMPP::JT_Register *)sender();
	//QString err = task->statusString();
	//int code = task->statusCode();
	Result = task->success();

	emit finished(this);
}
