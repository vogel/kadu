/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextCodec>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "buddies/buddy-list-configuration-helper.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"
#include "protocols/protocol.h"

#include "pending-messages-manager.h"

PendingMessagesManager * PendingMessagesManager::Instance = 0;

PendingMessagesManager *  PendingMessagesManager::instance()
{
	if (0 == Instance)
		Instance = new PendingMessagesManager();

	return Instance;
}

PendingMessagesManager::PendingMessagesManager() : msgs()
{
}

void PendingMessagesManager::deleteMsg(int index)
{
	kdebugm(KDEBUG_INFO, "PendingMessagesManager::(pre)deleteMsg(%d), count=%d\n", index, count());
	Contact e = msgs[index].sender();
	msgs.removeAt(index);
	storeConfiguration(xml_config_file);
	kdebugm(KDEBUG_INFO, "PendingMessagesManager::deleteMsg(%d), count=%d\n", index, count());
	emit messageFromUserDeleted(e);
}

bool PendingMessagesManager::pendingMsgs(Contact contact) const
{
	foreach (const Message &msg, msgs)
		if (msg.sender() == contact)
			return true;

	return false;
}

unsigned int PendingMessagesManager::pendingMsgsCount(Chat *chat) const
{
	unsigned int count = 0;

	foreach (const Message &msg, msgs)
	{
		if (chat == msg.chat())
			count++;
	}

	return count;
}

bool PendingMessagesManager::pendingMsgs() const
{
	return !msgs.isEmpty();
}

int PendingMessagesManager::count() const
{
	return msgs.count();
}

Message &PendingMessagesManager::operator[](int index)
{
	return msgs[index];
}

void PendingMessagesManager::addMsg(const Message &msg)
{
	Message message = msg;
	msgs.append(message);
	storeConfiguration(xml_config_file);
	emit messageFromUserAdded(msg.sender());
}

void PendingMessagesManager::loadConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement pendingMsgsNode = configurationStorage->getNode("PendingMessages", XmlConfigFile::ModeFind);
	if (pendingMsgsNode.isNull())
		return;

	QDomNodeList pendingMsgsNodes = configurationStorage->getNodes(pendingMsgsNode, "PendingMessage");
	int count = pendingMsgsNodes.count();
	for (int i = 0; i < count; i++)
	{
		QDomElement messageElement = pendingMsgsNodes.item(i).toElement();
		if (messageElement.isNull())
			continue;
		Message msg;
		QDomElement chatNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Chat", XmlConfigFile::ModeFind);
		Chat *chat = ChatManager::instance()->byUuid(chatNode.text());
		msg.setChat(chat);

		QDomElement timeNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "ReceiveTime", XmlConfigFile::ModeFind);
		msg.setReceiveDate(QDateTime::fromString(timeNode.text()));

		timeNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "SentTime", XmlConfigFile::ModeFind);
		msg.setSendDate(QDateTime::fromString(timeNode.text()));

		QDomElement messageNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Message", XmlConfigFile::ModeFind);
		msg.setContent(codec_latin2->toUnicode(messageNode.text().toLocal8Bit().data()));

		QDomElement senderNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Sender", XmlConfigFile::ModeFind);
		Contact sender = ContactManager::instance()->byUuid(senderNode.text());
		msg.setSender(sender);

		msgs.append(msg);
		emit messageFromUserAdded(sender);
	}
}

void PendingMessagesManager::storeConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement pendingMsgsNode = configurationStorage->getNode("PendingMessages");
	configurationStorage->removeChildren(pendingMsgsNode);
	foreach (const Message &i, msgs)
	{
		QDomElement pendingMessageNode = configurationStorage->getNode(pendingMsgsNode,
			"PendingMessage", XmlConfigFile::ModeCreate);

		configurationStorage->createTextNode(pendingMessageNode, "Chat", i.chat()->uuid().toString());
		configurationStorage->createTextNode(pendingMessageNode, "ReceiveTime", QString::number(i.receiveDate().toTime_t()));
		configurationStorage->createTextNode(pendingMessageNode, "SentTime", QString::number(i.sendDate().toTime_t()));

		configurationStorage->createTextNode(pendingMessageNode, "Message", codec_latin2->fromUnicode(i.content()));

		configurationStorage->createTextNode(pendingMessageNode, "Sender", i.sender().uuid().toString());
	}
}

void PendingMessagesManager::openMessages()
{
	ChatWidgetManager::instance()->openPendingMsgs();
}

bool PendingMessagesManager::removeContactFromStorage(Contact contact)
{
	return !pendingMsgs(contact);
}


// void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
// {
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
// }
