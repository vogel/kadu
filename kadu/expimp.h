#ifndef EXPIMP_H
#define EXPIMP_H

#include <qdialog.h>
#include <qlistview.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include "libgadu.h"
#include "userlist.h"

/**
	Dialog umo¿liwiaj±cy import listy kontaktów z serwera GG
**/
class UserlistImport : public QDialog {
	Q_OBJECT
	public:
		UserlistImport(QWidget *parent=0, const char *name=0);
		void init(void);
		
	private:
		int ret;
		QArray<struct userlist> importedusers;
		QPushButton *fetchbtn;
		UserList importedUserlist;
		QString importreply;

	protected:
		QListView *results;

	public slots:
		void userlistReplyReceivedSlot(char type, char *reply);

	private slots:
		void startTransfer(void);
		void makeUserlist(void);
		void updateUserlist(void);
		void fromfile();
	protected:
		void closeEvent(QCloseEvent *e);
};

/**
	Dialog umo¿liwiaj±cy eksport listy kontaktów na serwer GG
**/
class UserlistExport : public QDialog {
	Q_OBJECT
	public:
		UserlistExport(QWidget *parent=0, const char *name=0);

	private:
		int ret;
		QPushButton * sendbtn;
		QPushButton * deletebtn;
		QPushButton * tofilebtn;
		QString saveContacts();

	public slots:
		void userlistReplyReceivedSlot(char type, char *reply);

	private slots:
		void startTransfer(void);
		void clean(void);
		void ExportToFile(void);

	protected:
		void closeEvent(QCloseEvent * e);
};

#endif
