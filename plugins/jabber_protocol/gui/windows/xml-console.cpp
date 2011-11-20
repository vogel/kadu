/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 badboy (badboy@gen2.org)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "QtGui/QTextEdit"
#include "QtGui/QTextFrame"
#include "QtGui/QVBoxLayout"

#include "client/jabber-client.h"
#include "jabber-protocol.h"

#include "xml-console.h"

XmlConsole::XmlConsole(Account account) :
		WatchedAccount(account)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("XML Console - %1").arg(WatchedAccount.id()));
	setWindowRole("kadu-xml-console");

	JabberProtocol *protocol = qobject_cast<JabberProtocol *>(account.protocolHandler());
	if (protocol)
	{
		createGui();

		connect(protocol->client(), SIGNAL(incomingXML(const QString &)), SLOT(xmlIncomingSlot(const QString &)));
		connect(protocol->client(), SIGNAL(outgoingXML(const QString &)), SLOT(xmlOutgoingSlot(const QString &)));
	}
	else
		deleteLater();
}

void XmlConsole::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	Viewer = new QTextEdit(this);
	Viewer->setUndoRedoEnabled(false);
	Viewer->setReadOnly(true);
	Viewer->setAcceptRichText(false);
	Viewer->viewport()->setObjectName("XmlViewport");
	// context menu shouldn't inherit it
	Viewer->viewport()->setStyleSheet("#XmlViewport { background-color: black; }");

	mainLayout->addWidget(Viewer);

	resize(560, 400);
}

void XmlConsole::xmlIncomingSlot(const QString &str)
{
	Viewer->setTextColor(Qt::yellow);
	Viewer->append(str + '\n');
}

void XmlConsole::xmlOutgoingSlot(const QString &str)
{
	Viewer->setTextColor(Qt::red);
	Viewer->append(str + '\n');
}
