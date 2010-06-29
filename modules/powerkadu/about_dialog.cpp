/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QFont>
#include <QtGui/QTextBrowser>
#include <QtGui/QPushButton>
#include <QtCore/QTextStream>

#include "about_dialog.h"
#include "powerkadu.h"
#include "config_file.h"
#include "icons_manager.h"
#include "misc.h"
#include "debug.h"
#include "modules.h"

AboutDialog::AboutDialog(const char *name, bool modal, Qt::WindowFlags f)
	: QDialog(0, name, modal, Qt::WDestructiveClose)
{
	kdebugf();
	setWindowParent(this);
	QVBoxLayout *mainLayout = new QVBoxLayout(this, 10, -1, "about_dialog_main_layout");
	QHBoxLayout *topLayout = new QHBoxLayout(mainLayout, -1, "about_dialog_top_layout");
	// Icon
	QLabel *powerKaduIcon = new QLabel(this);
	powerKaduIcon->setPixmap(icons_manager->loadIcon(
			dataPath("kadu/modules/data/powerkadu/powerkadu_big.png")).pixmap());
	topLayout->addWidget(powerKaduIcon);
	// Module's name and version
	QString nameAndVersion = "<span style=\"font-size: 14pt\"><b>PowerKadu, " + 
			tr("version") + " " + version() + "</b>";
	
	QLabel *nameLabel = new QLabel(nameAndVersion, this);
	topLayout->addWidget(nameLabel);
	nameLabel->setMinimumWidth(500);
	topLayout->addStretch();

	// "About PowerKadu" tab
	QTabWidget *aboutTabs = new QTabWidget(this, "about_tabs");
	mainLayout->addWidget(aboutTabs);
	QWidget *aboutTab = new QWidget(this,"about_tab");
	QVBoxLayout *aboutTabLayout = new QVBoxLayout(aboutTab, 40, 0, "about_tab_layout");
	QLabel *descAndCopyrightLabel =
			new QLabel(tr("PowerKadu - many features for Kadu gathered "
			"in a single meta-module<br><br>"
			"Copyright (C) 2006 PK Team<br>"), aboutTab);

	aboutTabLayout->addStretch();
	aboutTabLayout->addWidget(descAndCopyrightLabel);
	aboutTabLayout->addStretch();

	// Modules tab
	QTextBrowser *modulesTab = new QTextBrowser(aboutTabs, "modules_tab");
	QStringList moduleList = modules();
	QString modulesInfos = tr("<table><tr><th>Name</th><th>Version</th><th>Author</th></tr>");
	for (QStringList::Iterator it = moduleList.begin(); it != moduleList.end(); ++it)
		modulesInfos += moduleInfo(*it);
	modulesTab->setText(modulesInfos + "</table>");

	// "Authors" tab
	QTextBrowser *authors = new QTextBrowser(aboutTabs, "authors_tab");
	if(config_file.readEntry("General", "Language") == "pl")
		authors->setText(loadFile("modules/data/powerkadu/AUTHORS.pl"));
	else
		authors->setText(loadFile("modules/data/powerkadu/AUTHORS"));

	// License tab
	QTextBrowser *license = new QTextBrowser(aboutTabs, "license_tab");
	license->setText(loadFile("COPYING"));

	// ChangeLog tab
	QTextBrowser *changeLog = new QTextBrowser(aboutTabs, "changelog_tab");
	changeLog->setText(loadFile("modules/data/powerkadu/ChangeLog"));

	// Adding tabs
	aboutTabs->addTab(aboutTab, tr("&About PowerKadu"));
	aboutTabs->addTab(modulesTab, tr("&Modules"));
	aboutTabs->addTab(authors, tr("A&uthors"));
	aboutTabs->addTab(license, tr("&License"));
	aboutTabs->addTab(changeLog, tr("&ChangeLog"));

	// Ok button
	QHBoxLayout *buttonLayout = new QHBoxLayout(mainLayout, -1, "button_layout");
	QPushButton *closeButton = new QPushButton(icons_manager->loadIcon("CloseWindow"),
			 tr("&Close"), this, "close_button");
	connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));
	buttonLayout->addStretch();
	buttonLayout->addWidget(closeButton);
	
	kdebugf2();
}

AboutDialog::~AboutDialog()
{
	kdebugf();
	kdebugf2();
}

QString AboutDialog::loadFile(QString fileName)
{
	kdebugf();
	QString result;
	QFile file(dataPath("kadu/" + fileName));

	if (!file.open(IO_ReadOnly))
	{
		kdebugm(KDEBUG_INFO, "cannot open file: %s\n", fileName.data());
		return QString::null;
	}
	QTextStream str(&file);
	str.setCodec(codec_latin2);
	result = str.read();
	file.close();
	
	kdebugf2();
	return result;
}

QStringList AboutDialog::modules()
{
	kdebugf();
	ModuleInfo info;
	if (modules_manager->moduleInfo("powerkadu", info)) {
		return info.depends;
	}

	kdebugf2();
	return QStringList();
}

QString AboutDialog::version()
{
	ModuleInfo info;
	if (modules_manager->moduleInfo("powerkadu", info)) {
		return info.version;
	}
	return "unknown";
}

QString AboutDialog::moduleInfo(const QString moduleName)
{
	kdebugf();
	QString result = "";
	ModuleInfo info;
	if (modules_manager->moduleInfo(moduleName, info)) {
		result += "<tr><td>" + moduleName + "</td><td>" + info.version + "</td><td>" +
		info.author + "</td></tr>";
	}
	kdebugf2();
	return result;
	
}
