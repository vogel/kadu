#ifndef KADU_CHAT_MANAGER_H
#define KADU_CHAT_MANAGER_H

#include <qobject.h>
#include <qtimer.h>

#include "chat_widget.h"
#include "chat_window.h"
#include "chat_message.h"
#include "configuration_aware_object.h"
#include "toolbar.h"
#include "usergroup.h"

class Protocol;

/**
	Klasa pozwalaj±ca zarz±dzaæ otwartymi oknami rozmów: otwieraæ,
	zamykac, szukaæ okna ze wglêdu na listê u¿ytkowników itp.
	\class ChatManager
	\brief Klasa zarz±dzaj±ca oknami ChatWidget
**/

class ChatManager : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	private:
		ChatList ChatWidgets; /*!< lista okien*/
		QValueList<UserListElements> ClosedChatUsers; /*!< u¿ytkownicy, których okna zosta³y zamkniête*/

		/**
			\fn int openPendingMsg(int index, ChatMessage &msg)
			Funkcja otwieraj±ca zakolejkowan± wiadomo¶æ
			\param index nr wiadomo¶ci z kolejki
			\param k zwracany nr okna w którym zosta³a otwarta wiadomo¶æ
			\return wiadomo¶æ z kolejki
		**/
		ChatMessage * openPendingMsg(int index, int &k);

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
		void clearActionActivated(const UserGroup* users);
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

	protected:
		virtual void configurationUpdated();

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
			\fn QValueList<UserListElements> closedChatsUsers() const
			Funkcja zwraca listê u¿ytkowników, dla których zamkniêto okna Chat
		**/
		const QValueList<UserListElements> closedChatUsers() const;

		/**
			\fn ChatWidget* findChatWidget(const UserGroup *group) const;
			Funkcja zwraca wska¼nik do okna z list±
			u¿ytkowników group
			\param group lista u¿ytkowników
			\return wska¼nik do okna je¶li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		ChatWidget* findChatWidget(const UserGroup *group) const;

		/**
			\fn Chat* findChat(UserListElements users) const;
			Funkcja zwraca wska¼nik do okna z list±
			u¿ytkowników group
			\param users lista u¿ytkowników
			\return wska¼nik do okna je¶li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		ChatWidget* findChatWidget(UserListElements users) const;

		// co za g³upota
		// TODO: przenie¶æ do klasy ChatWidget / ewentualnie do nowo-utworzonej klasy Chat
		/**
			\fn QVariant& getChatWidgetProperty(const UserGroup *group, const QString &name)
			Funkcja zwraca warto¶æ w³asno¶ci "name" okna
			okre¶lonego przez group
			\param group grupa u¿ytkowników identyfikuj±ca okno
			\param name nazwa w³asno¶ci
			\return zwraca warto¶æ w³asno¶ci je¶li okre¶lone okno
			istnieje,\n je¶li nie to tworzy tak±
			w³asno¶æ (ustawia na pust±)
		**/
		QVariant& getChatWidgetProperty(const UserGroup *group, const QString &name);

		void loadOpenedWindows();
		void saveOpenedWindows();

	public slots:

		/**
			\fn int openChatWidget(QString initialProtocol, UserListElements users, time_t time = 0)
			Funkcja otwiera nowe okno Chat z wymienionymi rozmowcami.
			\param initialProtocol protokó³ pocz±tkowy
			\param users lista u¿ytkowników identyfikuj±cych okno
			\param time time of pending message that created a chat or 0 if not applicable
			\return zwracany jest numer otwartego okna
		**/
		int openChatWidget(Protocol *initialProtocol, const UserListElements &users, time_t time = 0);

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
			\fn int registerChatWidget(ChatWidget* chat)
			Dodaje okno do menad¿era
			\param chat wska¼nik do okna ktore chcemy dodaæ
			\return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChatWidget(ChatWidget* chat);

		/**
			\fn void unregisterChat(Chat* chat)
			Funkcja wyrejestrowuje okno z managera \n
			Zapisuje w³asno¶ci okna \n
			wysy³a sygna³ chatDestroying i chatDestroyed
			\param chat okno które bêdzie wyrejestrowane
		**/
		void unregisterChatWidget(ChatWidget* chat);

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
			\fn void setChatWidgetProperty(const UserGroup *group, const QString &name, const QVariant &value)
			Funkcja pozwala przypisaæ okre¶lonemu czatowi
			(nawet je¿eli on jeszcze nie istnieje) pewne w³asno¶ci
			\param group grupa u¿ytkowników identyfikuj±cych okno
			\param name nazwa w³asno¶ci
			\param value warto¶æ w³asno¶ci
		**/
		void setChatWidgetProperty(const UserGroup *group, const QString &name, const QVariant &value);

	signals:

		/**
			\fn void handleNewChatWidget(ChatWidget *chat, bool &handled)
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat.
			Je¶li zmienna handled zostanie ustawiona na true, to 
			niezostanie utworzony nowy obiekt ChatWindiw
			\param chat nowe okno chat
		**/
		void handleNewChatWidget(ChatWidget *chat, bool &handled);
		/**
			\fn void chatWidgetCreated(ChatWidget *chat)
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat
			\param chat nowe okno chat
		**/
		void chatWidgetCreated(ChatWidget *chat);

		void chatWidgetActivated(ChatWidget *);

		/**
			\fn void chatCreated(const UserGroup *group)
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat
			\param chat nowe okno chat
			\param time time of pending message that created a chat or 0 if not applicable
		**/
		void chatWidgetCreated(ChatWidget *chat, time_t time);

		/**
			\fn void chatDestroying(const UserGroup *group)
		 	Sygna³ ten jest wysy³any przed zamnkniêciem okna chat
			\param chat zamykane okno
		**/
		void chatWidgetDestroying(ChatWidget *chat);

		/**
			\fn void chatOpen(UserListElements users)
			Sygna³ ten jest wysy³aniy podczas ka¿dej próby
			otwarcia nowego okna chat nawet je¶li ju¿ taki istnieje
			\param chat otwarte okno
		**/
		void chatWidgetOpen(ChatWidget *chat);

		void chatWidgetTitlesUpdated();

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
