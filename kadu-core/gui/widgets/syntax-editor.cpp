/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>

#include "gui/windows/message-dialog.h"
#include "gui/windows/syntax-editor-window.h"
#include "misc/misc.h"
#include "misc/syntax-list.h"

#include "syntax-editor.h"

SyntaxEditor::SyntaxEditor(QWidget *parent) :
		QWidget(parent)
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
}

void SyntaxEditor::setCurrentSyntax(const QString &syntax)
{
	syntaxListCombo->setCurrentIndex(syntaxListCombo->findText(syntax));
	syntaxChangedSlot(syntax);
}

QString SyntaxEditor::currentSyntax()
{
	return syntaxListCombo->currentText();
}

void SyntaxEditor::setCategory(const QString &category)
{
	if (this->category == category)
		return;

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
		MessageDialog::show("dialog-warning", tr("Kadu"), tr("Unable to remove syntax: %1").arg(currentSyntax()));
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
		fileName = dataPath("kadu") + "/syntax/" + category.toLower() + '/' + newSyntax + ".syntax";
	else
		fileName = profilePath() + "/syntax/" + category.toLower() + '/' + newSyntax + ".syntax";

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
	syntaxList = QSharedPointer<SyntaxList>(new SyntaxList(category.toLower()));

	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());

	connect(syntaxList.data(), SIGNAL(updated()), this, SLOT(syntaxListUpdated()));
}

void SyntaxEditor::syntaxListUpdated()
{
	syntaxListCombo->clear();
	syntaxListCombo->addItems(syntaxList->keys());
}
