#ifndef EXPIMP_H
#define EXPIMP_H

#include <qdialog.h>
#include <qlistview.h>
#include <qstring.h>
#include <qsocketnotifier.h>

#include "userlist.h"

/**
	Dialog umo¿liwiaj±cy import listy kontaktów z serwera GG
**/
class UserlistImport : public QDialog {
	Q_OBJECT
	public:
		UserlistImport(QWidget *parent=0, const char *name=0);
		
	private:
		QPushButton *fetchbtn;
		UserList importedUserlist;

	protected:
		QListView *results;

	private slots:
		void startTransfer(void);
		void makeUserlist(void);
		void updateUserlist(void);
		void fromfile();
		void userListImported(bool ok, UserList& userList);

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

	private slots:
		void startTransfer(void);
		void clean(void);
		void ExportToFile(void);
		void userListExported(bool ok);
		void userListCleared(bool ok);

};

#endif

