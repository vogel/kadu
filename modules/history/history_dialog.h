#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <qwidget.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <Q3ValueList>
#include <QCloseEvent>
#include "gadu.h"
#include "history.h"
#include "history_search_dialog.h"

class ChatMessage;
class ChatMessagesView;

class Q3ListView;
class Q3ListViewItem;

class UinsListViewText : public Q3ListViewItem {
	public:
		UinsListViewText(Q3ListView *parent, const UinsList &uins);
		const UinsList &getUinsList() const;

	private:
		UinsList uins;
};

class DateListViewText : public Q3ListViewItem {
	public:
		DateListViewText(Q3ListViewItem *parent, const HistoryDate &date);
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

	Q3ListView *uinslv;
	ChatMessagesView* body;
	UinsList uins;
	int start;
	HistoryFindRec findrec;
	bool closeDemand;
	bool finding;
	Q3ValueList<HistoryDate> dateentries;

	virtual void keyPressEvent(QKeyEvent *e);

private slots:
	void showStatusChanged(bool);

public:
	HistoryDialog(UinsList uins);

public slots:
	void uinsChanged(Q3ListViewItem *item);
	void dateChanged(Q3ListViewItem *item);
	void searchBtnClicked();
	void searchNextBtnClicked();
	void searchPrevBtnClicked();

};

#endif
