#ifndef EMOTICONS_H
#define EMOTICONS_H

#include <QList>
#include <QPair>
#include <QToolButton>

#include "configuration_aware_object.h"
#include "html_document.h"
#include "themes.h"

class ChatWidget;
class EmotsWalker;

enum EmoticonsStyle
{
	EMOTS_NONE,
	EMOTS_STATIC,
	EMOTS_ANIMATED
};

/**
	Menad�er emotikon�w
**/
class EmoticonsManager : public Themes, ConfigurationAwareObject
{
	struct EmoticonsListItem
	{
		QString alias;
		QString anim;
		QString stat;
		EmoticonsListItem();
	};
	QList<EmoticonsListItem> Aliases;
	QList<EmoticonsListItem> Selector;
	EmotsWalker *walker;

	static QString getQuoted(const QString &s, unsigned int &pos);

	bool loadGGEmoticonThemePart(const QString &subdir);
	bool loadGGEmoticonTheme();

public:
	static void initModule();
	static void closeModule();

	/**
		Konstruktor tworzy obiekt oraz wyszukuje list� zestaw�w w katalogu
		$DATADIR/kadu/themes/emoticons
	**/
	EmoticonsManager(const QString &name, const QString &configname);
	~EmoticonsManager();

	/**
		\brief Funkcja ustawia zestaw emotikonek na theme
		\param theme nazwa zestawu
		Funkcja ustawi zestaw je�li taki istnieje w przeciwnym wypadku
		ustawionym zestawem b�dzie "gadu-gadu"
	**/
	void setEmoticonsTheme(const QString &theme);

	/**
		Funkcja wstawia w text zamiast tag�w emotikonek odpowiednie emotikonki
		\param text dokument w kt�rym maj� by� zamienione tagi
		\param bgcolor kolor t�a emotikonki
		\param style styl emotikonki jaki ma by� wstawiony - domy�lnie jest do
		 aktualny zestaw
	**/
	void expandEmoticons(HtmlDocument &text, const QColor &bgcolor, EmoticonsStyle style);

	/**
		Funkcja zwraca ilo�� emotikonek w zestawie
	**/
	int selectorCount() const;

	/**
		Funkcja zwraca alias odpowiedniej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracany jest alias.
	**/
	QString selectorString(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej animowanej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorAnimPath(int emot_num) const;

	/**
		Funkcja zwraca �cie�k� do odpowiedniej statycznej emotikonki
		\param emot_num nr emotikonki z listy
		\return Zwracany jest pusty ci�g je�li nie znaleziono emotikonki w przeciwnym razie zwracana jest �cie�ka do emotikonki
	**/
	QString selectorStaticPath(int emot_num) const;

	virtual void configurationUpdated();

};

extern EmoticonsManager *emoticons;

/**
	Klasa s�u��ca do wyboru emotikonki z zestawu
**/
class EmoticonSelectorButton : public QToolButton
{
	Q_OBJECT

	QString EmoticonString;
	QString AnimPath;
	QString StaticPath;
	QMovie *Movie;

private slots:
	void buttonClicked();
	void movieUpdate();

protected:
	/**
		Funkcja obs�uguj�ca najechanie kursorem myszki na dan� emotikonk�.
	**/
	void enterEvent(QEvent *e);

	/**
		Funkcja obs�uguj�ca opuszczenie obszaru wy�wietlania emotikonki.
	**/
	void leaveEvent(QEvent *e);

public:
	/**
		Konstruktor tworz�cy przycisk z emotikonk�
		\param parent widget na kt�rym osadzona ma by� przycisk z emotikonk�
		\param emoticon_string nazwa kt�ra ma by� wy�wietlana po najechaniu na
		przycisk
		\param static_path �cie�ka do statycznej emotikonki
		\param anim_path �cie�ka do animowanej emotikonki
	**/
	EmoticonSelectorButton(const QString &emoticon_string, const QString &static_path, const QString &anim_path, QWidget *parent);
	~EmoticonSelectorButton();
	
signals:
	/**
		Sygna� emitowany po naci�ni�ciu przycisku z emotikonk�
		przekazywana jest warto�� emoticon_string (przewa�nie jest to tag)
	**/
	void clicked(const QString &emoticon_string);

};
/**
	Klasa wy�wietlaj�ca list� emotikonek z aktualnego zestawu.
**/
class EmoticonSelector : public QWidget
{
	Q_OBJECT

	ChatWidget *callingwidget;

private slots:
	void iconClicked(const QString &emoticon_string);

protected:
	/**
		Funkcja obs�uguj�ca zamkni�cie listy.
	**/
	void closeEvent(QCloseEvent *e);

public:
	/**
		Konstruktor tworz�cy list� emotikonek.
		\param parent rodzic na kt�rym ma by� wy�wietlona lista
		\param name nazwa obiektu
		\param caller okno chat do ktorego ma by� wpisana wybrana emotikonka
	**/
	EmoticonSelector(ChatWidget *caller, QWidget *parent = 0);

public slots:
	/**
		Slot obs�uguj�cy poprawne wy�wietlenie listy emotikonek, wyr�wnanie do
		okna wywo�uj�cego.
	**/
	void alignTo(QWidget *w);

};

struct PrefixNode;
typedef QPair<QChar, PrefixNode *> Prefix;

struct PrefixNode
{
	int emotIndex;
	QList<Prefix> childs;
	PrefixNode();
};

/**
	this class serves as dictionary of emots, allowing easy
	finding of their occurrences in text;
	new search is initialized by calling 'initWalking()'
	then characters are put into analysis by 'checkEmotOccurrence(c)'
*/
class EmotsWalker
{
	/** dictionary is based on prefix tree */
	PrefixNode *root;
	QPair<QChar, PrefixNode *> myPair;
	/** positions in prefix tree, representing current analysis of text */
	QList<const PrefixNode *> positions;
	QList<int> lengths;
	int amountPositions;

	PrefixNode * findChild(const PrefixNode *node, const QChar &c);
	PrefixNode * insertChild(PrefixNode *node, const QChar &c);
	void removeChilds(PrefixNode *node);

public:
	EmotsWalker();
	~EmotsWalker();

	/**
		adds given string (emot) to dictionary of emots, giving it
		number, which will be used later to notify occurrences of
		emot in analyzed text
	*/
	void insertString(const QString &str, int num);

	/**
		return number of emot, which occurre in analyzed text just
		after adding given character (thus ending on this character)
		beginning of text analysis is turned on by 'initWalking()'
		if no emot occures, -1 is returned
	*/
	int checkEmotOccurrence(const QChar &c);

	/**
		clear internal structures responsible for analyzing text, it allows
		begin of new text analysis
	*/
	void initWalking();

};

#endif
