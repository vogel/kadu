#ifndef ABOUT_H
#define ABOUT_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qsimplerichtext.h>
#include <qtimer.h>

/*
	Okno z informacjami o programie
*/
class About : public QDialog {
	Q_OBJECT

public:
	About();
	~About();

private slots:
	virtual void paintEvent(QPaintEvent *);
	void scrollText();

private:
	int posy;
	int w,h;
	QLabel *m_pfield;
	QPixmap *m_ppix_text;
	QPixmap *m_pPixmap;
	QPixmap *m_pBackground;
	QSimpleRichText * doc;
	QTimer * timer;
};

#endif
