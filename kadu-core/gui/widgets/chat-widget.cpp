/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009, 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include <QtCore/QFileInfo>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/buddy.h"
#include "buddies/buddy-shared.h"
#include "buddies/buddy-set.h"
#include "chat/chat-geometry-data.h"
#include "chat/chat-manager.h"
#include "chat/message/message-render-info.h"
#include "configuration/configuration-file.h"
#include "contacts/contact.h"
#include "contacts/contact-set.h"
#include "contacts/model/contact-list-model.h"
#include "core/core.h"
#include "gui/hot-key.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "parser/parser.h"
#include "protocols/protocol.h"

#include "activate.h"
#include "chat-edit-box.h"
#include "custom-input.h"
#include "debug.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "chat-widget.h"

ChatWidget::ChatWidget(Chat chat, QWidget *parent) :
		QWidget(parent), CurrentChat(chat),
		BuddiesWidget(0), InputBox(0), horizSplit(0),
		NewMessagesCount(0)
{
	kdebugf();

	setAcceptDrops(true);
	ChatWidgetManager::instance()->registerChatWidget(this);

	createGui();
	configurationUpdated();

	connect(CurrentChat.chatAccount(), SIGNAL(buddyStatusChanged(Contact, Status)),
			this, SLOT(refreshTitle()));

	foreach (Contact contact, chat.contacts())
		connect(contact.ownerBuddy(), SIGNAL(updated()), this, SLOT(refreshTitle()));


	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();

	ChatWidgetManager::instance()->unregisterChatWidget(this);

//	disconnectAcknowledgeSlots();

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed\n");
}

void ChatWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	vertSplit = new QSplitter(Qt::Vertical, this);

#ifdef Q_OS_MAC
	/* Dorr: workaround for mac tabs issue */
	vertSplit->setAutoFillBackground(true);
#endif

	mainLayout->addWidget(vertSplit);

	horizSplit = new QSplitter(Qt::Horizontal, this);
	horizSplit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	MessagesView = new ChatMessagesView(CurrentChat);

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageUp()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageDown()));
	horizSplit->addWidget(MessagesView);

	// shit, createContactsList needs that
	InputBox = new ChatEditBox(CurrentChat, this);
	InputBox->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));

	if (CurrentChat.contacts().count() > 1)
		createContactsList();

	vertSplit->addWidget(horizSplit);
	vertSplit->addWidget(InputBox);

	connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));

	setFocusProxy(InputBox);
}

void ChatWidget::createContactsList()
{
	QWidget *contactsListContainer = new QWidget(horizSplit);

	QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	BuddiesWidget = new BuddiesListWidget(BuddiesListWidget::FilterAtTop, getChatEditBox(), this);
	BuddiesWidget->view()->setItemsExpandable(false);
	BuddiesWidget->setMinimumSize(QSize(30, 30));
	BuddiesWidget->view()->setModel(new ContactListModel(CurrentChat.contacts().toContactList(), this));
	BuddiesWidget->view()->setRootIsDecorated(false);
	BuddiesWidget->view()->setShowAccountName(false);

	connect(BuddiesWidget->view(), SIGNAL(chatActivated(Chat)),
			Core::instance()->kaduWindow(), SLOT(openChatWindow(Chat)));

	QPushButton *leaveConference = new QPushButton(tr("Leave conference"), contactsListContainer);
	leaveConference->setMinimumWidth(BuddiesWidget->minimumWidth());
	connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));

	layout->addWidget(BuddiesWidget);
	layout->addWidget(leaveConference);

	QList<int> sizes;
	sizes.append(3);
	sizes.append(1);
	horizSplit->setSizes(sizes);
}

void ChatWidget::configurationUpdated()
{
/* TODO: 0.6.6
	if (ContactsWidget)
	{
		ContactsWidget->viewport()->setStyleSheet(QString("QWidget {background-color:%1}").arg(config_file.readColorEntry("Look","UserboxBgColor").name()));
		ContactsWidget->setStyleSheet(QString("QFrame {color:%1}").arg(config_file.readColorEntry("Look","UserboxFgColor").name()));
		ContactsWidget->Q3ListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	}*/

	InputBox->inputBox()->setFont(config_file.readFontEntry("Look","ChatFont"));
 	InputBox->inputBox()->setStyleSheet(QString("QTextEdit {background-color: %1}").arg(config_file.readColorEntry("Look", "ChatTextBgColor").name()));

	refreshTitle();
}

bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (e->matches(QKeySequence::Copy))
	{
		MessagesView->page()->action(QWebPage::Copy)->trigger();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
	{
		clearChatWindow();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
	{
		emit closed();
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
	{
		KaduActions.createAction("whoisAction", InputBox)->activate(QAction::Trigger);
		return true;
	}

	if (HotKey::shortCut(e,"ShortCuts", "kadu_openchatwith"))
	{
		KaduActions.createAction("openChatWithAction", InputBox)->activate(QAction::Trigger);
		return true;
	}

	return false;
}

void ChatWidget::keyPressEvent(QKeyEvent *e)
{
	kdebugf();
	if (keyPressEventHandled(e))
		e->accept();
	else
		QWidget::keyPressEvent(e);
	kdebugf2();
}

void ChatWidget::refreshTitle()
{
	kdebugf();
	QString title;

	int contactsCount = chat().contacts().count();
	kdebugmf(KDEBUG_FUNCTION_START, "chat().contacts().size() = %d\n", contactsCount);
	if (contactsCount > 1)
	{
		title = config_file.readEntry("Look", "ConferencePrefix");
		if (title.isEmpty())
			title = tr("Conference with ");

		QString conferenceContents = config_file.readEntry("Look", "ConferenceContents");
		QStringList contactslist;
		foreach (Contact contact, chat().contacts())
			contactslist.append(Parser::parse(conferenceContents.isEmpty() ? "%a" : conferenceContents, BuddyOrContact(contact), false));

		title.append(contactslist.join(", "));
	}
	else if (contactsCount == 1)
	{
		Contact contact = chat().contacts().toContact();

		if (config_file.readEntry("Look", "ChatContents").isEmpty())
		{
			if (contact.ownerBuddy().isAnonymous())
				title = Parser::parse(tr("Chat with ") + "%a", BuddyOrContact(contact), false);
			else
				title = Parser::parse(tr("Chat with ") + "%a (%s[: %d])", BuddyOrContact(contact), false);
		}
		else
			title = Parser::parse(config_file.readEntry("Look", "ChatContents"), BuddyOrContact(contact), false);
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	setTitle(title);

	kdebugf2();
}

void ChatWidget::setTitle(const QString &title)
{
	if (title != Title)
	{
		Title = title;
		emit titleChanged(this, title);
	}
}

void ChatWidget::appendMessages(const QList<MessageRenderInfo *> &messages, bool pending)
{
	MessagesView->appendMessages(messages);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(MessageRenderInfo *message, bool pending)
{
	MessagesView->appendMessage(message);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor)
{
	Message message = Message::create();
	message.setMessageChat(CurrentChat);
	message.setType(Message::TypeSystem);
	message.setContent(rawContent);
	message.setSendDate(QDateTime::currentDateTime());
	MessageRenderInfo *messageRenderInfo = new MessageRenderInfo(message);
	messageRenderInfo->setBackgroundColor(backgroundColor)
		.setFontColor(fontColor)
		.setNickColor(fontColor);

	MessagesView->appendMessage(messageRenderInfo);
}

/* invoked from outside when new message arrives, this is the window to the world */
void ChatWidget::newMessage(MessageRenderInfo *messageRenderInfo)
{
	MessagesView->appendMessage(messageRenderInfo);

	LastMessageTime = QDateTime::currentDateTime();
	NewMessagesCount++;

 	emit messageReceived(CurrentChat);
}

void ChatWidget::resetEditBox()
{
	InputBox->inputBox()->clear();

	Action *action;
	action = ChatWidgetManager::instance()->actions()->bold()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontWeight(action->isChecked() ? QFont::Bold : QFont::Normal);

	action = ChatWidgetManager::instance()->actions()->italic()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontItalic(action->isChecked());

	action = ChatWidgetManager::instance()->actions()->underline()->action(InputBox);
	if (action)
		InputBox->inputBox()->setFontUnderline(action->isChecked());
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageDialog::ask("", tr("Kadu"), tr("Chat window will be cleared. Continue?")))
	{
		MessagesView->clearMessages();
		activateWindow();
	}
	kdebugf2();
}

/*
void ChatWidget::messageStatusChanged(int messageId, ChatService::MessageStatus status)
{
	if (messageId != myLastMessage.id())
		return;

	switch (status)
	{
		case ChatService::StatusAcceptedDelivered:
		case ChatService::StatusAcceptedQueued:
			writeMyMessage();
			emit messageSentAndConfirmed(CurrentChat, myLastMessage.toHtml());
			disconnectAcknowledgeSlots();
			changeCancelSendToSend();
			return;

		case ChatService::StatusRejectedBlocked:
			MessageDialog::msg("Message blocked", true, "dialog-warning", this);
		case ChatService::StatusRejectedBoxFull:
			MessageDialog::msg("Message box if full", true, "dialog-warning", this);
		case ChatService::StatusRejectedUnknown:
			MessageDialog::msg("Message not delivered", true, "dialog-warning", this);
	}

	cancelMessage();
}*/

void ChatWidget::connectAcknowledgeSlots()
{
	if (!currentProtocol() || !currentProtocol()->chatService())
		return;

	ChatService *chatService = currentProtocol()->chatService();
	if (chatService)
		connect(chatService, SIGNAL(messageStatusChanged(int, ChatService::MessageStatus)),
				this, SLOT(messageStatusChanged(int, ChatService::MessageStatus)));
}

void ChatWidget::disconnectAcknowledgeSlots()
{
	if (!currentProtocol() || !currentProtocol()->chatService())
		return;

	ChatService *chatService = currentProtocol()->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(messageStatusChanged(int, ChatService::MessageStatus)),
				this, SLOT(messageStatusChanged(int, ChatService::MessageStatus)));
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (InputBox->inputBox()->toPlainText().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (!currentProtocol())
		return;

	if (!currentProtocol()->isConnected())
	{
		MessageDialog::show("dialog-error", tr("Kadu"), tr("Cannot send message while being offline.") + tr("Account:") + chat().chatAccount().id(),
				QMessageBox::Ok, this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}
/*
	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		InputBox->inputBox()->setReadOnly(true);
		InputBox->inputBox()->setEnabled(false);
		WaitingForACK = true;

		changeSendToCancelSend();
	}*/

	FormattedMessage message = FormattedMessage::parse(InputBox->inputBox()->document());
	ChatService *chatService = currentProtocol()->chatService();
	if (!chatService || !chatService->sendMessage(CurrentChat, message))
		return;
/*
	if (config_file.readBoolEntry("Chat", "MessageAcks"))
		connectAcknowledgeSlots();
	else
	{*/
	resetEditBox();
	emit messageSentAndConfirmed(CurrentChat, message.toHtml());
// 	}

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	kdebugf2();
}

CustomInput * ChatWidget::edit() const
{
	return InputBox->inputBox();
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == MessagesView)
		return false;

	QList<QUrl> urls = event->mimeData()->urls();

	foreach (const QUrl &url, urls)
	{
		QString file = url.toLocalFile();
		if (!file.isEmpty())
		{
			//is needed to check if file refer to local file?
			QFileInfo fileInfo(file);
			if (fileInfo.exists())
				files.append(file);
		}
	}
	return files.count() > 0;

}

void ChatWidget::dragEnterEvent(QDragEnterEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();

}

void ChatWidget::dragMoveEvent(QDragMoveEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
		e->acceptProposedAction();
}

void ChatWidget::dropEvent(QDropEvent *e)
{
	QStringList files;

	if (decodeLocalFiles(e, files))
	{
		e->acceptProposedAction();

		QStringList::iterator i = files.begin();
		QStringList::iterator end = files.end();

		for (; i != end; i++)
			emit fileDropped(CurrentChat, *i);
	}
}

Protocol *ChatWidget::currentProtocol() const
{
	return CurrentChat.chatAccount().protocolHandler();
}

void ChatWidget::makeActive()
{
	kdebugf();
	QWidget *win = this->window();
	_activateWindow(win);
	kdebugf2();
}

void ChatWidget::markAllMessagesRead()
{
	NewMessagesCount = 0;
}

void ChatWidget::kaduRestoreGeometry()
{
  	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry");

	if (!cgd)
		return;

	QList<int> vertSizes = cgd->widgetVerticalSizes();
	if (vertSizes.empty())
	{
		int h = height() / 3;
		vertSizes.append(h * 2);
		vertSizes.append(h);
	}
	vertSplit->setSizes(vertSizes);

	if (horizSplit)
	{
		QList<int> horizSizes = cgd->widgetHorizontalSizes();
		if (!horizSizes.empty())
			horizSplit->setSizes(horizSizes);
	}
}

void ChatWidget::kaduStoreGeometry()
{
  	ChatGeometryData *cgd = chat().data()->moduleStorableData<ChatGeometryData>("chat-geometry", true);
	if (!cgd)
		return;

	cgd->setWidgetVerticalSizes(vertSplit->sizes());

	if (horizSplit)
		cgd->setWidgetHorizontalSizes(horizSplit->sizes());

	cgd->store();
}

void ChatWidget::leaveConference()
{
	if (!MessageDialog::ask("dialog-warning", tr("Kadu"), tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"), this))
		return;

	if (CurrentChat)
		CurrentChat.setIgnoreAllMessages(true);

	emit closed();
}
