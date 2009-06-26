/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SYNTAX_EDITOR_WINDOW_H
#define SYNTAX_EDITOR_WINDOW_H

#include <QtGui/QWidget>

class QLineEdit;
class QTextEdit;

class Preview;
class SyntaxList;

class SyntaxEditorWindow : public QWidget
{
	Q_OBJECT

	SyntaxList *syntaxList;

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
	SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget* parent = 0);
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
