/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef CHAT_WIDGET_MANAGER
#define CHAT_WIDGET_MANAGER

#include <QtCore/QTimer>

#include "buddies/buddy-list.h"
#include "configuration/configuration-aware-object.h"
#include "gui/widgets/chat-widget.h"
#include "storage/storable-string-list.h"

#include "exports.h"

class ActionDescription;
class ChatWidgetActions;
class Protocol;

/**
	Klasa pozwalaj�ca zarz�dza� otwartymi oknami rozm�w: otwiera�,
	zamykac, szuka� okna ze wgl�du na list� u�ytkownik�w itp.
	\class ChatManagerOld
	\brief Klasa zarz�dzaj�ca oknami ChatWidget
**/

class KADUAPI ChatWidgetManager : public QObject, ConfigurationAwareObject, StorableStringList
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

	static ChatWidgetManager *Instance;

	ChatWidgetActions *Actions;

	QHash<Chat , ChatWidget *> Chats;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

	void autoSendActionCheck();
	void insertEmoticonActionEnabled();

	ChatWidget * openChatWidget(const Chat &chat, bool forceActivate = false);

private slots:
	void messageSent(const Message &message);

protected:
	virtual void load();

	virtual void configurationUpdated();

public:
	static ChatWidgetManager * instance();

	virtual StorableObject * storageParent();
	virtual QString storageNodeName();
	virtual QString storageItemNodeName();

	ChatWidgetActions * actions() { return Actions; }

	const QHash<Chat , ChatWidget *> & chats() const;

	ChatWidget * byChat(const Chat &chat, bool create = false) const;

	virtual void store();

	void activateChatWidget(ChatWidget *chatwidget, bool forceActivate);

	void openPendingMessages(const Chat &chat, bool forceActivate = false);

	void openPendingMessages(bool forceActivate = false);

	/**
		\fn void deletePendingMsgs(ContactList users)
		Funkcja usuwa zakolejkowane wiadomo�ci
		z u�ytkownikami "users"
		\param users lista u�ytkownik�w identyfikuj�cych okno
	**/
	void deletePendingMessages(const Chat &chat);

	/**
		\fn int registerChatWidget(ChatWidget* chatwidget)
		Dodaje okno do menad�era
		\param chat wska�nik do okna ktore chcemy doda�
		\return zwraca numer naszego okna po zarejestrowaniu
	**/
	void registerChatWidget(ChatWidget *chatwidget);

	/**
		\fn void unregisterChatWidget(ChatWidget* chatwidget)
		Funkcja wyrejestrowuje okno z managera \n
		Zapisuje w�asno�ci okna \n
		wysy�a sygna� chatDestroying i chatDestroyed
		\param chat okno kt�re b�dzie wyrejestrowane
	**/
	void unregisterChatWidget(ChatWidget *chatwidget);

public slots:
	// for imagelink module
	void messageReceived(const Message &message);

	void sendMessage(const Chat &chat);

	void closeChat(const Chat &chat);
	void closeAllChats(const Buddy &buddy);

	/**
		\fn void closeAllWindows()
		Funkcja zamyka wszystkie okna chat
	**/
	void closeAllWindows();

signals:
	/**
		\fn void handleNewChatWidget(ChatWidget *chatwidget, bool &handled)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat.
		Je�li zmienna handled zostanie ustawiona na true, to
		niezostanie utworzony nowy obiekt ChatWindiw
		\param chat nowe okno chat
	**/
	void handleNewChatWidget(ChatWidget *chatwidget, bool &handled);
	/**
		\fn void chatWidgetCreated(ChatWidget *chat)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
		\param chat nowe okno chat
	**/
	void chatWidgetCreated(ChatWidget *chatwidget);

	void chatWidgetActivated(ChatWidget *chatwidget);

	/**
		\fn void chatCreated(const UserGroup group)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
		\param chat nowe okno chat
		\param time time of pending message that created a chat or 0 if not applicable
	**/
	void chatWidgetCreated(ChatWidget *chatwidget, time_t time);

	/**
		\fn void chatDestroying(const UserGroup group)
	 	Sygna� ten jest wysy�any przed zamnkni�ciem okna chat
		\param chat zamykane okno
	**/
	void chatWidgetDestroying(ChatWidget *chatwidget);

	/**
		\fn void chatOpen(ContactList users)
		Sygna� ten jest wysy�aniy podczas ka�dej pr�by
		otwarcia nowego okna chat nawet je�li ju� taki istnieje
		\param chat otwarte okno
	**/
	void chatWidgetOpen(ChatWidget *chatwidget, bool activate);

	void chatWidgetTitlesUpdated();

	/**
		\fn void messageSentAndConfirmed(Chat chat, const QString& message)
		This signal is emitted when message was sent
		and it was confirmed.
		When confirmations are turned off signal is
		emitted immediately after message was send.
		\param receivers list of receivers
		\param message the message
	**/
	void messageSentAndConfirmed(Chat chat, const QString& message);

};

#endif // CHAT_WIDGET_MANAGER
