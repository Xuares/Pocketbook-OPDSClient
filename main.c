#include "opds_app.h"
#include <inkview.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef CFG_FILE
#define CFG_FILE "/mnt/ext1/system/config/opds_client.cfg"
#endif

// Global State Variables
enum AppState current_state = STATE_MAIN_MENU; 
int server_count = 0;
int current_server_index = -1;
int editing_server_index = -1;
int selected_entry_index = -1;
OPDSServer servers[MAX_SERVERS];
OPDSServer temp_server;
OPDSEntry current_entries[MAX_ENTRIES];
int entry_count = 0;
char current_host[MAX_STR_LEN];
int sys_width, sys_height;

// Debug logging mechanism (triggered only if LOGTRIGGER.TXT is present)
void LogDebug(const char *msg) {
    char trigger_file[MAX_STR_LEN];
    snprintf(trigger_file, sizeof(trigger_file), "%sLOGTRIGGER.TXT", APP_ROOT_DIR);

    // Exit early if the trigger file is absent to preserve device performance
    if (access(trigger_file, F_OK) != 0) {
        return; 
    }

    char log_file[MAX_STR_LEN];
    snprintf(log_file, sizeof(log_file), "%sopds_client.log", APP_ROOT_DIR);

    FILE *f = fopen(log_file, "a");
    if (f) {
        time_t now;
        time(&now);
        char *date = ctime(&now);
        date[strlen(date) - 1] = '\0'; // Strip the default newline from ctime
        fprintf(f, "[%s] %s\n", date, msg);
        fclose(f);
    }
}

// External UI Handlers
extern void DrawMainMenu();
extern void HandleMainMenuTouch(int x, int y);
extern void DrawServerOptions();
extern void HandleServerOptionsTouch(int x, int y);
extern void DrawServerForm();
extern void HandleServerFormTouch(int x, int y);
extern void DrawBrowsingView();
extern void HandleBrowsingTouch(int x, int y);
extern void DrawBookDetails();
extern void HandleBookDetailsTouch(int x, int y);
extern void HandleHardwareButtons(int key);
extern void InitNetwork();
extern void CleanupNetwork();

void Repaint() {
    ClearScreen();
    switch (current_state) {
        case STATE_MAIN_MENU:      DrawMainMenu(); break;
        case STATE_SERVER_OPTIONS: DrawServerOptions(); break;
        case STATE_SERVER_FORM:    DrawServerForm(); break;
        case STATE_BROWSING:       DrawBrowsingView(); break;
        case STATE_BOOK_DETAILS:   DrawBookDetails(); break;
    }
    FullUpdate();
}

void SaveServers() {
    FILE *fp = fopen(CFG_FILE, "wb");
    if (fp) {
        fwrite(&server_count, sizeof(int), 1, fp);
        fwrite(servers, sizeof(OPDSServer), server_count, fp);
        fclose(fp);
    }
}

void LoadServers() {
    FILE *fp = fopen(CFG_FILE, "rb");
    if (fp) {
        if (fread(&server_count, sizeof(int), 1, fp) == 1) {
            if (server_count > MAX_SERVERS) server_count = MAX_SERVERS;
            fread(servers, sizeof(OPDSServer), server_count, fp);
        }
        fclose(fp);
    } else {
        server_count = 0;
    }
}

static int main_handler(int event, int a, int b) {
    switch (event) {

        case EVT_INIT:
            sys_width = ScreenWidth();
            sys_height = ScreenHeight();
            LoadServers();
            LogDebug("--- App Started ---");
            break;

        case EVT_SHOW:
        case EVT_FOREGROUND:
            // Handle app un-minimization or device waking from sleep
            SetPanelType(0); 
            Repaint();       
            break;
            
        case EVT_BACKGROUND:
            // Handle device sleep or app minimization
            break;

        case EVT_KEYPRESS:
            if (a == IV_KEY_BACK) { 
                if (current_state == STATE_MAIN_MENU) CloseApp();
                else {
                    current_state = STATE_MAIN_MENU;
                    Repaint();
                }
            } else {
                HandleHardwareButtons(a);
            }
            break;

        case EVT_POINTERUP:
            switch (current_state) {
                case STATE_MAIN_MENU:      HandleMainMenuTouch(a, b); break;
                case STATE_SERVER_OPTIONS: HandleServerOptionsTouch(a, b); break;
                case STATE_SERVER_FORM:    HandleServerFormTouch(a, b); break;
                case STATE_BROWSING:       HandleBrowsingTouch(a, b); break;
                case STATE_BOOK_DETAILS:   HandleBookDetailsTouch(a, b); break;
            }
            break;

        case EVT_EXIT:
            break;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    InkViewMain(main_handler);
    return 0;
}
