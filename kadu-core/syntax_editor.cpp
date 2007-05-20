/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
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

/*
	Konstruktor - tutaj jest tworzone cale okno edytora.
*/
InfoPanelEditor::InfoPanelEditor(QWidget* parent, const char *name) : QWidget(parent, name)
{
	setCaption(tr("Kadu Information Panel Editor"));

	QGridLayout *layout = new QGridLayout(this);
	layout->setMargin(10);
	layout->setSpacing(5);
	layout->setColStretch(1, 2);

	nameEdit = new QLineEdit(this);
	layout->addWidget(new QLabel(tr("Name:"), this), 0, 0, Qt::AlignRight);
	layout->addWidget(nameEdit, 0, 1);

	editor = new QTextEdit(this);
	editor->setTextFormat(Qt::PlainText);
	QToolTip::add(editor, kadu->SyntaxText);
	layout->addWidget(new QLabel(tr("Syntax:"), this), 1, 0, Qt::AlignRight | Qt::AlignTop);
	layout->addWidget(editor, 1, 1);

	previewPanel = new KaduTextBrowser(this);
	layout->addWidget(previewPanel, 1, 2);

	QHBox *buttons = new QHBox(this);
	buttons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	buttons->setSpacing(5);
	layout->addMultiCellWidget(buttons, 2, 2, 0, 2);

	(new QWidget(buttons))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	QPushButton *saveSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save"), buttons);
	QPushButton *deleteSyntax = new QPushButton(tr("Delete"), buttons);
	QPushButton *resetSyntax = new QPushButton(tr("Reset to default"), buttons);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), buttons);

// 	QString panelLook = config_file.readEntry("Look", "PanelContents", "");
// 	t_editor->setText(panelLook);


//-- przymiarka pod zmiane podgladu
// 	infoPreview->setPaletteBackgroundColor(config_file.readColorEntry("Look", "InfoPanelBgColor"));
// 	infoPreview->setPaletteForegroundColor(config_file.readColorEntry("Look", "InfoPanelFgColor"));
// 	infoPreview->setFrameStyle(QFrame::Box | QFrame::Plain);
// 	infoPreview->setLineWidth(1);
// 	infoPreview->setAlignment(Qt::AlignVCenter | Qt::WordBreak | Qt::DontClip);

/*
	b_preview = new QPushButton(icons_manager->loadIcon("ChangeSelectPathDialogButton"), tr("Preview"), grp_infoPanelOptions);
	connect(b_preview, SIGNAL(clicked()), this, SLOT(previewPanelTheme()));*/

// 	if (c_showScrolls->isChecked())
// 		infoPreview->setVScrollBarMode(QScrollView::AlwaysOn);
// 	else
// 		infoPreview->setVScrollBarMode(QScrollView::AlwaysOff);
/*
	QHBox *grp_buttons = new QHBox(mainBox);
	grp_buttons->setFixedSize(884, 60);
	grp_buttons->setSpacing(1);

	QVBox *grp_bottom = new QVBox(grp_buttons);
	grp_bottom->setFixedSize(350, 60);

	QHBox *grp_name = new QHBox(grp_bottom);
	new QLabel(tr("Name: "), grp_name);
	t_name = new QLineEdit("", grp_name);

	connect(t_name, SIGNAL(textChanged(const QString &)), this, SLOT(updateFileName(const QString &)));

	QHBox *grp_fileName = new QHBox(grp_bottom);
	new QLabel(tr("File Name: "), grp_fileName);
	t_fileName = new QLineEdit("", grp_fileName);
	t_fileName->setReadOnly(true);

	QLabel *l_space = new QLabel("", grp_buttons);
	l_space->setFixedWidth(220);

	b_ok = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), grp_buttons);
	connect(b_ok, SIGNAL(clicked()), this, SLOT(ok()));

	b_apply = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), grp_buttons);
	connect(b_apply, SIGNAL(clicked()), this, SLOT(apply()));

	b_cancel = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), grp_buttons);
	connect(b_cancel, SIGNAL(clicked()), this, SLOT(cancel()));

	kdebugf2();*/
}

