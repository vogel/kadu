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
		About(QWidget *parent=NULL, const char *name=NULL);
		~About();

	private:
		QString loadFile(const QString &name);
		void keyPressEvent(QKeyEvent *);
};

#endif
