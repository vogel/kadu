/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WINDOW_NOTIFIER_WINDOW_H
#define WINDOW_NOTIFIER_WINDOW_H

#include <QtGui/QDialog>

class Notification;

class WindowNotifierWindow : public QDialog
{
	Notification *CurrentNotification;

	void createGui();
	void addButton(QWidget *parent, const QString &caption, const char *slot);

public:
	WindowNotifierWindow(Notification *notification, QWidget *parent = 0);
	virtual ~WindowNotifierWindow();

};

#endif // WINDOW_NOTIFIER_WINDOW_H
