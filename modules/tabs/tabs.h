#ifndef TABS_TABS_H
#define TABS_TABS_H

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "chat_manager.h"
#include "configuration_aware_object.h"
#include "kadu.h"
#include "main_configuration_window.h"
#include "misc.h"

#include "tabwidget.h"

class QMenu;
class Action;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:
		TabsManager(bool firstload);
		virtual ~TabsManager();

		/**
		* Metoda jest wywoływana w momencie otwarcia okna konfiguracji
		*/
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		/**
		* Odłącza rozmowę od kart.
		* @param chat wskaźnik do rozmowy
		* @return prawda jeśli rozmowa była w kartach
		*/
		bool detachChat(ChatWidget* chat);

	public slots:
		/**
		* Slot zostaje wywołany w momencie otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onNewChat(ChatWidget* chat, bool &handled);

		/**
		* Slot zostaje wywołany w momencie zamknięcia rozmowy.
		* @param chat okno
		*/
		void onDestroyingChat(ChatWidget* chat);

		/**
		* Slot zostaje wywołany za każdym razem gdy nastąpi próba otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onOpenChat(ChatWidget* chat);

		/**
		* Slot zostaje wywołany w momencie zmiany statusu.
		* @param ule kontakt, dla którego zmienił się status
		*/
		void onStatusChanged(UserListElement ule);
		/**
		* Slot zostaje wywołany w momencie zmiany danych kontaktu.
		* @param elem kontakt, którego dane się zmieniły
		* @param name nazwa własności
		* @param oldValue stara wartość
		* @param currentValue nowa (bieżąca) wartość
		* @param massively true, gdy jest to cześć większych zmian
		* @param last true, gdy massively == true i jest to ostatnia zmiana
		*/
		void userDataChanged(UserListElement ule, QString name, QVariant, QVariant, bool, bool);
		/**
		* Slot zostaje wywołany w momencie zmiany aktywnej karty
		* @param index numer aktywowanej karty
		*/
		void onTabChange(int index);

		void onNewTab(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywołany w momencie "upuszczenia" grupy kontaktów na pasek kart
		* @param altnicks lista użytkowników rozdzielonych "/n"
		* @param index pozycja na której ma zostać umieszczona nowa karta
		*/
		void openTabWith(QStringList altnicks, int index);

		/**
		* Slot zostaje wywołany w momencie zmiany stanu przycisku dołączenia/odłączania okna do/od kart.
		* @param users lista użytkowników rozmowy
		* @param source wskaźnik do widgeta
		* @param isOn stan przycisku
		*/
		void onTabAttach(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywołany w momencie dodania przycisku do toolbara.
		* @param users lista użytkowników rozmowy
		*/
		void attachToTabsActionCreated(KaduAction *action);

		/**
		* Slot zostaje wywołany w momencie zamykania chata skrótem klawiszowym
		*/
		void closeChat();

	protected:
		/**
		* Metoda jest wywoływana po zmianie w oknie konfiguracyjnym.
		*/
		virtual void configurationUpdated();
		
	private:
		ActionDescription* openInNewTabActionDescription;
		ActionDescription* attachToTabsActionDescription;
		TabWidget* tabdialog;
		QTimer timer;
		QList<ChatWidget *> chatsWithNewMessages, newchats, detachedchats;
		bool no_tabs, autoswith, force_tabs;

		/**
		* pozycja na której ma zostać dodana nowa karta
		*/
		int target_tabs;
		void insertTab(ChatWidget* chat);
		void makePopupMenu();
		int menuitem;
		ChatWidget* selectedchat;
		QMenu* menu;
		Action* action;

		/**
		* Przywraca stan kart w momencie uruchomienia kadu.
		*/
		void loadTabs();

		/**
		* Zapisuje stan kart w momencie wyjścia z kadu.
		*/
		void saveTabs();

		/**
		* Rezerwuje lub usuwa miejsce na kartach przeznaczone
		* na przycisk zamknięcia chata na karcie
		* Odświeża również ikonki
		*/
		void repaintTabs();

		/**
		* Zmienne konfiguracyjne.
		*/
		bool config_conferencesInTabs;
		bool config_tabsBelowChats;
		bool config_autoTabChange;
		bool config_defaultTabs;
		unsigned config_minTabs;
		bool config_closeButtonOnTab;
		bool config_blinkChatTitle;
		bool config_showNewMessagesNum;

	private slots:
		void onTimer();
		void onContextMenu(QWidget* w, const QPoint& pos);
		void onMenu(int id);
		void onMessageReceived(ChatWidget *chat);
	signals:
		void chatWidgetActivated(ChatWidget *);
};

extern TabsManager* tabs_manager;
#endif
