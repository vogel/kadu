#ifndef EXPIMP_H
#define EXPIMP_H

#include <qdialog.h>
#include <qlistview.h>
#include <qstring.h>
#include <qsocketnotifier.h>
#include <qhbox.h>
#include <qlabel.h>

#include "userlist.h"

/**
	Dialog umo¿liwiaj±cy import / export listy kontaktów z serwera GG
**/
class UserlistImportExport : public QHBox {
	Q_OBJECT
	public:
		UserlistImportExport(QWidget *parent=NULL, const char *name=NULL);
		~UserlistImportExport();
	private:
		int ret;
		QPushButton *pb_fetch;
		UserList importedUserlist;
		QPushButton *pb_send;
		QPushButton *pb_delete;
		QPushButton *pb_tofile;
		QLabel *l_itemscount;

	protected:
		QListView *lv_userlist;

	private slots:
		void startImportTransfer(void);
		void makeUserlist(void);
		void updateUserlist(void);
		void fromfile();
		void userListImported(bool ok, UserList& userList);
		void startExportTransfer(void);
		void clean(void);
		void ExportToFile(void);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void keyPressEvent(QKeyEvent *);
};

#endif

