#ifndef ABOUT_H
#define ABOUT_H

#include <qtabdialog.h>

/*
	Okno z informacjami o programie
*/
class About : public QTabDialog {
	Q_OBJECT

public:
	About();

private:
	void addTab1();
	void addTab2();
	void addTab3();
	void addTab4();
	void addTab5();

	QString loadFile(const QString &name);
};

#endif
