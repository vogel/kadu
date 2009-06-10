/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PASSWORD_WINDOW_H
#define PASSWORD_WINDOW_H

#include <QtGui/QWidget>

class QCheckBox;
class QLineEdit;

class PasswordWindow : public QWidget
{
	Q_OBJECT

	QLineEdit *Password;
	QCheckBox *Store;

private slots:
	void accepted();

public:
	static void getPassword(const QString &message, QObject *receiver, const char *slot);
	
	explicit PasswordWindow(const QString &message, QWidget* parent);
	virtual ~PasswordWindow();

signals:
	void passwordEntered(const QString &password, bool permament);

};

#endif // PASSWORD_WINDOW_H
