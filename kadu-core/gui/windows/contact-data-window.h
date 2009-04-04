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

	Account *UserAccount;
	QLineEdit *e_firstname;
	QLineEdit *e_lastname;
	QLineEdit *e_nickname;
	QComboBox *e_display;
	QLineEdit *e_mobile;
	QLineEdit *e_id;
	QLineEdit *e_addr;
	QLineEdit *e_ver;
	QLineEdit *e_email;
	QLineEdit *e_dnsname;
	QCheckBox *c_blocking;
	QCheckBox *c_offtouser;
	QCheckBox *c_notify;
	QPushButton *pb_addapply;
	QTabWidget *tw_main;
	QScrollArea* scrollArea;

	QList<QCheckBox *> groups;
	QLineEdit *newGroup;
	QWidget *groupsWidget;
	QVBoxLayout *groupsLayout;

	QList<ContactAccountDataWidget *> dataWidgets;

	void createGui();
	void createTabs(QLayout *layout);
	void createContactTab(QTabWidget *tabWidget);
	void createButtons(QLayout *layout);

	void keyPressEvent(QKeyEvent *);

private slots:
	void update();
	void updateAndClose();

	/**
		\fn void resultsReady(const QHostInfo &host)
		Ustawia warto�� pola DNS na znalezion� nazw� domeny.
	**/
	void resultsReady(const QHostInfo &host);

	/**
		\fn void newGroupClicked()
		wci�ni�to przycisk tworzenia nowej grupy
	**/
	void newGroupClicked();

	/**
		\fn void scrollToBottom()
		po dodaniu nowej grupy przewija list� grup aby dodana by�a widoczna
	**/
	void scrollToBottom();
		
protected:

	class Empty : public ConfigurationWindowDataManager
	{

	protected:
		virtual QVariant readEntry(const QString &section, const QString &name) { return ""; }
		virtual void writeEntry(const QString &section, const QString &name, const QVariant &value) {}

	};

	/**
		\fn void setupTab1()
		Tworzy pierwsz� zak�adk�.
	**/
	void setupTab1();

	/**
		\fn void setupTab2()
		Tworzy drug� zak�adk�.
	**/
	void setupTab2();

	/**
		\fn void setupTab3()
		Tworzy trzeci� zak�adk�.
	**/
	void setupTab3();

public:
	/**
		\fn UserInfo(const QString &altnick, bool addUser = false, QDialog* parent=0, const char *name=0)
		Konstruktor tworz�cy okno dialogowe z informacjami o danym kontakcie.
		\param altnick pseudonim kontaktu wy�wietlany na li�cie.
		\param addUser TRUE gdy dodajemy kontakt, FALSE gdy edytujemy dane istniej�cego. Domy�lnie FALSE.
		\param parent wska�nik do obiektu stanowi�cego rodzica. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
	**/
	ContactDataWindow(Contact contact, QWidget *parent = 0);
	virtual ~ContactDataWindow();

	QTabWidget *tabs() { return tw_main; }
	QList<ContactAccountDataWidget *> widgets() { return dataWidgets; }
	Contact contact() const { return CurrentContact; }

public slots:
	/**
		\fn void updateUserlist()
		Wci�ni�cie przycisku Dodaj/Uaktualnij
	**/
	void updateUserlist();
	void updateDisplay();

signals:
	void updateClicked(ContactDataWindow *);

};

#endif // CONTACT_DATA_WINDOW_H
