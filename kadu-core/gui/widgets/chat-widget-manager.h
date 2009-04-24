/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_WIDGET_MANAGER
#define CHAT_WIDGET_MANAGER

#include <QtCore/QTimer>

#include "contacts/contact-list.h"

#include "gui/widgets/chat-widget.h"

#include "configuration_aware_object.h"

#include "exports.h"

class ActionDescription;
class KaduAction;
class Protocol;

/**
	Klasa pozwalaj�ca zarz�dza� otwartymi oknami rozm�w: otwiera�,
	zamykac, szuka� okna ze wgl�du na list� u�ytkownik�w itp.
	\class ChatManagerOld
	\brief Klasa zarz�dzaj�ca oknami ChatWidget
**/

class KADUAPI ChatWidgetManager : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(ChatWidgetManager)

public: // TODO: 0.6.6 clean it up
	ActionDescription *autoSendActionDescription;
	ActionDescription *clearChatActionDescription;
	ActionDescription *insertImageActionDescription;
	ActionDescription *boldActionDescription;
	ActionDescription *italicActionDescription;
	ActionDescription *underlineActionDescription;
	ActionDescription *sendActionDescription;
	ActionDescription *whoisActionDescription;
	ActionDescription *ignoreUserActionDescription;
	ActionDescription *blockUserActionDescription;
	ActionDescription *chatActionDescription;
	ActionDescription *openChatWithActionDescription;
	ActionDescription *insertEmoticonActionDescription;
	ActionDescription *colorSelectorActionDescription;

private:
	static ChatWidgetManager *Instance;

	ChatList ChatWidgets; /*!< lista okien*/
	QList<Chat *> ClosedChats; /*!< u�ytkownicy, kt�rych okna zosta�y zamkni�te*/

	/**
		\struct ChatInfo
		Struktura przechowuje informacje o danym oknie
	**/
	struct ChatInfo
	{
		ContactSet contacts;          /*!< lista u�ytkownik�w identyfikuj�ca okno */
		QMap<QString, QVariant> map;     /*!< parametry danego okna */
		ChatInfo() : contacts(), map() {}
	};
	QList<ChatInfo> addons; /*!< lista parametr�w okien */
	QTimer refreshTitlesTimer;

	ChatWidgetManager();
	virtual ~ChatWidgetManager();

	void autoSendActionCheck();
	void insertEmoticonActionEnabled();

private slots:
	void openChatWith();

	void autoSendActionActivated(QAction *sender, bool toggled);
	void clearActionActivated(QAction *sender, bool toggled);
	void boldActionActivated(QAction *sender, bool toggled);
	void italicActionActivated(QAction *sender, bool toggled);
	void underlineActionActivated(QAction *sender, bool toggled);
	void sendActionActivated(QAction *sender, bool toggled);
	void whoisActionActivated(QAction *sender, bool toggled);
	void chatActionActivated(QAction *sender, bool toggled);
	void insertImageActionActivated(QAction *sender, bool toggled);
	void colorSelectorActionActivated(QAction *sender, bool toogled);
	void ignoreUserActionActivated(QAction *sender, bool toggled);
	void blockUserActionActivated(QAction *sender, bool toggled);

	void autoSendActionCreated(KaduAction *action);
	void sendActionCreated(KaduAction *action);

	void insertEmoticonActionCreated(KaduAction *action);
	void insertEmoticonActionActivated(QAction *sender, bool toggled);

	void messageReceived(Chat *chat, Contact sender, const QString &message);

protected:
	virtual void configurationUpdated();

public:
	static ChatWidgetManager * instance();

	/**
		\fn static void initModule()
		Rejestruje opcje modulu Chat w oknie konfiguracji
	**/
	static void initModule();

	/**
		\fn static void closeModule()
		Wyrejestrowuje opcje modu�u z okna konfiguracji
	**/
	static void closeModule();

	/**
		\fn const ChatList& chats() const
		Funkcja zwraca list� otwartych okien Chat
	**/
	const ChatList & chats() const;

	/**
		\fn QValueList<ContactList> closedChatsUsers() const
		Funkcja zwraca list� u�ytkownik�w, dla kt�rych zamkni�to okna Chat
	**/
	const QList<Chat *> closedChats() const;

	/**
		\fn ChatWidget* findChatWidget(const UserGroup *group) const;
		Funkcja zwraca wska�nik do okna z list�
		u�ytkownik�w group
		\param group lista u�ytkownik�w
		\return wska�nik do okna je�li istnieje w przeciwnym
		 wypadku zwraca NULL
	**/
	ChatWidget * findChatWidget(Chat *chat) const;

	/**
		\fn Chat* findChat(ContactList users) const;
		Funkcja zwraca wska�nik do okna z list�
		u�ytkownik�w group
		\param users lista u�ytkownik�w
		\return wska�nik do okna je�li istnieje w przeciwnym
		 wypadku zwraca NULL
	**/
	//ChatWidget * findChatWidget(ContactList users) const;

	// co za g�upota
	// TODO: przenie�� do klasy ChatWidget / ewentualnie do nowo-utworzonej klasy Chat
	/**
		\fn QVariant& chatWidgetProperty(const UserGroup *group, const QString &name)
		Funkcja zwraca warto�� w�asno�ci "name" okna
		okre�lonego przez group
		\param group grupa u�ytkownik�w identyfikuj�ca okno
		\param name nazwa w�asno�ci
		\return zwraca warto�� w�asno�ci je�li okre�lone okno
		istnieje,\n je�li nie to tworzy tak�
		w�asno�� (ustawia na pust�)
	**/
	QVariant & chatWidgetProperty(ContactSet contacts, const QString &name);

	void loadOpenedWindows();
	void saveOpenedWindows();

	ChatWidget * chatWidgetForChat(Chat *chat);
	void activateChatWidget(ChatWidget *chatWidget, bool forceActivate);

