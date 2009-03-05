/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTextStream>
#include <QtGui/QApplication>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QInputDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtNetwork/QHostAddress>

#include "gui/widgets/preview.h"

#include "config_file.h"
#include "icons_manager.h"
#include "message_box.h"
#include "misc.h"

#include "syntax_editor.h"

SyntaxList::SyntaxList(const QString &category)
	: category(category)
{
	reload();
}

QString SyntaxList::readSyntax(const QString &category, const QString &name, const QString &defaultSyntax)
{
	QString path;
	QFile syntaxFile;
	path = dataPath("kadu") + "/syntax/" + category + "/" + name + ".syntax";

	syntaxFile.setFileName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
	{
		path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";

		syntaxFile.setFileName(path);
		if (!syntaxFile.open(QIODevice::ReadOnly))
			return defaultSyntax;
	}

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.readAll();
	syntaxFile.close();

	if (result.isEmpty())
		return defaultSyntax;

	return result;
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

	dir.setNameFilters(QStringList("*.syntax"));
	files = dir.entryList();

	foreach(const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable())
			insert(fi.baseName(), info);
	}

	info.global = true;
	path = dataPath("kadu") + "/syntax/" + category + "/";
	dir.setPath(path);

	files = dir.entryList();

	foreach(const QString &file, files)
	{
		fi.setFile(path + file);
		if (fi.isReadable() && !contains(file))
			insert(fi.baseName(), info);
	}

	if (count() == 0)
	{
		info.global = false;
		insert("custom", info);

		updateSyntax("custom", "");
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
	syntaxFile.setFileName(path + name + ".syntax");
	if (!syntaxFile.open(QIODevice::WriteOnly))
		return false;

	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	stream << syntax;
	syntaxFile.close();

	SyntaxInfo info;
	info.global = false;
	insert(name, info);

	emit updated();

	return true;
}

QString SyntaxList::readSyntax(const QString &name)
{
	if (!contains(name))
		return QString();

	SyntaxInfo info = *(find(name));
	QString path;
	if (info.global)
		path = dataPath("kadu") + "/syntax/" + category + "/" + name + ".syntax";
	else
		path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";

	QFile syntaxFile;
	syntaxFile.setFileName(path);
	if (!syntaxFile.open(QIODevice::ReadOnly))
		return QString();

	QString result;
	QTextStream stream(&syntaxFile);
	stream.setCodec("UTF-8");
	result = stream.readAll();
	syntaxFile.close();

	return result;
}

bool SyntaxList::deleteSyntax(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	if (info.global)
		return false;

	QString path = ggPath() + "/syntax/" + category + "/" + name + ".syntax";
	QFile file;
	file.setFileName(path);

	if (!file.remove())
		return false;

	remove(name);
	emit updated();

	return true;
}

bool SyntaxList::isGlobal(const QString &name)
{
	if (!contains(name))
		return false;

	SyntaxInfo info = *(find(name));
	return info.global;
}

SyntaxEditor::SyntaxEditor(QWidget *parent)
	: QWidget(parent), syntaxList(0)
{
	QHBoxLayout *layout = new QHBoxLayout(this);

	syntaxListCombo = new QComboBox(this);
	connect(syntaxListCombo, SIGNAL(activated(const QString &)), this, SLOT(syntaxChangedSlot(const QString &)));

	QPushButton *editButton = new QPushButton(tr("Edit"), this);
	deleteButton = new QPushButton(tr("Delete"), this);
	connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));
	connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));

	layout->addWidget(syntaxListCombo, 100);
	layout->addWidget(editButton);
	layout->addWidget(deleteButton);
}

SyntaxEditor::~SyntaxEditor()
{
	if (syntaxList)
	{
		delete syntaxList;
		syntaxList = 0;
	}
}

void SyntaxEditor::setCurrentSyntax(const QString &syntax)
{
	syntaxListCombo->setEditText(syntax);
	syntaxChangedSlot(syntax);
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

void SyntaxEditor::setSyntaxHint(const QString &syntaxHint)
{
	this->syntaxHint = syntaxHint;
}

void SyntaxEditor::editClicked()
{
	SyntaxEditorWindow *editor = new SyntaxEditorWindow(syntaxList, syntaxListCombo->currentText(), category, syntaxHint);
	connect(editor, SIGNAL(updated(const QString &)), this, SLOT(setCurrentSyntax(const QString &)));

	emit onSyntaxEditorWindowCreated(editor);
	editor->refreshPreview();
	editor->show();
}

void SyntaxEditor::deleteClicked()
{
	if (!syntaxList)
		return;

	if (syntaxList->deleteSyntax(currentSyntax()))
		setCurrentSyntax(*(syntaxList->keys().begin()));
	else
		MessageBox::msg(tr("Unable to remove syntax: %1").arg(currentSyntax()), true, "Warning");
}

void SyntaxEditor::syntaxChangedSlot(const QString &newSyntax)
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
		fileName = dataPath("kadu") + "/syntax/" + category.toLower() + "/" + newSyntax + ".syntax";
	else
		fileName = ggPath() + "/syntax/" + category.toLower() + "/" + newSyntax + ".syntax";

	file.setFileName(fileName);
	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	content = stream.readAll();
	file.close();

	content.replace(QRegExp("%o"),  " ");

	deleteButton->setEnabled(!info.global);
	emit syntaxChanged(content);
}

