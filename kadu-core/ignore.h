#ifndef IGNORE_H
#define IGNORE_H

#include <qhbox.h>
#include <qvaluelist.h>

#include "usergroup.h"

class LayoutHelper;
class QLabel;
class QLineEdit;
class QListBox;
class QWidget;
class QResizeEvent;
class QSimpleRichText;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie list± ignorowanych
**/
class Ignored : public QHBox
{
	Q_OBJECT

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

public:
	Ignored(QWidget *parent = 0, const char *name = 0);
	~Ignored();

};

class IgnoredManager
{
	static QValueList<QPair<UserListElements, bool> > Ignored;

public:
	static void loadFromConfiguration();
	static void writeToConfiguration();
	static void insert(UserListElements uins, bool temporary = false);
	static void remove(UserListElements uins);
	static bool isIgnored(UserListElements uins);
	static void clear();
	static const QValueList<QPair<UserListElements, bool> > & getList();
};

#endif
