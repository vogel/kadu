#ifndef IGNORE_H
#define IGNORE_H

#include <qhbox.h>
#include <qvaluelist.h>

#include "usergroup.h"

class LayoutHelper;
class QLabel;
class QLineEdit;
class QListBox;
class QDialog;
class QResizeEvent;
class QSimpleRichText;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie list± ignorowanych
**/
class Ignored : public QHBox
{
	Q_OBJECT
	public:
		Ignored(QDialog *parent = 0, const char *name = 0);
		~Ignored();

	private:
		QListBox *lb_list;
		QLineEdit *e_uin;
		LayoutHelper *layoutHelper;

	private slots:
		void remove();
		void add();
		void getList();
		void keyPressEvent(QKeyEvent *);
	protected:
		virtual void resizeEvent(QResizeEvent *);
};

int readIgnored();
int writeIgnored(QString filename = QString::null);
void addIgnored(UserListElements uins);
void delIgnored(UserListElements uins);
bool isIgnored(UserListElements uins);
void clearIgnored();

// workaround for bug in gcc 3.2
#ifdef __GNUC__
#if __GNUC__ == 3 && __GNUC_MINOR__ == 2
static QValueList<UserListElements> ignored_gcc32_bug;
#endif
#endif
////////////////////////////

extern QValueList<UserListElements> ignored;

#endif
