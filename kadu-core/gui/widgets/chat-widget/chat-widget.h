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

#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "message/message.h"
#include "misc/memory.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

class QSplitter;

class ChatEditBox;
class ChatTopBarContainerWidget;
class ChatWidgetTitle;
class CustomInput;
class FilteredTreeView;
class FormattedStringFactory;
class Protocol;
class SortedMessages;
class TalkableProxyModel;
class WebkitMessagesView;

class KADUAPI ChatWidget : public QWidget, public ConfigurationAwareObject
{
	Q_OBJECT

	friend class ChatWidgetManager;

	QPointer<FormattedStringFactory> CurrentFormattedStringFactory;

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

#endif // CHAT_WIDGET_H
