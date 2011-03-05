/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QInputDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>
#include <QtGui/QStyle>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "gui/widgets/preview.h"
#include "gui/windows/message-dialog.h"
#include "misc/misc.h"
#include "misc/syntax-list.h"
#include "icons-manager.h"

#include "syntax-editor-window.h"

SyntaxEditorWindow::SyntaxEditorWindow(const QSharedPointer<SyntaxList> &syntaxList, const QString &syntaxName,
		const QString &category, const QString &syntaxHint, QWidget *parent) :
		QWidget(parent), syntaxList(syntaxList), syntaxName(syntaxName)
{
	setWindowRole("kadu-syntax-editor");

	setWindowTitle(tr("Kadu syntax editor"));
	setAttribute(Qt::WA_DeleteOnClose);

	QFrame *syntax = new QFrame();

	QGridLayout *syntax_layout = new QGridLayout(syntax);
 	syntax_layout->setColumnStretch(0, 2);
 	syntax_layout->setColumnStretch(1, 1);
	syntax_layout->setSpacing(5);

	editor = new QTextEdit(syntax);
	editor->setAcceptRichText(true);
	editor->setPlainText(syntaxList->readSyntax(syntaxName));

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
	QPushButton *saveSyntax = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Save"), 0);
	QPushButton *saveAsSyntax = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Save as..."), 0);
	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), 0);

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
			MessageDialog::show("dialog-warning", tr("Kadu"), tr("Syntax %1 already exists and cannot be modified").arg(newSyntaxName));
			continue;
		}
		else
		{
			if (MessageDialog::ask("dialog-question", tr("Kadu"), tr("Overwrite %1 syntax?").arg(newSyntaxName)))
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
