#ifndef IGNORE_H
#define IGNORE_H

#include <qhbox.h>
#include <qvaluelist.h>

#include "misc.h"

class QLineEdit;
class QListBox;

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

	private slots:
		void remove();
		void add();
		void getList();
		void keyPressEvent(QKeyEvent *);
};

int readIgnored();
int writeIgnored(QString filename = QString::null);
void addIgnored(UinsList uins);
void delIgnored(UinsList uins);
bool isIgnored(UinsList uins);
void clearIgnored();

// Ominiecie bledu w gcc 3.2
static QValueList<UinsList> ignored_gcc32_bug;
////////////////////////////
extern QValueList<UinsList> ignored;

#endif
