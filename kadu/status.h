#ifndef KADU_STATUS_H
#define KADU_STATUS_H

#include <qobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qstringlist.h>

enum eUserStatus
{
	Online,
	Busy,
	Invisible,
	Offline,
	Blocking
};

class UserStatus : public QObject
{
	Q_OBJECT

	private:
		bool Changed;

	protected:
		eUserStatus Stat;
		QString Description;
		bool FriendsOnly;

	public:
		UserStatus();
		UserStatus(const UserStatus &copyMe);
		void operator = (const UserStatus &copyMe);
		bool operator == (const UserStatus &compare) const;
		bool operator != (const UserStatus &compare) const;
		virtual ~UserStatus();

		bool isOnline() const;
		bool isBusy() const;
		bool isInvisible() const;
		bool isOffline() const;
		bool isBlocking() const;
		static bool isOffline(int index);
		bool hasDescription() const;
		bool isFriendsOnly() const;
		QString description() const;
		eUserStatus status() const;

		int index() const;
		static int index(eUserStatus stat, bool has_desc);

		virtual QPixmap pixmap(bool mobile = false) const;
		virtual QPixmap pixmap(const UserStatus &, bool mobile = false) const;
		virtual QPixmap pixmap(eUserStatus stat, bool has_desc, bool mobile = false) const;

		static eUserStatus fromString(const QString& stat);
		static QString toString(eUserStatus stat, bool has_desc);

		static int count();
		static int initCount();
		static QString name(int nr);
		QString name() const;

		void refresh();

	public slots:
		void setOnline(const QString& desc = "");
		void setBusy(const QString& desc = "");
		void setInvisible(const QString& desc = "");
		void setOffline(const QString& desc = "");
		void setBlocking();
		void setDescription(const QString& desc = "");
		void setStatus(const UserStatus& stat);
		void setStatus(eUserStatus stat, const QString& desc = "");
		void setIndex(int index, const QString& desc = "");
		void setFriendsOnly(bool f);

	signals:
		void goOnline(const QString& desc);
		void goBusy(const QString& desc);
		void goInvisible(const QString& desc);
		void goOffline(const QString& desc);
		void goBlocking();
		void changed(const UserStatus& status);
};

extern QStringList defaultdescriptions;

#endif
