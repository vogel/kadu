#ifndef SYNTAX_EDITOR_H
#define SYNTAX_EDITOR_H

#include <qcombobox.h>

// #include <qwidget.h>
// #include <qcheckbox.h>
// #include <qtextedit.h>
// #include <qlabel.h>
// #include <qpushbutton.h>
// #include <qtextedit.h>
// #include <qlineedit.h>
// #include <qstringlist.h>
// #include <qstring.h>
// #include <qcombobox.h>

#include "kadu_text_browser.h"
#include "userlistelement.h"

class QPushButton;

class SyntaxEditor : public QWidget
{
	Q_OBJECT

	UserListElement example;

	QComboBox *syntaxList;
	KaduTextBrowser *previewPanel;

	QString syntaxGroup;

	void updateSyntaxList();

private slots:
	void editClicked();
	void syntaxChanged(const QString &newSyntax);

public:
	SyntaxEditor(QWidget *parent = 0, char *name = 0);
	virtual ~SyntaxEditor();

	void setSyntaxGroup(const QString &syntaxGroup);
};
/*
class InfoPanelSyntaxList
{
    public:
	InfoPanelSyntaxList();
	~InfoPanelSyntaxList();
	QStringList names();
	QString syntax(QString fileName);
	QString syntax(int index);
	QString name(int index);
	QString fileName(int index);
	QString currentSyntax();
	QString toDisplay(const QString s);
	QString generateFileName(const QString s);

	bool deleteSyntax(int idx);
	bool saveSyntax(const QString fileName, const QString name, const QString syntax, bool scrolls);
	bool import();
	bool containsFile(QString fileName);
	bool containsName(QString name);
	int currentIndex();
	void changeFileName(int idx, QString newFileName);

    private:
	void readDefaultPanelSyntaxNameList(QStringList &result, QStringList &files);
	void readCustomPanelSyntaxNameList(QStringList &result, QStringList &files);

	QStringList fileList;
	QStringList nameList;
};*/

class InfoPanelEditor : public QWidget
{
	Q_OBJECT

	QLineEdit *nameEdit;
	QTextEdit *editor;
	KaduTextBrowser *previewPanel;

    public:
    	InfoPanelEditor(QWidget* parent=0, const char *name=0);
    	~InfoPanelEditor();

    	void display(int idx);
    	void setSyntax(int idx);
	void onApplyTab(int idx);

    private:
// 	KaduTextBrowser *infoPreview;
// 	QCheckBox *c_showScrolls;
	QPushButton *b_preview;
	QPushButton *b_ok;
	QPushButton *b_cancel;
	QPushButton *b_apply;


// 	QLineEdit *t_name;
// 	QLineEdit *t_fileName;
	int index;
	bool canSave(QString name, QString fileName);
	bool save();

    public slots:
	void previewPanelTheme();
// 	void addScrolls(bool);
	void ok();
	void apply();
	void cancel();
	void updateFileName(const QString &s);
};

#endif // SYNTAX_EDITOR
