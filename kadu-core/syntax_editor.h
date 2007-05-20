#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <qvbox.h>

#include "kadu_text_browser.h"
#include "userlistelement.h"

class QComboBox;
class QLineEdit;
class QPushButton;

struct SyntaxInfo
{
	bool global;
};

class SyntaxList : public QMap<QString, SyntaxInfo>
{
	QString category;

public:
	SyntaxList(const QString &category);
	void reload();

	bool updateSyntax(const QString &name, const QString &syntax);
};

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	UserListElement example;

	SyntaxList *syntaxList;
	QComboBox *syntaxListCombo;
	QPushButton *deleteButton;

	KaduTextBrowser *previewPanel;

	QString category;

	void updateSyntaxList();

private slots:
	void editClicked();
	void syntaxChanged(const QString &newSyntax);

public:
	SyntaxEditor(QWidget *parent = 0, char *name = 0);
	virtual ~SyntaxEditor();

	void setCurrentSyntax(const QString &syntax);
	QString currentSyntax();

	void setCategory(const QString &category);
};

class SyntaxEditorWindow : public QVBox
{
	Q_OBJECT

	QLineEdit *nameEdit;
	QTextEdit *editor;
	KaduTextBrowser *previewPanel;

public:
	SyntaxEditorWindow(QWidget* parent=0, const char *name=0);
	~SyntaxEditorWindow();

};

#endif // SYNTAX_EDITOR
