#ifndef HISTORY_H
#define HISTORY_H

#include <qdialog.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qhgroupbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qvbuttongroup.h>
#include <qevent.h>
#include <qlistbox.h>
#include <qlistview.h>

#include "misc.h"

#define	HISTORYMANAGER_ENTRY_CHATSEND	0x00000001
#define	HISTORYMANAGER_ENTRY_CHATRCV	0x00000002
#define	HISTORYMANAGER_ENTRY_MSGSEND	0x00000004
#define	HISTORYMANAGER_ENTRY_MSGRCV	0x00000008
#define	HISTORYMANAGER_ENTRY_STATUS	0x00000010
#define	HISTORYMANAGER_ENTRY_SMSSEND	0x00000020
#define HISTORYMANAGER_ENTRY_ALL	0x0000003f
#define HISTORYMANAGER_ENTRY_ALL_MSGS	0x0000002f

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
};

struct HistoryFindRec {
	QDateTime fromdate;
	QDateTime todate;
	int type;
	QString data;
	bool reverse;
	int actualrecord;
};

struct HistoryDate {
	QDateTime date;
	uint idx;
};

class UinsListViewText : public QListViewItem {
	public:
		UinsListViewText(QListView *parent, UinsList &uins);
		UinsList &getUinsList();

	private:
		UinsList uins;
};

class DateListViewText : public QListViewItem {
	public:
		DateListViewText(QListViewItem *parent, HistoryDate &date);
		HistoryDate &getDate();

	private:
		HistoryDate date;
};

/**
	Okno historii rozmowy
**/
class History : public QDialog {
	Q_OBJECT

	public:
		History(UinsList uins);
		static void initModule();

	public slots:
		void uinsChanged(QListViewItem *item);
		void dateChanged(QListViewItem *item);
		void searchBtnClicked();
		void searchNextBtnClicked();
		void searchPrevBtnClicked();

	protected:
		void formatHistoryEntry(QString &text, const HistoryEntry &entry, QStringList &paracolors);
		void showHistoryEntries(int from, int count);
		void setDateListViewText(QDateTime &datetime);
		void searchHistory();
		QString gaduStatus2symbol(unsigned int status);
		void closeEvent(QCloseEvent *e);

		QListView *uinslv;
		KaduTextBrowser* body;
		UinsList uins;
		int start;
		HistoryFindRec findrec;
		bool closeDemand;
		bool finding;
		QValueList<HistoryDate> dateentries;
		int ParagraphSeparator;
};

class HistorySearch : public QDialog {
	Q_OBJECT
	public:
		HistorySearch(QWidget *parent, UinsList uins);
		void setDialogValues(HistoryFindRec &findrec);
		HistoryFindRec getDialogValues();

	public slots:
		void correctFromDays(int index);
		void correctToDays(int index);
		void fromToggled(bool on);
		void toToggled(bool on);
		void criteriaChanged(int id);
		void findBtnClicked();
		void cancelBtnClicked();
		void resetBtnClicked();

	protected:
		QHGroupBox *from_hgb, *to_hgb, *phrase_hgb, *status_hgb;
		QCheckBox *from_chb, *to_chb, *reverse_chb;
		QComboBox *from_day_cob, *from_month_cob, *from_year_cob, *from_hour_cob, *from_min_cob;
		QComboBox *to_day_cob, *to_month_cob, *to_year_cob, *to_hour_cob, *to_min_cob;
		QComboBox *status_cob;
		QLineEdit *phrase_edit;
		QVButtonGroup *criteria_bg;
		QRadioButton *phrase_rb, *status_rb;
		QStringList numslist;
		UinsList uins;

		void resetFromDate();
		void resetToDate();
};

/**
	Menad¿er historii
**/
class HistoryManager : public QObject
{
	Q_OBJECT

	public:
		HistoryManager();
		int getHistoryEntriesCount(UinsList uins);
		int getHistoryEntriesCount(QString mobile = QString::null);
		QValueList<HistoryEntry> getHistoryEntries(UinsList uins, int from, int count, int mask = HISTORYMANAGER_ENTRY_ALL);
		QValueList<HistoryDate> getHistoryDates(UinsList uins);
		QValueList<UinsList> getUinsLists();
		int getHistoryEntryIndexByDate(UinsList uins, QDateTime &date, bool endate = false);
		static QString getFileNameByUinsList(UinsList &uins);
		static QStringList mySplit(const QChar &sep, const QString &str);

	private:
		QString text2csv(const QString &text);
		int getHistoryEntriesCountPrivate(const QString &filename);
		uint getHistoryDate(QTextStream &stream);
		void buildIndexPrivate(const QString &filename);

	public slots:
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);
		void appendMessage(UinsList receivers, UinType sender, const QString &msg, bool own, time_t=0, bool chat=true);
		void appendSms(const QString &mobile, const QString &msg);
		void appendStatus(UinType uin, const UserStatus &sstatus);
		void removeHistory(UinsList uins);

		void convHist2ekgForm(UinsList uins);
		void convSms2ekgForm();
		void buildIndex(UinsList uins);
		void buildIndex(QString mobile = QString::null);
};

extern HistoryManager history;

class HistorySlots: public QObject
{
	Q_OBJECT
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void updateQuoteTimeLabel(int);
};

#endif
