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

#include <ksystemtray.h>


class DockWidget : public KSystemTray
{
	Q_OBJECT

	protected:
		void mousePressEvent (QMouseEvent*);
	
	public:
		DockWidget(QWidget *parent=0, const char *name=0);
		void DockWidget::setType(char **gg_xpm);
	public slots:
		// Status change slots
		void dockletChange(int id);
		//Funkcja do migania koperty
		void changeIcon(void);

	private:
		QTimer *icon_timer;
		bool blink;
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
		void remove_hint(void);
};

class TrayIcon {
	public:
		TrayIcon(QWidget *parent = 0, const char *name = 0);
		~TrayIcon();
		void setType(char **gg_xpm);
		// Status change slots
		void dockletChange(int id);
		//Funkcja do migania koperty
		void changeIcon(void);
		void show(void);
		void connectSignals(void);

	private:
		DockWidget *dw;

	friend class DockHint;
};

extern DockHint* tip;
extern TrayIcon *trayicon;

#endif
