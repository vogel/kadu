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

/**
	Klasa ta reprezentuje okno, z kt�rym mamy do czynienia podczas dodawania nowego kontaktu,
	lub podczas edycji danych istniej�cego ju� kontaktu.
	\class ContactDataWindow
	\brief Okno dialogowe pokazuj�ce informacje o wybranym kontakcie
**/
class KADUAPI ContactDataWindow : public QWidget
{
	Q_OBJECT

	Contact CurrentContact;
	QList<ConfigurationWidget *> ConfigurationWidgets;

	void createGui();
	void createTabs(QLayout *layout);
	void createContactTab(QTabWidget *tabWidget);
	void createGroupsTab(QTabWidget *tabWidget);;
	void createAccountsTabs(QTabWidget *tabWidget);
	void createAccountTab(Account *account, QTabWidget *tabWidget);
	void createButtons(QLayout *layout);

	void keyPressEvent(QKeyEvent *);

private slots:
	void update();
	void updateAndClose();

public:
	ContactDataWindow(Contact contact, QWidget *parent = 0);
	virtual ~ContactDataWindow();

public slots:
	void updateDisplay();

};

#endif // CONTACT_DATA_WINDOW_H
