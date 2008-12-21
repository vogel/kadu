/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>

#include "accounts/account.h"
#include "accounts/account_manager.h"
#include "action.h"
#include "chat_edit_box.h"
#include "chat_manager.h"
#include "chat_message.h"
#include "color_selector.h"
#include "config_file.h"
#include "custom_input.h"
#include "debug.h"
#include "emoticons.h"
#include "hot_key.h"
#include "icons_manager.h"
#include "ignore.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "message_box.h"
#include "protocols/protocol.h"
#include "userbox.h"
#include "usergroup.h"

#include "contacts/contact-account-data.h"

#include "chat_widget.h"

ChatWidget::ChatWidget(Account *initialAccount, const ContactList &contacts, QWidget *parent)
	: QWidget(parent), CurrentAccount(initialAccount), Contacts(contacts),

	index(0), actcolor(),
	emoticon_selector(0), color_selector(0), WaitingForACK(false), userbox(0), horizSplit(0),
	activationCount(0), NewMessagesCount(0), Edit(0)
{
	kdebugf();
	QList<int> sizes;

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	vertSplit = new QSplitter(Qt::Vertical, this);
	layout->addWidget(vertSplit);

	setAcceptDrops(true);
	/* register us in the chats registry... */
	index = chat_manager->registerChatWidget(this);

	Edit = new ChatEditBox(this);

	if (Contacts.count() > 1)
	{
		horizSplit = new QSplitter(Qt::Horizontal, this, "horizSplit");
		horizSplit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

		body = new ChatMessagesView(horizSplit);

		QWidget *userlistContainer = new QWidget(horizSplit);
		QVBoxLayout *uc_layout = new QVBoxLayout(userlistContainer);
		uc_layout->setMargin(0);
		uc_layout->setSpacing(0);

		UserListElements forUserBox = UserListElements::fromContactList(Contacts, CurrentAccount);
		userbox = new UserBox(Edit, false, new UserGroup(forUserBox), userlistContainer, "userbox");
		userbox->setMinimumSize(QSize(30,30));

		connect(userbox, SIGNAL(doubleClicked(UserListElement)), kadu, SLOT(sendMessage(UserListElement)));
		connect(userbox, SIGNAL(returnPressed(UserListElement)), kadu, SLOT(sendMessage(UserListElement)));
		connect(userbox, SIGNAL(mouseButtonClicked(int, Q3ListBoxItem *, const QPoint &)),
		kadu, SLOT(mouseButtonClicked(int, Q3ListBoxItem *)));

		QPushButton *leaveConference = new QPushButton(tr("Leave conference"), userlistContainer);
		leaveConference->setMinimumWidth(userbox->minimumWidth());
		connect(leaveConference, SIGNAL(clicked()), this, SLOT(leaveConference()));

		uc_layout->addWidget(userbox);
		uc_layout->addWidget(leaveConference);

		sizes.append(3);
		sizes.append(1);
		horizSplit->setSizes(sizes);

		vertSplit->addWidget(horizSplit);
	}
	else
	{
		body = new ChatMessagesView(this);
		vertSplit->addWidget(body);
	}

	vertSplit->addWidget(Edit);

	connect(Edit->inputBox(), SIGNAL(keyPressed(QKeyEvent *, CustomInput *, bool &)), this, SLOT(keyPressedSlot(QKeyEvent *, CustomInput *, bool &)));

	setFocusProxy(Edit);

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return + Qt::CTRL), this);
	connect(shortcut, SIGNAL(activated()), this, SLOT(sendMessage()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageUp + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), body, SLOT(pageUp()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_PageDown + Qt::SHIFT), this);
	connect(shortcut, SIGNAL(activated()), body, SLOT(pageDown()));

	connect(Edit->inputBox(), SIGNAL(cursorPositionChanged()), this, SLOT(curPosChanged()));
	connect(Edit->inputBox(), SIGNAL(sendMessage()), this, SLOT(sendMessage()));
	connect(Edit->inputBox(), SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));

	Edit->installEventFilter(this);

	configurationUpdated();

	connect(chat_manager->colorSelectorActionDescription, SIGNAL(actionCreated(KaduAction *)), this, SLOT(colorSelectorActionCreated(KaduAction *)));


	kdebugf2();
}

