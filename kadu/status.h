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
	Offline
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
		virtual ~Status();

		bool isOnline() const;
		bool isBusy() const;
		bool isInvisible() const;
		bool isOffline() const;
		static bool isOffline(int index);
		bool hasDescription() const;
		bool isFriendsOnly() const;
		QString description() const;
		eStatus status() const;

		int index() const;
		static int index(eStatus stat, bool has_desc);

		virtual QPixmap pixmap() const;
		virtual QPixmap pixmap(const Status &) const;
		virtual QPixmap pixmap(eStatus stat, bool has_desc) const;

		static eStatus fromString(const QString& stat);
		static QString toString(eStatus stat, bool has_desc);

		static int count();
		static int initCount();
		static QString name(int nr);

		void refresh();

	public slots:
		void setOnline(const QString& desc = "");
		void setBusy(const QString& desc = "");
		void setInvisible(const QString& desc = "");
		void setOffline(const QString& desc = "");
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
		void changed(const Status& status);
};

extern QString gg_icons[];
extern int gg_statuses[];
extern const char *statustext[];
extern QStringList defaultdescriptions;
bool ifStatusWithDescription(int status);
bool isAvailableStatus(unsigned int);
int statusGGToStatusNr(int);

#endif
