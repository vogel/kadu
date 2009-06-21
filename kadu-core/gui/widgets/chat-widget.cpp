/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QFileInfo>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "chat/chat-manager.h"
#include "chat/chat_message.h"
#include "configuration/configuration-file.h"
#include "contacts/contact-account-data.h"
#include "contacts/model/contact-list-model.h"
#include "core/core.h"
#include "gui/actions/action.h"
#include "gui/actions/actions.h"
#include "gui/widgets/chat-widget-actions.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/widgets/color-selector.h"
#include "gui/widgets/contacts-list-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-box.h"
#include "protocols/protocol.h"

#include "chat-edit-box.h"
#include "custom_input.h"
#include "debug.h"
#include "emoticons.h"
#include "hot_key.h"
#include "icons-manager.h"
#include "kadu_parser.h"
#include "misc/misc.h"

#include "chat-widget.h"

ChatWidget::ChatWidget(Chat *chat, QWidget *parent) :
		QWidget(parent), CurrentChat(chat),
		WaitingForACK(false), ContactsWidget(0), horizSplit(0),
		activationCount(0), NewMessagesCount(0), SelectionFromMessagesView(true), InputBox(0)
{
	kdebugf();

	setAcceptDrops(true);
	ChatWidgetManager::instance()->registerChatWidget(this);

	triggerAllAccountsRegistered();
	createGui();
	configurationUpdated();

	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();

	triggerAllAccountsUnregistered();

	ChatWidgetManager::instance()->unregisterChatWidget(this);

	disconnectAcknowledgeSlots();

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed\n");
}

void ChatWidget::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);

	vertSplit = new QSplitter(Qt::Vertical, this);
	mainLayout->addWidget(vertSplit);

	horizSplit = new QSplitter(Qt::Horizontal, this);
	horizSplit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	MessagesView = new ChatMessagesView(this);
	connect(MessagesView, SIGNAL(selectionChanged()), this, SLOT(messagesViewSelectionChanged()));

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return + Qt::CTRL), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(sendMessage()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageUp()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), MessagesView, SLOT(pageDown()));
	horizSplit->addWidget(MessagesView);

	if (CurrentChat->contacts().count() > 1)
		createContactsList();

	vertSplit->addWidget(horizSplit);

	InputBox = new ChatEditBox(CurrentChat, this);
	vertSplit->addWidget(InputBox);

	connect(InputBox, SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)),
			this, SLOT(editBoxKeyPressed(QKeyEvent *, CustomInput *, bool &)));
	connect(InputBox->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(InputBox->inputBox(), SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));
	InputBox->installEventFilter(this);

	setFocusProxy(InputBox);
}

void ChatWidget::createContactsList()
{
	QWidget *contactsListContainer = new QWidget(horizSplit);

	QVBoxLayout *layout = new QVBoxLayout(contactsListContainer);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	ContactsWidget = new ContactsListWidget(getChatEditBox(), this);
	ContactsWidget->setModel(new ContactListModel(CurrentChat->contacts().toContactList(), this));
	ContactsWidget->setMinimumSize(QSize(30, 30));

	connect(ContactsWidget, SIGNAL(contactActivated(Contact)),
			Core::instance()->kaduWindow(), SLOT(sendMessage(Contact)));

	QPushButton *leaveConference = new QPushButton(tr("Leave conference"), contactsListContainer);
	leaveConference->setMinimumWidth(ContactsWidget->minimumWidth());
	connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));

	layout->addWidget(ContactsWidget);
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
}

bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (e->modifiers() == Qt::ControlModifier && (e->key() == 'c' || e->key() == 'C'))
	{
		if (SelectionFromMessagesView)
		{
			MessagesView->page()->action(QWebPage::Copy)->trigger();
			return true;
		}
		else
			return false;
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

QIcon ChatWidget::icon()
{
	return pix;
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

void ChatWidget::accountRegistered(Account *account)
{
	connect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(onStatusChanged(Account *, Contact, Status)));
}

void ChatWidget::accountUnregistered(Account *account)
{
	disconnect(account, SIGNAL(contactStatusChanged(Account *, Contact, Status)),
			this, SLOT(onStatusChanged(Account *, Contact, Status)));
}

void ChatWidget::onStatusChanged(Account *account, Contact contact, Status oldStatus)
{
// TODO: fix
	if (account != CurrentChat->account() && (*CurrentChat->contacts().begin()) != contact)
		return;
}

// TODO: remove
bool ChatWidget::eventFilter(QObject *watched, QEvent *ev)
{
//	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_START, "watched: %p, Edit: %p, ev->type():%d, KeyPress:%d\n", watched, Edit, ev->type(), QEvent::KeyPress);

 	if (watched != InputBox || ev->type() != QEvent::KeyPress)
 		return QWidget::eventFilter(watched, ev);
 	kdebugf();
 	QKeyEvent *e = static_cast<QKeyEvent *>(ev);
 	if (keyPressEventHandled(e))
 		return true;
 	return QWidget::eventFilter(watched, ev);
}

QDateTime ChatWidget::lastMessageTime()
{
	return LastMessageTime;
}

void ChatWidget::appendMessages(const QList<ChatMessage *> &messages, bool pending)
{
	MessagesView->appendMessages(messages);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(ChatMessage *message, bool pending)
{
	MessagesView->appendMessage(message);

	if (pending)
		LastMessageTime = QDateTime::currentDateTime();
}

void ChatWidget::appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor)
{
	ChatMessage *message = new ChatMessage(rawContent, TypeSystem, QDateTime::currentDateTime(),
		backgroundColor, fontColor, fontColor);
	MessagesView->appendMessage(message);
}

/* invoked from outside when new message arrives, this is the window to the world */
void ChatWidget::newMessage(ChatMessage *chatMessage)
{
	MessagesView->appendMessage(chatMessage);

	LastMessageTime = QDateTime::currentDateTime();
	NewMessagesCount++;

 	emit messageReceived(CurrentChat);
}

void ChatWidget::writeMyMessage()
{
	kdebugf();

	Message msg;
	msg.chat = CurrentChat;
	msg.sender = Core::instance()->myself();
	msg.messageContent = myLastMessage.toHtml();
	msg.sendDate = QDateTime::currentDateTime();
	msg.receiveDate = QDateTime();

	ChatMessage *message = new ChatMessage(msg, TypeSent);
	MessagesView->appendMessage(message);

	if (!InputBox->inputBox()->isEnabled())
		cancelMessage();
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

	kdebugf2();
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageBox::ask(tr("Chat window will be cleared. Continue?")))
	{
		MessagesView->clearMessages();
		activateWindow();
	}
	kdebugf2();
}

void ChatWidget::cancelMessage()
{
	kdebugf();
//	seq = 0;
	disconnectAcknowledgeSlots();

	InputBox->inputBox()->setReadOnly(false);
	InputBox->inputBox()->setEnabled(true);
	InputBox->inputBox()->setFocus();

	WaitingForACK = false;

	changeCancelSendToSend();
	kdebugf2();
}

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
			MessageBox::msg("Message blocked", true, "Warning", this);
		case ChatService::StatusRejectedBoxFull:
			MessageBox::msg("Message box if full", true, "Warning", this);
		case ChatService::StatusRejectedUnknown:
			MessageBox::msg("Message not delivered", true, "Warning", this);
	}

	cancelMessage();
}

void ChatWidget::connectAcknowledgeSlots()
{
	ChatService *chatService = CurrentChat->account()->protocol()->chatService();
	if (chatService)
		connect(chatService, SIGNAL(messageStatusChanged(int, ChatService::MessageStatus)),
				this, SLOT(messageStatusChanged(int, ChatService::MessageStatus)));
}

void ChatWidget::disconnectAcknowledgeSlots()
{
	ChatService *chatService = CurrentChat->account()->protocol()->chatService();
	if (chatService)
		disconnect(chatService, SIGNAL(messageStatusChanged(int, ChatService::MessageStatus)),
				this, SLOT(messageStatusChanged(int, ChatService::MessageStatus)));
}

void ChatWidget::changeSendToCancelSend()
{
	Action *action = ChatWidgetManager::instance()->actions()->send()->action(InputBox);

	if (action)
	{
		action->setIcon(IconsManager::instance()->loadIcon("CancelMessage"));
		action->setText(tr("&Cancel"));
	}
}

