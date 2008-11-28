/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
 /*
 * autor:
 * Tomasz "Dorr(egaray)" Rostanski
 * rozteck (at) interia.pl
 *
 */

#include <QtCore/QDir>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtCore/QProcess>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include <stdlib.h>

#include "profiles.h" 
#include "debug.h"
#include "modules.h"
#include "../modules/gadu_protocol/gadu.h"
#include "message_box.h"
#include "kadu.h"
#include "misc.h"
#include "config_file.h"
#include "icons_manager.h"
#include "xml_config_file.h"

QString kaduConfFile = "kadu.conf.xml";

extern "C" int profiles_init()
{
	profileManager = new ProfileManager();
	return 0;
}

extern "C" void profiles_close()
{
	delete profileManager;
	profileManager = 0;
}


/*
 * ProfileManager
 */

ProfileManager::ProfileManager(QObject *parent, const char *name)
{
	dialogWindow = new ProfileConfigurationWindow();

	profileMenuActionDescription = new ActionDescription(
		ActionDescription::TypeMainMenu, "profileManagerAction",
		this, SLOT(showConfig()),
		"ProfileManager", tr("Profile Manager")
	);
	kadu->insertMenuActionDescription(0, profileMenuActionDescription);

//	ProfileMenu = new QMenu(kadu->mainMenu(), "ProfileMenu");
//	profilePos = kadu->mainMenu()->insertItem(icons_manager->loadIcon("Profiles"), 
//		tr("Profiles..."), ProfileMenu, 0, 1);

	//kadu->MainMenu->addMenu(ProfileMenu);
	//connect(ProfileMenu, SIGNAL(aboutToShow()), this, SLOT(createProfileMenu()));
	
	//odpal te ktore maja autostart
	runAutostarted();
	kdebugf2();
}

ProfileManager::~ProfileManager()
{
	kdebugf();
	
//	disconnect(ProfileMenu, SIGNAL(aboutToShow()), this, SLOT(createProfileMenu()));

	kadu->removeMenuActionDescription(profileMenuActionDescription);
	delete profileMenuActionDescription;
	delete dialogWindow;
	
	//czyszcze liste watkow
	/*
	if (!thread_list.empty())
	{
		ThreadList::iterator it;
		for ( it = thread_list.begin(); it != thread_list.end(); ++it )
        	if (it != NULL) delete (*it);
	}*/
	kdebugf2();
}

QString ProfileManager::dirString() {
	return ggPath()+"kadupro/clones/";
}

void ProfileManager::showConfig()
{	
	kdebugf();
	
	//jak pierwsze uruchomienie to probujemy "cos" znalezc
	if (config_file.readBoolEntry("Profiles", "firstRun", true)) 
		firstRun();
	
	//wyciagniecie listy profili z konfiguracji kadu
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");
	
	dialogWindow->clear();
	dialogWindow->profilesList->clear();
	
	QDomNodeList profile_list = profiles_elem.elementsByTagName("Profile");
	for (unsigned int i = 0, cnt = profile_list.count(); i < cnt; ++i)
	{
		QDomElement profile_elem = profile_list.item(i).toElement();
		dialogWindow->profilesList->insertItem(-1,profile_elem.attribute("name"));
	}
	dialogWindow->profilesList->insertItem(-1,tr("New"));
	
	//wyswietlenie okna
	dialogWindow->show();
	
	//i ew. ostrzezenia
	if (config_file.readBoolEntry("Profiles", "firstRun", true)) 
	{
		MessageBox::msg(tr("Please remember that all profile history and settings are stored in your home directory. \nCreating profile for other system users is not recommended because of security reasons. \nThe recommended solution is to create user in system for every person which will use Kadu. \nPlease notice that this module is contradictory with Linux system ideology and was provided for compatibility with Gadu-Gadu."), true, "Warning", NULL);
		config_file.writeEntry("Profiles", "firstRun", false);
	}
	kdebugf2();
}

