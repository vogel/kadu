#ifndef USERBOX_H
#define USERBOX_H

#include <qlistbox.h>
#include <qtooltip.h>
#include <qstringlist.h>
#include <qpainter.h>

class KaduListBoxPixmap : public QListBoxPixmap {
        public:
                KaduListBoxPixmap(const QPixmap &pix, const QString &text);

        protected:
                void paint(QPainter *painter);
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

	public:
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();
		virtual void clear() { QListBox::clear(); };
		void clearUsers() { Users.clear(); };
		void refresh();
		void addUser(const QString &altnick);
		void removeUser(const QString &altnick);
		void renameUser(const QString &oldaltnick, const QString &newaltnick);
		void changeAllToInactive();
		// Functions below works on all created userboxes
		static void all_refresh();
		static void all_removeUser(QString &altnick);		
		static void all_changeAllToInactive();
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);		
		static void initModule();
		void showHideInactive();
};

#endif
