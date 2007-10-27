#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <qwidget.h>
#include "gadu.h"
#include "history.h"
#include "history_search_dialog.h"

class ChatMessage;
class ChatMessagesView;

class QListView;
class QListViewItem;

class UinsListViewText : public QListViewItem {
	public:
		UinsListViewText(QListView *parent, const UinsList &uins);
		const UinsList &getUinsList() const;

	private:
		UinsList uins;
};

class DateListViewText : public QListViewItem {
	public:
		DateListViewText(QListViewItem *parent, const HistoryDate &date);
		const HistoryDate &getDate() const;

	private:
		HistoryDate date;
};

/**
	History dialog
**/
class HistoryDialog : public QWidget {
	Q_OBJECT

protected:
	ChatMessage * createChatMessage(const HistoryEntry &entry);
	void showHistoryEntries(int from, int count);
	void setDateListViewText(const QDateTime &datetime);
	void searchHistory();
	static const QString &gaduStatus2symbol(unsigned int status);
	void closeEvent(QCloseEvent *e);

	QListView *uinslv;
	ChatMessagesView* body;
	UinsList uins;
	int start;
	HistoryFindRec findrec;
	bool closeDemand;
	bool finding;
	QValueList<HistoryDate> dateentries;

private slots:
	void showStatusChanged(bool);

public:
	HistoryDialog(UinsList uins);

public slots:
	void uinsChanged(QListViewItem *item);
	void dateChanged(QListViewItem *item);
	void searchBtnClicked();
	void searchNextBtnClicked();
	void searchPrevBtnClicked();

};

#endif
