#ifndef KADU_USERBOX_H
#define KADU_USERBOX_H

#include <qlistbox.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qtooltip.h>

#include "misc.h"
#include "userlist.h"

class QFontMetrics;

class KaduListBoxPixmap : public QListBoxItem
{
	public:
		KaduListBoxPixmap(const QPixmap &pix, const QString &text);
		KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold);

		QString description() const { return descr; }
		bool isBold() const { return bold; }
		int height(const QListBox *lb) const;
		int width(const QListBox *lb) const;

	protected:
		int lineHeight(const QListBox *lb) const;
		void paint(QPainter *painter);
		void setDescription(const QString &d) { descr = d; }
		void setBold(bool b) { bold = b; }
		void calculateSize(const QString &text, int width, QStringList &out, int &height) const;

		void changeText(const QString &text);
		friend class UserBox;

		/*funkcje wprowadzone ¿eby zaoszczêdziæ na odwo³aniach do pliku konfiguracyjnego*/
		static void setFont(const QFont &f);
		static void setMyUIN(UinType u);
		static void setShowDesc(bool sd);
		static void setAlignTop(bool at);
		static void setShowMultilineDesc(bool m);
		static void setMultiColumn(bool m);
		static void setMultiColumnWidth(int w);
		static void setDescriptionColor(const QColor &col);

	private:
		QPixmap pm;
		QString descr;
		bool bold;
		static QFontMetrics *descriptionFontMetrics;

		static UinType myUIN;
		static bool ShowDesc;
		static bool AlignUserboxIconsTop;
		static bool ShowMultilineDesc;
		static bool MultiColumn;
		static int  MultiColumnWidth;
		static QColor descColor;
		
		mutable QString buf_text;
		mutable int buf_width;
		mutable QStringList buf_out;
		mutable int buf_height;
};

class UserBoxMenu : public QPopupMenu
{
	Q_OBJECT

	private:
		QValueList<QPair<QString, QString> > iconNames;
	private slots:
		void restoreLook();

	public:
		UserBoxMenu(QWidget *parent=0, const char* name=0);
		int getItem(const QString &caption);
	
	public slots:
		void show(QListBoxItem *item);
		int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
		/**
			@param iconname nazwa ikony z zestawu lub sciezka do pliku
		**/
		int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
		/**
			@param iconname nazwa ikony z zestawu lub sciezka do pliku
		**/
		int addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
		
		/**
			prze³adowuje wszystkie ikony w tym menu dodane przy pomocy powy¿szych funkcji 
		**/
		void refreshIcons();
	signals:
		void popup();
};


/**
	Klasa reprezentuj±ca listê kontaktów wraz z ikonkami stanów
**/
class UserBox : public QListBox , QToolTip
{
	Q_OBJECT

	private:
		static QValueList<UserBox*> UserBoxes;
		// Pixmaps
		static QPixmap* OnlineMobilePixmap;
		static QPixmap* OnlineDescMobilePixmap;
		static QPixmap* BusyMobilePixmap;
		static QPixmap* BusyDescMobilePixmap;
		static QPixmap* OfflineMobilePixmap;
		static QPixmap* OfflineDescMobilePixmap;		
		//
		QStringList Users;
		void sortUsersByAltNick(QStringList &);
			
	protected:
		virtual void maybeTip(const QPoint&);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseMoveEvent(QMouseEvent* e);
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void resizeEvent(QResizeEvent *);

	public:
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();
		static UserBoxMenu *userboxmenu;
		/**
			Funkcja sprawdza czy dany altnick, znajduje siê na liscie w UserBox
		**/
		bool containsAltNick(const QString &altnick);
		static void initModule();
		/**
			Funkcja zwraca liste zaznaczonych uzytkownikow 
			@return UinsList z zaznaczonymi 
			uzytkownikami posiadajacymi numer GG
		**/

		UinsList getSelectedUins();
		/**
			Funkcja zwraca liste zaznaczonych uzytkownikow
			@return UserList z zaznaczonymi uzytkownikami
		**/

		UserList getSelectedUsers();
		/**
			Funkcja zwraca liste zaznaczonych uzytkownikow
			@return QStringList z AltNick'ami
		**/
		QStringList getSelectedAltNicks();
		/**
			Funkcja znajdujaca aktywny UserBox.
			@return wskaznik do aktywnego UserBox'a, 
			jesli taki nie istnieje zwracana jest wartosc NULL
		**/
		static UserBox* getActiveUserBox();
	public slots:	
		void refresh();
		void showHideInactive();
		void showHideDescriptions();
		virtual void clear() { QListBox::clear(); };
		/**
			Funkcja czy¶ci ca³± listê u¿ytkowników w UserBox
		**/
		void clearUsers() { Users.clear(); };
		/**
			Funkcja dodaje u¿ytkownika o podanym altnick do listy w UserBox
		**/
		void addUser(const QString &altnick);
		/**
			Funkcja usuwa u¿ytkownika o podanym altnick z listy w UserBox
		**/
		void removeUser(const QString &altnick);
		/**
			Funkcja zmienia nazwe altnick z podanej na inna w liscie UserBox
		**/
		void renameUser(const QString &oldaltnick, const QString &newaltnick);
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString &altnick);
		static void all_changeAllToInactive();
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);
	signals:
		/**
			u¿ywaj tego sygna³u zamiast QListBox::doubleClicked(QListBoxItem *) !!!
			
			tamten ze wzglêdu na od¶wie¿anie listy w jednym ze slotów pod³±czonych
			do tego sygna³u czasami przekazuje wska¼nik do elementu, który ju¿ NIE ISTNIEJE
		**/
		void doubleClicked(const QString &text);
		/**
			sytuacja analogiczna do doubleClicked
		**/
		void returnPressed(const QString &text);
	private slots:
		void doubleClickedSlot(QListBoxItem *item);
		void returnPressedSlot(QListBoxItem *item);
};


class UserBoxSlots : public QObject
{
	Q_OBJECT
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseColor(const char *name, const QColor& color);
		void chooseFont(const char *name, const QFont& font);
		void onMultiColumnUserbox(bool toggled);
		void updatePreview();
};

#endif