ChatWidget::~ChatWidget()
{
	kdebugf();

	chat_manager->unregisterChatWidget(this);

	disconnectAcknowledgeSlots();

	Protocol *gadu = AccountManager::instance()->defaultAccount()->protocol();
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)),
		body, SLOT(imageReceivedAndSaved(UinType,uint32_t,uint32_t,const QString&)));

	if (userbox)
		delete userbox;

	kdebugmf(KDEBUG_FUNCTION_END, "chat destroyed: index %d\n", index);
}

void ChatWidget::configurationUpdated()
{
	if (config_file.readBoolEntry("Chat", "ChatPrune"))
		body->setPrune(config_file.readUnsignedNumEntry("Chat", "ChatPruneLen"));
	else
		body->setPrune(0);

	if (userbox)
	{
		userbox->viewport()->setStyleSheet(QString("QWidget {background-color:%1}").arg(config_file.readColorEntry("Look","UserboxBgColor").name()));
		userbox->setStyleSheet(QString("QFrame {color:%1}").arg(config_file.readColorEntry("Look","UserboxFgColor").name()));
		userbox->Q3ListBox::setFont(config_file.readFontEntry("Look","UserboxFont"));
	}

	Edit->inputBox()->setFont(config_file.readFontEntry("Look","ChatFont"));
 	Edit->inputBox()->setStyleSheet(QString("QTextEdit {background-color: %1}").arg(config_file.readColorEntry("Look", "ChatTextBgColor").name()));
	AutoSend = config_file.readBoolEntry("Chat", "AutoSend");
	Edit->inputBox()->setAutosend(AutoSend);

	setActColor(true);

	refreshTitle();
}

void ChatWidget::specialKeyPressed(int key)
{
 	kdebugf();
 	switch (key)
 	{
		KaduAction *action;
 		case CustomInput::KEY_BOLD:
 			action = chat_manager->boldActionDescription->action(Edit);
			if (action)
				action->setChecked(!action->isChecked());
 			Edit->inputBox()->setBold(action->isChecked());
 			break;
 		case CustomInput::KEY_ITALIC:
 			action = chat_manager->italicActionDescription->action(Edit);
			if (action)
				action->setChecked(!action->isChecked());
 			Edit->inputBox()->setFontItalic(action->isChecked());
 			break;
 		case CustomInput::KEY_UNDERLINE:
 			action = chat_manager->underlineActionDescription->action(Edit);
			if (action)
				action->setChecked(!action->isChecked());
 			Edit->inputBox()->setFontUnderline(action->isChecked());
 			break;
		 case CustomInput::KEY_COPY:
			body->pageAction(QWebPage::Copy)->trigger();
			break;
 	}
 	kdebugf2();
}

void ChatWidget::curPosChanged()
{
	kdebugf();

	KaduAction *action;

	action = chat_manager->boldActionDescription->action(Edit);
 	if (action)
		action->setChecked(Edit->inputBox()->fontWeight() >= QFont::Bold);

	action = chat_manager->italicActionDescription->action(Edit);
 	if (action)
		action->setChecked(Edit->inputBox()->fontItalic());

	action = chat_manager->underlineActionDescription->action(Edit);
 	if (action)
		action->setChecked(Edit->inputBox()->fontUnderline());

	setActColor(false);

	kdebugf2();
}

void ChatWidget::setActColor(bool force)
{
	kdebugf();

	KaduAction *action = chat_manager->colorSelectorActionDescription->action(Edit);

	if (action && (force || (Edit->inputBox()->color() != actcolor)))
	{
		int i;
		for (i = 0; i < 16; ++i)
			if (Edit->inputBox()->color() == QColor(colors[i]))
				break;
		QPixmap p(12, 12);
		if (i >= 16)
			actcolor = Edit->paletteForegroundColor();
		else
			actcolor = colors[i];
		p.fill(actcolor);

		action->setIcon(p);
	}

	kdebugf2();
}

