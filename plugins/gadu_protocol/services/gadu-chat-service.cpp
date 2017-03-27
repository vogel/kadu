/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QRegularExpression>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"
#include "chat/chat-storage.h"
#include "chat/type/chat-type-contact-set.h"
#include "chat/type/chat-type-contact.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-set.h"
#include "formatted-string/composite-formatted-string.h"
#include "formatted-string/formatted-string-factory.h"
#include "formatted-string/formatted-string-is-plain-text-visitor.h"
#include "formatted-string/formatted-string-plain-text-visitor.h"
#include "html/html-conversion.h"
#include "html/html-string.h"
#include "icons/icons-manager.h"
#include "message/message-storage.h"
#include "message/raw-message.h"
#include "misc/misc.h"
#include "services/image-storage-service.h"
#include "services/raw-message-transformer-service.h"
#include "status/status-type.h"
#include "windows/message-dialog.h"

#include "helpers/formatted-string-gadu-html-visitor.h"
#include "helpers/formatted-string-image-key-received-visitor.h"
#include "helpers/gadu-protocol-helper.h"
#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"

#include "gadu-chat-service.h"

#include <QtGui/QTextDocument>

// TODO: move to const or something
#define MAX_DELIVERY_TIME 60 /*seconds*/
#define REMOVE_TIMER_INTERVAL 1000

GaduChatService::GaduChatService(Account account, QObject *parent) :
		ChatService(account, parent)
{
	RemoveTimer = new QTimer(this);
	RemoveTimer->setInterval(REMOVE_TIMER_INTERVAL);
	connect(RemoveTimer, SIGNAL(timeout()), this, SLOT(removeTimeoutUndeliveredMessages()));
	RemoveTimer->start();
}

GaduChatService::~GaduChatService()
{
}

void GaduChatService::setChatManager(ChatManager *chatManager)
{
	m_chatManager = chatManager;
}

void GaduChatService::setChatStorage(ChatStorage *chatStorage)
{
	m_chatStorage = chatStorage;
}

void GaduChatService::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void GaduChatService::setContactManager(ContactManager *contactManager)
{
	m_contactManager = contactManager;
}

void GaduChatService::setFormattedStringFactory(FormattedStringFactory *formattedStringFactory)
{
	m_formattedStringFactory = formattedStringFactory;
}

void GaduChatService::setGaduChatImageService(GaduChatImageService *gaduChatImageService)
{
	CurrentGaduChatImageService = gaduChatImageService;
}

void GaduChatService::setGaduFileTransferService(GaduFileTransferService *gaduFileTransferService)
{
	CurrentFileTransferService = gaduFileTransferService;
}

void GaduChatService::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void GaduChatService::setImageStorageService(ImageStorageService *imageStorageService)
{
	CurrentImageStorageService = imageStorageService;
}

void GaduChatService::setMessageStorage(MessageStorage *messageStorage)
{
	m_messageStorage = messageStorage;
}

void GaduChatService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

int GaduChatService::maxMessageLength() const
{
	return 10000;
}

// see #2857 - old GG versions do not receive messages sent with new API
// but we can at least sent plain text messages with old API
int GaduChatService::sendRawMessage(const QVector<Contact> &contacts, const RawMessage &rawMessage, bool isPlainText)
{
	if (!Connection || !Connection.data()->hasSession())
		return -1;

	auto writableSessionToken = Connection.data()->writableSessionToken();
	unsigned int uinsCount = contacts.count();
	if (uinsCount > 1)
	{
		QScopedArrayPointer<UinType> uins(contactsToUins(contacts));
		if (isPlainText)
			return gg_send_message_confer(writableSessionToken.rawSession(), GG_CLASS_CHAT, uinsCount, uins.data(),
					(const unsigned char *) rawMessage.rawPlainContent().constData());
		else
			return gg_send_message_confer_html(writableSessionToken.rawSession(), GG_CLASS_CHAT, uinsCount, uins.data(),
					(const unsigned char *) rawMessage.rawContent().constData());
	}
	else if (uinsCount == 1)
	{
		UinType uin = GaduProtocolHelper::uin(contacts.at(0));

		if (isPlainText)
			return gg_send_message(writableSessionToken.rawSession(), GG_CLASS_CHAT, uin, (const unsigned char *) rawMessage.rawPlainContent().constData());
		else
			return gg_send_message_html(writableSessionToken.rawSession(), GG_CLASS_CHAT, uin, (const unsigned char *) rawMessage.rawContent().constData());
	}

	return -1;
}