/*
	Reakcja na przycisk Ok - zapisanie skladni i zamkniecie okna
*/
void InfoPanelEditor::ok()
{
	if (save())
		close();
}

/*
	Reakcja na przycisk Apply - apisanie stylu do pliku oraz wygenerowanie podgladu
*/
void InfoPanelEditor::apply()
{
	if (save())
		previewPanelTheme();
}

bool InfoPanelEditor::save()
{
// 	if (!canSave(t_name->text(), t_fileName->text()))
// 		return false;
//
// 	if (index >= 0)
// 	{
// 		if (t_fileName->text().compare(kadu->infoPanelSyntaxList->fileName(index)) != 0)
// 		{
// 			QFile::remove(kadu->infoPanelSyntaxList->fileName(index));
// 			kadu->infoPanelSyntaxList->changeFileName(index, t_fileName->text());
// 		}
//
// 		if (!kadu->infoPanelSyntaxList->saveSyntax(t_fileName->text(), t_name->text(), t_editor->text(), c_showScrolls->isChecked()))
// 		{
// 			MessageBox::msg(tr("Cannot save Panel Theme.\nCheck if you have write access to file ").append(t_fileName->text()));
// 			return false;
// 		}
// 		ConfigDialog::getComboBox("Look", "Select syntax")->changeItem(t_name->text(), index);
// 	}
// 	else
// 	{
// 		if (!kadu->infoPanelSyntaxList->saveSyntax(t_fileName->text(), t_name->text(), t_editor->text(), c_showScrolls->isChecked()))
// 		{
// 			MessageBox::msg(tr("Cannot save Panel Theme.\nCheck if you have write access to file ").append(t_fileName->text()));
// 			return false;
// 		}
//
// 		QComboBox *cb = ConfigDialog::getComboBox("Look", "Select syntax");
// 		cb->insertItem(t_name->text());
// 		index = cb->count()-1;
// 		cb->setCurrentItem(index);
//
// 		QPushButton *pb_editsyntax = ConfigDialog::getPushButton("Look", "Edit", "infopanel_edit");
// 		if (pb_editsyntax->isEnabled() == false)
// 		{
// 			ConfigDialog::getPushButton("Look", "Delete", "infopanel_delete")->setEnabled(true);
// 			pb_editsyntax->setEnabled(true);
// 		}
// 	}
// 	return true;
}

/*
	Reakcja na przycisk Cancel - zamkniecie okna.
*/
void InfoPanelEditor::cancel()
{
	close();
}

/*
	Tworzy nazwe pliku na podstawie nazwy stylu
*/
void InfoPanelEditor::updateFileName(const QString &s)
{
// 	t_fileName->setText(kadu->infoPanelSyntaxList->generateFileName(s));
}

/*
	W momencie usuwania rodzica wszyscy potomkowie sa usuwani automatycznie...
*/
InfoPanelEditor::~InfoPanelEditor()
{
}

/**
	Sprawdza czy skladnia panelu inf. moze zostac zapisana
**/
bool InfoPanelEditor::canSave(QString name, QString fileName)
{
// 	bool result = true;
// 	QString message = tr("Cannot save Panel Theme.");
// 	if ((index < 0) && (kadu->infoPanelSyntaxList->containsFile(t_fileName->text())))
// 	{
// 		message.append("\n").append("There is another Panel Syntax stored in file ").append(t_fileName->text()).append(".");
// 		result = false;
// 	}
// 	if (name.isEmpty())
// 	{
// 		message.append("\n").append("The name cannot be empty.");
// 		result = false;
// 	}
// 	if (fileName.isEmpty())
// 	{
// 		message.append("\n").append("The filename cannot be empty.");
// 		result = false;
// 	}
// 	if ((index < 0) && (kadu->infoPanelSyntaxList->containsName(name)))
// 	{
// 		message.append("\n").append("The Syntax with the same name already exists.");
// 		result = false;
// 	}
// 	if (result == false)
// 		MessageBox::wrn(message);
//
// 	return result;
}

