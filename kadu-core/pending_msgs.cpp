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
#include "contacts/contact-manager.h"
#include "contacts/contact-list-configuration-helper.h"
#include "gui/widgets/chat-widget-manager.h"

#include "debug.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "xml_config_file.h"


#include "pending_msgs.h"

PendingMsgs::Element::Element() : chat(0), sender(), msg(), time(0)
{
}

PendingMsgs::PendingMsgs(QObject *parent)
	: QObject(parent), msgs()
{
}

void PendingMsgs::deleteMsg(int index)
{
	kdebugm(KDEBUG_INFO, "PendingMsgs::(pre)deleteMsg(%d), count=%d\n", index, count());
	Contact e = msgs[index].sender;
	msgs.removeAt(index);
	storeConfiguration(xml_config_file);
	kdebugm(KDEBUG_INFO, "PendingMsgs::deleteMsg(%d), count=%d\n", index, count());
	emit messageFromUserDeleted(e);
}

bool PendingMsgs::pendingMsgs(Contact contact) const
{
	foreach (const Element &msg, msgs)
		if (msg.sender == contact)
			return true;

	return false;
}

unsigned int PendingMsgs::pendingMsgsCount(Chat *chat) const
{
	unsigned int count = 0;

	foreach (const Element &msg, msgs)
	{
		if (chat == msg.chat)
			count++;
	}

	return count;
}

bool PendingMsgs::pendingMsgs() const
{
	return !msgs.isEmpty();
}

int PendingMsgs::count() const
{
	return msgs.count();
}

PendingMsgs::Element &PendingMsgs::operator[](int index)
{
	return msgs[index];
}

void PendingMsgs::addMsg(Chat *chat, Contact sender, QString msg, time_t time)
{
	Element e;
	e.sender = sender;
	e.chat = chat;
	e.msg = msg;
	e.time = time;
	msgs.append(e);
	storeConfiguration(xml_config_file);
	emit messageFromUserAdded(sender);
}

void PendingMsgs::loadConfiguration(XmlConfigFile *configurationStorage)
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
		Element e;
		QDomElement chatNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Chat", XmlConfigFile::ModeFind);
		Chat *chat = ChatManager::instance()->byUuid(chatNode.text());

		QDomElement timeNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Time", XmlConfigFile::ModeFind);
		QDateTime d = QDateTime::fromString(timeNode.text());
		e.time = d.toTime_t();
		
		QDomElement messageNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Message", XmlConfigFile::ModeFind);
		e.msg = codec_latin2->toUnicode(messageNode.text().toLocal8Bit().data());

		QDomElement senderNode = configurationStorage->getNode(pendingMsgsNodes.item(i).toElement(), "Sender", XmlConfigFile::ModeFind);
		Contact sender = ContactManager::instance()->byUuid(senderNode.text());

		msgs.append(e);
		emit messageFromUserAdded(sender);
	}
}

void PendingMsgs::storeConfiguration(XmlConfigFile *configurationStorage)
{
	QDomElement pendingMsgsNode = configurationStorage->getNode("PendingMessages");
	configurationStorage->removeChildren(pendingMsgsNode);
	foreach (const Element &i, msgs)
	{
		QDomElement pendingMessageNode = configurationStorage->getNode(pendingMsgsNode,
			"PendingMessage", XmlConfigFile::ModeCreate);

		configurationStorage->createTextNode(pendingMessageNode, "Chat", i.chat->uuid().toString());
		configurationStorage->createTextNode(pendingMessageNode, "Time", QString::number(i.time));

		configurationStorage->createTextNode(pendingMessageNode, "Message", codec_latin2->fromUnicode(i.msg));

		configurationStorage->createTextNode(pendingMessageNode, "Sender", i.sender.uuid().toString());
	}
}

void PendingMsgs::openMessages()
{
	ChatWidgetManager::instance()->openPendingMsgs();
}

PendingMsgs pending(0);


// void Kadu::imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &/*path*/)
// {
// 	for (int i = 0, count = pending.count(); i < count; i++)
// 	{
// 		PendingMsgs::Element& e = pending[i];
//	TODO: 0.6.6 or sth?
// 		e.msg = gadu_images_manager.replaceLoadingImages(e.msg, sender, size, crc32);
// 	}
// }
