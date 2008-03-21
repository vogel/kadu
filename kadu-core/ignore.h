#ifndef IGNORE_H
#define IGNORE_H

#include <QWidget>

#include "usergroup.h"

class LayoutHelper;
class QLineEdit;
class QListWidget;

/**
	Dialog umo¿liwiaj±cy zarz±dzanie list± ignorowanych
**/
class Ignored : public QWidget
{
	Q_OBJECT

	QListWidget *lb_list;
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
	static QList<QPair<UserListElements, bool> > Ignored;

public:
	static void loadFromConfiguration();
	static void writeToConfiguration();
	static void insert(UserListElements uins, bool temporary = false);
	static void remove(UserListElements uins);
	static bool isIgnored(UserListElements uins);
	static void clear();
	static const QList<QPair<UserListElements, bool> > & getList();
};

#endif
