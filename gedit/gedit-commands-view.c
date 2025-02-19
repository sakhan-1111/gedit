/*
 * gedit-view-commands.c
 * This file is part of gedit
 *
 * Copyright (C) 1998, 1999 Alex Roberts, Evan Lawrence
 * Copyright (C) 2000, 2001 Chema Celorio, Paolo Maggi
 * Copyright (C) 2002-2005 Paolo Maggi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "gedit-commands.h"
#include "gedit-commands-private.h"
#include <tepl/tepl.h>
#include "gedit-debug.h"
#include "gedit-window.h"
#include "gedit-window-private.h"

void
_gedit_cmd_view_focus_active (GSimpleAction *action,
                              GVariant      *state,
                              gpointer       user_data)
{
	GeditView *active_view;
	GeditWindow *window = GEDIT_WINDOW (user_data);

	gedit_debug (DEBUG_COMMANDS);

	active_view = gedit_window_get_active_view (window);

	if (active_view)
	{
		gtk_widget_grab_focus (GTK_WIDGET (active_view));
	}
}

void
_gedit_cmd_view_toggle_side_panel (GSimpleAction *action,
                                   GVariant      *state,
                                   gpointer       user_data)
{
	GeditWindow *window = GEDIT_WINDOW (user_data);
	GeditSidePanel *panel;
	gboolean visible;

	gedit_debug (DEBUG_COMMANDS);

	panel = _gedit_window_get_whole_side_panel (window);

	visible = g_variant_get_boolean (state);
	gtk_widget_set_visible (GTK_WIDGET (panel), visible);

	if (visible)
	{
		gtk_widget_grab_focus (GTK_WIDGET (panel));
	}

	g_simple_action_set_state (action, state);
}

void
_gedit_cmd_view_toggle_bottom_panel (GSimpleAction *action,
                                     GVariant      *state,
                                     gpointer       user_data)
{
	GeditWindow *window = GEDIT_WINDOW (user_data);
	GtkWidget *panel;
	gboolean visible;

	gedit_debug (DEBUG_COMMANDS);

	panel = gedit_window_get_bottom_panel (window);

	visible = g_variant_get_boolean (state);
	gtk_widget_set_visible (panel, visible);

	if (visible)
	{
		gtk_widget_grab_focus (panel);
	}

	g_simple_action_set_state (action, state);
}

void
_gedit_cmd_view_toggle_fullscreen_mode (GSimpleAction *action,
                                        GVariant      *state,
                                        gpointer       user_data)
{
	GeditWindow *window = GEDIT_WINDOW (user_data);

	gedit_debug (DEBUG_COMMANDS);

	if (g_variant_get_boolean (state))
	{
		_gedit_window_fullscreen (window);
	}
	else
	{
		_gedit_window_unfullscreen (window);
	}
}

void
_gedit_cmd_view_leave_fullscreen_mode (GSimpleAction *action,
                                       GVariant      *parameter,
                                       gpointer       user_data)
{
	_gedit_window_unfullscreen (GEDIT_WINDOW (user_data));
}

static void
language_activated_cb (TeplLanguageChooserDialog *dialog,
		       GtkSourceLanguage         *language,
		       GeditWindow               *window)
{
	GeditDocument *active_document;

	active_document = gedit_window_get_active_document (window);
	if (active_document != NULL)
	{
		gedit_document_set_language (active_document, language);
	}

	gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
language_chooser_dialog_response_after_cb (TeplLanguageChooserDialog *dialog,
					   gint                       response_id,
					   gpointer                   user_data)
{
	gtk_widget_destroy (GTK_WIDGET (dialog));
}

void
_gedit_cmd_view_highlight_mode (GSimpleAction *action,
                                GVariant      *parameter,
                                gpointer       user_data)
{
	GeditWindow *window = GEDIT_WINDOW (user_data);
	TeplLanguageChooserDialog *dialog;
	GeditDocument *active_document;

	dialog = tepl_language_chooser_dialog_new (GTK_WINDOW (window));

	active_document = gedit_window_get_active_document (window);
	if (active_document != NULL)
	{
		GtkSourceLanguage *language;

		language = gtk_source_buffer_get_language (GTK_SOURCE_BUFFER (active_document));
		tepl_language_chooser_select_language (TEPL_LANGUAGE_CHOOSER (dialog), language);
	}

	g_signal_connect_object (dialog,
				 "language-activated",
				 G_CALLBACK (language_activated_cb),
				 window,
				 0);

	g_signal_connect_after (dialog,
				"response",
				G_CALLBACK (language_chooser_dialog_response_after_cb),
				NULL);

	gtk_widget_show (GTK_WIDGET (dialog));
}

/* ex:set ts=8 noet: */