UinType * GaduChatService::contactsToUins(const QVector<Contact> &contacts) const
{
	UinType * uins = new UinType[contacts.count()];
	unsigned int i = 0;

	foreach (const Contact &contact, contacts)
		uins[i++] = GaduProtocolHelper::uin(contact);

	return uins;
}

bool GaduChatService::sendMessage(const Message &message)
{
	if (!Connection || !Connection.data()->hasSession())
		return false;

	auto formattedContent = m_formattedStringFactory->fromHtml(message.content());

	FormattedStringIsPlainTextVisitor isPlainTextVisitor;
	formattedContent->accept(&isPlainTextVisitor);

	FormattedStringPlainTextVisitor plainTextVisitor;
	formattedContent->accept(&plainTextVisitor);

	FormattedStringGaduHtmlVisitor htmlVisitor(CurrentGaduChatImageService, CurrentImageStorageService);
	formattedContent->accept(&htmlVisitor);

	auto rawMessage = RawMessage{plainTextVisitor.result().toUtf8(), htmlVisitor.result().string().toUtf8()};
	if (rawMessageTransformerService())
		rawMessage = rawMessageTransformerService()->transform(rawMessage, message);

	if (rawMessage.rawContent().length() > maxMessageLength())
	{
		MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"), tr("Message too long (%1 >= %2)").arg(rawMessage.rawContent().length()).arg(10000));
		return false;
	}

	int messageId = sendRawMessage(message.messageChat().contacts().toContactVector(), rawMessage, isPlainTextVisitor.isPlainText());

	if (-1 == messageId)
		return false;

	message.setId(QString::number(messageId));
	UndeliveredMessages.insert(messageId, message);

	return true;
}

bool GaduChatService::sendRawMessage(const Chat &chat, const QByteArray &rawMessage)
{
	int messageId = sendRawMessage(chat.contacts().toContactVector(), {rawMessage}, true);
	return messageId != -1;
}

bool GaduChatService::isSystemMessage(gg_event *e)
{
	return 0 == e->event.msg.sender;
}

Contact GaduChatService::getSender(gg_event *e)
{
	return m_contactManager->byId(account(), QString::number(e->event.msg.sender), ActionCreateAndAdd);
}

bool GaduChatService::ignoreSender(gg_event *e, Buddy sender)
{
	bool ignore =
			sender.isAnonymous() &&
			m_configuration->deprecatedApi()->readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			(
				(e->event.msg.recipients_count == 0) ||
				m_configuration->deprecatedApi()->readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")
			);

	return ignore;
}

ContactSet GaduChatService::getRecipients(gg_event *e)
{
	ContactSet recipients;
	for (int i = 0; i < e->event.msg.recipients_count; ++i)
		recipients.insert(m_contactManager->byId(account(), QString::number(e->event.msg.recipients[i]), ActionCreateAndAdd));

	return recipients;
}

RawMessage GaduChatService::getRawMessage(gg_event *e)
{
	return {
		(const char *)e->event.msg.message,
		(const char *)e->event.msg.xhtml_message
	};
}

bool GaduChatService::ignoreRichText(Contact sender)
{
	return sender.isAnonymous() && m_configuration->deprecatedApi()->readBoolEntry("Chat","IgnoreAnonymousRichtext");
}