void ProfileManager::firstRun() {
	kdebugf();

	QString dirnameString = dirString();
	//pobierz katalog ze wszystkimi jego podkatalogami
	QDir directory(dirnameString, QString::null, QDir::Name | QDir::IgnoreCase, QDir::Dirs | QDir::Readable | QDir::Writable);
	
	//jak katalog nie istnieje to stworz nowy
	if (!directory.exists()) {
		directory.mkdir(ggPath()+"kadupro", true);
		directory.mkdir(dirnameString, true);
	}
	else {
		//jak istnieje dodaj wszytskie podkatalogi jako profile
		//TODO: zrobic sprawdzenie czy zawiera plik kadu.conf.xml jesli nie to zignorowac
		for (unsigned int i = 0; i < directory.count(); i++) {
			if ((QString::compare(directory[i], "." ) != 0) && (QString::compare(directory[i], ".." ) != 0))
			{
				QString profileDir = dirnameString+directory[i];
				dialogWindow->saveProfile(directory[i], profileDir, "", "", "", true, true, false);
			}
		}
	}
	kdebugf2();	
}

void ProfileManager::runAutostarted()
{
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");

	QDomNodeList profile_list = profiles_elem.elementsByTagName("Profile");
	for (unsigned int i = 0, cnt = profile_list.count(); i < cnt; ++i)
	{
		QDomElement profile_elem = profile_list.item(i).toElement();
		if (QString::compare(profile_elem.attribute("autostart"), "1") == 0) {
			QString profilePath = profile_elem.attribute("directory");
			profilePath = profilePath.right(profilePath.length()-profilePath.find(".kadu"));
			runKadu(profilePath, pwHash(profile_elem.attribute("protectPassword")));
		}
	}	
}

int ProfileManager::runKadu(QString profilePath, QString protectPassword)
{
	kdebugf();

	if (!protectPassword.isEmpty()) {
		PasswordDialog *p = new PasswordDialog();
		p->exec();

		if (p->isCancelled()) {
			delete p;
			return 0;
		}

		if (p->getPassword().compare(protectPassword)) {
			MessageBox::msg(tr("The password is invalid. Sorry"), true, "Error", NULL);
			delete p;
			return 0;
		}

		delete p;
	}

	MyThread *thread = new MyThread();
	//thread_list.append(thread);
	thread->path = profilePath;
	thread->command = qApp->argv()[0];
	thread->start();
	kdebugf2();
	
	return 1;
}

void ProfileManager::createProfileMenu()
{
	kdebugf();

	ProfileMenu->clear();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");

	QDomNodeList profile_list = profiles_elem.elementsByTagName("Profile");

	unsigned int index = 0;
	if (profile_list.count() > 0) {
		for (unsigned int i = 0, cnt = profile_list.count(); i < cnt; ++i) {
			QDomElement profile_elem = profile_list.item(i).toElement();
			ProfileMenu->insertItem(icons_manager->loadIcon("ProfilesUser"),
				profile_elem.attribute("name"), this,	SLOT(openProfile(int)), 0, index++);
		}
		ProfileMenu->insertSeparator(index++);
	}

	ProfileMenu->insertItem(icons_manager->loadIcon("ProfilesConfigure"),
		tr("Profile Manager"), this, SLOT(showConfig()), 0, index);

	kdebugf2();
}

void ProfileManager::openProfile(int index)
{
	kdebugf();

	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");

	QDomNodeList profile_list = profiles_elem.elementsByTagName("Profile");
	QDomElement profile_elem = profile_list.item(index).toElement();

	QString profilePath = profile_elem.attribute("directory");
	profilePath = profilePath.right(profilePath.length()-profilePath.find(".kadu"));
	runKadu(profilePath, pwHash(profile_elem.attribute("protectPassword")));

	kdebugf2();
}


/*
 * ProfileConfigurationWindow
 */

