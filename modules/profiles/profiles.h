/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef PROFILES_H
#define PROFILES_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QMenu>
#include <QtXml/QDomElement>

#include "action.h"

/*
 * ProfileConfigurationWindow
 * okno Menedzera Profili
 */

/*
 * Profile
 * Klasa Profilu
 * przechowuje dane profilu
 */

class Profile
{
	public:
		Profile(): config(true), userlist(true), autostart(false) {};
		Profile(const Profile &p) : name(p.name), directory(p.directory), uin(p.uin),
					    password(p.password), protectPassword(p.protectPassword),
					    config(p.config), userlist(p.userlist), autostart(p.autostart) {};
		Profile(QString name, QString dir): name(name), directory(dir), config(true), userlist(true), autostart(false) {};
		~Profile() {};

		QString name;
		QString directory;
		QString uin;
		QString password;
		QString protectPassword;
		bool config;
		bool userlist;
		bool autostart;
};


class ProfileConfigurationWindow : public QDialog
{
	Q_OBJECT
	public:
		ProfileConfigurationWindow(QWidget * parent = 0, const char * name = 0, bool modal = FALSE, Qt::WindowFlags f = 0);
		~ProfileConfigurationWindow();
		void initConfiguration();
		void clear();
		void refreshList();
		void saveProfile(Profile p, bool update);
		void removeProfile(QString name);
//		QDomElement getProfile(QString name);
		QListWidget *profilesList;
	private:
		QLineEdit *profileName;
		QLineEdit *profileUIN;
		QLineEdit *profileDir;
		QLineEdit *profilePassword;
		QLineEdit *protectPassword;
		QCheckBox *advancedCheck;
		QCheckBox *configCheck;
		QCheckBox *userlistCheck;
		QCheckBox *autostartCheck;
		QCheckBox *passwordProtectCheck;
		QPushButton *saveButton;
		QPushButton *deleteButton;
		QPushButton *openButton;
		QPushButton *closeButton;
		QString profileProtectPassword;

	private slots:
		void closeBtnPressed();
		void openBtnPressed();
		void saveBtnPressed();
		void deleteBtnPressed();
		void profileSelected(QListWidgetItem *item);
		void advancedChecked(bool state);
		void configChecked(bool state);
		void passwordProtectChecked(bool state);
		void fillDir(const QString &s);
};


/*
 * MyThread
 * klasa implementujaca watek.
 * nazwa nie jest ambitna - wiem
 */

class MyThread : public QThread {
	Q_OBJECT

	public:
		MyThread() {};
		QString path;
		QString command;
		virtual void run();
};

//typedef QList<MyThread *> ThreadList;



/*
 * ProfileManager
 * Klasa Menedzera Profili
 * odpowiedzialna za tworzenie okna menedzera,
 * uruchamianie profili, etc.
 */

class ProfileManager : public QObject
{
	Q_OBJECT
	public:
		ProfileManager(QObject *parent=0, const char *name=0);
		~ProfileManager();
		void firstRun();
		void runAutostarted();
		int runKadu(QString profilePath, QString password);
		static QString dirString();

		void addProfile(Profile p);
		void updateProfile(Profile p);
		void deleteProfile(const QString &name);
		QList<Profile> getProfileList();
		QStringList getProfileNames();
		Profile getProfile(const QString &name);

	private:
		int profilePos;
		ProfileConfigurationWindow *dialogWindow;
		//ThreadList thread_list;
		QMenu *ProfileMenu;
		ActionDescription *profileMenuActionDescription;
		QList<Profile> list;

		void getProfiles();

	private slots:
		void showConfig();
		void showMenu();
		void createProfileMenu();
		void openProfile(int index);
};


/*
 * PasswordDialog
 * Okno z zapytaniem o haslo
 */

class PasswordDialog : public QDialog
{
	Q_OBJECT
	public:
		PasswordDialog(QDialog *parent=0, const char *name=0);
		~PasswordDialog();
		QString getPassword();

	private:
		QLineEdit *password;
		QPushButton *okButton;
		QPushButton *cancelButton;
	private slots:
		void okBtnPressed();
		void cancelBtnPressed();
};


extern ProfileManager *profileManager;

#endif
