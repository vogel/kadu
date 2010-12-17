/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEXT_PROGRESS_H
#define TEXT_PROGRESS_H

#include <QtGui/QLabel>

class QTimer;

class TextProgress : public QLabel
{
	Q_OBJECT
	
	QString text_;
	QTimer *timer_;
	static const int dotCount_;
	int counter_;
	
	void setProgressText();
	
private slots:
	void timeout();

public:
	TextProgress(QWidget *parent, const QString &text = QString());
	void clearProgressText();

public slots:
	void setText(\const QString &text);
};

#endif // TEXT_PROGRESS_H
