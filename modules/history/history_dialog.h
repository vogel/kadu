#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <qdialog.h>
#include "gadu.h"
#include "history.h"

class KaduTextBrowser;
class QListView;
class QListViewItem;

/**
	History dialog
**/
class HistoryDialog : public QDialog {
	Q_OBJECT

	public:
		HistoryDialog(UinsList uins);

	public slots:
		void uinsChanged(QListViewItem *item);
		void dateChanged(QListViewItem *item);
		void searchBtnClicked();
		void searchNextBtnClicked();
		void searchPrevBtnClicked();

	protected:
		void formatHistoryEntry(QString &text, const HistoryEntry &entry, QStringList &paracolors);
		void showHistoryEntries(int from, int count);
		void setDateListViewText(const QDateTime &datetime);
		void searchHistory();
		static const QString &gaduStatus2symbol(unsigned int status);
		void closeEvent(QCloseEvent *e);

		QListView *uinslv;
		KaduTextBrowser* body;
		UinsList uins;
		int start;
		HistoryFindRec findrec;
		bool closeDemand;
		bool finding;
		QValueList<HistoryDate> dateentries;
};

#endif
