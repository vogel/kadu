/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOKEN_DIALOG_H
#define TOKEN_DIALOG_H

#include <QtGui/QDialog>

class QLineEdit;

class TokenDialog : public QDialog
{
	QLineEdit *tokenedit;

public:
	TokenDialog(QPixmap tokenImage, QDialog *parent = 0);
	void getValue(QString &tokenValue);
	QString getValue();
};

#endif // TOKEN_DIALOG_H
