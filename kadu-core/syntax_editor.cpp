/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qvbox.h>

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "kadu_parser.h"
#include "misc.h"

#include "syntax_editor.h"

SyntaxList::SyntaxList(const QString &category)
	: category(category)
{
	reload();
}

void SyntaxList::reload()
{
	QDir dir;
	QString path;
	QFileInfo fi;
	QStringList files;

	SyntaxInfo info;

	info.global = false;
	path = ggPath() + "/syntax/" + category + "/";
	dir.setPath(path);

	dir.setNameFilter("*.syntax");
	files = dir.entryList();

	CONST_FOREACH(file, files)
	{
		fi.setFile(path + *file);
		if (fi.isReadable())
			insert(fi.baseName(), info);
	}

	info.global = true;
	path = dataPath("kadu") + "/syntax/" + category + "/";
	dir.setPath(path);

	files = dir.entryList();

	CONST_FOREACH(file, files)
	{
		fi.setFile(path + *file);
		if (fi.isReadable() && !contains(*file))
			insert(fi.baseName(), info);
	}
}

bool SyntaxList::updateSyntax(const QString &name, const QString &syntax)
{
	QString path = ggPath() + "/syntax/";
	QDir dir(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	path = ggPath() + "/syntax/" + category + "/";
	dir.setPath(path);
	if (!dir.exists())
		if (!dir.mkdir(path))
			return false;

	QFile syntaxFile;
	syntaxFile.setName(path + name + ".syntax");
	if (!syntaxFile.open(IO_WriteOnly))
		return false;

	QTextStream stream(&syntaxFile);
	stream.setEncoding(QTextStream::UnicodeUTF8);
	stream << syntax;
	syntaxFile.close();

	return true;
}

SyntaxEditor::SyntaxEditor(QWidget *parent, char *name)
	: QWidget(parent, name), syntaxList(0)
{
	UserStatus status;
	status.setBusy(qApp->translate("@default", "Description"));

	example.addProtocol("Gadu", "999999");
	example.setStatus("Gadu", status);
	example.setFirstName(qApp->translate("@default", "Mark"));
	example.setLastName(qApp->translate("@default", "Smith"));
	example.setNickName(qApp->translate("@default", "Jimbo"));
	example.setAltNick(qApp->translate("@default", "Jimbo"));
	example.setMobile("+48123456789");
	example.setEmail("jimbo@mail.server.net");
	example.setHomePhone("+481234567890");
	example.setAddressAndPort("Gadu", QHostAddress(2130706433), 80);
	example.setDNSName("Gadu", "host.server.net");

	QGridLayout *layout = new QGridLayout(this);
	layout->setSpacing(5);
	layout->setColStretch(0, 100);

	syntaxListCombo = new QComboBox(this);
	connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(syntaxChanged(const QString &)));

	QPushButton *editButton = new QPushButton(tr("Edit"), this);
	deleteButton = new QPushButton(tr("Delete"), this);
	connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));

	previewPanel = new KaduTextBrowser(this);
	previewPanel->setMinimumHeight(170);

	layout->addWidget(syntaxListCombo, 0, 0);
	layout->addWidget(editButton, 0, 1);
	layout->addWidget(deleteButton, 0, 2);
	layout->addMultiCellWidget(previewPanel, 1, 1, 0, 2);
}

SyntaxEditor::~SyntaxEditor()
{
}

void SyntaxEditor::setCurrentSyntax(const QString &syntax)
{
	syntaxListCombo->setCurrentText(syntax);
	syntaxChanged(syntax);
}

QString SyntaxEditor::currentSyntax()
{
	return syntaxListCombo->currentText();
}

void SyntaxEditor::setCategory(const QString &category)
{
	this->category = category;
	updateSyntaxList();
}

void SyntaxEditor::editClicked()
{
	SyntaxEditorWindow *editor = new SyntaxEditorWindow();
	editor->show();
}

void SyntaxEditor::syntaxChanged(const QString &newSyntax)
{
	if (!syntaxList)
		return;

	if (!syntaxList->contains(newSyntax))
		return;

	QFile file;
	QString fileName;
	QString content;

	SyntaxInfo info = (*syntaxList)[newSyntax];
	if (info.global)
		fileName = dataPath("kadu") + "/syntax/" + category + "/" + newSyntax + ".syntax";
	else
		fileName = ggPath() + "/syntax/" + category + "/" + newSyntax + ".syntax";

	file.setName(fileName);
	if (!file.open(IO_ReadOnly))
		return;

	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);
	content = stream.read();
	file.close();

	content.replace(QRegExp("%o"),  " ");

	// to nam zapewnia odswieżenie tla jesli wczesniej byl obrazek
	// TODO: fix it
	if (previewPanel->text().contains("background=", false) == 0)
		previewPanel->setText("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor") + "\"></body>");
	previewPanel->setText(KaduParser::parse(content, example));

	deleteButton->setEnabled(!info.global);
}

void SyntaxEditor::updateSyntaxList()
{
	if (syntaxList)
		delete syntaxList;

	syntaxList = new SyntaxList(category);

	syntaxListCombo->clear();
	syntaxListCombo->insertStringList(syntaxList->keys());
}

SyntaxEditorWindow::SyntaxEditorWindow(QWidget* parent, const char *name)
	: QVBox(parent, name)
{
	setCaption(tr("Kadu syntax editor"));

	setMargin(10);
	setSpacing(5);

	QGroupBox *syntax = new QGroupBox(tr("Syntax"), this);
	syntax->setInsideMargin(10);
	syntax->setColumns(2);
	syntax->adjustSize();

	editor = new QTextEdit(syntax);
	editor->setTextFormat(Qt::PlainText);
	QToolTip::add(editor, kadu->SyntaxText);

	previewPanel = new KaduTextBrowser(syntax);

	QHBox *buttons = new QHBox(this);
	buttons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	buttons->setSpacing(5);

	(new QWidget(buttons))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *saveSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save"), buttons);
	QPushButton *saveAsSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save as.."), buttons);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), buttons);
}

SyntaxEditorWindow::~SyntaxEditorWindow()
{
}

// void SyntaxEditorWindow::previewPanelTheme()
// {
// 	kdebugf();
//
// 	if (t_editor->text().contains("background=", false) == 0)	//to nam zapewnia odswieżenie tla jesli wczesniej byl obrazek
// 		infoPreview->setText("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor")+"\"></body>");
//
// 	infoPreview->setText(kadu->infoPanelSyntaxList->toDisplay(t_editor->text()));
// 	kdebugf2();
// }