public slots:
	ChatWidget * openChatWidget(Chat *chat, bool forceActivate = false);

	/**
		\fn void openPendingMsgs(ContactList users)
		Funkcja wpisuje zakolejkowane wiadomo�ci do okna
		z u�ytkownikami "users"
		\param users lista u�ytkownik�w identyfikuj�cych okno
	**/
	void openPendingMsgs(Chat *chat, bool forceActivate = false);

	/**
		\fn void openPendingMsgs()
		Funkcja wpisuje wszystkie zakolejkowane wiadomo�ci
		do odpowiednich okien
	**/
	void openPendingMsgs(bool forceActivate = false);

	/**
		\fn void deletePendingMsgs(ContactList users)
		Funkcja usuwa zakolejkowane wiadomo�ci
		z u�ytkownikami "users"
		\param users lista u�ytkownik�w identyfikuj�cych okno
	**/
	void deletePendingMsgs(ContactSet users);

	void sendMessage(Chat *chat);

	/**
		\fn void closeAllWindows()
		Funkcja zamyka wszystkie okna chat
	**/
	void closeAllWindows();

	/**
		\fn int registerChatWidget(ChatWidget* chat)
		Dodaje okno do menad�era
		\param chat wska�nik do okna ktore chcemy doda�
		\return zwraca numer naszego okna po zarejestrowaniu
	**/
	int registerChatWidget(ChatWidget *chat);

	/**
		\fn void unregisterChat(Chat* chat)
		Funkcja wyrejestrowuje okno z managera \n
		Zapisuje w�asno�ci okna \n
		wysy�a sygna� chatDestroying i chatDestroyed
		\param chat okno kt�re b�dzie wyrejestrowane
	**/
	void unregisterChatWidget(ChatWidget *chat);

	/**
		\fn void refreshTitles()
		Funkcja od�wie�a tytu�y wszystkich okien
	**/
	void refreshTitles();
	void refreshTitlesLater();

	/**
		\fn void refreshTitlesForUser(Contact user)
		Funkcja od�wie�a tytu�y okien kt�re zawieraj� uin
		\param user u�ytkownik, kt�rego
		opis/status b�dzie od�wie�any
	**/
	void refreshTitlesForUser(Contact user);

	/**
		\fn void setChatWidgetProperty(const UserGroup *group, const QString &name, const QVariant &value)
		Funkcja pozwala przypisa� okre�lonemu czatowi
		(nawet je�eli on jeszcze nie istnieje) pewne w�asno�ci
		\param group grupa u�ytkownik�w identyfikuj�cych okno
		\param name nazwa w�asno�ci
		\param value warto�� w�asno�ci
	**/
	void setChatWidgetProperty(ContactSet contacts, const QString& name, const QVariant& value);

signals:
	/**
		\fn void handleNewChatWidget(ChatWidget *chat, bool &handled)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat.
		Je�li zmienna handled zostanie ustawiona na true, to 
		niezostanie utworzony nowy obiekt ChatWindiw
		\param chat nowe okno chat
	**/
	void handleNewChatWidget(ChatWidget *chat, bool &handled);
	/**
		\fn void chatWidgetCreated(ChatWidget *chat)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
		\param chat nowe okno chat
	**/
	void chatWidgetCreated(ChatWidget *chat);

	void chatWidgetActivated(ChatWidget *chat);

	/**
		\fn void chatCreated(const UserGroup *group)
	 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
		\param chat nowe okno chat
		\param time time of pending message that created a chat or 0 if not applicable
	**/
	void chatWidgetCreated(ChatWidget *chat, time_t time);

	/**
		\fn void chatDestroying(const UserGroup *group)
	 	Sygna� ten jest wysy�any przed zamnkni�ciem okna chat
		\param chat zamykane okno
	**/
	void chatWidgetDestroying(ChatWidget *chat);

	/**
		\fn void chatOpen(ContactList users)
		Sygna� ten jest wysy�aniy podczas ka�dej pr�by
		otwarcia nowego okna chat nawet je�li ju� taki istnieje
		\param chat otwarte okno
	**/
	void chatWidgetOpen(ChatWidget *chat);

	void chatWidgetTitlesUpdated();

	/**
		\fn void messageSentAndConfirmed(ContactList receivers, const QString& message)
		This signal is emited when message was sent
		and it was confirmed.
		When confirmations are turned off signal is
		emited immediately after message was send.
		\param receivers list of receivers
		\param message the message
	**/
	void messageSentAndConfirmed(ContactList receivers, const QString& message);

};

#endif // CHAT_WIDGET_MANAGER
