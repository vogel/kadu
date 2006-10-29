#ifndef KADU_CHAT_MANAGER_H
#define KADU_CHAT_MANAGER_H

#include <qobject.h>
#include <qtimer.h>

#include "chat.h"
#include "chat_message.h"
#include "toolbar.h"
#include "usergroup.h"

class ChatManagerSlots;
class Protocol;

/**
	Klasa pozwalaj±ca zarz±dzaæ otwartymi oknami rozmów: otwieraæ,
	zamykac, szukaæ okna ze wglêdu na listê u¿ytkowników itp.
	\class ChatManager
	\brief Klasa zarz±dzaj±ca oknami Chat
**/

class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats; /*!< lista okien*/

		static ChatManagerSlots *chatslots;

		/**
			\fn int openPendingMsg(int index, ChatMessage &msg)
			Funkcja otwieraj±ca zakolejkowan± wiadomo¶æ
			\param index nr wiadomo¶ci z kolejki
			\param msg wiadomo¶æ z kolejki
			\return zwracany jest nr okna w którym zosta³a otwarta wiadomo¶æ
		**/
		int openPendingMsg(int index, ChatMessage &msg);

		/**
			\struct ChatInfo
			Struktura przechowuje informacje o danym oknie
		**/
		struct ChatInfo
		{
			UserListElements users;/*!< lista u¿ytkowników
					identyfikuj±ca okno */
			QMap<QString, QVariant> map; /*!< parametry
							danego okna */
			ChatInfo() : users(), map() {}
		};
		QValueList<ChatInfo> addons; /*!< lista parametrów okien */
		QTimer refreshTitlesTimer;

	private slots:

		void autoSendActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void scrollLockActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void clearActionActivated(const UserGroup* users);
		void historyActionActivated(const UserGroup* users);
		void boldActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void italicActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void underlineActionActivated(const UserGroup* users, const QWidget* source, bool is_on);
		void colorActionActivated(const UserGroup* users, const QWidget* source);
		void insertEmoticonActionActivated(const UserGroup* users, const QWidget* source);
		void insertEmoticonActionAddedToToolbar(const UserGroup*, ToolButton* button, ToolBar* toolbar);
		void whoisActionActivated(const UserGroup* users);
		void ignoreUserActionActivated(const UserGroup* users);
		void blockUserActionActivated(const UserGroup* users);
		void insertImageActionActivated(const UserGroup* users);
		void sendActionActivated(const UserGroup* users);
		void chatActionActivated(const UserGroup* users);

	public:
		/**
			\fn ChatManager(QObject* parent=NULL, const char* name=NULL)
			Konstruktor tworz±cy obiekt zarz±dzaj±cy oknami
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		ChatManager(QObject* parent=NULL, const char* name=NULL);

		/**
			\fn ~ChatManager()
			Destruktor zamyka wszystkie otwarte okna
		**/
		~ChatManager();

		/**
			\fn static void initModule()
			Rejestruje opcje modulu Chat w oknie konfiguracji
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			Wyrejestrowuje opcje modu³u z okna konfiguracji
		**/
		static void closeModule();

		/**
			\fn const ChatList& chats() const
			Funkcja zwraca listê otwartych okien Chat
		**/
		const ChatList& chats() const;

		/**
			\fn Chat* findChat(const UserGroup *group) const;
			Funkcja zwraca wska¼nik do okna z list±
			u¿ytkowników group
			\param group lista u¿ytkowników
			\return wska¼nik do okna je¶li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChat(const UserGroup *group) const;

		/**
			\fn Chat* findChat(UserListElements users) const;
			Funkcja zwraca wska¼nik do okna z list±
			u¿ytkowników group
			\param users lista u¿ytkowników
			\return wska¼nik do okna je¶li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChat(UserListElements users) const;

		/**
			\fn QVariant& getChatProperty(const UserGroup *group, const QString &name)
			Funkcja zwraca warto¶æ w³asno¶ci "name" okna
			okre¶lonego przez group
			\param group grupa u¿ytkowników identyfikuj±ca okno
			\param name nazwa w³asno¶ci
			\return zwraca warto¶æ w³asno¶ci je¶li okre¶lone okno
			istnieje,\n je¶li nie to tworzy tak±
			w³asno¶æ (ustawia na pust±)
		**/
		QVariant& getChatProperty(const UserGroup *group, const QString &name);

	public slots:

		/**
			\fn void chatMsgReceived(const QString &protocolName, UserListElements senders, const QString& msg, time_t time, bool& grab)
			Slot informuj±cy o odebraniu wiadomo¶ci
			\param protocolName nazwa protoko³u
			\param senders lista u¿ytkowników identyfikuj±cych okno
			\param msg tre¶æ otrzymanej wiadomo¶ci
			\param time czas otrzymania wiadomo¶ci
			\param grab
		**/
		void chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool& grab);

		/**
			\fn int openChat(QString initialProtocol, UserListElements users, time_t time = 0)
			Funkcja otwiera nowe okno Chat z wymienionymi rozmowcami.
			\param initialProtocol protokó³ pocz±tkowy
			\param users lista u¿ytkowników identyfikuj±cych okno
			\param time parametr sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
			\return zwracany jest numer otwartego okna
		**/
		int openChat(QString initialProtocol, UserListElements users, time_t time = 0);

		/**
			\fn void openPendingMsgs(UserListElements users)
			Funkcja wpisuje zakolejkowane wiadomo¶ci do okna
			z u¿ytkownikami "users"
			\param users lista u¿ytkowników identyfikuj±cych okno
		**/
		void openPendingMsgs(UserListElements users);

		/**
			\fn void openPendingMsgs()
			Funkcja wpisuje wszystkie zakolejkowane wiadomo¶ci
			do odpowiednich okien
		**/
		void openPendingMsgs();

		/**
			\fn void deletePendingMsgs(UserListElements users)
			Funkcja usuwa zakolejkowane wiadomo¶ci
			z u¿ytkownikami "users"
			\param users lista u¿ytkowników identyfikuj±cych okno
		**/
		void deletePendingMsgs(UserListElements users);


		//TODO: opisac funkcje sendMessage(..)
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)
		*/
		void sendMessage(UserListElement user, UserListElements selected_users);

		/**
			\fn void closeAllWindows()
			Funkcja zamyka wszystkie okna chat
		**/
		void closeAllWindows();

		/**
			\fn int registerChat(Chat* chat)
			Dodaje okno do menad¿era
			\param chat wska¼nik do okna ktore chcemy dodaæ
			\return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);

		/**
			\fn void unregisterChat(Chat* chat)
			Funkcja wyrejestrowuje okno z managera \n
			Zapisuje w³asno¶ci okna \n
			wysy³a sygna³ chatDestroying i chatDestroyed
			\param chat okno które bêdzie wyrejestrowane
		**/
		void unregisterChat(Chat* chat);

		/**
			\fn void refreshTitles()
			Funkcja od¶wie¿a tytu³y wszystkich okien
		**/
		void refreshTitles();
		void refreshTitlesLater();

		/**
			\fn void refreshTitlesForUser(UserListElement user)
			Funkcja od¶wie¿a tytu³y okien które zawieraj± uin
			\param user u¿ytkownik, którego
			opis/status bêdzie od¶wie¿any
		**/
		void refreshTitlesForUser(UserListElement user);

		/**
			\fn void changeAppearance()
			Funkcja od¶wie¿a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();

		/**
			\fn void setChatProperty(const UserGroup *group, const QString &name, const QVariant &value)
			Funkcja pozwala przypisaæ okre¶lonemu czatowi
			(nawet je¿eli on jeszcze nie istnieje) pewne w³asno¶ci
			\param group grupa u¿ytkowników identyfikuj±cych okno
			\param name nazwa w³asno¶ci
			\param value warto¶æ w³asno¶ci
		**/
		void setChatProperty(const UserGroup *group, const QString &name, const QVariant &value);

	signals:

		/**
			\fn void chatCreated(const UserGroup *group)
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat
			\param group lista u¿ytkowników, z którymi tworzymy nowy chat
		**/
		void chatCreated(const UserGroup *group);

		/**
			\fn void chatDestroying(const UserGroup *group)
		 	Sygna³ ten jest wysy³any przed zamnkniêciem okna chat
			\param group lista u¿ytkownikówów, którzy wystêpuj± w tym chacie
		**/
		void chatDestroying(const UserGroup *group);

		/**
			\fn void chatDestroyed(const UserGroup *group)
		 	Sygna³ ten jest wysy³any po zamnkniêciem okna chat
			\param group lista u¿ytkowników, którzy
			wystêpowa³y w tym chacie
		**/
		void chatDestroyed(const UserGroup *group);

		/**
			\fn void chatOpen(UserListElements users)
			Sygna³ ten jest wysy³aniy podczas ka¿dej próby
			otwarcia nowego okna chat nawet je¶li ju¿ taki istnieje
			\param users lista u¿ytkowników
		**/
		void chatOpen(UserListElements users);

		/**
			\fn void messageSentAndConfirmed(UserListElements receivers, const QString& message)
			This signal is emited when message was sent
			and it was confirmed.
			When confirmations are turned off signal is
			emited immediately after message was send.
			\param receivers list of receivers
			\param message the message
		**/
		void messageSentAndConfirmed(UserListElements receivers, const QString& message);
};

/**
	wska¼nik do obiektu ChatManager'a
**/
extern ChatManager* chat_manager;

#endif
