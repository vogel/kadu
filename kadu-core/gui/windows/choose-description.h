/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHOOSE_DESCRIPTION_H
#define CHOOSE_DESCRIPTION_H

#include <QtGui/QDialog>

#include "status/status.h"

class QComboBox;
class QLabel;

class StatusContainer;

class ChooseDescription : public QDialog
{
	Q_OBJECT

	static QMap<StatusContainer *, ChooseDescription *>Dialogs;

	QPushButton *OkButton;
	QComboBox *Description;
	QLabel *AvailableChars;

	Status CurrentStatus;
	StatusContainer *MyStatusContainer;

private slots:
	void okPressed();
	void cancelPressed();
	void updateAvailableChars(const QString &);

public:
	static void show(const Status &status, StatusContainer *statusContainer, const QPoint &position);

	ChooseDescription(StatusContainer *statusContainer, QWidget *parent = 0);
	virtual ~ChooseDescription();

	void setStatus(const Status &status);
	void setPosition(const QPoint &position);
};

#endif // CHOOSE_DESCRIPTION_H
