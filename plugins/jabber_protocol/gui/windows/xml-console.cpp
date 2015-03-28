/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "QtGui/QTextFrame"
#include "QtWidgets/QTextEdit"
#include "QtWidgets/QVBoxLayout"

#include "services/jabber-stream-debug-service.h"
#include "jabber-protocol.h"

#include "xml-console.h"

XmlConsole::XmlConsole(Account account) :
		// using C++ initializers breaks Qt's lupdate
		WatchedAccount(account),
		Viewer()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(tr("XML Console - %1").arg(WatchedAccount.id()));
	setWindowRole("kadu-xml-console");

	JabberProtocol *protocol = qobject_cast<JabberProtocol *>(account.protocolHandler());
	if (protocol)
	{
		createGui();

		connect(protocol->streamDebugService(), SIGNAL(incomingStream(QString)), this, SLOT(xmlIncomingSlot(QString)));
		connect(protocol->streamDebugService(), SIGNAL(outgoingStream(QString)), this, SLOT(xmlOutgoingSlot(QString)));
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

#include "moc_xml-console.cpp"