void ChatWidget::insertImage()
{
	kdebugf();

	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	ImageDialog* id = new ImageDialog(this);
	id->setDir(config_file.readEntry("Chat", "LastImagePath"));
	id->setWindowTitle(tr("Insert image"));
	if (id->exec() == QDialog::Accepted)
	{
		config_file.writeEntry("Chat", "LastImagePath", id->dirPath());
		QString selectedFile = id->selectedFile();
		QFileInfo f(selectedFile);
		delete id;
		id = NULL;
		if (!f.isReadable())
		{
			MessageBox::msg(tr("This file is not readable"), true, "Warning", this);
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}

		if (f.size() >= (1 << 18)) // 256kB
		{
			MessageBox::msg(tr("This file is too big (%1 >= %2)").arg(f.size()).arg(1<<18), true, "Warning", this);
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}

		int counter = 0;

		foreach (Contact contact, Contacts)
		{
			// TODO: 0.6.6
			ContactAccountData *contactAccountData = contact.accountData(CurrentAccount);
			if (contactAccountData && contactAccountData->hasFeature(/*EmbedImageInChatMessage*/))
			{
// 				unsigned long maxImageSize = contactAccountData->maxEmbededImageSize();
// 				if (f.size() > maxImageSize)
					counter++;
			}
			else
				counter++;
			// unsigned int maximagesize = user.protocolData("Gadu", "MaxImageSize").toUInt();
		}
		if (counter == 1 && Contacts.count() == 1)
		{
			if (!MessageBox::ask(tr("This file is too big for %1.\nDo you really want to send this image?\n").arg((*users.constBegin()).altNick())))
			{
				QTimer::singleShot(0, this, SLOT(insertImage()));
				kdebugf2();
				return;
			}
		}
		else if (counter > 0 &&
			!MessageBox::ask(tr("This file is too big for %1 of %2 contacts.\nDo you really want to send this image?\nSome of them probably will not get it.").arg(counter).arg(users.count())))
		{
			QTimer::singleShot(0, this, SLOT(insertImage()));
			kdebugf2();
			return;
		}
		Edit->inputBox()->insert(QString("[IMAGE ") + selectedFile + ']');
	}
	else
		delete id;

	kdebugf2();
}

void ChatWidget::colorSelectorActionCreated(KaduAction *action)
{
	kdebugf();
	if (action->parent() == Edit)
		setActColor(true);
	kdebugf2();
}

void ChatWidget::refreshTitle()
{
	kdebugf();
	QString title;

	int uinsSize = Contacts.count();
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	kdebugmf(KDEBUG_FUNCTION_START, "Uins.size() = %d\n", uinsSize);
	if (uinsSize > 1)
	{
		if (config_file.readEntry("Look","ConferencePrefix").isEmpty())
			title = tr("Conference with ");
		else
			title = config_file.readEntry("Look","ConferencePrefix");
		int i = 0;

		if (config_file.readEntry("Look", "ConferenceContents").isEmpty())
			foreach(const UserListElement &user, users)
			{
				title.append(KaduParser::parse("%a", user, false));

				if (++i < uinsSize)
					title.append(", ");
			}
		else
			foreach(const UserListElement &user, users)
			{
				title.append(KaduParser::parse(config_file.readEntry("Look","ConferenceContents"), user, false));

				if (++i < uinsSize)
					title.append(", ");
			}

 		pix = icons_manager->loadPixmap("Online");
	}
	else
	{
		UserListElement user = UserListElement::fromContact(Contacts[0], account());
		if (config_file.readEntry("Look", "ChatContents").isEmpty())
		{
			if (user.isAnonymous())
				title = KaduParser::parse(tr("Chat with ")+"%a", user, false);
			else
				title = KaduParser::parse(tr("Chat with ")+"%a (%s[: %d])", user, false);
		}
		else
			title = KaduParser::parse(config_file.readEntry("Look","ChatContents"), user, false);

		pix = user.status("Gadu").pixmap();
	}

	title.replace("<br/>", " ");
	title.replace("&nbsp;", " ");

	Caption = title;

	// qt treats [*] as 'modified placeholder'
	// we escape each [*] with double [*][*] so it gets properly handled
	EscapedCaption = Caption.replace(QLatin1String("[*]"), QLatin1String("[*][*]"));

	emit captionUpdated();
	kdebugf2();
}