void ProfileConfigurationWindow::initConfiguration() 
{
	//stworz okno konfiguracyjne
	this->setFixedSize(600, 380);
	this->setCaption(tr("Profile Manager"));

	QGridLayout *grid = new QGridLayout(this);
	
	profilesList = new QListWidget;
	profilesList->setFixedWidth(100);
	grid->addWidget(profilesList, 0, 0, 5, 1);
	
	QGroupBox *profileSettings = new QGroupBox(tr("Profile Settings"), this);
	QGridLayout *profileGrid = new QGridLayout(profileSettings);
	profileGrid->setVerticalSpacing(0);

	profileName = new QLineEdit(profileSettings);
	profileGrid->addWidget(new QLabel(tr("Name")), 0, 0);
	profileGrid->addWidget(profileName, 0, 1);

	// TODO: 0.6.6
	profileUIN = new QLineEdit(profileSettings);
	profileGrid->addWidget(new QLabel(tr("UIN"), profileSettings), 1, 0);
	profileGrid->addWidget(profileUIN, 1, 1);

	profilePassword = new QLineEdit(profileSettings);
	profilePassword->setEchoMode(QLineEdit::Password);
	profileGrid->addWidget(new QLabel(tr("Password"), profileSettings), 2, 0);
	profileGrid->addWidget(profilePassword, 2, 1);

	profileDir = new QLineEdit(profileSettings);
	profileDir->setReadOnly(true);
	profileGrid->addWidget(new QLabel(tr("Directory"), profileSettings), 3, 0);
	profileGrid->addWidget(profileDir, 3, 1);

	passwordProtectCheck = new QCheckBox(tr("Password Protected"), profileSettings);
	passwordProtectCheck->setChecked(false);
	profileGrid->addWidget(passwordProtectCheck, 4, 0);

	protectPassword = new QLineEdit(profileSettings);
	protectPassword->setHidden(true);
	protectPassword->setEchoMode(QLineEdit::Password);
	profileGrid->addWidget(protectPassword, 4, 1);

	advancedCheck = new QCheckBox(tr("Advanced"), profileSettings);
	advancedCheck->setFixedWidth(140);
	profileGrid->addWidget(advancedCheck, 5, 0);

	configCheck = new QCheckBox(tr("Copy Configuration"), profileSettings);
	configCheck->setHidden(true);
	configCheck->setChecked(true);

	profileGrid->addWidget(configCheck, 6, 1);

	userlistCheck = new QCheckBox(tr("Copy Userlist"), profileSettings);
	userlistCheck->setHidden(true);
	userlistCheck->setChecked(false);
	profileGrid->addWidget(userlistCheck, 7, 1);
	
	autostartCheck = new QCheckBox(tr("Autostart"), profileSettings);
	autostartCheck->setHidden(true);
	autostartCheck->setChecked(false);
	profileGrid->addWidget(autostartCheck, 8, 1);

	profileSettings->setLayout(profileGrid);
	grid->addWidget(profileSettings, 0, 1, 5, 1);
	
	saveButton = new QPushButton(tr("Save"), this);
	deleteButton = new QPushButton(tr("Delete"), this);
	openButton = new QPushButton(tr("Run"), this);
	closeButton = new QPushButton(tr("Close"), this);
	grid->addWidget(saveButton, 0, 2);
	grid->addWidget(deleteButton, 1, 2);
	grid->addWidget(openButton, 2, 2);
	grid->addWidget(closeButton, 4, 2);
}

ProfileConfigurationWindow::ProfileConfigurationWindow(QWidget * parent, const char * name, bool modal, Qt::WindowFlags f)
	: QDialog(parent, name, modal, f)
{
	initConfiguration();
	
	QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(closeBtnPressed()));
	QObject::connect(openButton, SIGNAL(clicked()), this, SLOT(openBtnPressed()));
	QObject::connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteBtnPressed()));
	QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(saveBtnPressed()));
	
	QObject::connect(profileName, SIGNAL(textChanged(const QString &)), this, SLOT(fillDir(const QString &)));
	QObject::connect(profilesList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(profileSelected(QListWidgetItem *)));
	QObject::connect(advancedCheck, SIGNAL(toggled(bool)), this, SLOT(advancedChecked(bool)));
	QObject::connect(configCheck, SIGNAL(toggled(bool)), this, SLOT(configChecked(bool)));
	QObject::connect(passwordProtectCheck, SIGNAL(toggled(bool)), this, SLOT(passwordProtectChecked(bool)));
}

