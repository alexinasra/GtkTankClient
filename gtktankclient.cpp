#include <iostream>
#include <unistd.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <glib.h>
#include <curl/curl.h>




#define PROJECT_NAME "GTKTankClient"


GThread *forwardThread, *backwardThread;
GThread *leftThread, *rightThread;

GtkWidget *window;
GtkBuilder *builder;

GtkAdjustment *speedAdj;
GtkAdjustment *wheelAdj;

GtkLabel *leftVal;
GtkLabel *rightVal;

GtkToggleButton *breakBtn;
GtkToggleButton *forwardBtn;
GtkToggleButton *backwardBtn;
GtkToggleButton *leftBtn;
GtkToggleButton *rightBtn;

GtkScale *speedSkl;
GtkScale *wheelSkl;

void send_wheel_speed(int left, int right) {
    float data[2];
    char bufptr[255];
    data[0] = left / 100.0;
    data[1] = right / 100.0;
    CURL *curl = curl_easy_init();

    sprintf(bufptr,"direction=(%f,%f)", data[0], data[1]);
    curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.4.1/drive");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    /* size of the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(bufptr));

    /* pass in a pointer to the data - libcurl will not copy */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bufptr);

    int ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}
int oldLeftSpeed = 0;
int oldRightSpeed = 0;

void handle_speed_change () {
    int speed = gtk_adjustment_get_value(speedAdj),
        wheel = gtk_adjustment_get_value(wheelAdj);
    int speedLeft = 0, speedRight = 0;

    if (speed > 0) {
        g_print("driving forward");
        if (wheel < 0) {
            g_print("and stering left. \n");
            speedLeft = speed + (wheel*wheel);
            speedRight = speed;
            if (speedLeft < 0) speedLeft = 0;
        } else if (wheel > 0) {
            g_print("and stering right. \n");
            speedLeft = speed;
            speedRight = speed + (wheel*wheel);
            if (speedRight < 0) speedRight = 0;
        } else {
            g_print(".\n");
            speedLeft = speedRight = speed;
        }
    } else if (speed < 0) {
        g_print("driving backward");
        if (wheel < 0) {
            g_print("and stering left. \n");
            speedLeft = speed - (wheel*wheel);
            speedRight = speed;
            if (speedLeft > 0) speedLeft = 0;
        } else if (wheel > 0) {
            g_print("and stering right. \n");
            speedLeft = speed;
            speedRight = speed - (wheel*wheel);
            if (speedRight > 0) speedRight = 0;

        } else {
            g_print(".\n");
            speedLeft = speedRight = speed;
        }
    } else {
        if (wheel != 0) {
            g_print("rotating. \n");
            speedLeft = -wheel;
            speedRight = wheel;
        } else {
            g_print("stop.\n");

            speedLeft = 0;
            speedRight = 0;
        }
    }
    char bufptr[5];
    sprintf(bufptr, "%d", speedLeft);
    gtk_label_set_text(leftVal, bufptr);
    sprintf(bufptr, "%d", speedRight);
    gtk_label_set_text(rightVal, bufptr);
    send_wheel_speed(speedLeft, speedRight);
    oldLeftSpeed = speedLeft;
    oldRightSpeed = speedRight;
}

