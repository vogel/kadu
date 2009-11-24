/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DELAYED_LINE_EDIT_H
#define DELAYED_LINE_EDIT_H

#include <QtCore/QTimer>
#include <QtGui/QLineEdit>
#include "exports.h"

class KADUAPI DelayedLineEdit : public QLineEdit
{
	Q_OBJECT

	QTimer Timer;

private slots:
	void timeout();
	void textChangedSlot(const QString &text);

public:
	DelayedLineEdit(QWidget *parent = 0);
	virtual ~DelayedLineEdit();

	void setDelay(unsigned int delay);

signals:
	void delayedTextChanged(const QString &text);

};

#endif // DELAYED_LINE_EDIT_H