bool ChatWidget::keyPressEventHandled(QKeyEvent *e)
{
	if (HotKey::shortCut(e,"ShortCuts", "chat_clear"))
	{
		clearChatWindow();
		return true;
	}
	else if (HotKey::shortCut(e,"ShortCuts", "chat_close"))
	{
		emit closed();
		return true;
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_searchuser"))
	{
		KaduActions.createAction("whoisAction", Edit)->activate(QAction::Trigger);
		return true;
	}
	else if (HotKey::shortCut(e,"ShortCuts", "kadu_openchatwith"))
	{
		KaduActions.createAction("openChatWithAction", Edit)->activate(QAction::Trigger);
		return true;
	}
	else
		return false;
}

QPixmap ChatWidget::icon()
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


// TODO: remove
bool ChatWidget::eventFilter(QObject *watched, QEvent *ev)
{
//	kdebugmf(KDEBUG_INFO|KDEBUG_FUNCTION_START, "watched: %p, Edit: %p, ev->type():%d, KeyPress:%d\n", watched, Edit, ev->type(), QEvent::KeyPress);

 	if (watched != Edit || ev->type() != QEvent::KeyPress)
 		return QWidget::eventFilter(watched, ev);
 	kdebugf();
 	QKeyEvent *e = static_cast<QKeyEvent *>(ev);
 	if (keyPressEventHandled(e))
 		return true;
 	return QWidget::eventFilter(watched, ev);
}

QDateTime ChatWidget::getLastMsgTime()
{
	return lastMsgTime;
}

void ChatWidget::appendMessages(const QList<ChatMessage *> &messages, bool pending)
{
	body->appendMessages(messages);

	if (pending)
		lastMsgTime = QDateTime::currentDateTime();
}

void ChatWidget::appendMessage(ChatMessage *message, bool pending)
{
	body->appendMessage(message);

	if (pending)
		lastMsgTime = QDateTime::currentDateTime();
}

void ChatWidget::appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor)
{
	ChatMessage *message = new ChatMessage(rawContent, TypeSystem, QDateTime::currentDateTime(),
		backgroundColor, fontColor, fontColor);
	body->appendMessage(message);
}

/* invoked from outside when new message arrives, this is the window to the world */
void ChatWidget::newMessage(Account* account, ContactList senders, const QString &message, time_t time)
{
	QDateTime date;
	date.setTime_t(time);

	UserListElement sender = UserListElement::fromContact(senders[0], account);
	ChatMessage *chatMessage = new ChatMessage(sender, UserListElements(kadu->myself()), message,
			TypeReceived, QDateTime::currentDateTime(), date);
	body->appendMessage(chatMessage);

	lastMsgTime = QDateTime::currentDateTime();
	NewMessagesCount++;

 	emit messageReceived(this);
}

void ChatWidget::writeMyMessage()
{
	kdebugf();

	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	ChatMessage *message = new ChatMessage(kadu->myself(), users, myLastMessage.toHtml(),
			TypeSent, QDateTime::currentDateTime());
	body->appendMessage(message);

	if (!Edit->inputBox()->isEnabled())
		cancelMessage();
	Edit->inputBox()->clear();

	KaduAction *action;
	action = chat_manager->boldActionDescription->action(Edit);
	if (action)
		Edit->inputBox()->setBold(action->isChecked());

	action = chat_manager->italicActionDescription->action(Edit);
	if (action)
		Edit->inputBox()->setFontItalic(action->isChecked());

	action = chat_manager->underlineActionDescription->action(Edit);
	if (action)
		Edit->inputBox()->setFontUnderline(action->isChecked());

	kdebugf2();
}

void ChatWidget::clearChatWindow()
{
	kdebugf();
	if (!config_file.readBoolEntry("Chat", "ConfirmChatClear") || MessageBox::ask(tr("Chat window will be cleared. Continue?")))
	{
		body->clearMessages();
		setActiveWindow();
	}
	kdebugf2();
}

void ChatWidget::setAutoSend(bool auto_send)
{
	kdebugf();
	AutoSend = auto_send;
	if (Edit && Edit->inputBox())
		Edit->inputBox()->setAutosend(auto_send);
	kdebugf2();
}