ProfileConfigurationWindow::~ProfileConfigurationWindow() 
{
	QObject::disconnect(closeButton, SIGNAL(clicked()), this,  SLOT(closeBtnPressed()));
	QObject::disconnect(openButton, SIGNAL(clicked()), this,  SLOT(openBtnPressed()));
	QObject::disconnect(deleteButton, SIGNAL(clicked()), this,  SLOT(deleteBtnPressed()));
	QObject::disconnect(saveButton, SIGNAL(clicked()), this,  SLOT(saveBtnPressed()));
	
	QObject::disconnect(profileName, SIGNAL(textChanged(const QString &)), this, SLOT(fillDir(const QString &)));
	QObject::disconnect(profilesList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(profileSelected(QListWidgetItem *)));
	QObject::disconnect(advancedCheck, SIGNAL(toggled(bool)), this, SLOT(advancedChecked(bool)));
	QObject::disconnect(configCheck, SIGNAL(toggled(bool)), this, SLOT(configChecked(bool)));
	QObject::disconnect(passwordProtectCheck, SIGNAL(toggled(bool)), this, SLOT(passwordProtectChecked(bool)));
}

void ProfileConfigurationWindow::closeBtnPressed()
{
	this->close();
}

void ProfileConfigurationWindow::openBtnPressed()
{
	kdebugf();
	
	if (profileName->text().compare("") == 0) return;
	
	QString profilePath = this->profileDir->text();
	profilePath = profilePath.right(profilePath.length()-profilePath.find(".kadu"));

	//uruchom kadu w nowym watku
	if (profileManager->runKadu(profilePath,
		 	passwordProtectCheck->isChecked() ? protectPassword->text() : "")) {
		//jesli sie powiodlo zamknij okienko
		this->close();
	}
	
	kdebugf2();
}

void ProfileConfigurationWindow::saveBtnPressed()
{
	kdebugf();
	
	if (profileName->text().compare("") == 0) 
	{
		MessageBox::msg(tr("Please write at least the name of the Profile"), true, "Warning", NULL);
		return;
	}
	if (profileName->text().find("..") != -1) 
	{
		MessageBox::msg(tr("Profile Name contains wrong characters (\"..\")."), true, "Warning", NULL);
		return;
	}

	//sprawdz haslo przed zapisaniem profilu
	if (!profileProtectPassword.isEmpty()) {
		PasswordDialog *p = new PasswordDialog();
		p->exec();
		if (p->isCancelled()) {
			delete p;
			return;
		}

		if (p->getPassword().compare(profileProtectPassword)) {
			MessageBox::msg(tr("The password is invalid. Sorry"), true, "Error", NULL);
			delete p;
			return;
		}

		delete p;
	}
	
	//stworz katalog dla profilu
	QDir dir;
	dir.mkdir(profileDir->text(), true);
	kdebugm(KDEBUG_INFO, profileDir->text());
	dir.mkdir(profileDir->text()+"/kadu", true);
	kdebugm(KDEBUG_INFO, profileDir->text()+"/kadu");
	
	//przepisanie konfiguracji ze zmiana uinu i hasla
	//if (this->configCheck->isChecked()) {
		
		XmlConfigFile *xml_config_file = new XmlConfigFile();
		QDomElement root_elem = xml_config_file->rootElement();
		
		//jak nie kopiowac konfiguracji to usuwam wszystko
		if (!this->configCheck->isChecked())
			xml_config_file->removeChildren(root_elem);
		
		//dodaje/zmieniam wpis odnosnie uinu i hasla
		QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
		QDomElement config_file_elem = xml_config_file->accessElementByProperty(
			deprecated_elem, "ConfigFile", "name", "kadu.conf");
		QDomElement group_elem = xml_config_file->accessElementByProperty(
			config_file_elem, "Group", "name", "General");
		
		QDomElement entry_elem = xml_config_file->accessElementByProperty(
			group_elem, "Entry", "name", "UIN");
		entry_elem.setAttribute("value", profileUIN->text());
		
		QDomElement entry_elem2 = xml_config_file->accessElementByProperty(
			group_elem, "Entry", "name", "Password");
		entry_elem2.setAttribute("value", pwHash(profilePassword->text()));

		//jak nie zaznaczono kopiowania listy kontaktow to kasujemy je
		if ((this->configCheck->isChecked()) && (!this->userlistCheck->isChecked()))
		{
			QDomElement contacts_elem = xml_config_file->accessElement(root_elem, "Contacts");
			xml_config_file->removeChildren(contacts_elem);	
		}
	
		//zapisujemy konfiguracje do nowego pliku
		xml_config_file->saveTo(profileDir->text()+"/kadu/"+kaduConfFile);		
		delete xml_config_file;
	//}
	
	//zapisanie profilu do konfiguracji kadu
	saveProfile(profileName->text(), profileDir->text(), profileUIN->text(), 
		profilePassword->text(), protectPassword->text(), configCheck->isChecked(), userlistCheck->isChecked(), autostartCheck->isChecked());
	
	//dodaj profil do listy
	QList<QListWidgetItem *> matches = profilesList->findItems(profileName->text(), Qt::MatchExactly);
	if (matches.isEmpty()) {
		removeProfile(profileName->text());
	}
	else 
		profilesList->addItem(profileName->text());
	
	kdebugf2();
}

