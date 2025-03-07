#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TUI_IMPLEMENTATION
#include "tui.h"

#define TAB_WIDTH 15
#define BODY_WIDTH 100

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

static unsigned int current_window = 0;
static Vector2 wn_size = {0};

static const char *usage_button_texts[] = {
    "F1 Threads",
    "F2 Profile",
    "F3 Settings",
};

static const char *strings[] = {
    "All discussion",
    "My Threads",
    "Viewed Threads",
    "Bookmarks",
    "Scheduled threads",
};

void draw_window_threads(void)
{
    for (size_t i = 0; i < ARRAY_LEN(strings); i++) {
        draw_text(0, i, strings[i]);
    }
}

void draw_window_profile(void)
{
    draw_text(0, 0, "Profile");
}

void draw_bottom_buttons(void)
{
    size_t total_string_size = 0;
    size_t last_string_size = 0;

    for (size_t i = 0; i < ARRAY_LEN(usage_button_texts); i++) {
        total_string_size += strlen(usage_button_texts[i]) + 2;
    }

    if (wn_size.x > (int)total_string_size) {
        for (size_t i = 0; i < ARRAY_LEN(usage_button_texts); i++) {
            draw_text(last_string_size, wn_size.y - 1, usage_button_texts[i]);
            last_string_size += strlen(usage_button_texts[i]) + 2;
        }
    }
}

int main(void)
{
    set_config_flags(FLAG_CURSES_CBREAK | FLAG_CURSES_NOECHO | FLAG_CURSES_KEYPAD | FLAG_CURSES_NODELAY);
    init_curses();
    set_target_fps(30);

    int last_input_key = -1;

    while (last_input_key != 'q') {
        wn_size = get_window_size();
        last_input_key = get_pressed_key();

        if (last_input_key == KEY_F1) current_window = 0;
        if (last_input_key == KEY_F2) current_window = 1;

        begin_drawing();
            if (current_window == 0) draw_window_threads();
            if (current_window == 1) draw_window_profile();
            draw_bottom_buttons();
        end_drawing();
    }

    close_curses();

    return 0;
}