void ChatWidget::cancelMessage()
{
	kdebugf();
//	seq = 0;
	disconnectAcknowledgeSlots();

	Edit->inputBox()->setReadOnly(false);
	Edit->inputBox()->setEnabled(true);
	Edit->inputBox()->setFocus();

	WaitingForACK = false;

	changeCancelSendToSend();
	kdebugf2();
}

void ChatWidget::messageStatusChanged(int messageId, Protocol::MessageStatus status)
{
	if (messageId != myLastMessage.id())
		return;

	switch (status)
	{
		case Protocol::StatusAcceptedDelivered:
		case Protocol::StatusAcceptedQueued:
			writeMyMessage();
			emit messageSentAndConfirmed(Contacts, myLastMessage.toHtml());
			disconnectAcknowledgeSlots();
			changeCancelSendToSend();
			return;

		case Protocol::StatusRejectedBlocked:
			MessageBox::msg("Message blocked", true, "Warning", this);
		case Protocol::StatusRejectedBoxFull:
			MessageBox::msg("Message box if full", true, "Warning", this);
		case Protocol::StatusRejectedUnknown:
			MessageBox::msg("Message not delivered", true, "Warning", this);
	}

	cancelMessage();
}

void ChatWidget::connectAcknowledgeSlots()
{
// TODO: 0.6.6
// 	connect(CurrentProtocol, SIGNAL(messageStatusChanged(int, Protocol::MessageStatus)),
// 			this, SLOT(messageStatusChanged(int, Protocol::MessageStatus)));
}

void ChatWidget::disconnectAcknowledgeSlots()
{
// TODO: 0.6.6
// 	disconnect(CurrentProtocol, SIGNAL(messageStatusChanged(int, Protocol::MessageStatus)),
// 			this, SLOT(messageStatusChanged(int, Protocol::MessageStatus)));
}

void ChatWidget::changeSendToCancelSend()
{
	KaduAction *action = chat_manager->sendActionDescription->action(Edit);

	if (action)
	{
		action->setIcon(icons_manager->loadIcon("CancelMessage"));
		action->setText(tr("&Cancel"));
	}
}

void ChatWidget::changeCancelSendToSend()
{
	KaduAction *action = chat_manager->sendActionDescription->action(Edit);

	if (action)
	{
		action->setIcon(icons_manager->loadIcon("SendMessage"));
		action->setText(tr("&Send"));
	}
}

/* sends the message typed */
void ChatWidget::sendMessage()
{
	kdebugf();
	if (Edit->inputBox()->toPlainText().isEmpty())
	{
		kdebugf2();
		return;
	}

	emit messageSendRequested(this);

	if (currentProtocol()->currentStatus().isOffline())
	{
		MessageBox::msg(tr("Cannot send message while being offline."), false, "Critical", this);
		kdebugmf(KDEBUG_FUNCTION_END, "not connected!\n");
		return;
	}

	if (config_file.readBoolEntry("Chat","MessageAcks"))
	{
		Edit->inputBox()->setReadOnly(true);
		Edit->inputBox()->setEnabled(false);
		WaitingForACK = true;

		changeSendToCancelSend();
	}

	myLastMessage = Message::parse(Edit->inputBox()->document());

	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	if (!currentProtocol()->sendMessage(users, myLastMessage))
	{
		cancelMessage();
		return;
	}

	if (config_file.readBoolEntry("Chat", "MessageAcks"))
		connectAcknowledgeSlots();
	else
	{
		writeMyMessage();
		emit messageSentAndConfirmed(Contacts, myLastMessage.toHtml());
	}

	emit messageSent(this);
	kdebugf2();
}

void ChatWidget::openEmoticonSelector(const QWidget *activating_widget)
{
	//emoticons_selector zawsze b�dzie NULLem gdy wchodzimy do tej funkcji
	//bo EmoticonSelector ma ustawione flagi Qt::WDestructiveClose i Qt::WType_Popup
	//akcj� na opuszczenie okna jest ustawienie zmiennej emoticons_selector w Chacie na NULL
	emoticon_selector = new EmoticonSelector(this, this);
	emoticon_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	emoticon_selector->show();
}

