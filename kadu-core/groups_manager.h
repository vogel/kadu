#ifndef KADU_GROUPS_MANAGER_H
#define KADU_GROUPS_MANAGER_H

#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qvariant.h>
#include <qwmatrix.h>

#include "configuration_aware_object.h"
#include "usergroup.h"
#include "userlistelement.h"

class KaduTabBar;

/**
	Klasa s³u¿±ca przede wszystkim zarz±dzaniu nazwanymi grupami kontaktów,
	ale tak¿e zarz±dza grupami typu: blokowani, blokuj±cy, tylko z opisami, tylko dostêpni, anonimowi
	\class GroupsManager
	\brief
**/
class GroupsManager : public QObject, ConfigurationAwareObject
{
	Q_OBJECT
	public:
		/**
			\fn static void initModule()
			inicjuje modu³ zarz±dcy grup
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			sprz±ta po module zarz±dcy grup
		**/
		static void closeModule();

		/**
			\fn UserGroup *group(const QString &name) const
			zwraca grupê kontaktów o nazwie name
			\param name nazwa grupy
			\return grupa kontaktów
		**/
		UserGroup *group(const QString &name) const;

		/**
			\fn UserGroup *addGroup(const QString &name)
			dodaje now± grupê kontaktów o nazwie name
			\param name nazwa grupy
			\return grupa kontaktów
		**/
		UserGroup *addGroup(const QString &name);

		/**
			\fn void removeGroup(const QString &name)
			usuwa grupê kontaktów o nazwie name
			\param name nazwa grupy
		**/
		void removeGroup(const QString &name);

		/**
			\fn void setTabBar(KaduTabBar *bar)
			ustawia pas zak³adek, na którym klasa bêdzie operowaæ oraz inicjuje
			wewnêtrzne dane klasy
			\param bar pas zak³adek, w którym bêdê umieszczane zak³adki grup
		**/
		void setTabBar(KaduTabBar *bar);

		/**
			\fn QStringList groups() const
			\return lista grup
		**/
		QStringList groups() const;

		/**
			\fn bool groupExists(const QString &name)
			\param name nazwa grupy
			\return informacja czy grupa istnieje
		**/
		bool groupExists(const QString &name);

		/**
			\fn QString currentGroupName() const
			\return nazwa aktualnie wybranej grupy
		**/
		QString currentGroupName() const;

		/**
			\fn void setIconForTab(const QString &name, bool showIcon)
			ustawia lub usuwa ikonkê dla grupy o nazwie name
			\param name nazwa grupy
		**/
		void setIconForTab(const QString &name);

	protected:
		virtual void configurationUpdated();

	public slots:

		/**
			\fn void setActiveGroup(const QString &name)
			ustawia aktywn± grupê na name
			\param name nazwa grupy
		**/
		void setActiveGroup(const QString& group);

		/**
			\fn void refreshTabBar()
			od¶wie¿a pasek zak³adek grup
		**/
		void refreshTabBar();

		/**
			\fn void refreshTabBarLater()
			od¶wie¿a pasek zak³adek grup, ale po zakoñczeniu bie¿acych operacji
		**/
		inline void refreshTabBarLater() { refreshTimer.start(0, true); }

		/**
			\fn void changeDisplayingBlocking()
			w³±cza lub wy³±cza wy¶wietlanie kontaktów blokuj±cych
			w g³ównej li¶cie kontaktów w zale¿no¶ci od poprzedniego stanu
		**/
		void changeDisplayingBlocking();

		/**
			\fn void changeDisplayingBlocked()
			w³±cza lub wy³±cza wy¶wietlanie kontaktów blokowanych
			w g³ównej li¶cie kontaktów w zale¿no¶ci od poprzedniego stanu
		**/
		void changeDisplayingBlocked();

		/**
			\fn void changeDisplayingOffline()
			w³±cza lub wy³±cza wy¶wietlanie kontaktów niedostêpnych
			w g³ównej li¶cie kontaktów w zale¿no¶ci od poprzedniego stanu
		**/
		void changeDisplayingOffline();

		/**
			\fn void changeDisplayingWithoutDescription()
			w³±cza lub wy³±cza wy¶wietlanie kontaktów o statusach bez opisu
			w g³ównej li¶cie kontaktów w zale¿no¶ci od poprzedniego stanu
		**/
		void changeDisplayingWithoutDescription();

		/**
			\fn void changeDisplayingOnlineAndDescription()
			w³±cza lub wy³±cza wy¶wietlanie kontatków o statusach "dostêpny"
			lub "zajêty" oraz kontatków z opisem
		**/
		void changeDisplayingOnlineAndDescription();

