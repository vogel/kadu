/***************************************************************************
                          about.h  -  description
                             -------------------
    begin                : Sat Nov 3 2001
    copyright            : (C) 2001 by tomee
    email                : tomee@cpi.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ABOUT_H
#define ABOUT_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qsimplerichtext.h>
#include <qtimer.h>
#include "../libgadu/lib/libgadu.h"

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
