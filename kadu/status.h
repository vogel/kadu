#ifndef STATUS_H
#define STATUS_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

enum eStatus
{
	Online,
	Busy,
	Invisible,
	Offline,
	Blocking
};

class Status : public QObject
{
	Q_OBJECT

	private:
		bool Changed;

	protected:
		eStatus Stat;
		QString Description;
		bool FriendsOnly;

	public:
		Status();
		Status(const Status &copyMe);
		void operator = (const Status &copyMe);
		virtual ~Status();

		bool isOnline() const;
		bool isBusy() const;
		bool isInvisible() const;
		bool isOffline() const;
		bool isBlocking() const;
		static bool isOffline(int index);
		bool hasDescription() const;
		bool isFriendsOnly() const;
		QString description() const;
		eStatus status() const;

		int index() const;
		static int index(eStatus stat, bool has_desc);

		virtual QPixmap pixmap(bool mobile = false) const;
		virtual QPixmap pixmap(const Status &, bool mobile = false) const;
		virtual QPixmap pixmap(eStatus stat, bool has_desc, bool mobile = false) const;

		static eStatus fromString(const QString& stat);
		static QString toString(eStatus stat, bool has_desc);

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
		void setStatus(const Status& stat);
		void setStatus(eStatus stat, const QString& desc = "");
		void setIndex(int index, const QString& desc = "");
		void setFriendsOnly(bool f);

	signals:
		void goOnline(const QString& desc);
		void goBusy(const QString& desc);
		void goInvisible(const QString& desc);
		void goOffline(const QString& desc);
		void goBlocking();
		void changed(const Status& status);
};

extern QStringList defaultdescriptions;

#endif
