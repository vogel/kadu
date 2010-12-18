#ifndef KEYSMANAGER_H
#define KEYSMANAGER_H

#include <QtGui/QWidget>

#include "usergroup.h"
#include "userlistelement.h"

class QDialog;
class QPushButton;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;

/**
	Dialog umo�liwiaj�cy zarz�dzanie list� kluczy
**/
class KeysManager : public QWidget
{
	Q_OBJECT

		QTreeWidget *lv_keys;
		QTextEdit *e_key;
		QPushButton *pb_del;
		QPushButton *pb_on;
		void getKeysList(QStringList &uins);
		void getKeyInfo();
		void turnEncryptionBtn(bool on);
		QString bool2text(bool on);
		QTreeWidgetItem *getSelected();

	private slots:
		void removeKey();
		void selectionChanged();
		void turnEncryption();
		void turnEncryption(QTreeWidgetItem *);
	protected:
		virtual void keyPressEvent(QKeyEvent *event);
	public:
		KeysManager(QDialog *parent = 0);
		~KeysManager();
		void turnContactEncryptionText(QString id, bool on);
		void refreshKeysList();

	signals:
		void keyRemoved(UserListElement ule);
		void turnEncryption(UserGroup group, bool on);
};
#endif
