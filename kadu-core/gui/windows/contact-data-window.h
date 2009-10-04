/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_DATA_WINDOW_H
#define CONTACT_DATA_WINDOW_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "configuration/configuration-window-data-manager.h"
#include "contacts/contact.h"

#include "exports.h"

class QCheckBox;
class QHostInfo;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class ConfigurationWidget;
class Contact;
class ContactAccountDataWidget;


class KADUAPI ContactDataWindow : public QWidget
{
	Q_OBJECT

	Contact CurrentContact;
	QList<ConfigurationWidget *> ConfigurationWidgets;

	void createGui();
	void createTabs(QLayout *layout);
	void createGeneralTab(QTabWidget *tabWidget);
	void createGroupsTab(QTabWidget *tabWidget);;
	void createPersonalInfoTab(QTabWidget *tabWidget);
	void createOptionsTab(QTabWidget *tabWidget);
	void createButtons(QLayout *layout);

	void keyPressEvent(QKeyEvent *);

private slots:
	void updateContact();
	void updateContactAndClose();

public:
	ContactDataWindow(Contact contact, QWidget *parent = 0);
	virtual ~ContactDataWindow();

public slots:
	void updateDisplay();

};

#endif // CONTACT_DATA_WINDOW_H
