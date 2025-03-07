#include <locale.h>

#define UI_IMPLEMENTATION
#include "ui.h"

#define TAB_WIDTH 12
#define BODY_WIDTH 100

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

static Vector2 wn_size = {0};
static const char *strings[] = {"Оффтопик", "Компьютеры", "Телефоны"};

void draw_side_panel(void)
{
    if (wn_size.x > BODY_WIDTH / 2) {
        draw_vline(TAB_WIDTH, 0, wn_size.y);

        for (size_t i = 0; i < ARRAY_LEN(strings); i++) {
            draw_text(1, i + 1, strings[i]);
        }
    }
}

int main(void)
{
    setlocale(LC_ALL, "");

    set_config_flags(FLAG_CURSES_CBREAK | FLAG_CURSES_NOECHO | FLAG_CURSES_KEYPAD | FLAG_CURSES_NODELAY);
    init_curses();
    set_target_fps(30);

    while (!curses_should_close()) {
        wn_size = get_window_size();

        begin_drawing();
            draw_side_panel();
        end_drawing();
    }

    close_curses();

    return 0;
}
