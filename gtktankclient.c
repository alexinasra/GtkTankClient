#include <stdio.h>
#include <unistd.h>
#include <gtk-3.0/gtk/gtk.h>
#include <pthread.h>
#include <glib.h>
#include <curl/curl.h>

#include <gstreamer-1.0/gst/gst.h>
#include <gstreamer-1.0/gst/video/videooverlay.h>
#include <gdk/gdk.h>
#if defined (GDK_WINDOWING_X11)
#include <gdk/gdkx.h>
#elif defined (GDK_WINDOWING_WIN32)
#include <gdk/gdkwin32.h>
#elif defined (GDK_WINDOWING_QUARTZ)
#include <gdk/gdkquartz.h>
#endif


#include "./network.h"
#include "./vehicle.h"

GtkWidget *window;
GtkBuilder *builder;

GtkLabel *leftval;
GtkLabel *rightval;
GtkLabel *gearval;

gboolean my_keypress_function (GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Up) {
      forward_key_pressed();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Down) {
      backward_key_pressed();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Right) {
      right_key_pressed();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Left) {
      left_key_pressed();
        return TRUE;
    } else if (event->keyval == GDK_KEY_space) {
      handbreak_key_pressed();
        return TRUE;
    } else if (event->keyval == 102) {
      frontled_key_pressed();
      return TRUE;
    }
    g_print("keyval: %d", event->keyval);
    return FALSE;
}

gboolean my_keyrelease_function (GtkWidget *widget, GdkEventKey *event, gpointer data) {

    if (event->keyval == GDK_KEY_Up) {
      forward_key_released ();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Down) {
      backward_key_released ();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Right) {
      right_key_released ();
        return TRUE;
    } else if (event->keyval == GDK_KEY_Left) {
      left_key_released ();
        return TRUE;
    } else if (event->keyval == GDK_KEY_space) {
      handbreak_key_released ();
        return TRUE;
    } else if (event->keyval == 65505) { //shift_key
      gear_up_key_released ();
      return TRUE;
    } else if (event->keyval == 65507) {
      gear_down_key_released ();
      return TRUE;
    }
    //g_print("keyval: %d", event->keyval);
    return FALSE;
}

static void vechile_update_ui(struct vehicle_state state) {
  char buf[9]="";
  sprintf(buf, "%d%%\0", state.left_speed);
  gtk_label_set_text(leftval, buf);
  sprintf(buf, "%d%%\0", state.right_speed);
  gtk_label_set_text(rightval, buf);
  sprintf(buf, "%d\0", state.gear);
  gtk_label_set_text(gearval, buf);
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "MainWindow.glade", NULL);

    leftval = (GtkLabel*) GTK_WIDGET(gtk_builder_get_object(builder, "LEFTVAL"));
    rightval = (GtkLabel*) GTK_WIDGET(gtk_builder_get_object(builder, "RIGHTVAL"));
    gearval = (GtkLabel*) GTK_WIDGET(gtk_builder_get_object(builder, "GEARVAL"));

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_widget_add_events(window, GDK_KEY_RELEASE_MASK);
    g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (my_keypress_function), NULL);
    g_signal_connect (G_OBJECT (window), "key_release_event", G_CALLBACK (my_keyrelease_function), NULL);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    gtk_widget_show_all (window);

    GThread *driver_t = g_thread_new("driver_thread", driver_thread, (void*) vechile_update_ui);
    gtk_main();
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

  /* init threads */
    app = gtk_application_new ("org.gtk.GtkTankClient", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}



