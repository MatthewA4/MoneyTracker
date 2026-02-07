//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "MoneyTrackerGUI.h"
#include <gtk/gtk.h>

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);
    
    MoneyTrackerGUI app;
    app.run();
    
    return 0;
}
