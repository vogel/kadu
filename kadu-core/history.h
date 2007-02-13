#ifndef HISTORY_H
#define HISTORY_H

#include <qdatetime.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qmap.h>
#include <qstring.h>

#include <time.h>
#include "gadu.h"

#define HISTORYMANAGER_ENTRY_CHATSEND   0x00000001
#define HISTORYMANAGER_ENTRY_CHATRCV    0x00000002
#define HISTORYMANAGER_ENTRY_MSGSEND    0x00000004
#define HISTORYMANAGER_ENTRY_MSGRCV     0x00000008
#define HISTORYMANAGER_ENTRY_STATUS     0x00000010
#define HISTORYMANAGER_ENTRY_SMSSEND    0x00000020
#define HISTORYMANAGER_ENTRY_ALL        0x0000003f
#define HISTORYMANAGER_ENTRY_ALL_MSGS   0x0000002f

class QComboBox;
class QRadioButton;
class QVButtonGroup;
class QHGroupBox;
class QCheckBox;
class QLineEdit;

struct HistoryEntry {
	int type;
	UinType uin;
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString message;
	unsigned int status;
	QString ip;
	QString description;
	QString mobile;
	HistoryEntry();
};

struct HistoryDate {
	QDateTime date;
	uint idx;
	HistoryDate();
};

/**
	Menad¿er historii
**/
class HistoryManager : public QObject
{
	Q_OBJECT

	public:
		HistoryManager(QObject *parent=0, const char *name=0);
		int getHistoryEntriesCount(const UinsList &uins);
		int getHistoryEntriesCount(const QString &mobile = QString::null);
		QValueList<HistoryEntry> getHistoryEntries(UinsList uins, int from, int count, int mask = HISTORYMANAGER_ENTRY_ALL);
		QValueList<HistoryDate> getHistoryDates(const UinsList &uins);
		QValueList<UinsList> getUinsLists() const;
		int getHistoryEntryIndexByDate(const UinsList &uins, const QDateTime &date, bool endate = false);
		static QString getFileNameByUinsList(UinsList uins);
		static QStringList mySplit(const QChar &sep, const QString &str);

	private:
		QString text2csv(const QString &text);
		int getHistoryEntriesCountPrivate(const QString &filename) const;
		uint getHistoryDate(QTextStream &stream);
		void buildIndexPrivate(const QString &filename);

		class BuffMessage
		{
			public:
				UinsList uins;
				QString message;
				time_t tm;
				time_t arriveTime;
				bool own;
				int counter;
				BuffMessage(const UinsList &uins1=UinsList(),
							const QString &msg=QString(),
							time_t t=0,
							time_t arriveTime1=time(NULL),
							bool own1=false,
							int cntr=1)
					: uins(uins1), message(msg), tm(t),	arriveTime(arriveTime1),
					own(own1), counter(cntr) {}
		};
		QMap<UinType, QValueList<BuffMessage> > bufferedMessages;
		QTimer *imagesTimer;

		void checkImageTimeout(UinType uin);
	private slots:
		void chatMsgReceived(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool& grab);
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void checkImagesTimeouts();
		void statusChanged(UserListElement elem, QString protocolName,
					const UserStatus &oldStatus, bool massively, bool last);

	public slots:
		void addMyMessage(const UinsList &senders, const QString &msg);

		/**
			raczej nie u¿ywaæ...
		**/
		void appendMessage(UinsList receivers, UinType sender, const QString &msg,
				bool own, time_t t=0, bool chat=true, time_t arriveTime=time(NULL));
		void appendSms(const QString &mobile, const QString &msg);
		void appendStatus(UinType uin, const UserStatus &sstatus);
		void removeHistory(const UinsList &uins);

		void convHist2ekgForm(UinsList uins);
		void convSms2ekgForm();
		void buildIndex(const UinsList &uins);
		void buildIndex(const QString &mobile = QString::null);
};

extern HistoryManager *history;

#endif
