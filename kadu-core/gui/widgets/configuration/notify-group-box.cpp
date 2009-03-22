/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QHBoxLayout>

#include "notify-group-box.h"

NotifyGroupBox::NotifyGroupBox(const QString &notificator, const QString &caption, QWidget *parent) :
		QGroupBox(caption, parent), Notificator(notificator)
{
	setCheckable(true);
	new QHBoxLayout(this);

	connect(this, SIGNAL(toggled(bool)), this, SLOT(toggledSlot(bool)));
}

void NotifyGroupBox::toggledSlot(bool toggle)
{
	emit toggled(Notificator, toggle);
}
