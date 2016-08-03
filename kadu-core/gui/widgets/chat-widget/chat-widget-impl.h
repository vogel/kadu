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

#include "gui/widgets/chat-widget/chat-widget.h"

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <injeqt/injeqt.h>

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
class IconsManager;
class InjectedFactory;
class KaduWindowService;
class MessageManager;
class MessageStorage;
class Protocol;
class SortedMessages;
class TalkableProxyModel;
class WebkitMessagesViewFactory;
class WebkitMessagesView;

class KADUAPI ChatWidgetImpl : public ChatWidget, public ConfigurationAwareObject
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
	QPointer<IconsManager> m_iconsManager;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<KaduWindowService> m_kaduWindowService;
	QPointer<MessageManager> m_messageManager;
	QPointer<MessageStorage> m_messageStorage;
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
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setKaduWindowService(KaduWindowService *kaduWindowService);
	INJEQT_SET void setMessageManager(MessageManager *messageManager);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
	INJEQT_SET void setWebkitMessagesViewFactory(WebkitMessagesViewFactory *webkitMessagesViewFactory);
	INJEQT_INIT void init();

	virtual void configurationUpdated() override;
	void chatUpdated();

	void setUpVerticalSizes();
	void commonHeightChanged(int height);
	void verticalSplitterMoved(int pos, int index);

	void checkComposing();
	void updateComposing();
	void contactActivityChanged(const Contact &contact, ChatState state);

	void keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled);

protected:
	virtual void keyPressEvent(QKeyEvent *e) override;
	virtual void resizeEvent(QResizeEvent *e) override;
 	virtual void showEvent(QShowEvent *e) override;
	bool keyPressEventHandled(QKeyEvent *);

public:
	explicit ChatWidgetImpl(Chat chat, QWidget *parent = nullptr);
	virtual ~ChatWidgetImpl();

	virtual Chat chat() const override { return CurrentChat; }

	virtual void appendSystemMessage(QString htmlContent) override;

	virtual CustomInput * edit() const override;
	virtual TalkableProxyModel * talkableProxyModel() const override;
	virtual ChatEditBox * getChatEditBox() const override { return InputBox; }
	virtual WebkitMessagesView * chatMessagesView() const override { return MessagesView.get(); }

	virtual void dragEnterEvent(QDragEnterEvent *e) override;
	virtual void dropEvent(QDropEvent *e) override;
	virtual void dragMoveEvent(QDragMoveEvent *e) override;

	Protocol * currentProtocol() const;

	virtual ChatWidgetTitle * title() const override;

	virtual const QDateTime & lastReceivedMessageTime() const override { return LastReceivedMessageTime; }

	virtual void kaduStoreGeometry() override;
	virtual void kaduRestoreGeometry() override;

	virtual void addMessages(const SortedMessages &messages) override;
	virtual void addMessage(const Message &message) override;
	virtual SortedMessages messages() const override;
	int countMessages() const;

	virtual ChatState chatState() const override;

public slots:
	virtual void sendMessage() override;
	virtual void colorSelectorAboutToClose() override;
	virtual void clearChatWindow() override;

	virtual void requestClose() override;

};
