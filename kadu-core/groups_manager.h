#ifndef KADU_GROUPS_MANAGER_H
#define KADU_GROUPS_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include "configuration_aware_object.h"
#include "usergroup.h"
#include "userlistelement.h"
#include "exports.h"

/**
	Klasa s�u��ca przede wszystkim zarz�dzaniu nazwanymi grupami kontakt�w,
	ale tak�e zarz�dza grupami typu: blokowani, blokuj�cy, tylko z opisami, tylko dost�pni, anonimowi
	\class GroupsManager
	\brief
**/
class KADUAPI GroupsManagerOld : public QObject, ConfigurationAwareObject
{
	Q_OBJECT

	GroupsManagerOld();
	~GroupsManagerOld();
	QMap<QString, UserGroup *> Groups;
	int lastId;
	QString currentGroup;
	bool showBlocked;
	bool showBlocking;
	bool showOffline;
	bool showWithoutDescription;
	bool showOnlineAndDescription;
	QTimer refreshTimer;

protected:
	virtual void configurationUpdated();

public:
	/**
		\fn static void initModule()
		inicjuje modu� zarz�dcy grup
	**/
	static void initModule();

	/**
		\fn static void closeModule()
		sprz�ta po module zarz�dcy grup
	**/
	static void closeModule();

	/**
		\fn UserGroup *group(const QString &name) const
		zwraca grup� kontakt�w o nazwie name
		\param name nazwa grupy
		\return grupa kontakt�w
	**/
	UserGroup *group(const QString &name) const;

	/**
		\fn UserGroup *addGroup(const QString &name)
		dodaje now� grup� kontakt�w o nazwie name
		\param name nazwa grupy
		\return grupa kontakt�w
	**/
	UserGroup *addGroup(const QString &name);

	/**
		\fn void removeGroup(const QString &name)
		usuwa grup� kontakt�w o nazwie name
		\param name nazwa grupy
	**/
	void removeGroup(const QString &name);

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
		ustawia lub usuwa ikonk� dla grupy o nazwie name
		\param name nazwa grupy
	**/
	void setIconForTab(const QString &name);

};

extern KADUAPI GroupsManagerOld *groups_manager;

/**
	\class OnlineUsers
	Klasa grupuj�ca kontakty o statusie "dost�pny" lub "zaj�ty",
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class OnlineUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

public:
	OnlineUsers();
	virtual ~OnlineUsers();

};

extern OnlineUsers *onlineUsers;

/**
	\class OnlineAndDescriptionUsers
	Klasa grupuj�ca kontakty o statusie "dost�pny",
	lub "zaj�ty", oraz kontakty maj�ca opis. Klasa
	automatycznie synchronizuje si� z g��wn� list� kontakt�w
**/
class OnlineAndDescriptionUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);
	void userChangedSlot(UserListElement elem, bool massively, bool last);
	void protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last);

public:
	OnlineAndDescriptionUsers();
	virtual ~OnlineAndDescriptionUsers();

};

extern OnlineAndDescriptionUsers *onlineAndDescriptionUsers;

/**
	\class OfflineUsers
	Klasa grupuj�ca kontakty o statusie "niedost�pny",
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class OfflineUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);
	void userChangedSlot(UserListElement elem, bool massively, bool last);
	void protocolAddedOrRemoved(UserListElement elem, QString protocolName, bool massively, bool last);

	public:
		OfflineUsers();
		virtual ~OfflineUsers();

};

extern OfflineUsers *offlineUsers;

/**
	\class BlockedUsers
	Klasa grupuj�ca kontakty, kt�re s� blokowane,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class BlockedUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void protocolUserDataChangedSlot(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue,
		bool massively, bool last);

public:
	BlockedUsers();
	virtual ~BlockedUsers();

};

extern BlockedUsers *blockedUsers;

/**
	\class BlockingUsers
	Klasa grupuj�ca kontakty o statusie blokuj�cym,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class BlockingUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void statusChangedSlot(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

public:
	BlockingUsers();
	virtual ~BlockingUsers();

};

extern BlockingUsers *blockingUsers;

/**
	\class AnonymousUsers
	Klasa grupuj�ca kontakty anonimowe,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class AnonymousUsers : public UserGroup
{
	Q_OBJECT

private slots:
	void userDataChangedSlot(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);
	void userAdded(UserListElement elem, bool massively, bool last);
	void removingUser(UserListElement elem, bool massively, bool last);

public:
	AnonymousUsers();
	virtual ~AnonymousUsers();

};

extern AnonymousUsers *anonymousUsers;

/**
	\class AnonymousUsersWithoutMessages
	Klasa grupuj�ca kontakty anonimowe, nie maj�ce wiadomo�ci do odebrania
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class AnonymousUsersWithoutMessages : public UserGroup
{
	Q_OBJECT

private slots:
	void messageFromUserAdded(Contact elem);
	void messageFromUserDeleted(Contact elem);

	void userDataChangedSlot(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool last);
	void userAdded(UserListElement elem, bool massively, bool last);
	void removingUser(UserListElement elem, bool massively, bool last);

public:
	AnonymousUsersWithoutMessages();
	virtual ~AnonymousUsersWithoutMessages();

};

extern AnonymousUsersWithoutMessages *anonymousUsersWithoutMessages;

#endif