void ChatWidget::changeColor(const QWidget *activating_widget)
{
	//sytuacja podobna jak w przypadku emoticon_selectora
	color_selector = new ColorSelector(Edit->paletteForegroundColor(), this);
	color_selector->alignTo(const_cast<QWidget*>(activating_widget)); //TODO: do something about const_cast
	color_selector->show();
	connect(color_selector, SIGNAL(colorSelect(const QColor&)), this, SLOT(colorChanged(const QColor&)));
	connect(color_selector, SIGNAL(aboutToClose()), this, SLOT(colorSelectorAboutToClose()));
}

void ChatWidget::colorSelectorAboutToClose()
{
	kdebugf();
	color_selector = 0;
	kdebugf2();
}

void ChatWidget::colorChanged(const QColor &color)
{
	color_selector = 0;

	QPixmap p(12, 12);
	p.fill(color);

	KaduAction *action = chat_manager->colorSelectorActionDescription->action(Edit);
	if (action)
		action->setIcon(p);

	Edit->inputBox()->setColor(color);
	actcolor = color;
}

/* adds an emoticon code to the edit window */
void ChatWidget::addEmoticon(QString emot)
{
	if (emot.length())
	{
		emot.replace("&lt;", "<");
		emot.replace("&gt;", ">");
		Edit->inputBox()->insert(emot);
	}
	emoticon_selector = NULL;
}


const UserGroup *ChatWidget::users() const
{
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	return new UserGroup(users);
}

const QString& ChatWidget::caption() const
{
 	return Caption;
}

const QString& ChatWidget::escapedCaption() const
{
 	return EscapedCaption;
}

CustomInput* ChatWidget::edit()
{
	return Edit->inputBox();
}

UserBox* ChatWidget::getUserbox()
{
	return userbox;
}

bool ChatWidget::autoSend() const
{
	return AutoSend;
}

bool ChatWidget::waitingForACK() const
{
	return WaitingForACK;
}

bool ChatWidget::decodeLocalFiles(QDropEvent *event, QStringList &files)
{
	if (!event->mimeData()->hasUrls() || event->source() == body)
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

	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	if (decodeLocalFiles(e, files))
	{
		e->acceptProposedAction();

		QStringList::iterator i = files.begin();
		QStringList::iterator end = files.end();

		for (; i != end; i++)
			emit fileDropped(new UserGroup(users), *i);
	}
}

Protocol *ChatWidget::currentProtocol()
{
	return CurrentAccount->protocol();
}

// TODO: do dupy, zmieni� przed 0.6
void ChatWidget::makeActive()
{
	kdebugf();
	QWidget *win = this->window();
	win->setActiveWindow();
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
{
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	QList<int> vertSizes = toIntList(chat_manager->chatWidgetProperty(new UserGroup(users), "VerticalSizes").toList());
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
		QList<int> horizSizes = toIntList(chat_manager->chatWidgetProperty(new UserGroup(users), "HorizontalSizes").toList());
		if (!horizSizes.empty())
			horizSplit->setSizes(horizSizes);
	}
}

void ChatWidget::kaduStoreGeometry()
{
	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	QList<int> sizes = vertSplit->sizes();
	chat_manager->setChatWidgetProperty(new UserGroup(users), "VerticalSizes", toVariantList(sizes));

	if (users.count() == 1)
		(*users.begin()).setData("VerticalSizes", QString("%1,%2").arg(sizes[0]).arg(sizes[1]));

	if (horizSplit)
		chat_manager->setChatWidgetProperty(new UserGroup(users), "HorizontalSizes", toVariantList(horizSplit->sizes()));
}

void ChatWidget::leaveConference()
{
	if (!MessageBox::ask(tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"), "Warning", this))
		return;

	UserListElements users = UserListElements::fromContactList(Contacts,
			AccountManager::instance()->defaultAccount());

	if (!IgnoredManager::isIgnored(users))
		IgnoredManager::insert(users);

	emit closed();
}

void ChatWidget::keyPressedSlot(QKeyEvent *e, CustomInput *sender, bool &handled)
{
	emit keyPressed(e, this, handled);
}
