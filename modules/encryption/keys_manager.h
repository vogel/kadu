#ifndef KEYSMANAGER_H
#define KEYSMANAGER_H

#include <qhbox.h>
#include <qlistview.h>
#include "usergroup.h"
#include "userlistelement.h"

class LayoutHelper;
class QDialog;
class QPushButton;
class QResizeEvent;
class QTextEdit;
class QListView;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie list± kluczy
**/
class KeysManager : public QHBox
{
	Q_OBJECT
	public:
		KeysManager(QDialog *parent = 0, const char *name = "keys_manager");
		~KeysManager();
		void turnContactEncryptionText(QString id, bool on);
		void refreshKeysList();

	private:
		LayoutHelper *layoutHelper;
		QListView *lv_keys;
		QTextEdit *e_key;
		QPushButton *pb_del;
		QPushButton *pb_on;
		void getKeysList(QStringList &uins);
		void getKeyInfo();
		void turnEncryptionBtn(bool on);
		QString bool2text(bool on);

	private slots:
		void removeKey();
		void selectionChanged();
		void keyPressEvent(QKeyEvent *);
		void turnEncryption();
		void turnEncryption(QListViewItem *);

	protected:
		virtual void resizeEvent(QResizeEvent *);

	signals:
		void keyRemoved(UserListElement ule);
		void turnEncryption(UserGroup* group, bool on);
};
#endif
