/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DOCKAPP_H
#define DOCKAPP_H

#include <qlabel.h>

class TrayIcon : protected QLabel
{
	Q_OBJECT

	private:
		QTimer *icon_timer;
		bool blink;
		friend class DockHint;

	protected:
		void setPixmap(const QPixmap& pixmap);
		void mousePressEvent(QMouseEvent*);

	public:
		TrayIcon(QWidget *parent = 0, const char *name = 0);
		~TrayIcon();
		void setType(char **gg_xpm);
		void show();
		void connectSignals();

	public slots:
		// Status change slots
		void dockletChange(int id);
		//Funkcja do migania koperty
		void changeIcon();
};

class DockHint : public QLabel
{
	Q_OBJECT
	
	private:
		QTimer *remove_timer;
	public:
		DockHint(QWidget *parent=0);
		void Show(QString Text);
	public slots:
		void remove_hint();
};

extern DockHint* tip;
extern TrayIcon *trayicon;

#endif
