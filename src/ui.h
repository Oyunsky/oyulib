#ifndef UI_H_
#define UI_H_

#include <stdarg.h>
#include <curses.h>

typedef struct {
    int x;
    int y;
} Vector2;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

typedef enum {
    FLAG_CURSES_CBREAK    = 0x00000002,
    FLAG_CURSES_NOECHO    = 0x00000004,
    FLAG_CURSES_KEYPAD    = 0x00000008,
    FLAG_CURSES_NODELAY   = 0x00000010,
    FLAG_CURSOR_INVISIBLE = 0x00000012,
    FLAG_CURSOR_NORMAL    = 0x00000014,
    FLAG_CURSOR_HIGH      = 0x00000016,
} CursesFlags;

void init_curses(void);
void close_curses(void);
void set_target_fps(unsigned int fps);
int curses_should_close(void);
void set_config_flags(unsigned int flags);

Vector2 get_cursor_position(void);
Vector2 get_window_size(void);

void begin_drawing(void);
void end_drawing(void);

void draw_hline(int x, int y, int width);
void draw_hline_v(Vector2 position, int width);
void draw_vline(int x, int y, int height);
void draw_vline_v(Vector2 position, int height);

void draw_rectangle(int x, int y, int width, int height);
void draw_rectangle_v(Vector2 position, Vector2 size);
void draw_rectangle_rect(Rectangle rect);

void draw_text(int x, int y, const char *fmt, ...);

#endif // UI_H_

#ifdef UI_IMPLEMENTATION

static unsigned int curses_flags = 0x00000000;
static int exit_key = 113;
static unsigned int target_fps = 0;

void init_curses(void)
{
    initscr();

    if (curses_flags & FLAG_CURSES_CBREAK) cbreak();

    if (curses_flags & FLAG_CURSES_NOECHO) noecho();

    if (curses_flags & FLAG_CURSES_KEYPAD) keypad(stdscr, TRUE);

    if (curses_flags & FLAG_CURSES_NODELAY) nodelay(stdscr, TRUE);

    if (curses_flags & FLAG_CURSOR_INVISIBLE) curs_set(0);
    else if (curses_flags & FLAG_CURSOR_NORMAL) curs_set(1);
    else if (curses_flags & FLAG_CURSOR_HIGH) curs_set(2);
}

void close_curses(void)
{
    endwin();
}

void set_target_fps(unsigned int fps)
{
    if (fps < 1) target_fps = 0;
    else target_fps = (1 / fps) * 1000;
}

int curses_should_close(void)
{
    return getch() == exit_key;
}

void set_config_flags(unsigned int flags)
{
    curses_flags |= flags;
}

Vector2 get_cursor_position(void)
{
    return (Vector2){getcurx(stdscr), getcury(stdscr)};
}

Vector2 get_window_size(void)
{
    return (Vector2){getmaxx(stdscr), getmaxy(stdscr)};
}

void begin_drawing(void)
{
    erase();
}

void end_drawing(void)
{
    refresh();
    timeout(target_fps);
}

void draw_hline(int x, int y, int width)
{
    const Vector2 last_position = get_cursor_position();

    move(y, x);
    hline(ACS_HLINE, width);

    move(last_position.y, last_position.x);
}

void draw_hline_v(Vector2 position, int width)
{
    draw_hline(position.x, position.y, width);
}

void draw_vline(int x, int y, int height)
{
    const Vector2 last_position = get_cursor_position();

    move(y, x);
    vline(ACS_VLINE, height);

    move(last_position.y, last_position.x);
}

void draw_vline_v(Vector2 position, int height)
{
    draw_vline(position.x, position.y, height);
}

void draw_rectangle(int x, int y, int width, int height)
{
    const Vector2 last_position = get_cursor_position();

    move(y, x);
    hline(ACS_HLINE, width);
    vline(ACS_VLINE, height);
    addch(ACS_ULCORNER);

    move(y + height, x);
    hline(ACS_HLINE, width);
    addch(ACS_LLCORNER);

    move(y, x + width);
    vline(ACS_VLINE, height);
    addch(ACS_URCORNER);

    move(y + height, x + width);
    addch(ACS_LRCORNER);

    move(last_position.y, last_position.x);
}

void draw_rectangle_v(Vector2 position, Vector2 size)
{
    draw_rectangle(position.x, position.y, size.x, size.y);
}

void draw_rectangle_rect(Rectangle rect)
{
    draw_rectangle(rect.x, rect.y, rect.width, rect.height);
}

void draw_text(int x, int y, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    move(y, x);
    vw_printw(stdscr, fmt, args);

    va_end(args);
}

#endif // UI_IMPLEMENTATION
