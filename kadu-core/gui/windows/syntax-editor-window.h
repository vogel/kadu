/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SYNTAX_EDITOR_WINDOW_H
#define SYNTAX_EDITOR_WINDOW_H

#include <QtCore/QSharedPointer>
#include <QtGui/QWidget>

class QLineEdit;
class QTextEdit;

class Preview;
class SyntaxList;

class SyntaxEditorWindow : public QWidget
{
	Q_OBJECT

	QSharedPointer<SyntaxList> syntaxList;

	QLineEdit *nameEdit;
	QTextEdit *editor;
	Preview *previewPanel;

	QString category;
	QString syntaxName;

private slots:
	void save();
	void saveAs();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	SyntaxEditorWindow(const QSharedPointer<SyntaxList> &syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget *parent = 0);
	~SyntaxEditorWindow();

	Preview * preview() { return previewPanel; }

public slots:
	void refreshPreview();

signals:
	void updated(const QString &syntaxName);
	void syntaxAdded(const QString &syntaxName);
	void isNameValid(const QString &syntaxName, bool &valid);

};

#endif // SYNTAX_EDITOR_WINDOW_H
