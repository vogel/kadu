#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <qvbox.h>

#include "userlistelement.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class QTextEdit;

class Preview;
class SyntaxEditorWindow;

struct SyntaxInfo
{
	bool global;
};

class SyntaxList : public QObject, public QMap<QString, SyntaxInfo>
{
	Q_OBJECT

	QString category;

public:
	SyntaxList(const QString &category);
	virtual ~SyntaxList() {}

	static QString readSyntax(const QString &category, const QString &name);

	void reload();

	bool updateSyntax(const QString &name, const QString &syntax);
	QString readSyntax(const QString &name);
	bool deleteSyntax(const QString &name);

	bool isGlobal(const QString &name);

signals:
	void updated();

};

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	UserListElement example;

	SyntaxList *syntaxList;
	QComboBox *syntaxListCombo;
	QPushButton *deleteButton;

	QString category;

	void updateSyntaxList();

private slots:
	void editClicked();
	void deleteClicked();

	void syntaxChangedSlot(const QString &newSyntax);
	void syntaxListUpdated();

public:
	SyntaxEditor(QWidget *parent = 0, char *name = 0);
	virtual ~SyntaxEditor();

	QString currentSyntax();

	void setCategory(const QString &category);

public slots:
	void setCurrentSyntax(const QString &syntax);

signals:
	void syntaxChanged(const QString &newSyntax);
	void onSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);

};

class SyntaxEditorWindow : public QVBox
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

public:
	SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, QWidget* parent=0, const char *name=0);
	~SyntaxEditorWindow();

	Preview *preview() { return previewPanel; }

public slots:
	void refreshPreview();

signals:
	void updated(const QString &syntaxName);

};

#endif // SYNTAX_EDITOR
