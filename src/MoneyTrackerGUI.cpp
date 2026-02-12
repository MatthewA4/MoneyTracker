//Copyright (C) 2026 Matthew Anderson
//MIT License

#include "MoneyTrackerGUI.h"
#include "CSVParser.h"
#include "BudgetAnalyzer.h"
#include "Logger.h"
#include <glib.h>
#include <sstream>
#include <iomanip>

MoneyTrackerGUI::MoneyTrackerGUI()
    : window(nullptr), transaction_data(std::make_shared<TransactionData>()),
      config_manager(std::make_shared<ConfigManager>()) {
    
    config_manager->loadCategoriesFromFile("data/categories.json");
}

MoneyTrackerGUI::~MoneyTrackerGUI() {
    if (window) {
        gtk_widget_destroy(window);
    }
}

void MoneyTrackerGUI::run() {
    build_ui();
    gtk_widget_show_all(window);
    gtk_main();
}

void MoneyTrackerGUI::build_ui() {
    // Apply custom CSS styling for modern look
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const char* css_styles =
        "window { background-color: #f5f5f5; }\n"
        "button { padding: 6px 12px; border-radius: 3px; }\n"
        "button:hover { background-color: #e0e0e0; }\n"
        "entry { padding: 4px 8px; border: 1px solid #ccc; border-radius: 3px; }\n"
        "textview { padding: 4px; }\n"
        "label { font-size: 11pt; }\n"
        ".header-label { font-size: 14pt; font-weight: bold; color: #333; margin: 10px 0px 5px 0px; }\n";
    gtk_css_provider_load_from_data(css_provider, css_styles, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                               GTK_STYLE_PROVIDER(css_provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "MoneyTracker - Budget Analysis Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name(GTK_WINDOW(window), "application-vnd.oasis.opendocument.spreadsheet");
    
    g_signal_connect(window, "delete-event", G_CALLBACK(on_window_close), this);
    
    // Main container with padding
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 12);
    
    // Create notebook (tabs)
    notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), notebook, TRUE, TRUE, 0);
    
    // ========== TAB 1: Input Files ==========
    GtkWidget* input_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(input_page), 12);
    
    GtkWidget* files_header = gtk_label_new("📁 Input CSV Files");
    gtk_style_context_add_class(gtk_widget_get_style_context(files_header), "header-label");
    gtk_label_set_xalign(GTK_LABEL(files_header), 0.0);
    gtk_box_pack_start(GTK_BOX(input_page), files_header, FALSE, FALSE, 0);
    
    // File list with scrolling
    GtkWidget* scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 200);
    file_list = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(file_list), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(file_list), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), file_list);
    gtk_box_pack_start(GTK_BOX(input_page), scrolled, TRUE, TRUE, 0);
    
    // File buttons with icons
    GtkWidget* file_btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_set_homogeneous(GTK_BOX(file_btn_box), FALSE);
    
    GtkWidget* add_btn = gtk_button_new_from_icon_name("list-add", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(add_btn), "Add File");
    gtk_button_set_always_show_image(GTK_BUTTON(add_btn), TRUE);
    gtk_widget_set_tooltip_text(add_btn, "Add a CSV file to analyze");
    
    GtkWidget* remove_btn = gtk_button_new_from_icon_name("list-remove", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(remove_btn), "Remove");
    gtk_button_set_always_show_image(GTK_BUTTON(remove_btn), TRUE);
    gtk_widget_set_tooltip_text(remove_btn, "Remove the last added file");
    
    gtk_box_pack_start(GTK_BOX(file_btn_box), add_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(file_btn_box), remove_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(input_page), file_btn_box, FALSE, FALSE, 0);
    
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_file), this);
    g_signal_connect(remove_btn, "clicked", G_CALLBACK(on_remove_file), this);
    
    // Output file selection section
    GtkWidget* output_header = gtk_label_new("📊 Output File");
    gtk_style_context_add_class(gtk_widget_get_style_context(output_header), "header-label");
    gtk_label_set_xalign(GTK_LABEL(output_header), 0.0);
    gtk_box_pack_start(GTK_BOX(input_page), output_header, FALSE, FALSE, 0);
    
    GtkWidget* output_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget* output_label = gtk_label_new("File:");
    output_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(output_entry), "budget_report.xlsx");
    gtk_widget_set_size_request(output_entry, 300, -1);
    
    GtkWidget* output_btn = gtk_button_new_from_icon_name("document-open", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(output_btn), "Browse");
    gtk_button_set_always_show_image(GTK_BUTTON(output_btn), TRUE);
    gtk_widget_set_tooltip_text(output_btn, "Choose output Excel file location");
    
    gtk_box_pack_start(GTK_BOX(output_box), output_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(output_box), output_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(output_box), output_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(input_page), output_box, FALSE, FALSE, 0);
    
    g_signal_connect(output_btn, "clicked", G_CALLBACK(on_choose_output), this);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), input_page,
                             gtk_label_new("📁 Input Files"));
    
    // ========== TAB 2: Configuration ==========
    GtkWidget* config_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(config_page), 12);
    
    GtkWidget* config_header = gtk_label_new("⚙️ Configuration");
    gtk_style_context_add_class(gtk_widget_get_style_context(config_header), "header-label");
    gtk_label_set_xalign(GTK_LABEL(config_header), 0.0);
    gtk_box_pack_start(GTK_BOX(config_page), config_header, FALSE, FALSE, 0);
    
    GtkWidget* cat_config_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    GtkWidget* cat_label = gtk_label_new("Category Rules:");
    category_config_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(category_config_entry), "data/categories.json");
    gtk_widget_set_size_request(category_config_entry, 350, -1);
    gtk_widget_set_tooltip_text(category_config_entry,
        "Path to categories.json file with transaction categorization rules");
    
    GtkWidget* cat_btn = gtk_button_new_from_icon_name("document-open", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(cat_btn), "Browse");
    gtk_button_set_always_show_image(GTK_BUTTON(cat_btn), TRUE);
    gtk_widget_set_tooltip_text(cat_btn, "Select category configuration file");
    
    gtk_box_pack_start(GTK_BOX(cat_config_box), cat_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(cat_config_box), category_config_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(cat_config_box), cat_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(config_page), cat_config_box, FALSE, FALSE, 0);
    
    g_signal_connect(cat_btn, "clicked", G_CALLBACK(on_choose_config), this);
    
    GtkWidget* info_label = gtk_label_new(
        "Use categories.json to define custom transaction categorization rules.\n"
        "Format: JSON file with category patterns and keywords.");
    gtk_label_set_xalign(GTK_LABEL(info_label), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(info_label), TRUE);
    gtk_box_pack_start(GTK_BOX(config_page), info_label, FALSE, FALSE, 0);
    
    // Add an info box
    GtkWidget* info_box = gtk_event_box_new();
    GdkRGBA color = {0.9, 0.95, 1.0, 1.0};
    gtk_widget_override_background_color(info_box, GTK_STATE_FLAG_NORMAL, &color);
    
    GtkWidget* info_inner = gtk_label_new("💡 Tip: Add your own patterns to categories.json for better categorization");
    gtk_widget_set_margin_top(info_inner, 8);
    gtk_widget_set_margin_bottom(info_inner, 8);
    gtk_widget_set_margin_start(info_inner, 8);
    gtk_widget_set_margin_end(info_inner, 8);
    gtk_container_add(GTK_CONTAINER(info_box), info_inner);
    gtk_box_pack_start(GTK_BOX(config_page), info_box, FALSE, FALSE, 0);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), config_page,
                             gtk_label_new("⚙️ Config"));
    
    // ========== TAB 3: Summary Results ==========
    GtkWidget* summary_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(summary_scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    summary_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(summary_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(summary_text), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(summary_text), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(summary_text), 8);
    gtk_container_add(GTK_CONTAINER(summary_scrolled), summary_text);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), summary_scrolled,
                             gtk_label_new("📈 Summary"));
    
    // ========== TAB 4: Category Breakdown ==========
    GtkWidget* category_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(category_scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    category_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(category_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(category_text), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(category_text), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(category_text), 8);
    gtk_container_add(GTK_CONTAINER(category_scrolled), category_text);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), category_scrolled,
                             gtk_label_new("🏷️ Categories"));
    
    // ========== TAB 5: Monthly Trends ==========
    GtkWidget* monthly_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(monthly_scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    monthly_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(monthly_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(monthly_text), GTK_WRAP_WORD);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(monthly_text), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(monthly_text), 8);
    gtk_container_add(GTK_CONTAINER(monthly_scrolled), monthly_text);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), monthly_scrolled,
                             gtk_label_new("📅 Trends"));
    
    // ========== TAB 6: Transactions ==========
    GtkWidget* trans_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(trans_scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    transactions_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(transactions_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(transactions_text), GTK_WRAP_NONE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(transactions_text), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(transactions_text), 8);
    gtk_container_add(GTK_CONTAINER(trans_scrolled), transactions_text);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), trans_scrolled,
                             gtk_label_new("💰 Transactions"));
    
    // Progress and status
    GtkWidget* progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(progress_box), 8);
    gtk_style_context_add_class(gtk_widget_get_style_context(progress_box), "progress-box");
    
    progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progress_bar), TRUE);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress_bar), "Ready");
    gtk_widget_set_size_request(progress_bar, -1, 24);
    
    status_label = gtk_label_new("✓ Ready to analyze");
    gtk_label_set_xalign(GTK_LABEL(status_label), 0.0);
    gtk_style_context_add_class(gtk_widget_get_style_context(status_label), "status-label");
    
    gtk_box_pack_start(GTK_BOX(progress_box), progress_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(progress_box), status_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), progress_box, FALSE, FALSE, 0);
    
    // Action buttons with better styling
    GtkWidget* button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(button_box), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(button_box), 8);
    
    GtkWidget* analyze_btn = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(analyze_btn), "Analyze Files");
    gtk_button_set_always_show_image(GTK_BUTTON(analyze_btn), TRUE);
    gtk_widget_set_tooltip_text(analyze_btn, "Analyze selected CSV files and generate report (Ctrl+A)");
    gtk_widget_set_size_request(analyze_btn, 140, 40);
    
    GtkWidget* excel_btn = gtk_button_new_from_icon_name("document-open-recent", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(excel_btn), "Open Report");
    gtk_button_set_always_show_image(GTK_BUTTON(excel_btn), TRUE);
    gtk_widget_set_tooltip_text(excel_btn, "Open the generated Excel file (Ctrl+O)");
    
    gtk_box_pack_start(GTK_BOX(button_box), analyze_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), excel_btn, FALSE, FALSE, 0);
    
    // Spacer
    gtk_box_pack_start(GTK_BOX(button_box), gtk_label_new(""), TRUE, TRUE, 0);
    
    // Quit button on the right
    GtkWidget* quit_btn = gtk_button_new_from_icon_name("application-exit", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(GTK_BUTTON(quit_btn), "Quit");
    gtk_button_set_always_show_image(GTK_BUTTON(quit_btn), TRUE);
    gtk_widget_set_tooltip_text(quit_btn, "Close MoneyTracker (Ctrl+Q)");
    gtk_box_pack_end(GTK_BOX(button_box), quit_btn, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    // Connect signals
    g_signal_connect(analyze_btn, "clicked", G_CALLBACK(on_analyze), this);
    g_signal_connect(excel_btn, "clicked", G_CALLBACK(on_open_excel), this);
    g_signal_connect(quit_btn, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    // Add keyboard accelerators
    GtkAccelGroup* accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
    
    gtk_widget_add_accelerator(analyze_btn, "clicked", accel_group,
                               GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(excel_btn, "clicked", accel_group,
                               GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(quit_btn, "clicked", accel_group,
                               GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(add_btn, "clicked", accel_group,
                               GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
}

void MoneyTrackerGUI::on_add_file(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Select CSV File",
        GTK_WINDOW(self->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        GtkWidget* name_dialog = gtk_dialog_new_with_buttons(
            "Enter Account Name",
            GTK_WINDOW(self->window),
            GTK_DIALOG_MODAL,
            "_OK", GTK_RESPONSE_OK,
            "_Cancel", GTK_RESPONSE_CANCEL,
            NULL);
        
        GtkWidget* entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(entry), "Account");
        gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(name_dialog))), entry);
        gtk_widget_show(entry);
        
        if (gtk_dialog_run(GTK_DIALOG(name_dialog)) == GTK_RESPONSE_OK) {
            const char* account_name = gtk_entry_get_text(GTK_ENTRY(entry));
            self->input_files.push_back({filename, account_name});
            self->update_file_list();
        }
        
        gtk_widget_destroy(name_dialog);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void MoneyTrackerGUI::on_remove_file(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    if (!self->input_files.empty()) {
        self->input_files.pop_back();
        self->update_file_list();
    }
}

void MoneyTrackerGUI::on_choose_output(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Save Excel File",
        GTK_WINDOW(self->window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "budget_report.xlsx");
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(self->output_entry), filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void MoneyTrackerGUI::on_choose_config(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Select Category Config",
        GTK_WINDOW(self->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(self->category_config_entry), filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void MoneyTrackerGUI::on_analyze(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    self->run_analysis();
}

void MoneyTrackerGUI::on_open_excel(GtkWidget* widget, gpointer data) {
    (void)widget;
    MoneyTrackerGUI* self = static_cast<MoneyTrackerGUI*>(data);
    const char* output_file = gtk_entry_get_text(GTK_ENTRY(self->output_entry));
    std::string cmd = std::string("xdg-open \"") + output_file + "\"";
    mt::Logger::info("Opening file: " + std::string(output_file));
    // Use GLib to spawn command asynchronously (safer than system())
    if (!g_spawn_command_line_async(cmd.c_str(), NULL)) {
        mt::Logger::error("Failed to spawn command to open file: " + std::string(output_file));
    }
}

gboolean MoneyTrackerGUI::on_window_close(GtkWidget* widget, GdkEvent* event, gpointer data) {
    (void)widget;
    (void)event;
    (void)data;
    gtk_main_quit();
    return FALSE;
}

void MoneyTrackerGUI::update_file_list() {
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_list));
    std::stringstream ss;
    
    for (size_t i = 0; i < input_files.size(); ++i) {
        ss << (i + 1) << ". " << input_files[i].first 
           << " [" << input_files[i].second << "]\n";
    }
    
    gtk_text_buffer_set_text(buffer, ss.str().c_str(), -1);
}

void MoneyTrackerGUI::run_analysis() {
    if (input_files.empty()) {
        show_error("Please add input files first");
        return;
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Analyzing...");
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.25);
    
    try {
        // Load configuration
        const char* config_path = gtk_entry_get_text(GTK_ENTRY(category_config_entry));
        config_manager->loadCategoriesFromFile(config_path);
        
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.5);
        
        // Parse files
        transaction_data = std::make_shared<TransactionData>();
        CSVParser parser(config_manager);
        
        for (const auto& file_pair : input_files) {
            auto transactions = parser.parse(file_pair.first, file_pair.second);
            transaction_data->addTransactions(transactions);
        }
        
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.75);
        
        // Analyze and display results
        display_results();
        
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 1.0);
        gtk_label_set_text(GTK_LABEL(status_label), "Analysis complete");
        show_info("Analysis completed successfully!");
        
    } catch (const std::exception& e) {
        show_error(std::string("Error: ") + e.what());
        gtk_label_set_text(GTK_LABEL(status_label), "Error during analysis");
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.0);
    }
}

void MoneyTrackerGUI::display_results() {
    BudgetAnalyzer analyzer(*transaction_data);
    BudgetSummary summary = analyzer.analyzeBudget();
    
    // Summary tab
    {
        std::stringstream ss;
        ss << "BUDGET SUMMARY\n";
        ss << "==============\n\n";
        ss << "Total Income:    $" << summary.totalIncome << "\n";
        ss << "Total Expenses:  $" << summary.totalExpenses << "\n";
        ss << "Net Change:      $" << summary.netChange << "\n";
        ss << "\nAverage Transaction: $" << analyzer.getAverageTransaction() << "\n";
        ss << "Average Monthly:      $" << analyzer.getAverageMonthlySpending() << "\n";
        
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(summary_text));
        gtk_text_buffer_set_text(buffer, ss.str().c_str(), -1);
    }
    
    // Category tab
    {
        std::stringstream ss;
        ss << "SPENDING BY CATEGORY\n";
        ss << "====================\n\n";
        for (const auto& cat : summary.categoryBreakdown) {
            ss << std::left << std::setw(25) << cat.first << ": $" 
               << std::fixed << std::setprecision(2) << cat.second << "\n";
        }
        
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(category_text));
        gtk_text_buffer_set_text(buffer, ss.str().c_str(), -1);
    }
    
    // Monthly tab
    {
        std::stringstream ss;
        ss << "MONTHLY TRENDS\n";
        ss << "==============\n\n";
        for (const auto& month : summary.monthlyTrends) {
            ss << month.first << ": $" << std::fixed << std::setprecision(2) 
               << month.second << "\n";
        }
        
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(monthly_text));
        gtk_text_buffer_set_text(buffer, ss.str().c_str(), -1);
    }
    
    // Transactions tab
    {
        std::stringstream ss;
        ss << std::left << std::setw(12) << "Date" << " | " 
           << std::setw(30) << "Description" << " | "
           << std::setw(12) << "Amount" << " | "
           << std::setw(15) << "Category" << " | "
           << std::setw(15) << "Account" << "\n";
        ss << std::string(90, '-') << "\n";
        
        for (const auto& trans : transaction_data->getAllTransactions()) {
            ss << std::left << std::setw(12) << trans.date << " | "
               << std::setw(30) << trans.description.substr(0, 27) << " | "
               << std::setw(12) << std::fixed << std::setprecision(2) << trans.amount << " | "
               << std::setw(15) << trans.category << " | "
               << std::setw(15) << trans.accountName << "\n";
        }
        
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(transactions_text));
        gtk_text_buffer_set_text(buffer, ss.str().c_str(), -1);
    }
}

void MoneyTrackerGUI::show_error(const std::string& message) {
    mt::Logger::error(message);
    GtkWidget* dialog = gtk_message_dialog_new(
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message.c_str());
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void MoneyTrackerGUI::show_info(const std::string& message) {
    mt::Logger::info(message);
    GtkWidget* dialog = gtk_message_dialog_new(
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message.c_str());
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
