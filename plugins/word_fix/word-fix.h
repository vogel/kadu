#ifndef WORD_FIX_H
#define WORD_FIX_H

#include "gui/windows/main-configuration-window.h"
#include "plugins/generic-plugin.h"

class ChatWidget;
class UserGroup;
class QTreeWidgetItem;
class QTreeWidget;
class QListBox;
class QPushButton;
class QLineEdit;

/*!
 * This class is responsible for fixing small mistakes (and not only)
 * while sending chat messages, like "<i>nei</i>" to "<i>nie</i>", etc.
 * Generally it replaces one word with another, assigned to first one.
 * \class WordFix
 * \brief Words fixing.
 */
class WordFix : public ConfigurationUiHandler, GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	/*!
	 * \var QMap<QString,QString> wordsList
	 * Contains whole list of words for replacing in form:
	 * wordList["to_replace"] = "correct_word";
	 */
	QMap<QString,QString> wordsList;

	QRegExp ExtractBody;

	QPushButton *changeButton;
	QPushButton *deleteButton;
	QPushButton *addButton;
	QLineEdit *wordEdit;
	QLineEdit *valueEdit;
	QTreeWidget* list;

	void saveList();

private slots:
	/*!
	 * \fn void chatCreated(ChatWidget *chat)
	 * This slot calls connectToClass"("Chat* chat")" to connect
	 * the object to chat "<i>send message</i>" signal.
	 * \param chat Pointer to the created chat window.
	 */
	void chatCreated(ChatWidget *chat);

	/*!
	 * \fn void chatDestroying(ChatWidget *chat)
	 * This slot calls disconnectFromClass"("Chat* chat")" to disconnect
	 * the object from chat "<i>send message</i>" signal.
	 * \param chat Pointer to the chat window.
	 */
	void chatDestroying(ChatWidget *chat);

	/*!
	 * \fn void sendRequest(Chat* chat)
	 * Handles chat message send request.
	 * It checks configuration if words fixing is enabled. If it's not,
	 * then the method makes no changes in input text.
	 * \param chat Chat window which sending request is invoked by.
	 */
	void sendRequest(ChatWidget *chat);

	void configurationApplied();

public:
	/*!
	 * \fn WordFix()
	 * Default constructor. Reads words list or (if it's not defined yet) loads default list.
	 */
	explicit WordFix(QObject *parent = 0);

	/*!
	 * \fn ~WordFix()
	 * Default destructor.
	 */
	virtual ~WordFix();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	virtual int init(bool firstLoad);
	virtual void done();

	/*!
	 * \fn void connectToChat(const Chat* chat)
	 * Connects handling slot to given chat signal.
	 * \param chat Chat to connect slot to.
	 */
	void connectToChat(const ChatWidget* chat);

	/*!
	 * \fn void disconnectFromChat(const Chat* chat)
	 * Disconnects handling slot from given chat signal.
	 * \param chat Chat to disconnect slot from.
	 */
	void disconnectFromChat(const ChatWidget* chat);

	/*!
	 * \fn void doReplace(QString &text)
	 * Does the whole job - replaces wrong words with the right ones.
	 * \param text To replace. After processing it will contain corrected text.
	 */
	void doReplace(QString &text);

public slots:
	/*!
	 * \fn void wordSelected(QTreeWidgetItem* item)
	 * Called when a word was selected in configuration dialog
	 * from the words to fix list.
	 * \param item Selected row on the list.
	 */
	void wordSelected();

	/*!
	 * \fn void changeSelected()
	 * Called when "Change selected" button in configuration dialog
	 * was clicked. Changes current replace value for selected word.
	 */
	void changeSelected();

	/*!
	 * \fn void deleteSelected()
	 * Called when "Delete selected" button in configuration dialog
	 * was clicked. Deletes selected word from configuration list
	 * and also from internal words fix list.
	 */
	void deleteSelected();

	/*!
	 * \fn void addNew()
	 * Called when "Add new" button in configuration dialog
	 * was clicked. Adds new word and its fix value from filled fields.
	 */
	void addNew();

	/*!
	 * \fn void moveToNewValue()
	 * Called when return key is pressed in "new word" field
	 * in configuration dialog. Moves input focus to word fix value.
	 */
	void moveToNewValue();

};

#endif