void ProfileConfigurationWindow::deleteBtnPressed()
{
	kdebugf();
	if (profileName->text().compare("") == 0) return;
		
	//wpierw ostrzez
	if ((QString::compare(profileDir->text(), "") != 0) && (MessageBox::ask(tr("Are you sure to continue? Directory (with all contents) will be deleted."))))
	{
		//usun katalog i jego pliki
		QDir directory(profileDir->text(), QString::null, QDir::Name, QDir::Files);
		if (directory.exists()) {
			//to tak na wszelki wypadek by komus nie wyciac systemu		
			if ((QString::compare(profileDir->text(), "/") == 0) ||
				(QString::compare(profileDir->text(), "/bin") == 0) ||
				(QString::compare(profileDir->text(), "/boot") == 0) ||	
				(QString::compare(profileDir->text(), "/dev") == 0) ||	
				(QString::compare(profileDir->text(), "/etc") == 0) ||
				(QString::compare(profileDir->text(), "/lib") == 0) ||
				(QString::compare(profileDir->text(), "/mnt") == 0) ||	
				(QString::compare(profileDir->text(), "/opt") == 0) ||
				(QString::compare(profileDir->text(), "/proc") == 0) ||			
				(QString::compare(profileDir->text(), "/sbin") == 0) ||
				(QString::compare(profileDir->text(), "/sys") == 0) ||
				(QString::compare(profileDir->text(), "/usr") == 0) ||
				(QString::compare(profileDir->text(), "/var") == 0))
					return;			
			
			system("rm -fr "+profileDir->text());
		}
	
		removeProfile(profileName->text());
		
		//usun profil z listy i wyczysc pola
		profilesList->removeItemWidget(profilesList->currentItem());	
		clear();
	}

	kdebugf2();
}

void ProfileConfigurationWindow::profileSelected(QListWidgetItem *item)
{
	kdebugf();

	clear();
	if (item->text().compare(tr("New")) == 0) return;
	QDomElement profile_elem = getProfile(item->text());
	profileName->setText(profile_elem.attribute("name"));
	profileDir->setText(profile_elem.attribute("directory"));
	profileUIN->setText(profile_elem.attribute("uin"));
	profilePassword->setText(pwHash(profile_elem.attribute("password")));
	
	if (QString::compare(profile_elem.attribute("config"), "0") == 0)
		configCheck->setChecked(false);
	else
		configCheck->setChecked(true);

	if (QString::compare(profile_elem.attribute("userlist"), "0") == 0)
		userlistCheck->setChecked(false);
	else 
		userlistCheck->setChecked(true);
	
	if (QString::compare(profile_elem.attribute("autostart"), "0") == 0)
		autostartCheck->setChecked(false);
	else 
		autostartCheck->setChecked(true);


	if (!profile_elem.attribute("protectPassword").isEmpty()) {
		profileProtectPassword = pwHash(profile_elem.attribute("protectPassword"));
		protectPassword->setText(profileProtectPassword);
		passwordProtectCheck->setChecked(true);
	}

	kdebugf2();
}

void ProfileConfigurationWindow::advancedChecked(bool state)
{
	userlistCheck->setHidden(!state);
	configCheck->setHidden(!state);
	autostartCheck->setHidden(!state);
}

void ProfileConfigurationWindow::configChecked(bool state)
{
	//profilePassword->setEnabled(state);
	//profileUIN->setEnabled(state);
	userlistCheck->setEnabled(state);
	if (!state)
		userlistCheck->setChecked(false);
}

void ProfileConfigurationWindow::passwordProtectChecked(bool state)
{
	protectPassword->setHidden(!state);
	//protectPasswordLabel->setHidden(!state);
}

