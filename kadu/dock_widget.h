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
};

extern DockWidget* dw;

#endif
