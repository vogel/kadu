#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QString>

/*!
 * This class represents the "About PowerKadu" dialog
 * \brief "About PowerKadu" dialog
 */
class AboutDialog : public QDialog
{
	Q_OBJECT
	public:
		/*!
	 	 * \brief Default constructor
		 * \param name object's name
		 * \param modal if true, dialog will be modal
		 * \param f dialogs flags or'ed by WDestructiveClose
		 */
		AboutDialog(const char *name = 0, bool modal = FALSE, Qt::WindowFlags f = 0);

		//! Default destructor
		~AboutDialog();

	private:
		QString loadFile(QString fileName);
		QString moduleInfo(QString moduleName);
		QStringList modules();
		QString version();
};

#endif
