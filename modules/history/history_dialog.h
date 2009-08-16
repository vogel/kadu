#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <QtGui/QWidget>
#include <QtGui/QTreeWidgetItem>

#include "gadu.h"
#include "history.h"
#include "history_search_dialog.h"

class QTreeWidget;
class QMenu;

class ChatMessage;
class ChatMessagesView;

class UinsListViewText : public QTreeWidgetItem
{
	UinsList uins;

public:
	UinsListViewText(QTreeWidget *parent, const UinsList &uins);
	const UinsList &getUinsList() const;

};

class DateListViewText : public QObject, public QTreeWidgetItem
{
	Q_OBJECT

	HistoryDate date;
	bool containsMessages;

public:
	DateListViewText(QTreeWidgetItem *parent, UinsList uins, const HistoryDate &date, const QList<QDate> &messageDates);
	const HistoryDate &getDate() const;

public slots:
	void showStatusChanges(bool showStatus);

};

/**
	History dialog
**/
class HistoryDialog : public QWidget
{
	Q_OBJECT

	bool ShowStatus;

private slots:
	void showStatusChangesSlot(bool);

protected:
	ChatMessage * createChatMessage(const HistoryEntry &entry);
	void showHistoryEntries(int from, int count);
	void setDateListViewText(const QDateTime &datetime);
	void searchHistory();
	static const QString &gaduStatus2symbol(unsigned int status);
	void closeEvent(QCloseEvent *e);

	QTreeWidget *uinsTreeWidget;
	ChatMessagesView* body;
	UinsList uins;
	quint64 start;
	HistoryFindRec findRec;
	bool closeDemand;
	bool finding;
	bool showResults;
	QList<HistoryDate> dateEntries;
	QMenu *popupMenu;

	virtual void keyPressEvent(QKeyEvent *e);

public:
	HistoryDialog(UinsList uins);

public slots:
	void uinsChanged(QTreeWidgetItem *item);
	void dateChanged(QTreeWidgetItem*item);
	void searchButtonClicked();
	void searchNextButtonClicked();
	void searchPrevButtonClicked();
	void showPopupMenu(const QPoint &pos);
	void clearHistory(bool);
	void rebuildIndex(bool);
	void pageLoaded(bool);

signals:
	void showStatusChanges(bool);

};

#endif
