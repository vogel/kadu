#ifndef ABOUT_H
#define ABOUT_H

#include <qwidget.h>
#include <qhbox.h>
/*
	Okno z informacjami o programie
*/
class About : public QHBox {
	Q_OBJECT

public:
	About();
	~About();

private:
	QString loadFile(const QString &name);
};

#endif
