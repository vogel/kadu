#ifndef USERLIST_H
#define USERLIST_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qtimer.h>
#include <qhostaddress.h>
#include <qdns.h>
#include <qptrlist.h>

#include "gadu.h"

class DnsHandler : public QObject
{
	Q_OBJECT

	public:
		DnsHandler(UinType uin);
		~DnsHandler();
		bool isCompleted();

		static int counter;

	private:
		QDns dnsresolver;
		UinType uin;
		bool completed;

	public slots:
		void resultsReady();
};

typedef QPtrList<DnsHandler> DnsLookups;

class UserList;

struct UserListElement
{
	private:
		QString Group;
		QString FirstName;
		QString LastName;
		QString NickName;
		QString AltNick;
		QString Mobile;
		QString Email;
		UinType Uin;
		Status *Stat;
		int MaxImageSize;
		bool Anonymous;
		QHostAddress Ip;
		QString DnsName;
		short Port;
		int Version;
		bool Blocking;
		bool OfflineTo;
		bool Notify;

		UserList *Parent;
		friend class UserList;

	public:
		UserListElement(UserList* parent);
		UserListElement(const UserListElement &copyMe);
		UserListElement();
		virtual ~UserListElement();

		void operator = (const UserListElement &copyMe);

		QString group() const;
		void setGroup(const QString& group);

		QString firstName() const;
		void setFirstName(const QString &firstName);

		QString lastName() const;
		void setLastName(const QString &lastName);

		QString nickName() const;
		void setNickName(const QString &nickName);

		QString altNick() const;
		void setAltNick(const QString &altNick);

		QString mobile() const;
		void setMobile(const QString &mobile);

		QString email() const;
		void setEmail(const QString &email);

		UinType uin() const;
		void setUin(const UinType &uin);

		const Status & status() const;
		Status & status();

		int maxImageSize() const;
		void setMaxImageSize(const int maxImageSize);

		bool anonymous() const;
		void setAnonymous(const bool anonymous);

		const QHostAddress & ip() const;
		QHostAddress & ip();

		QString dnsName() const;
		void setDnsName(const QString &dnsName);

		short port() const;
		void setPort(short port);

		int version() const;
		void setVersion(const int version);

		bool blocking() const;
		void setBlocking(const bool blocking);

		bool offlineTo() const;
		void setOfflineTo(const bool offlineTo);

		bool notify() const;
		void setNotify(const bool notify);
};

/**
	Klasa reprezentuj±ca listê kontaktów.
	indexami s± altnicki.
**/
class UserList : public QObject, public QMap<QString,UserListElement>
{
	Q_OBJECT

	protected:
		DnsLookups dnslookups;
		friend class UserListElement;

	public:
		UserList();
		UserList(const UserList& source);
		~UserList();
		UserList& operator=(const UserList& userlist);

		UserListElement& byUin(UinType uin);
		UserListElement& byNick(const QString& nickname);
		UserListElement& byAltNick(const QString& altnick);
		UserListElement byUinValue(UinType uin);

		bool containsUin(UinType uin) const;
		bool containsAltNick(const QString& altnick) const;

	public slots:
		void addUser(UserListElement &ule);
		void addAnonymous(UinType uin);
		void removeUser(const QString &altnick);
		void changeUserInfo(const QString& old_altnick, const UserListElement& new_data);
		bool writeToFile(QString filename = "");
		bool readFromFile();
		void setDnsName(UinType uin, const QString &name);
		void addDnsLookup(UinType uin, const QHostAddress &ip);
		void merge(UserList &userlist);

	signals:
		void modified();
		void userAdded(const UserListElement& user);
		void dnsNameReady(UinType);

		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);
};

extern UserList userlist;

#endif

