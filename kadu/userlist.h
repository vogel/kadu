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
		UserList* Parent;
		friend class UserList;

	public:
		UserListElement(UserList* parent);
		UserListElement(const UserListElement &copyMe);
		UserListElement();
		virtual ~UserListElement();

		void operator = (const UserListElement &copyMe);

		QString group() const;
		void setGroup(const QString& group);

		// te trzeba kiedys trzeba tak uporzadkowac
		// jak Group
		QString first_name;
		QString last_name;
		QString nickname;
		QString altnick;
		QString mobile;
		QString email;
		UinType uin;
		Status *status;
		int image_size;
		bool anonymous;
		QHostAddress ip;
		QString dnsname;
		short port;
		int version;
		bool blocking;
		bool offline_to_user;
		bool notify;
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
};

extern UserList userlist;

#endif

