#ifndef KADU_USERLIST_PRIVATE_H
#define KADU_USERLIST_PRIVATE_H

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QSharedData>

#include "userlistelement.h"

class ProtocolData;
class UserGroup;

#pragma GCC visibility push(hidden)

typedef QSet<UserGroup *> UserGroupSet;

class ULEPrivate : public QObject, public QSharedData
{
	Q_OBJECT

public:
	QHash<QString, QVariant *> informations;
	QHash<QString, ProtocolData *> protocols;
	QList<UserGroup *> Parents;
	ULEPrivate();
	~ULEPrivate();

	static QHash<QString, QHash<QString, UserGroupSet> > protocolUserDataProxy; // protocolName -> (fieldName -> UserGroupSet)
	static QHash<QString, UserGroupSet> userDataProxy; // field name -> UserGroupSet
//	static QDict<QPtrDict<void> > addProtocolProxy;
//	static QDict<QPtrDict<void> > removeProtocolProxy;
	static QHash<QString, UserGroupSet> statusChangeProxy;
	static void closeModule();

public slots:
	/* potrzebne, �eby refreshDNSName() mia�o do czego si� pod��czy�
	   inaczej mo�e si� zdarzy�, �e w�a�ciwy obiekt ULE ju� nie istnieje,
	   gdy przychodzi odpowied� od serwera dns*/
	void setDNSName(const QString &protocolName, const QString &dnsname);

};

class GaduStatus;
class ProtocolData : public QObject
{

public:
	QString ID;
	GaduStatus *Stat;
	QHash<QString, QVariant *> data;

	ProtocolData(const QString &protocolName, const QString &id);
	ProtocolData();
	ProtocolData(const ProtocolData &);
	virtual ~ProtocolData();
	ProtocolData & operator = (const ProtocolData &copyMe);

};

#pragma GCC visibility pop

#endif
