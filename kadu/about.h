#ifndef ABOUT_H
#define ABOUT_H

#include <qdialog.h>

/*
	Okno z informacjami o programie
*/
class About : public QDialog {
	Q_OBJECT

public:
	About();

private:
	QString loadFile(const QString &name);
};

#endif