/*
	Generuje podglad skladni panelu
*/
void InfoPanelEditor::previewPanelTheme()
{
// 	kdebugf();
//
// 	if (t_editor->text().contains("background=", false) == 0)	//to nam zapewnia odswieżenie tla jesli wczesniej byl obrazek
// 		infoPreview->setText("<body bgcolor=\"" + config_file.readEntry("Look", "InfoPanelBgColor")+"\"></body>");
//
// 	infoPreview->setText(kadu->infoPanelSyntaxList->toDisplay(t_editor->text()));
// 	kdebugf2();
}

/*
    Ustawia skladnie panelu na skladnie o pozycji idx na liscie
*/
void InfoPanelEditor::setSyntax(int idx)
{
/*	index = idx;
	t_name->setText(kadu->infoPanelSyntaxList->name(index));
	t_fileName->setText(kadu->infoPanelSyntaxList->fileName(index));
	t_editor->setText(kadu->infoPanelSyntaxList->syntax(index));

	//jesli okno jest widoczne odswiez podglad
	if (this->isVisible())
		previewPanelTheme();*/
}

/*
	Wyswietla okno edytora ustawiajac nazwe oraz nazwe pliku.
	Przy tworzeniu nowej skladni ona parametry sa puste - powoduje
	to wylaczenie trybu edit
*/
void InfoPanelEditor::display(int idx)
{
// 	kdebugf();
// 	index = idx;
// 	if (index >= 0)
// 	{
// 		t_name->setText(kadu->infoPanelSyntaxList->name(index));
// 		t_fileName->setText(kadu->infoPanelSyntaxList->fileName(index));
// 		t_editor->setText(kadu->infoPanelSyntaxList->syntax(index));
// 	}
// 	else
// 	{
// 		t_name->setText(tr("New"));
// 		t_fileName->setText(kadu->infoPanelSyntaxList->generateFileName(tr("New")));
// 		t_editor->setText("");
// 	}
//
// 	previewPanelTheme();
// 	show();
// 	kdebugf2();
}



/*
	InfoPanelSyntaxList
	Klasa przechowujaca liste skladni panelu informacyjnego i umozliwiajaca
	zarzadzanie nimi (dodawanie, usuwanie, edycje).
*/

/*
	Konstruktor
*/
// InfoPanelSyntaxList::InfoPanelSyntaxList()
// {
// 	fileList.clear();
// 	nameList.clear();
// 	readDefaultPanelSyntaxNameList(nameList, fileList);
// 	readCustomPanelSyntaxNameList(nameList, fileList);
// }
//
// InfoPanelSyntaxList::~InfoPanelSyntaxList()
// {
// }

/*
	Zwraca nazwe skladni znajdujaca sie na pozycji index na liscie
*/
// QString InfoPanelSyntaxList::name(int index)
// {
// 	return (*nameList.at(index));
// }

/*
	Zwraca nazwe pliku zawierajacego skladnie znajdujaca sie na pozycji index na liscie
*/
/*
QString InfoPanelSyntaxList::fileName(int index)
{
	return (*fileList.at(index));
}*/

/*
	Zwraca liste nazw domyslnych skladni panelu inf.
*/

/*
	Zapisuje skladnie panelu informacyjnego.
	Jesli sie uda zwraca true, w przeciwnym wypadku false.
*/
// bool InfoPanelSyntaxList::saveSyntax(const QString fileName, const QString name, const QString syntax, bool scrolls)
// {
/*
	kdebugf();
	QDomDocument doc("panelSyntax");
	QDomElement root = doc.createElement("panelSyntax");
	doc.appendChild(root);

	QDomElement syntaxTag = doc.createElement("syntax");*/