void *forward_thread_func(gpointer ptr) {
    do {
        int speed = gtk_adjustment_get_value(speedAdj) + 1;
        gtk_adjustment_set_value(speedAdj, speed);
        handle_speed_change();
        usleep(1000);
    } while(gtk_toggle_button_get_active(forwardBtn));
    gtk_adjustment_set_value(speedAdj, 0);
    handle_speed_change();

    return NULL;
}
void *backward_thread_func(gpointer ptr) {
    do {

        int speed = gtk_adjustment_get_value(speedAdj) - 1;
        gtk_adjustment_set_value(speedAdj, speed);
        handle_speed_change();

        usleep(1000);
    } while(gtk_toggle_button_get_active(backwardBtn));

    gtk_adjustment_set_value(speedAdj, 0);
    handle_speed_change();

    return NULL;
}
void *left_thread_func(gpointer ptr) {
    do {
        int wheel = gtk_adjustment_get_value(wheelAdj) - 3;
        if (wheel > -10) wheel = -10;
        gtk_adjustment_set_value(wheelAdj, wheel);
        handle_speed_change();
        usleep(100);
    } while(gtk_toggle_button_get_active(leftBtn));

    gtk_adjustment_set_value(wheelAdj, 0);
    handle_speed_change();
    return NULL;
}
void *right_thread_func(gpointer ptr) {
    do {
        int wheel = gtk_adjustment_get_value(wheelAdj) + 3;
        if (wheel < 10) wheel = 10;
        gtk_adjustment_set_value(wheelAdj, wheel);
        handle_speed_change();
        usleep(100);
    } while(gtk_toggle_button_get_active(rightBtn));

    gtk_adjustment_set_value(wheelAdj, 0);
    handle_speed_change();

    return NULL;
}
gboolean my_keypress_function (GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Up) {
        if (!gtk_toggle_button_get_active(forwardBtn)) {
            gtk_toggle_button_set_active(forwardBtn, true);
            if(forwardThread) g_thread_unref(forwardThread);
            forwardThread = g_thread_new("forward thread", forward_thread_func, NULL);
        }
        return true;
    } else if (event->keyval == GDK_KEY_Down) {
        if (!gtk_toggle_button_get_active(backwardBtn)) {
            gtk_toggle_button_set_active(backwardBtn, true);

            if(backwardThread) g_thread_unref(backwardThread);
            backwardThread = g_thread_new("backward thread", backward_thread_func, NULL);
        }
        return true;
    } else if (event->keyval == GDK_KEY_Right) {
        if (!gtk_toggle_button_get_active(rightBtn)) {
            gtk_toggle_button_set_active(rightBtn, true);
            if(rightThread) g_thread_unref(rightThread);
            rightThread = g_thread_new("right thread", right_thread_func, NULL);
        }
        return true;
    } else if (event->keyval == GDK_KEY_Left) {
        if (!gtk_toggle_button_get_active(leftBtn)) {
            gtk_toggle_button_set_active(leftBtn, true);
            if(leftThread) g_thread_unref(leftThread);
            leftThread = g_thread_new("left thread", left_thread_func, NULL);
        }
        return true;
    } else if (event->keyval == GDK_KEY_space) {
        gtk_adjustment_set_value(speedAdj, 0);
        gtk_adjustment_set_value(wheelAdj, 0);
        send_wheel_speed(0, 0);
        return true;
    }
    //g_print("keyval: %d", event->keyval);
    return FALSE;
}

gboolean my_keyrelease_function (GtkWidget *widget, GdkEventKey *event, gpointer data) {

    if (event->keyval == GDK_KEY_Up) {
        gtk_toggle_button_set_active(forwardBtn, false);
        return true;
    } else if (event->keyval == GDK_KEY_Down) {
        gtk_toggle_button_set_active(backwardBtn, false);
        return true;
    } else if (event->keyval == GDK_KEY_Right) {
        gtk_toggle_button_set_active(rightBtn, false);
        return true;
    } else if (event->keyval == GDK_KEY_Left) {
        gtk_toggle_button_set_active(leftBtn, false);
        return true;
    } else if (event->keyval == GDK_KEY_space) {
        return true;
    }
    //g_print("keyval: %d", event->keyval);
    return FALSE;
}

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "MainWindow.glade", NULL);

    //assign widgets
    speedAdj = (GtkAdjustment*) GTK_ADJUSTMENT(gtk_builder_get_object(builder, "SPEEDADJ"));
    wheelAdj = (GtkAdjustment*) GTK_ADJUSTMENT(gtk_builder_get_object(builder, "WHEELADJ"));

    leftVal = (GtkLabel*) GTK_WIDGET(gtk_builder_get_object(builder, "LEFTVAL"));
    rightVal = (GtkLabel*) GTK_WIDGET(gtk_builder_get_object(builder, "RIGHTVAL"));

    breakBtn = (GtkToggleButton*) GTK_WIDGET(gtk_builder_get_object(builder, "BREAKBTN"));
    forwardBtn = (GtkToggleButton*) GTK_WIDGET(gtk_builder_get_object(builder, "FORWARDBTN"));
    backwardBtn = (GtkToggleButton*) GTK_WIDGET(gtk_builder_get_object(builder, "BACKWARDBTN"));
    leftBtn = (GtkToggleButton*) GTK_WIDGET(gtk_builder_get_object(builder, "LEFTBTN"));
    rightBtn = (GtkToggleButton*) GTK_WIDGET(gtk_builder_get_object(builder, "RIGHTBTN"));
    speedSkl = (GtkScale*) GTK_WIDGET(gtk_builder_get_object(builder, "SPEED"));
    wheelSkl = (GtkScale*) GTK_WIDGET(gtk_builder_get_object(builder, "WHEEL"));

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    gtk_widget_add_events(window, GDK_KEY_RELEASE_MASK);
    g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (my_keypress_function), NULL);
    g_signal_connect (G_OBJECT (window), "key_release_event", G_CALLBACK (my_keyrelease_function), NULL);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
    gtk_widget_show_all (window);

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



