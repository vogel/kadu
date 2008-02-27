#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <qglobal.h>

#include <q3vbox.h>
//Added by qt3to4:
#include <QKeyEvent>

#include "userlistelement.h"

class QComboBox;
class QLineEdit;
class QPushButton;
class Q3TextEdit;

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

	static QString readSyntax(const QString &category, const QString &name, const QString &defaultSyntax);

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
	QString syntaxHint;

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
	void setSyntaxHint(const QString &syntaxHint);

public slots:
	void setCurrentSyntax(const QString &syntax);

signals:
	void syntaxChanged(const QString &newSyntax);
	void onSyntaxEditorWindowCreated(SyntaxEditorWindow *syntaxEditorWindow);

};

class SyntaxEditorWindow : public Q3VBox
{
	Q_OBJECT

	SyntaxList *syntaxList;

	QLineEdit *nameEdit;
	Q3TextEdit *editor;
	Preview *previewPanel;

	QString category;
	QString syntaxName;

private slots:
	void save();
	void saveAs();

protected:
	virtual void keyPressEvent(QKeyEvent *e);

public:
	SyntaxEditorWindow(SyntaxList *syntaxList, const QString &syntaxName, const QString &category, const QString &syntaxHint, QWidget* parent = 0, const char *name = 0);
	~SyntaxEditorWindow();

	Preview *preview() { return previewPanel; }

public slots:
	void refreshPreview();

signals:
	void updated(const QString &syntaxName);

};

#endif // SYNTAX_EDITOR