	private slots:
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);
		void userAddedToMainUserlist(UserListElement elem, bool massively, bool last);
		void userRemovedFromMainUserlist(UserListElement elem, bool massively, bool last);

		void userAdded(UserListElement elem, bool massively, bool last);
		void userRemoved(UserListElement elem, bool massively, bool last);

		void tabSelected(int id);

	private:
		GroupsManager();
		~GroupsManager();
		QMap<QString, UserGroup *> Groups;
		KaduTabBar *GroupBar;
		int lastId;
		QString currentGroup;
		bool showBlocked;
		bool showBlocking;
		bool showOffline;
		bool showWithoutDescription;
		bool showOnlineAndDescription;
		QTimer refreshTimer;
};
extern GroupsManager *groups_manager;

/**
	\class UsersWithDescription
	Klasa grupuj±ca kontakty o statusie z opisem,
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class UsersWithDescription : public UserGroup
{
	Q_OBJECT
	public:
		UsersWithDescription();
		virtual ~UsersWithDescription();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern UsersWithDescription *usersWithDescription;

/**
	\class OnlineUsers
	Klasa grupuj±ca kontakty o statusie "dostêpny" lub "zajêty",
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class OnlineUsers : public UserGroup
{
	Q_OBJECT
	public:
		OnlineUsers();
		virtual ~OnlineUsers();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern OnlineUsers *onlineUsers;

/**
	\class OnlineAndDescriptionUsers
	Klasa grupuj±ca kontakty o statusie "dostêpny",
	lub "zajêty", oraz kontakty maj±ca opis. Klasa
	automatycznie synchronizuje siê z g³ówn± list± kontaktów
**/
class OnlineAndDescriptionUsers : public UserGroup
{
	Q_OBJECT

	public:
		OnlineAndDescriptionUsers();
		virtual ~OnlineAndDescriptionUsers();

	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
					    const UserStatus &oldStatus, bool massively, bool last);
		void userChangedSlot(UserListElement elem, bool massively, bool last);
		void protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last);
};
extern OnlineAndDescriptionUsers *onlineAndDescriptionUsers;

/**
	\class OfflineUsers
	Klasa grupuj±ca kontakty o statusie "niedostêpny",
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class OfflineUsers : public UserGroup
{
	Q_OBJECT
	public:
		OfflineUsers();
		virtual ~OfflineUsers();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
					    const UserStatus &oldStatus, bool massively, bool last);
		void userChangedSlot(UserListElement elem, bool massively, bool last);
		void protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last);
};
extern OfflineUsers *offlineUsers;

/**
	\class BlockedUsers
	Klasa grupuj±ca kontakty, które s± blokowane,
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class BlockedUsers : public UserGroup
{
	Q_OBJECT
	public:
		BlockedUsers();
		virtual ~BlockedUsers();
	private slots:
		void protocolUserDataChangedSlot(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
};
extern BlockedUsers *blockedUsers;

/**
	\class BlockingUsers
	Klasa grupuj±ca kontakty o statusie blokuj±cym,
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class BlockingUsers : public UserGroup
{
	Q_OBJECT
	public:
		BlockingUsers();
		virtual ~BlockingUsers();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern BlockingUsers *blockingUsers;

/**
	\class AnonymousUsers
	Klasa grupuj±ca kontakty anonimowe,
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class AnonymousUsers : public UserGroup
{
	Q_OBJECT
	public:
		AnonymousUsers();
		virtual ~AnonymousUsers();
	private slots:
		void userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);
		void removingUser(UserListElement elem, bool massively, bool last);
};
extern AnonymousUsers *anonymousUsers;

/**
	\class AnonymousUsersWithoutMessages
	Klasa grupuj±ca kontakty anonimowe, nie maj±ce wiadomo¶ci do odebrania
	automatycznie synchronizuj±ca siê z g³ówn± list± kontaktów
**/
class AnonymousUsersWithoutMessages : public UserGroup
{
	Q_OBJECT
	public:
		AnonymousUsersWithoutMessages();
		virtual ~AnonymousUsersWithoutMessages();
	private slots:
		void messageFromUserAdded(UserListElement elem);
		void messageFromUserDeleted(UserListElement elem);

		void userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);
		void removingUser(UserListElement elem, bool massively, bool last);
};
extern AnonymousUsersWithoutMessages *anonymousUsersWithoutMessages;

#endif
