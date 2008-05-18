#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <qwidget.h>
#include <QKeyEvent>
#include <QList>
#include <QCloseEvent>
#include "gadu.h"
#include "history.h"
#include "history_search_dialog.h"
#include <qtreewidget.h>

class ChatMessage;
class ChatMessagesView;

class QTreeWidget;
class QTreeWidgetItem;

class UinsListViewText : public QTreeWidgetItem {
	public:
		UinsListViewText(QTreeWidget *parent, const UinsList &uins);
		const UinsList &getUinsList() const;

	private:
		UinsList uins;
};

class DateListViewText : public QTreeWidgetItem {
	public:
		DateListViewText(QTreeWidgetItem *parent, const HistoryDate &date);
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

	QTreeWidget *uinslv;
	ChatMessagesView* body;
	UinsList uins;
	int start;
	HistoryFindRec findrec;
	bool closeDemand;
	bool finding;
	QList<HistoryDate> dateentries;

	virtual void keyPressEvent(QKeyEvent *e);

private slots:
	void showStatusChanged(bool);

public:
	HistoryDialog(UinsList uins);

public slots:
	void uinsChanged(QTreeWidgetItem *item, int col);
	void dateChanged(QTreeWidgetItem *item, int col);
	void searchBtnClicked();
	void searchNextBtnClicked();
	void searchPrevBtnClicked();

};

#endif
