/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

class QSplitter;

class Actions;
class ChatConfigurationHolder;
class ChatEditBoxSizeManager;
class ChatEditBox;
class ChatTopBarContainerWidget;
class ChatTopBarWidgetFactoryRepository;
class ChatTypeManager;
class ChatWidgetActions;
class ChatWidgetTitle;
class Configuration;
class CustomInput;
class FilteredTreeView;
class FormattedStringFactory;
class InjectedFactory;
class MessageManager;
class Protocol;
class SortedMessages;
class TalkableProxyModel;
class WebkitMessagesViewFactory;
class WebkitMessagesView;

class KADUAPI ChatWidget : public QWidget, public ConfigurationAwareObject
{
	Q_OBJECT

	friend class ChatWidgetManager;

	QPointer<Actions> m_actions;
	QPointer<ChatConfigurationHolder> m_chatConfigurationHolder;
	QPointer<ChatEditBoxSizeManager> m_chatEditBoxSizeManager;
	QPointer<ChatTopBarWidgetFactoryRepository> m_chatTopBarWidgetFactoryRepository;
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<ChatWidgetActions> m_chatWidgetActions;
	QPointer<Configuration> m_configuration;
	QPointer<FormattedStringFactory> m_formattedStringFactory;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<MessageManager> m_messageManager;
	QPointer<WebkitMessagesViewFactory> m_webkitMessagesViewFactory;

	Chat CurrentChat;
	ChatTopBarContainerWidget *TopBarContainer;
	owned_qptr<WebkitMessagesView> MessagesView;
	FilteredTreeView *BuddiesWidget;
	TalkableProxyModel *ProxyModel;
	ChatEditBox *InputBox;

	QSplitter *VerticalSplitter;
	QSplitter *HorizontalSplitter;

	QTimer ComposingTimer;
	bool IsComposing;
	ChatState CurrentContactActivity;

	bool SplittersInitialized;

	ChatWidgetTitle *Title;

	QDateTime LastReceivedMessageTime;

	void createGui();
	void createContactsList();

	void resetEditBox();

	bool decodeLocalFiles(QDropEvent *event, QStringList &files);

	void composingStopped();

private slots:
	INJEQT_SET void setActions(Actions *actions);
	INJEQT_SET void setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder);
	INJEQT_SET void setChatEditBoxSizeManager(ChatEditBoxSizeManager *chatEditBoxSizeManager);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setChatWidgetActions(ChatWidgetActions *chatWidgetActions);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setFormattedStringFactory(FormattedStringFactory *formattedStringFactory);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory);
	INJEQT_INIT void init();

	void configurationUpdated();
	void chatUpdated();

	void setUpVerticalSizes();
	void commonHeightChanged(int height);
	void verticalSplitterMoved(int pos, int index);

	void checkComposing();
	void updateComposing();
	void contactActivityChanged(const Contact &contact, ChatState state);

	void keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
 	virtual void showEvent(QShowEvent *e);
	bool keyPressEventHandled(QKeyEvent *);

public:
	explicit ChatWidget(Chat chat, QWidget *parent = 0);
	virtual ~ChatWidget();

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

	ChatWidgetTitle * title() const;

	const QDateTime & lastReceivedMessageTime() const { return LastReceivedMessageTime; }

	void kaduStoreGeometry();
	void kaduRestoreGeometry();

	void addMessages(const SortedMessages &messages);
	void addMessage(const Message &message);
	SortedMessages messages() const;
	int countMessages() const;

	ChatState chatState() const;

public slots:
	void sendMessage();
	void colorSelectorAboutToClose();
	void clearChatWindow();

	void requestClose();

signals:
	void messageReceived(ChatWidget *chatWidget);
	void messageSendRequested(ChatWidget *chat);
	void messageSent(ChatWidget *chat);

	void fileDropped(Chat chat, const QString &fileName);

	void widgetDestroyed(Chat chat);
	void widgetDestroyed(ChatWidget *widget);

	void closeRequested(ChatWidget *chatWidget);
	void chatStateChanged(ChatState state);

};

Q_DECLARE_METATYPE(ChatWidget *);
