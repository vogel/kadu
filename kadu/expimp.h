#ifndef EXPIMP_H
#define EXPIMP_H

#include <qhbox.h>
#include <qvaluelist.h>

#include "userlistelement.h"

class LayoutHelper;
class QLabel;
class QListView;
class QPushButton;
class QResizeEvent;

/**
	Dialog umo¿liwiaj±cy import / export listy kontaktów z serwera GG
	\class UserlistImportExport
	\brief Import / Export userlisty
**/
class UserlistImportExport : public QHBox {
	Q_OBJECT
	public:
		/**
			Konstruktor okna importu / exportu
			\fn UserlistImportExport(QWidget *parent=NULL, const char *name=NULL)
			\param parent rodzic okna
			\param name nazwa obiektu
		**/
		UserlistImportExport(QWidget *parent=NULL, const char *name=NULL);

		/**
			\fn ~UserlistImportExport()
			Destruktor okna importu / exportu
		**/
		~UserlistImportExport();
	private:
		QPushButton *pb_fetch; /*!< przycisk pobierania listy */
		QValueList<UserListElement> importedUserlist;
		QPushButton *pb_send; /*!< przycisk wysy³ania listy */
		QPushButton *pb_delete; /*!< przycisk kasuj±cy listê */
		QPushButton *pb_tofile;
		QLabel *l_itemscount;
		LayoutHelper *layoutHelper;

	protected:
		QListView *lv_userlist;
		virtual void resizeEvent(QResizeEvent *);

	private slots:
		void startImportTransfer(void);
		void makeUserlist(void);
		void updateUserlist(void);
		void fromfile();
		void userListImported(bool ok, QValueList<UserListElement> userList);
		void startExportTransfer(void);
		void clean(void);
		void ExportToFile(void);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void keyPressEvent(QKeyEvent *);
		void updateUserListCount();
};

#endif
