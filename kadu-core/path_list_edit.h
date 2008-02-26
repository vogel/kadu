#ifndef PATH_LIST_EDIT_H
#define PATH_LIST_EDIT_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QCloseEvent>

class QLineEdit;
class Q3ListBox;
class QPushButton;

class PathListEditWindow;

class PathListEdit : public QPushButton
{
	Q_OBJECT

	PathListEditWindow *Dialog;
	QStringList PathList;

private slots:
	void showDialog();
	void dialogDestroyed();
	void pathListChanged(const QStringList &pathList);

public:
	PathListEdit(QWidget *parent = 0, char *name = 0);
	virtual ~PathListEdit() {}

	QStringList pathList() const { return PathList; }
	void setPathList(const QStringList &pathList);

signals:
	void changed();
};

class PathListEditWindow : public QWidget
{
	Q_OBJECT

	Q3ListBox *PathListBox;
	QLineEdit *PathEdit;

	bool validatePath(QString &path);

private slots:
	void addPathClicked();
	void changePathClicked();
	void deletePathClicked();

	void choosePathClicked();

	void okClicked();

	void closeEvent(QCloseEvent *e);
	void keyPressEvent(QKeyEvent *e);
	void currentItemChanged(const QString &newItem);

public:
	PathListEditWindow(const QStringList &pathList, QWidget *parent = 0, const char *name = 0);
	~PathListEditWindow();

public slots:
	void setPathList(const QStringList &list);

signals:
	void changed(const QStringList &paths);

};

#endif // PATH_LIST_EDIT_H
