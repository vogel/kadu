/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TABS_TABS_H
#define TABS_TABS_H

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "storage/storable-object.h"

#include "tabwidget.h"

class QMenu;
class Action;
class ActionDescription;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject, StorableObject
{
	Q_OBJECT

		void createDefaultConfiguration();

		ActionDescription *openInNewTabActionDescription;
		ActionDescription *attachToTabsActionDescription;
		TabWidget *tabdialog;
		QTimer timer;
		QList<ChatWidget *> chatsWithNewMessages, newchats, detachedchats;
		bool no_tabs, autoswith, force_tabs;

		/**
		* pozycja na kt��rej ma zosta�0�4 dodana nowa karta
		*/
		int target_tabs;
		void insertTab(ChatWidget *chat);
		void makePopupMenu();
		int menuitem;
		ChatWidget *selectedchat;
		QMenu *menu;
		Action *action;

		/**
		* Przywraca stan kart w momencie uruchomienia kadu.
		*/
		void loadTabs();

		/**
		* Zapisuje stan kart w momencie wyj�0�2cia z kadu.
		*/
		void saveTabs();

		/**
		* Rezerwuje lub usuwa miejsce na kartach przeznaczone
		* na przycisk zamkni�0�1cia chata na karcie
		* Od�0�2wie�0�4a r��wnie�0�4 ikonki
		*/
		void repaintTabs();

		QString formatTabName(ChatWidget * chatWidget);

		void refreshTab(int tabIndex, ChatWidget * chatWidget);

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
		void onContextMenu(QWidget *w, const QPoint &pos);
		void onMenuActionDetach();
		void onMenuActionDetachAll();
		void onMenuActionClose();
		void onMenuActionCloseAll();
		void onMessageReceived(Chat chat);

	protected:
		/**
		* Metoda jest wywo�0�0ywana po zmianie w oknie konfiguracyjnym.
		*/
		virtual void configurationUpdated();

		/**
		* Metoda odczytujaca konfiguracje.
		*/
		virtual void load();

	public:
		TabsManager(bool firstload);
		virtual ~TabsManager();

		/**
		* Metoda jest wywo�0�0ywana w momencie otwarcia okna konfiguracji
		*/
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		/**
		* Od�0�0�0�2cza rozmow�0�1 od kart.
		* @param chat wska�0�2nik do rozmowy
		* @return prawda je�0�2li rozmowa by�0�0a w kartach
		*/
		bool detachChat(ChatWidget *chat);

		/**
		* Metoda zapisujaca konfiguracje.
		*/
		virtual void store();

	public slots:
		/**
		* Slot zostaje wywo�0�0any w momencie otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onNewChat(ChatWidget *chat, bool &handled);

		/**
		* Slot zostaje wywo�0�0any w momencie zamkni�0�1cia rozmowy.
		* @param chat okno
		*/
		void onDestroyingChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�0�0any za ka�0�4dym razem gdy nast�0�2pi pr��ba otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onOpenChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�0�0any w momencie zmiany statusu.
		* @param account konto, na kt��rym zmieni�0�0 si�0�1 status
		* @param contact kontakt, dla kt��rego zmieni�0�0 si�0�1 status
		* @param oldStatus poprzedni status
		*/
		void onTitleChanged(Chat chatChanged, const QString &newTitle);

		/**
		* Slot zostaje wywo�0�0any w momencie zmiany aktywnej karty
		* @param index numer aktywowanej karty
		*/
		void onTabChange(int index);

		void onNewTab(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�0�0any w momencie "upuszczenia" grupy kontakt��w na pasek kart
		* @param altnicks lista u�0�4ytkownik��w rozdzielonych "/n"
		* @param index pozycja na kt��rej ma zosta�0�4 umieszczona nowa karta
		*/
		void openTabWith(QStringList altnicks, int index);

		/**
		* Slot zostaje wywo�0�0any w momencie zmiany stanu przycisku do�0�0�0�2czenia/od�0�0�0�2czania okna do/od kart.
		* @param users lista u�0�4ytkownik��w rozmowy
		* @param source wska�0�2nik do widgeta
		* @param isOn stan przycisku
		*/
		void onTabAttach(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�0�0any w momencie dodania przycisku do toolbara.
		* @param users lista u�0�4ytkownik��w rozmowy
		*/
		void attachToTabsActionCreated(Action *action);

		/**
		* Slot zostaje wywo�0�0any w momencie zamykania chata skr��tem klawiszowym
		*/
		void closeChat();

	signals:
		void chatWidgetActivated(ChatWidget *);
};

extern TabsManager *tabs_manager;
#endif