void GaduChatService::handleMsg(Contact sender, ContactSet recipients, MessageType type, gg_event *e)
{
	ContactSet conference = recipients;
	conference += sender;

	ContactSet chatContacts = conference;
	chatContacts.remove(account().accountContact());

	if (chatContacts.isEmpty())
		return;

	Chat chat = 1 == chatContacts.size()
			? ChatTypeContact::findChat(m_chatManager, m_chatStorage, *chatContacts.constBegin(), ActionCreateAndAdd)
			: ChatTypeContactSet::findChat(m_chatManager, m_chatStorage, chatContacts, ActionCreateAndAdd);

	// create=true in our call for findChat(), but chat might be null for example if chatContacts was empty
	if (!chat || chat.isIgnoreAllMessages())
		return;

	if (CurrentFileTransferService)
	{
		auto content = QString::fromUtf8(reinterpret_cast<const char *>(e->event.msg.message));
		auto fileTransferRegExp = QRegularExpression{"^http\\:\\/\\/www\\.gg\\.pl\\/dysk\\/([a-zA-Z0-9-_]{23})\\/(.+)$"};
		auto fileTransferMatch = fileTransferRegExp.match(content);

		if (fileTransferMatch.hasMatch())
			CurrentFileTransferService->fileTransferReceived(sender, fileTransferMatch.captured(1), fileTransferMatch.captured(2));
	}

	Message message = m_messageStorage->create();
	message.setMessageChat(chat);
	message.setType(type);
	message.setMessageSender(sender);
	message.setSendDate(QDateTime::fromTime_t(e->event.msg.time));
	message.setReceiveDate(QDateTime::currentDateTime());

	auto rawMessage = getRawMessage(e);

	if (rawMessageTransformerService())
		rawMessage = rawMessageTransformerService()->transform(rawMessage, message);

	auto htmlContent = normalizeHtml(HtmlString{QString::fromUtf8(rawMessage.rawContent())});
	auto formattedString = m_formattedStringFactory->fromHtml(htmlContent);
	if (!ignoreRichText(sender))
	{
		FormattedStringHtmlVisitor htmlVisitor;
		formattedString->accept(&htmlVisitor);
		htmlContent = normalizeHtml(htmlVisitor.result());
	}
	else
		htmlContent = normalizeHtml(plainToHtml(htmlToPlain(htmlContent)));

	if (htmlContent.string().isEmpty())
		return;

	message.setContent(std::move(htmlContent));

	if (MessageTypeReceived == type)
	{
		emit messageReceived(message);

		FormattedStringImageKeyReceivedVisitor imageKeyReceivedVisitor(sender.id());
		connect(&imageKeyReceivedVisitor, SIGNAL(chatImageKeyReceived(QString,ChatImage)),
		        this, SIGNAL(chatImageKeyReceived(QString,ChatImage)));

		formattedString->accept(&imageKeyReceivedVisitor);
	}
	else
		emit messageSent(message);
}

void GaduChatService::handleEventMsg(struct gg_event *e)
{
	if (isSystemMessage(e))
		return;

	if (GG_CLASS_CTCP == e->event.msg.msgclass) // old DCC6, not supported now
		return;

	Contact sender = getSender(e);
	if (ignoreSender(e, sender.ownerBuddy()))
		return;

	ContactSet recipients = getRecipients(e);

	handleMsg(sender, recipients, MessageTypeReceived, e);
}

void GaduChatService::handleEventMultilogonMsg(gg_event *e)
{
	// warning: this may be not intuitive code

	// we are sender
	Contact sender = account().accountContact();

	// e.sender + e.recipeints are real recipients
	ContactSet recipients = getRecipients(e);
	recipients.insert(getSender(e));

	handleMsg(sender, recipients, MessageTypeSent, e);
}

void GaduChatService::handleEventAck(struct gg_event *e)
{
	int messageId = e->event.ack.seq;
	if (!UndeliveredMessages.contains(messageId))
		return;

	UinType uin = e->event.ack.recipient;
	Q_UNUSED(uin) // only in debug mode

	switch (e->event.ack.status)
	{
		case GG_ACK_DELIVERED:
		case GG_ACK_QUEUED:
			UndeliveredMessages[messageId].setStatus(MessageStatusDelivered);
			emit sentMessageStatusChanged(UndeliveredMessages[messageId]);
			break;
		case GG_ACK_BLOCKED:
		case GG_ACK_MBOXFULL:
		case GG_ACK_NOT_DELIVERED:
			UndeliveredMessages[messageId].setStatus(MessageStatusWontDeliver);
			emit sentMessageStatusChanged(UndeliveredMessages[messageId]);
			break;
		default:
			return;
	}
	UndeliveredMessages.remove(messageId);

	removeTimeoutUndeliveredMessages();
}

void GaduChatService::removeTimeoutUndeliveredMessages()
{
	QDateTime now = QDateTime::currentDateTime();
	QHash<int, Message>::iterator it = UndeliveredMessages.begin();
	QVector<Message> removedMessages;

	while (it != UndeliveredMessages.end())
		if (it.value().sendDate().addSecs(MAX_DELIVERY_TIME) < now)
		{
			removedMessages.append(it.value());
			it = UndeliveredMessages.erase(it);
		}
		else
			++it;

	for (auto const &message : removedMessages)
	{
		message.setStatus(MessageStatusWontDeliver);
		emit sentMessageStatusChanged(message);
	}
}

void GaduChatService::leaveChat(const Chat& chat)
{
	chat.setIgnoreAllMessages(true);
}

#include "moc_gadu-chat-service.cpp"
