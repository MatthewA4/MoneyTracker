//Copyright (C) 2026 Matthew Anderson
//MIT License

#pragma once

#include <gtk/gtk.h>
#include <memory>
#include <vector>
#include "TransactionData.h"
#include "BudgetAnalyzer.h"
#include "ConfigManager.h"

class MoneyTrackerGUI {
public:
    MoneyTrackerGUI();
    ~MoneyTrackerGUI();
    
    void run();
    
private:
    // GTK widgets
    GtkWidget* window;
    GtkWidget* main_box;
    GtkWidget* notebook;
    
    // File selection
    GtkWidget* file_list;
    GtkWidget* account_list;
    GtkWidget* output_entry;
    GtkWidget* category_config_entry;
    
    // Results display
    GtkWidget* summary_text;
    GtkWidget* category_text;
    GtkWidget* monthly_text;
    GtkWidget* transactions_text;
    
    // Processing
    GtkWidget* progress_bar;
    GtkWidget* status_label;
    
    // Data
    std::vector<std::pair<std::string, std::string>> input_files;  // <path, account_name>
    std::shared_ptr<TransactionData> transaction_data;
    std::shared_ptr<ConfigManager> config_manager;
    
    // Callbacks
    static void on_add_file(GtkWidget* widget, gpointer data);
    static void on_remove_file(GtkWidget* widget, gpointer data);
    static void on_choose_output(GtkWidget* widget, gpointer data);
    static void on_choose_config(GtkWidget* widget, gpointer data);
    static void on_analyze(GtkWidget* widget, gpointer data);
    static void on_open_excel(GtkWidget* widget, gpointer data);
    static gboolean on_window_close(GtkWidget* widget, GdkEvent* event, gpointer data);
    
    // Helper methods
    void build_ui();
    void update_file_list();
    void run_analysis();
    void display_results();
    void show_error(const std::string& message);
    void show_info(const std::string& message);
};