void SyntaxEditor::updateSyntaxList()
{
	if (syntaxList)
		delete syntaxList;
	syntaxList = new SyntaxList(category.toLower());

	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());

	connect(syntaxList, SIGNAL(updated()), this, SLOT(syntaxListUpdated()));
}

void SyntaxEditor::syntaxListUpdated()
{
	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());
}

SyntaxEditorWindow::SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget* parent)
	: QWidget(parent), syntaxList(syntaxList), syntaxName(syntaxName)
{
	setWindowTitle(tr("Kadu syntax editor"));
	setAttribute(Qt::WA_DeleteOnClose);

	QFrame *syntax = new QFrame();

	QGridLayout *syntax_layout = new QGridLayout(syntax);
 	syntax_layout->setColumnStretch(0, 2);
 	syntax_layout->setColumnStretch(1, 1);
	syntax_layout->setSpacing(5);

	editor = new QTextEdit(syntax);
	editor->setAcceptRichText(true);
	editor->setText(syntaxList->readSyntax(syntaxName));

	if (!syntaxHint.isEmpty())
		editor->setToolTip(syntaxHint);

	syntax_layout->addWidget(editor, 0, 0, 2, 1);

	previewPanel = new Preview(syntax);
	previewPanel->setResetBackgroundColor(config_file.readEntry("Look", category + "BgColor"));
	syntax_layout->addWidget(previewPanel, 0, 1);

	QPushButton *preview = new QPushButton(tr("Preview"), syntax);
	connect(preview, SIGNAL(clicked()), this, SLOT(refreshPreview()));
	syntax_layout->addWidget(preview, 1, 1);

	QWidget *buttons = new QWidget();
	QHBoxLayout *buttons_layout = new QHBoxLayout;
	buttons->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	buttons_layout->setSpacing(5);
#ifndef Q_OS_MAC
	(new QWidget(buttons))->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
#endif
	QPushButton *saveSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save"), 0);
	QPushButton *saveAsSyntax = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Save as..."), 0);
	QPushButton *cancel = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), 0);

	buttons_layout->addWidget(saveSyntax);
	buttons_layout->addWidget(saveAsSyntax);
	buttons_layout->addWidget(cancel);
	buttons->setLayout(buttons_layout);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(syntax);
	layout->addWidget(buttons);

	if (syntaxList->isGlobal(syntaxName))
		saveSyntax->setDisabled(true);
	else
		connect(saveSyntax, SIGNAL(clicked()), this, SLOT(save()));

	connect(saveAsSyntax, SIGNAL(clicked()), this, SLOT(saveAs()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

	loadWindowGeometry(this, "Look", "SyntaxEditorGeometry", 0, 50, 790, 480);
}

SyntaxEditorWindow::~SyntaxEditorWindow()
{
 	saveWindowGeometry(this, "Look", "SyntaxEditorGeometry");
}

void SyntaxEditorWindow::refreshPreview()
{
	QString content = editor->toPlainText();
	previewPanel->syntaxChanged(content);
}

void SyntaxEditorWindow::save()
{
	syntaxList->updateSyntax(syntaxName, editor->toPlainText());
	emit updated(syntaxName);
	close();
}

void SyntaxEditorWindow::saveAs()
{
	QString newSyntaxName = syntaxName;
	bool ok;

	while (true)
	{
		newSyntaxName = QInputDialog::getText(0, tr("New syntax name"), tr("Enter new syntax name"), QLineEdit::Normal, newSyntaxName, &ok);
		if (!ok)
			return;

		if (newSyntaxName.isEmpty())
			continue;

		if (newSyntaxName == syntaxName && !syntaxList->isGlobal(newSyntaxName))
			break;

		emit isNameValid(syntaxName, ok);
		if (!ok)
			continue;

		if (!syntaxList->contains(newSyntaxName))
			break;

		if (syntaxList->isGlobal(newSyntaxName))
		{
			MessageBox::msg(tr("Syntax %1 already exists and cannot be modified").arg(newSyntaxName), true, "Warning");
			continue;
		}
		else
		{
			if (MessageBox::ask(tr("Overwrite %1 syntax?").arg(newSyntaxName)))
				break;
		}
	}

	syntaxList->updateSyntax(newSyntaxName, editor->toPlainText());
	emit updated(newSyntaxName);
	emit syntaxAdded(newSyntaxName);
	close();
}

void SyntaxEditorWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
