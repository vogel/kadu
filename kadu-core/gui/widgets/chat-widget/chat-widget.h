/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtGui/QWidget>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

class QSplitter;

class ChatEditBox;
class WebkitMessagesView;
class ChatTopBarContainerWidget;
class ChatWidget;
class CustomInput;
class FilteredTreeView;
class FormattedStringFactory;
class Protocol;
class SortedMessages;
class TalkableProxyModel;

class KADUAPI ChatWidget : public QWidget, public ConfigurationAwareObject
{
	Q_OBJECT

	friend class ChatWidgetManager;

	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

	Chat CurrentChat;

	ChatTopBarContainerWidget *TopBarContainer;
	qobject_ptr<WebkitMessagesView> MessagesView;
	FilteredTreeView *BuddiesWidget;
	TalkableProxyModel *ProxyModel;
	ChatEditBox *InputBox;

	QSplitter *VerticalSplitter;
	QSplitter *HorizontalSplitter;

	QTimer ComposingTimer;
	bool IsComposing;
	ChatStateService::State CurrentContactActivity;

	bool SplittersInitialized;

	QString Title;

	QDateTime LastReceivedMessageTime;
	int UnreadMessagesCount;

	void createGui();
	void createContactsList();

	void resetEditBox();

	bool decodeLocalFiles(QDropEvent *event, QStringList &files);

	void composingStopped();

private slots:
	void configurationUpdated();

	void setUpVerticalSizes();
	void commonHeightChanged(int height);
	void verticalSplitterMoved(int pos, int index);

	void checkComposing();
	void updateComposing();
	void contactActivityChanged(const Contact &contact, ChatStateService::State state);

	void keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
 	virtual void showEvent(QShowEvent *e);
	bool keyPressEventHandled(QKeyEvent *);

public:
	explicit ChatWidget(const Chat &chat, QWidget *parent = 0);
	virtual ~ChatWidget();

	void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);

	Chat chat() const { return CurrentChat; }

	void appendSystemMessage(const QString &content);
	void appendSystemMessage(std::unique_ptr<FormattedString> &&content);

	void repaintMessages();

	CustomInput * edit() const;
	TalkableProxyModel * talkableProxyModel() const;
	ChatEditBox * getChatEditBox() const { return InputBox; }
	WebkitMessagesView * chatMessagesView() const { return MessagesView.get(); }

	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dropEvent(QDropEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);

	Protocol * currentProtocol() const;

	const QString & title() { return Title; }
	void setTitle(const QString &title);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Icon of chat.
	 *
	 * Chat icon is used to display in window titles. For 'contact' chats it is icon of status
	 * of peer, for 'conference' chats it is generic icon.
	 */
	QIcon icon();

	const QDateTime & lastReceivedMessageTime() const { return LastReceivedMessageTime; }

	void kaduStoreGeometry();
	void kaduRestoreGeometry();

	int countMessages() const;

	void setUnreadMessagesCount(int unreadMessagesCount);
	int unreadMessagesCount() const;

public slots:
	void appendMessages(const SortedMessages &messages);
	void appendMessage(const Message &message);

	void sendMessage();
	void colorSelectorAboutToClose();
	void clearChatWindow();

	void requestClose();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Updates chat title.
	 */
	void refreshTitle();

signals:
	void messageReceived(ChatWidget *chatWidget);
	void messageSendRequested(ChatWidget *chat);
	void messageSent(ChatWidget *chat);

	void fileDropped(Chat chat, const QString &fileName);

	void iconChanged();
	void titleChanged(ChatWidget *chatWidget, const QString &newTitle);

	void widgetDestroyed(Chat chat);
	void widgetDestroyed(ChatWidget *widget);

	void closeRequested(ChatWidget *chatWidget);
	void unreadMessagesCountChanged(ChatWidget *chatWidget);

};

Q_DECLARE_METATYPE(ChatWidget *);

#endif // CHAT_WIDGET_H
