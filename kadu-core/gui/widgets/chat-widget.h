/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "buddies/buddy-list.h"
#include "chat/chat.h"
#include "chat/message/formatted-message.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/chat-messages-view.h"
#include "protocols/services/chat-service.h"
#include "protocols/services/chat-state-service.h"
#include "exports.h"

class QSplitter;

class ChatEditBox;
class ChatWidget;
class CustomInput;
class MessageRenderInfo;
class Protocol;

class KADUAPI ChatWidget : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	friend class ChatWidgetManager;

	Chat CurrentChat;

	ChatMessagesView *MessagesView;
	BuddiesListWidget *BuddiesWidget;
	ChatEditBox *InputBox;

	QSplitter *VerticalSplitter;
	QSplitter *HorizontalSplitter;

	QTimer ComposingTimer;
	bool IsComposing;
	ChatStateService::ContactActivity CurrentContactActivity;

	bool SplittersInitialized;

	QString Title;

	QDateTime LastMessageTime;

	unsigned int NewMessagesCount;
	bool SelectionFromMessagesView;

	void createGui();
	void createContactsList();

	void resetEditBox();

	bool decodeLocalFiles(QDropEvent *event, QStringList &files);

private slots:
	void connectAcknowledgeSlots();
	void disconnectAcknowledgeSlots();

	void setUpVerticalSizes();
	void commonHeightChanged(int height);
	void verticalSplitterMoved(int pos, int index);

	void checkComposing();
	void updateComposing();
	void contactActivityChanged(ChatStateService::ContactActivity state, const Contact &contact);

	void keyPressedSlot(QKeyEvent *e, CustomInput *input, bool &handled);

protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
 	virtual void showEvent(QShowEvent *e);
	bool keyPressEventHandled(QKeyEvent *);

	virtual void configurationUpdated();

public:
	explicit ChatWidget(const Chat &chat, QWidget *parent = 0);
	virtual ~ChatWidget();

	Chat chat() const { return CurrentChat; }

	/**
		Dodaje now� wiadomos� systemow� do okna.

		@param rawContent tre�� wiadomo�ci w postaci HTML
		@param backgroundColor kolor t�a wiadomo�ci (format HTML)
		@param fontColor kolor wiadomo�ci (format HTML)
	 **/
	void appendSystemMessage(const QString &rawContent, const QString &backgroundColor, const QString &fontColor);

	/**
		\fn void newMessage(Account account, ContactList senders, const QString &message, time_t time)
		Add new message to window

		\param account account on which the message was received
		\param senders list of sender
		\param message message content
		\param time czas
		**/
	void newMessage(MessageRenderInfo *message);

	/**
		\fn void repaintMessages()
		Od�wie�a zawarto�� okna uwzgl�dniaj�c ewentualne
		zmiany dokonane w kt�rej� wiadomo�ci z listy
		uzyskanej za pomoc� metody chatMessages(),
		dodanie nowych wiadomo�ci lub usuni�cie istniej�cych.
	**/
	void repaintMessages();

	CustomInput * edit() const;
	BuddiesListView * contactsListWidget() const { return BuddiesWidget ? BuddiesWidget->view() : 0; }
	ChatEditBox * getChatEditBox() const { return InputBox; }
	ChatMessagesView * chatMessagesView() const { return MessagesView; }

	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dropEvent(QDropEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);

	Protocol * currentProtocol() const;

	unsigned int newMessagesCount() const { return NewMessagesCount; }

	const QString & title() { return Title; }
	void setTitle(const QString &title);

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Icon of chat.
	 *
	 * Chat icon is used to display in window titles. For 'simple' chats it is icon of status
	 * of peer, for 'conference' chats it is generic icon.
	 */
	QIcon icon();

	const QDateTime & lastMessageTime() const { return LastMessageTime; }

	void kaduStoreGeometry();
	void kaduRestoreGeometry();

	unsigned int countMessages() const { return MessagesView->countMessages(); }

public slots:
// 	void messageStatusChanged(int messageId, ChatService::MessageStatus status);

	/**
		\fn void appendMessages(const QValueList<MessageRenderInfo *> &)
		Slot dodaj wiadomo�ci do okna
		\param messages lista wiadomo�ci
	**/
	void appendMessages(const QList<MessageRenderInfo *> &, bool pending = false);

	/**
	\fn void appendMessage(MessageRenderInfo *)
		Slot dodaj wiadomo�� do okna
		\param messages lista wiadomo�ci
	**/
	void appendMessage(MessageRenderInfo *, bool pending = false);

	/**
		\fn void sendMessage()
		Slot wywo�ywany po naci�ni�ciu przycisku
		do wysy�ania wiadomo�ci
	**/
	void sendMessage();

	/**
		\fn void colorSelectorAboutToClose()
		Slot zostaje wywo�any przy zamykaniu okna wyboru ikonek
	**/
	void colorSelectorAboutToClose();

	/**
		\fn void clearChatWindow()
		Slot czyszcz�cy okno rozmowy
	**/
	void clearChatWindow();

	void makeActive();
	void markAllMessagesRead();

	void leaveConference();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Updates chat title.
	 */
	void refreshTitle();

signals:
	/**
		\fn void messageSendRequested(Chat* chat)
		Sygnal jest emitowany gdy uzytkownik wyda polecenie
		wyslania wiadomosci, np klikajac na guzik "wyslij".
		\param chat wska�nik do okna kt�re emituje sygna�
	**/
	void messageSendRequested(ChatWidget *chat);

	/**
		\fn void messageSent(Chat* chat)
		Sygnal jest emitowany gdy zakonczy sie proces
		wysylania wiadomosci i zwiazanych z tym czynnosci.
		Oczywiscie nie wiemy czy wiadomosc dotarla.
		\param chat wska�nik do okna rozmowy,
		 kt�re emitowa�o sygna�
	**/
	void messageSent(ChatWidget *chat);

	/**
		\fn void messageSentAndConfirmed(ContactList receivers, const QString& message)
		This signal is emitted when message was sent
		and it was confirmed.
		When confirmations are turned off signal is
		emitted immediately after message was send
		just like messageSent() signal.
		\param receivers list of receivers
		\param message the message
	**/
	void messageSentAndConfirmed(Chat chat, const QString &message);
	void messageReceived(Chat chat);

	/**
		\fn void fileDropped(const UserGroupusers, const QString& fileName)
		Sygna� jest emitowany, gdy w oknie Chat
		upuszczono plik.
		\param users lista u�ytkownik�w
		\param fileName nazwa pliku
	**/
	void fileDropped(Chat chat, const QString &fileName);

	void iconChanged();
	void titleChanged(ChatWidget *chatWidget, const QString &newTitle);
	void closed();
};

/**
	@class ChatContainer
	@brief Klasa abstrakcyjna opisuj�ca rodzica klasy ChatWidget.

	Klasa abstrakcyjna z kt�rej powinny dziedziczy� klasy b�d�ce rodzicami obiekt�w
	klasy ChatWidget.

	Informuje kt�ry chat powinien zosta� zamkni�ty w przypadku np. ignorowania kontaktu
	z kt�rym prowadzona jest rozmowa
**/
class ChatContainer
{
public:
	ChatContainer() {}
	virtual ~ChatContainer() {}

	/**
		Metoda informuj�ca, kt�ry chat powinien zosta� zamkni�ty
	 **/
	virtual void closeChatWidget(ChatWidget *chat) = 0;
};

#endif // CHAT_WIDGET_H