// 	syntaxTag.setAttribute("value", syntax);
// 	root.appendChild(syntaxTag);
//
// 	QDomElement nameTag = doc.createElement("name");
// 	nameTag.setAttribute("value", name);
// 	root.appendChild(nameTag);
//
// 	//sprawdz czy katalog istnieje. jesli nie - stworz
// 	QString dirName = fileName.left(fileName.findRev('/'));
// 	QDir dir(dirName);
// 	if (!dir.exists()) {
// 		//mkdir niestety nie tworzy calej struktury... dla ~/.kadu/themes/infopanel
// 		//musimy stworzyc dwa katalogi
// 		dir.mkdir(dirName.left(dirName.findRev('/')));
// 		dir.mkdir(dirName, true);
// 	}
//
// 	QFile file;
// 	file.setName(fileName);
// 	//jesli nie ma dostepu do pliku zwracam false
// 	if (!file.open(IO_WriteOnly))
// 		return false;
//
// 	//najpierw zapisuje do pliku xml
// 	QTextStream stream(&file);
// 	stream.setEncoding(QTextStream::UnicodeUTF8);
// 	stream << doc.toString();
// 	file.close();
//
// 	//pozniej do kadu.conf.xml
// 	config_file.writeEntry("Look", "PanelThemeName", name);
// 	config_file.writeEntry("Look", "PanelContents", syntax);
// 	config_file.writeEntry("Look", "PanelVerticalScrollbar", scrolls);
//
// 	//dodaj nazwe skladni i nazwe pliku do listy jesli jej tam nie bylo
// 	if (fileList.isEmpty() || fileList.contains(fileName) == 0)
// 	{
// 		nameList << name;
// 		fileList << fileName;
// 	}
// 	else /* zaktualizuj nazwe na liscie */
// 	{
// 		int i = fileList.findIndex(fileName);
// 		if (i >= 0)
// 		{
// 			QStringList::Iterator it = nameList.at(i);
// 			(*it) = name;
// 		}
// 	}

// 	return true;
// }

SyntaxEditor::SyntaxEditor(QWidget *parent, char *name)
	: QWidget(parent, name)
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

	syntaxList = new QComboBox(this);
	connect(syntaxList, SIGNAL(activated(const QString &)), this, SLOT(syntaxChanged(const QString &)));

	QPushButton *editButton = new QPushButton(tr("Edit"), this);
	connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));

	previewPanel = new KaduTextBrowser(this);
	previewPanel->setMinimumHeight(170);

	layout->addWidget(syntaxList, 0, 0);
	layout->addWidget(editButton, 0, 1);
	layout->addMultiCellWidget(previewPanel, 1, 1, 0, 1);
}

SyntaxEditor::~SyntaxEditor()
{
}

void SyntaxEditor::setSyntaxGroup(const QString &syntaxGroup)
{
	this->syntaxGroup = syntaxGroup;
	updateSyntaxList();
}

void SyntaxEditor::editClicked()
{
	InfoPanelEditor *editor = new InfoPanelEditor();
	editor->show();
}

void SyntaxEditor::syntaxChanged(const QString &newSyntax)
{
	QFileInfo fi;
	QFile file;
	QString fileName;
	QString content;

	fileName = ggPath().append("/syntax/infopanel/").append(newSyntax).append(".syntax");
	fi.setFile(fileName);

	if (!fi.isReadable())
	{
		fileName = dataPath("kadu").append("/syntax/infopanel/").append(newSyntax).append(".syntax");
		fi.setFile(fileName);

		if (!fi.isReadable())
			return;
	}

	file.setName(fileName);
	file.open(IO_ReadOnly);
	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);
	content = stream.read();
	file.close();

	content.replace(QRegExp("%o"),  " ");

	previewPanel->setText(KaduParser::parse(content, example));
}

void SyntaxEditor::updateSyntaxList()
{
	QDir dir;
	QString path;
	QFileInfo fi;
	QStringList result;
	QStringList files;

	path = dataPath("kadu").append("/syntax/infopanel/");
	dir.setPath(path);

	dir.setNameFilter("*.syntax");
	files = dir.entryList();

	CONST_FOREACH(file, files)
	{
		fi.setFile(path + *file);
		if (fi.isReadable())
			result.append(fi.baseName());
	}

	path = ggPath().append("/syntax/infopanel/");
	dir.setPath(path);

	CONST_FOREACH(file, files)
	{
		if (result.contains(path + *file))
			continue;

		fi.setFile(*file);
		if (fi.isReadable())
			result.append(fi.baseName());
	}

	syntaxList->clear();
	syntaxList->insertStringList(result);
}