void ChatWidget::changeCancelSendToSend()
{
	Action *action = ChatWidgetManager::instance()->actions()->send()->action(InputBox);

	if (action)
	{
		action->setIcon(IconsManager::instance()->loadIcon("SendMessage"));
		action->setText(tr("&Send"));
	}
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

	if (!currentProtocol()->isConnected())
	{
		MessageBox::msg(tr("Cannot send message while being offline."), false, "Critical", this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		InputBox->inputBox()->setReadOnly(true);
		InputBox->inputBox()->setEnabled(false);
		WaitingForACK = true;

		changeSendToCancelSend();
	}

	myLastMessage = FormattedMessage::parse(InputBox->inputBox()->document());

	ChatService *chatService = currentProtocol()->chatService();

	if (!chatService || !chatService->sendMessage(CurrentChat, myLastMessage))
	{
		cancelMessage();
		return;
	}

	if (config_file.readBoolEntry("Chat", "MessageAcks"))
		connectAcknowledgeSlots();
	else
	{
		writeMyMessage();
		emit messageSentAndConfirmed(CurrentChat, myLastMessage.toHtml());
	}

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	kdebugf2();
}

CustomInput * ChatWidget::edit()
{
	return InputBox->inputBox();
}

bool ChatWidget::waitingForACK() const
{
	return WaitingForACK;
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == ContactsWidget)
		return false;

	QList<QUrl> urls = event->mimeData()->urls();

	foreach(const QUrl &url, urls)
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

Protocol *ChatWidget::currentProtocol()
{
	return CurrentChat->account()->protocol();
}

// TODO: do dupy, zmieni� przed 0.6
void ChatWidget::makeActive()
{
	kdebugf();
	QWidget *win = this->window();
	win->activateWindow();
	// workaround for kwin which sometimes don't make window active when it's requested right after "unminimization"
	if (!win->isActiveWindow() && activationCount++ < 20)
		QTimer::singleShot(100, this, SLOT(makeActive()));
	else
		activationCount = 0;
	kdebugf2();
}

void ChatWidget::markAllMessagesRead()
{
	NewMessagesCount = 0;
}

unsigned int ChatWidget::newMessagesCount() const
{
	return NewMessagesCount;
}

void ChatWidget::kaduRestoreGeometry()
{// TODO: 0.6.6
/*
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	QList<int> vertSizes = toIntList(chat_manager->chatWidgetProperty(Contacts, "VerticalSizes").toList());
	if (vertSizes.empty() && users.count() == 1)
	{
		QString vert_sz_str = (*(users.constBegin())).data("VerticalSizes").toString();
		if (!vert_sz_str.isEmpty())
		{
			bool ok[2];
			QStringList s = QStringList::split(",", vert_sz_str);
			vertSizes.append(s[0].toInt(ok));
			vertSizes.append(s[1].toInt(ok + 1));
			if (!(ok[0] && ok[1]))
				vertSizes.clear();
		}
	}

	if (vertSizes.empty())
	{
		int h = height() / 3;
		vertSizes.append(h * 2);
		vertSizes.append(h);
	}
	vertSplit->setSizes(vertSizes);

	if (horizSplit)
	{
		QList<int> horizSizes = toIntList(chat_manager->chatWidgetProperty(Contacts, "HorizontalSizes").toList());
		if (!horizSizes.empty())
			horizSplit->setSizes(horizSizes);
	}*/
}

void ChatWidget::kaduStoreGeometry()
{/* TODO: 0.6.6
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	QList<int> sizes = vertSplit->sizes();
	chat_manager->setChatWidgetProperty(Contacts, "VerticalSizes", toVariantList(sizes));

	if (users.count() == 1)
		users[0].setData("VerticalSizes", QString("%1,%2").arg(sizes[0]).arg(sizes[1]));

	if (horizSplit)
		chat_manager->setChatWidgetProperty(Contacts, "HorizontalSizes", toVariantList(horizSplit->sizes()));*/
}

void ChatWidget::leaveConference()
{
	if (!MessageBox::ask(tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"), "Warning", this))
		return;

// TODO: 0.6.6
// 	if (!IgnoredHelper::isIgnored(Contacts))
// 		IgnoredHelper::insert(Contacts);

	emit closed();
}

void ChatWidget::editBoxKeyPressed(QKeyEvent *e, CustomInput *sender, bool &handled)
{
	keyPressEvent(e);

	emit keyPressed(e, this, handled);
}

void ChatWidget::messagesViewSelectionChanged()
{
	printf("messages view selection change\n");
	SelectionFromMessagesView = true;
}

void ChatWidget::editBoxSelectionChanged()
{
	printf("edit box selection\n");
	SelectionFromMessagesView = false;
}
