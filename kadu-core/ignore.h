#ifndef IGNORE_H
#define IGNORE_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <q3hbox.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QKeyEvent>
#include <QLabel>

#include "usergroup.h"

class LayoutHelper;
class QLabel;
class QLineEdit;
class Q3ListBox;
class QWidget;
class QResizeEvent;
class Q3SimpleRichText;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie list± ignorowanych
**/
class Ignored : public Q3HBox
{
	Q_OBJECT

	Q3ListBox *lb_list;
	QLineEdit *e_uin;
	LayoutHelper *layoutHelper;

private slots:
	void remove();
	void add();
	void getList();
	void keyPressEvent(QKeyEvent *);

protected:
	virtual void resizeEvent(QResizeEvent *);

public:
	Ignored(QWidget *parent = 0, const char *name = 0);
	~Ignored();

};

class IgnoredManager
{
	static Q3ValueList<QPair<UserListElements, bool> > Ignored;

public:
	static void loadFromConfiguration();
	static void writeToConfiguration();
	static void insert(UserListElements uins, bool temporary = false);
	static void remove(UserListElements uins);
	static bool isIgnored(UserListElements uins);
	static void clear();
	static const Q3ValueList<QPair<UserListElements, bool> > & getList();
};

#endif