void ProfileConfigurationWindow::fillDir(const QString &s)
{
	if (s.find("..") == -1) 
	{
		QString dir = ProfileManager::dirString()+s;
		profileDir->setText(dir);
	}
}

void ProfileConfigurationWindow::clear()
{
	profileName->setText("");
	profileUIN->setText("");
	profilePassword->setText("");
	profileDir->setText("");
	protectPassword->setText("");
	configCheck->setChecked(true);
	userlistCheck->setChecked(false);
	autostartCheck->setChecked(false);
	passwordProtectCheck->setChecked(false);
	profileProtectPassword = "";
}

void ProfileConfigurationWindow::saveProfile(QString name, QString directory, QString uin, QString password, QString protectPassword, bool config, bool userlist, bool autostart)
{
	kdebugf();
	
	//zapisanie profilu do konfiguracji kadu
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");
	
	QDomElement profile_elem = xml_config_file->createElement(profiles_elem, "Profile");
	profile_elem.setAttribute("name", name);
	profile_elem.setAttribute("directory", directory);
	profile_elem.setAttribute("uin", uin);
	profile_elem.setAttribute("password", pwHash(password));
	profile_elem.setAttribute("config", config);
	profile_elem.setAttribute("userlist", userlist);
	profile_elem.setAttribute("autostart", autostart);
	if (!protectPassword.isEmpty())
		profile_elem.setAttribute("protectPassword", pwHash(protectPassword));
	else
		profile_elem.setAttribute("protectPassword", "");
	
	xml_config_file->sync();
	
	kdebugf2();
}

void ProfileConfigurationWindow::removeProfile(QString name)
{
	kdebugf();
	
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");
	QDomElement profile_elem = xml_config_file->accessElementByProperty(
		profiles_elem, "Profile", "name", name);
	profiles_elem.removeChild(profile_elem);
	
	xml_config_file->sync();
	
	kdebugf2();
}

QDomElement ProfileConfigurationWindow::getProfile(QString name)
{
	kdebugf();
	
	QDomElement root_elem = xml_config_file->rootElement();
	QDomElement deprecated_elem = xml_config_file->accessElement(root_elem, "Deprecated");
	QDomElement config_file_elem = xml_config_file->accessElementByProperty(
		deprecated_elem, "ConfigFile", "name", "kadu.conf");
	QDomElement profiles_elem = xml_config_file->accessElementByProperty(
		config_file_elem, "Group", "name", "Profiles");
	QDomElement profile_elem = xml_config_file->findElementByProperty(
		profiles_elem, "Profile", "name", name);
	
	kdebugf2();
	return profile_elem;
}


/*
 * MyThread
 */

void MyThread::run()
{
	system("bash -c \"export CONFIG_DIR="+path+" ; "+command+"\"");	
}


/*
 * PasswordDialog
 */

PasswordDialog::PasswordDialog(QDialog *parent, const char *name): QDialog(parent, name)
{
	cancelled = true;
	this->resize(300, 150);

	QHBoxLayout *alayout = new QHBoxLayout;
	QWidget *a = new QWidget(this);
// 	a->setSpacing(10);
//	a->resize(300, 150);
//	a->setMargin(10);

	new QLabel(tr("The profile is protected by password.\nPlease provide the password and press Ok."), a);
	password = new QLineEdit(a);
	password->setEchoMode(QLineEdit::Password);

	QVBoxLayout *blayout = new QVBoxLayout;
	QWidget *b = new QWidget(a);
//	b->setSpacing(10);
//	b->resize(300, 50);
//	b->setMargin(10);

	okButton = new QPushButton(tr("Ok"), b);
	cancelButton = new QPushButton(tr("Cancel"), b);

	QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(okBtnPressed()));
	QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelBtnPressed()));

	a->setLayout(alayout);
	b->setLayout(blayout);
}

PasswordDialog::~PasswordDialog()
{
	QObject::disconnect(okButton, SIGNAL(clicked()), this, SLOT(okBtnPressed()));
	QObject::disconnect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelBtnPressed()));
}

QString PasswordDialog::getPassword()
{
	return password->text();
}

void PasswordDialog::okBtnPressed()
{
	cancelled = false;
	close();
}

void PasswordDialog::cancelBtnPressed()
{
	close();
}

bool PasswordDialog::isCancelled()
{
	return cancelled;
}


ProfileManager *profileManager;
