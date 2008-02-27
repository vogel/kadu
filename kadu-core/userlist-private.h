#ifndef KADU_USERLIST_PRIVATE_H
#define KADU_USERLIST_PRIVATE_H

#include <qglobal.h>

#include <q3dict.h>
#include <q3intdict.h>
#include <qmap.h>
#include <qobject.h>
#include <q3ptrdict.h>
#include <q3shared.h>
#include <qstring.h>
#include <qvariant.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "userlistelement.h"

class UserStatus;
class UserGroup;
class ProtocolData;

#pragma GCC visibility push(hidden)

class UserGroupSet : public Q3PtrDict<void>
{
	public:
		UserGroupSet() {}
		bool contains(UserGroup *g) {return Q3PtrDict<void>::find(g) != 0;}
		void insert(UserGroup *g) { Q3PtrDict<void>::insert(g, (void *)1); }
};

class ULEPrivate : public QObject, public Q3Shared
{
	Q_OBJECT
	public:
		Q3Dict<QVariant> informations;
		Q3Dict<ProtocolData> protocols;
		QList<UserGroup *> Parents;
		UserListKey key;
		ULEPrivate();
		~ULEPrivate();

		static Q3Dict<Q3Dict<UserGroupSet> > protocolUserDataProxy; // protocolName -> (fieldName -> UserGroupSet)
		static Q3Dict<UserGroupSet> userDataProxy; // field name -> UserGroupSet
//		static QDict<QPtrDict<void> > addProtocolProxy;
//		static QDict<QPtrDict<void> > removeProtocolProxy;
		static Q3Dict<UserGroupSet> statusChangeProxy;
		static void closeModule();
	public slots:
		/* potrzebne, ¿eby refreshDNSName() mia³o do czego siê pod³±czyæ
		   inaczej mo¿e siê zdarzyæ, ¿e w³a¶ciwy obiekt ULE ju¿ nie istnieje,
		   gdy przychodzi odpowied¼ od serwera dns*/
		void setDNSName(const QString &protocolName, const QString &dnsname);
};

class UserGroupData
{
	public:
		UserGroupData(int size);
		~UserGroupData();
		Q3IntDict<UserListElement> data;
		QList<UserListElement> list;
};

class ProtocolData : public QObject {
	public:
		QString ID;
		UserStatus *Stat;
		Q3Dict<QVariant> data;
	public:
		ProtocolData(const QString &protocolName, const QString &id);
		ProtocolData();
		ProtocolData(const ProtocolData &);
		virtual ~ProtocolData();
		ProtocolData &operator = (const ProtocolData &copyMe);
};

#pragma GCC visibility pop

#endif
