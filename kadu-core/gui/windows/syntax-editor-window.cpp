/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QInputDialog>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QStyle>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

#include "configuration/configuration-file.h"
#include "gui/widgets/preview.h"
#include "gui/windows/message-dialog.h"
#include "icons/icons-manager.h"
#include "misc/misc.h"
#include "misc/syntax-list.h"

#include "syntax-editor-window.h"

#define  EDITOR_MINIMUM_SIZE   340, 200
#define  PREVIEW_MINIMUM_SIZE  150, 200

SyntaxEditorWindow::SyntaxEditorWindow(const QSharedPointer<SyntaxList> &syntaxList, const QString &syntaxName,
		const QString &category, const QString &syntaxHint, QWidget *parent) :
		QWidget(parent), DesktopAwareObject(this), syntaxList(syntaxList), syntaxName(syntaxName)
{
	// TODO: unused since a1a2ce99fa7c849560021ef9e9436f3cc1b7a3d6, fix API
	Q_UNUSED(category);

	setWindowRole("kadu-syntax-editor");

	setWindowTitle(tr("Kadu syntax editor"));
	setAttribute(Qt::WA_DeleteOnClose);

	QVBoxLayout *layout = new QVBoxLayout(this);

	QSplitter *splitter = new QSplitter(this);
	layout->addWidget(splitter);
	splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	splitter->setChildrenCollapsible(false);

	QWidget *splitterleft = new QWidget(splitter);
	QVBoxLayout *splitterleftlayout = new QVBoxLayout(splitterleft);
	splitterleftlayout->setMargin(0);
	splitterleftlayout->setSpacing(5);

	editor = new QTextEdit(this);
	splitterleftlayout->addWidget(editor);
	editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	editor->setAcceptRichText(true);
	editor->setPlainText(syntaxList->readSyntax(syntaxName));
	QFont font = this->font();
	font.setFamily("monospace");
	if(font.pixelSize() == -1)
		font.setPointSizeF(font.pointSizeF() - 0.5);
	else
		font.setPixelSize(font.pixelSize() - 2);
	editor->setFont(font);
	editor->setMinimumSize(EDITOR_MINIMUM_SIZE);

	if (!syntaxHint.isEmpty())
	{
		QLabel *editorhint = new QLabel(this);
		splitterleftlayout->addWidget(editorhint);
		editorhint->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
		editorhint->setWordWrap(true);
		editorhint->setText(syntaxHint);
	}

	QWidget *splitterright = new QWidget(splitter);
	QVBoxLayout *splitterrightlayout = new QVBoxLayout(splitterright);
	splitterrightlayout->setMargin(0);
	splitterrightlayout->setSpacing(5);

	previewPanel = new Preview(this);
	splitterrightlayout->addWidget(previewPanel);
	previewPanel->setMinimumHeight(0);
	previewPanel->setMaximumHeight(QWIDGETSIZE_MAX);
	previewPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	previewPanel->setMinimumSize(PREVIEW_MINIMUM_SIZE);

	QPushButton *previewbutton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_BrowserReload), tr("Refresh Preview"), this);
	splitterrightlayout->addWidget(previewbutton);
	connect(previewbutton, SIGNAL(clicked()), this, SLOT(refreshPreview()));

	QDialogButtonBox *buttonslayout = new QDialogButtonBox(Qt::Horizontal, this);
	layout->addWidget(buttonslayout);
	QPushButton *saveSyntax = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Save"), this);
	QPushButton *saveAsSyntax = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogSaveButton), tr("Save as..."), this);
	QPushButton *cancel = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttonslayout->addButton(saveSyntax, QDialogButtonBox::YesRole);
	buttonslayout->addButton(saveAsSyntax, QDialogButtonBox::ActionRole);
	buttonslayout->addButton(cancel, QDialogButtonBox::RejectRole);

	splitter->setSizes( QList<int>() << splitter->sizeHint().width() << 1 );

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
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"), tr("Syntax %1 already exists and cannot be modified").arg(newSyntaxName));
			continue;
		}
		else
		{
			if (MessageDialog::ask(KaduIcon("dialog-question"), tr("Kadu"), tr("Overwrite %1 syntax?").arg(newSyntaxName)))
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
