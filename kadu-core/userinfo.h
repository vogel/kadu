/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_USERINFO_H
#define KADU_USERINFO_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "userlistelement.h"

class QCheckBox;
class QHostInfo;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class CreateNotifier;

/**
	Klasa ta reprezentuje okno, z kt�rym mamy do czynienia podczas dodawania nowego kontaktu,
	lub podczas edycji danych istniej�cego ju� kontaktu.
	\class UserInfo
	\brief Okno dialogowe pokazuj�ce informacje o wybranym kontakcie
**/
class UserInfo : public QWidget
{
	Q_OBJECT

	UserListElement User;
	QLineEdit *e_firstname;
	QLineEdit *e_lastname;
	QLineEdit *e_nickname;
	QLineEdit *e_altnick;
	QLineEdit *e_mobile;
	QLineEdit *e_uin;
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

	QMap<QString, QLabel *> pixmapLabels;

	void keyPressEvent(QKeyEvent *);

private slots:
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
		
	/**
		void selectIcon()
		wci�ni�to przycisk dodania ikony dla grupy
	**/
	void selectIcon();

	/**
		void deleteIcon()
		wci�ni�to przycisk usuni�cia ikony grupy
	**/
	void deleteIcon();

protected:
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
		\var static CreateNotifier createNotifier
		Statyczny obiekt wysy�aj�cy sygna� CreateNotifier::objectCreated
		je�li powstanie nowa instancja okienka.
	**/
	static CreateNotifier createNotifier;

	/**
		\fn UserInfo(const QString &altnick, bool addUser = false, QDialog* parent=0, const char *name=0)
		Konstruktor tworz�cy okno dialogowe z informacjami o danym kontakcie.
		\param altnick pseudonim kontaktu wy�wietlany na li�cie.
		\param addUser TRUE gdy dodajemy kontakt, FALSE gdy edytujemy dane istniej�cego. Domy�lnie FALSE.
		\param parent wska�nik do obiektu stanowi�cego rodzica. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
	**/
	UserInfo(UserListElement user, QWidget *parent = 0);
	~UserInfo();

	QTabWidget *tabs() { return tw_main; }
	UserListElement user() const { return User; }
	static bool acceptableGroupName(const QString &groupName);

public slots:
	/**
		\fn void updateUserlist()
		Wci�ni�cie przycisku Dodaj/Uaktualnij
	**/
	void updateUserlist();

signals:
	void updateClicked(UserInfo *);

};

#endif
