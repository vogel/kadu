#ifndef HOT_KEY_H
#define HOT_KEY_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <qkeysequence.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QKeyEvent>

class HotKey
{
public:
	/**
	  Pobiera skrót klawiszowy, z pliku konfiguracyjnego
	  z grupy "groupname", o polu "name"
	 **/
	static QKeySequence shortCutFromFile(const QString& groupname, const QString &name);

	/**
	  Sprawdza czy skrót naci¶niêty przy zdarzeniu QKeyEvent zgadza siê
	  ze skrótem klawiszowym, z pliku konfiguracyjnego, o polu "name"
	 **/
	static bool shortCut(QKeyEvent *e, const QString& groupname, const QString &name);

	static QString keyEventToString(QKeyEvent *e);
};

class HotKeyEdit : public QLineEdit
{
protected:
	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

public:
	HotKeyEdit(QWidget *parent = 0, const char *name = 0)
		: QLineEdit(parent, name) {}
	/**
	  Pobiera skrót klawiszowy
	**/
	QString shortCutString() const { return text(); }

	/**
	  Pobiera skrót klawiszowy
	**/
	QKeySequence shortCut() const { return QKeySequence(text()); }

	/**
	  Ustawia skrót klawiszowy
	**/
	void setShortCut(const QString &shortcut);

	/**
	  Ustawia skrót klawiszowy
	**/
	void setShortCut(const QKeySequence &shortcut) { setText(shortcut); }

};

#endif // HOT_KEY_H
