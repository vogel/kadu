#ifndef KADU_USERLIST_PRIVATE_H
#define KADU_USERLIST_PRIVATE_H
#include <qdict.h>
#include <qintdict.h>
#include <qmap.h>
#include <qobject.h>
#include <qptrdict.h>
#include <qshared.h>
#include <qstring.h>
#include <qvariant.h>

#include "userlistelement.h"

class UserStatus;
class UserGroup;
class ProtocolData;

class UserGroupSet : public QPtrDict<void>
{
	public:
		UserGroupSet() {}
		bool contains(UserGroup *g) {return QPtrDict<void>::find(g) != 0;}
		void insert(UserGroup *g) { QPtrDict<void>::insert(g, (void *)1); }
};

class ULEPrivate : public QObject, public QShared
{
	Q_OBJECT
	public:
		QDict<QVariant> informations;
		QDict<ProtocolData> protocols;
		QValueList<UserGroup *> Parents;
		UserListKey key;
		ULEPrivate();
		~ULEPrivate();

		static QDict<QDict<UserGroupSet> > protocolUserDataProxy; // protocolName -> (fieldName -> UserGroupSet)
		static QDict<UserGroupSet> userDataProxy; // field name -> UserGroupSet
//		static QDict<QPtrDict<void> > addProtocolProxy;
//		static QDict<QPtrDict<void> > removeProtocolProxy;
		static QDict<UserGroupSet> statusChangeProxy;
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
		QIntDict<UserListElement> data;
		QValueList<UserListElement> list;
};

class ProtocolData : public QObject {
	public:
		QString ID;
		UserStatus *Stat;
		QDict<QVariant> data;
	public:
		ProtocolData(const QString &protocolName, const QString &id);
		ProtocolData();
		ProtocolData(const ProtocolData &);
		virtual ~ProtocolData();
		ProtocolData &operator = (const ProtocolData &copyMe);
};

#endif
