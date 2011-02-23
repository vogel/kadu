/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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


#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtGui/QTextDocument>

#include "gui/widgets/custom_input.h"
#include "config_file.h"
#include "chat/chat_manager-old.h"
#include "debug.h"
#include "exports.h"
#include "misc/misc.h"
#include "../gadu_protocol/gadu.h"

#include "split.h"

Split *split;

extern "C" KADU_EXPORT int split_messages_init(bool firstLoad)
{
	Q_UNUSED(firstLoad)

	kdebugf();

	split = new Split();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/split_messages.ui"), split);

	kdebugf2();
	return 0;
}


extern "C" KADU_EXPORT void split_messages_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/split_messages.ui"), split);
	delete split;
	split = NULL;

	kdebugf2();
}

void fillEditor(ChatWidget *chat, QStringList &messages)
{
	kdebugf();
	chat->edit()->clear();
	chat->edit()->insert(*messages.begin());
	messages.pop_front();
	kdebugf2();
}

Split::Split(QObject *parent, const char *name)
: QObject(parent, name)
{
	kdebugf();

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *, time_t)), this, SLOT(onChatCreated(ChatWidget *, time_t)));

	for(ChatList::ConstIterator it = chat_manager->chats().begin(); it != chat_manager->chats().end(); ++it)
		connect(*it, SIGNAL(messageSendRequested(ChatWidget *)), this, SLOT(onMessageSendRequested(ChatWidget *) ));
	kdebugf2();
}

Split::~Split()
{
	kdebugf();
	kdebugf2();
}

void Split::onChatCreated(ChatWidget *chat, time_t time)
{
	kdebugf();
	connect(chat, SIGNAL(messageSendRequested(ChatWidget *)), this, SLOT(onMessageSendRequested(ChatWidget *)));
	kdebugf2();
}

void Split::onMessageSendRequested(ChatWidget *chat)
{
	kdebugf();

	if (!config_file.readBoolEntry("PowerKadu", "enable_split_message", false))
		return;

	const unsigned int maxL = 1000;
	unsigned int length;
	unsigned char *format;
	QTextDocument doc;
	QString text = chat->edit()->text();
	text.replace('\n', QLatin1String("\r\n"));
	doc.setHtml(text);
	text = doc.toPlainText();
	text.replace("\r\n", "\n");
	kdebugm(KDEBUG_INFO, "Tekst: %s\n", text.latin1());
	QStringList splitedMessages;
	if(text.length() > maxL)
	{
		unsigned int pos = 0;
		QString tmpStr;
		while(pos < text.length())
		{
			tmpStr = text.mid(pos, maxL);
			if (!tmpStr.isEmpty())
				splitedMessages += tmpStr;
			pos += maxL;
		}
		fillEditor(chat, splitedMessages);

		SendSplitted *send = new SendSplitted(chat, splitedMessages, this, "send_splited");
		send->name();
	}
	kdebugf2();
}

void Split::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

void Split::configurationUpdated()
{
}

SendSplitted::SendSplitted(ChatWidget *chat, QStringList messagesToSend, QObject *parent, const char *name)
{
	kdebugf();
	messages = messagesToSend;
	chatWindow = chat;
	connect(chat, SIGNAL( messageSentAndConfirmed(BuddyList , const QString &) ),
		this, SLOT( onMessageSent(BuddyList , const QString &) ));
	connect(&destroingTimer, SIGNAL( timeout() ), this, SLOT( onDestroyThis() ));
	connect(chat, SIGNAL( destroyed() ), this, SLOT( onDestroyThis() ));

	// Delete "this" object after 2 minutes to prevent mem leak in case user aborts sending a part
	// of the message.
	connect(&timer, SIGNAL( timeout() ), this, SLOT( sendNextPart() ));
	destroingTimer.start(2 * 60 * 1000, true);

	kdebugf2();
}

SendSplitted::~SendSplitted()
{
	kdebugf();
	//kdebugm(KDEBUG_INFO, "?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?!?\n");
	kdebugf2();
}

void SendSplitted::onMessageSent(BuddyList receivers, const QString &message)
{
	kdebugf();
	if(messages.count() == 0)
		onDestroyThis();
	else
		timer.start(0, true);
	kdebugf2();
}

void SendSplitted::sendNextPart()
{
	kdebugf();
	fillEditor(chatWindow, messages);
	chatWindow->sendMessage();
	kdebugf2();
}

void SendSplitted::onDestroyThis()
{
	kdebugf();
	deleteLater();
	kdebugf2();
}
