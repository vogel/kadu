/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <gtk/gtk.h>
#include <stdio.h>

GtkFontSelectionDialog *dialog;
gboolean ok(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	printf("%s\n", gtk_font_selection_dialog_get_font_name(dialog));
	gtk_exit(0);
}

gboolean end(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	gtk_exit(0);
}

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	dialog = (GtkFontSelectionDialog *)gtk_font_selection_dialog_new("XOSD font");
	if (argc > 1)
		gtk_font_selection_dialog_set_font_name(dialog, argv[1]);
	gtk_widget_show((GtkWidget*)dialog);

	gtk_signal_connect(GTK_OBJECT(dialog->ok_button), "button-release-event", GTK_SIGNAL_FUNC(ok), NULL);
	gtk_signal_connect(GTK_OBJECT(dialog->cancel_button), "button-release-event", GTK_SIGNAL_FUNC(end), NULL);
	gtk_signal_connect(GTK_OBJECT(dialog), "delete-event", GTK_SIGNAL_FUNC(end), NULL);

	gtk_main();
	return 0;
}
