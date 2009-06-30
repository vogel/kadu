/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STATUS_BUTTONS_H
#define STATUS_BUTTONS_H

#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "status/status-container-aware-object.h"

class QHBoxLayout;

class StatusButton;
class StatusContainer;

class StatusButtons : public QWidget, private StatusContainerAwareObject
{
	Q_OBJECT

	QHBoxLayout *Layout;

	QMap<StatusContainer *, StatusButton *> Buttons;

	void createGui();

protected:
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

public:
	explicit StatusButtons(QWidget *parent = 0);
	virtual ~StatusButtons();

};

#endif // STATUS_BUTTONS_H
