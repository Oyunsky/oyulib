#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define PDCURSES_BUILD_PATH "./build/pdcurses"

#ifdef _WIN32
    #define PDCURSES_PLATFORM "wincon"
    #define PDCURSES_MODULES pdcurses_wincon_modules
#elif __linux__
    #define PDCURSES_PLATFORM "x11"
    #define PDCURSES_MODULES pdcurses_x11_modules
#endif

static const char *pdcurses_base_modules[] = {
    "addch", "addchstr", "addstr", "attr", "beep", "bkgd", "border", "clear",
    "color", "debug", "delch", "deleteln", "getch", "getstr", "getyx", "inch",
    "inchstr", "initscr", "inopts", "insch", "insstr", "instr", "kernel",
    "keyname", "mouse", "move", "outopts", "overlay", "pad", "panel", "printw",
    "refresh", "scanw", "scr_dump", "scroll", "slk", "termattr", "touch",
    "util", "window",
};

static const char *pdcurses_wincon_modules[] = {
    "pdcclip", "pdcdisp", "pdcgetsc", "pdckbd", "pdcscrn", "pdcsetsc", "pdcutil",
};

static const char *pdcurses_x11_modules[] = {
    "pdcclip", "pdcdisp", "pdcgetsc", "pdckbd", "pdcscrn", "pdcsetsc", "pdcutil",
    "sb", "scrlbox",
};

bool build_pdcurses_modules(Cmd *cmd, Procs *procs, File_Paths *obj_files, const char *module_path, const char *modules[], size_t modules_count)
{
    const size_t thread_count = 8;

    for (size_t i = 0; i < modules_count; i++) {
        const char *input_path = temp_sprintf("pdcurses/%s/%s.c", module_path, modules[i]);
        const char *output_path = temp_sprintf("%s/%s.o", PDCURSES_BUILD_PATH, modules[i]);

        da_append(obj_files, output_path);

        if (needs_rebuild(output_path, &input_path, 1)) {
            cmd_append(cmd, "cc", "-Wall", "-O2", "-I./pdcurses", "-c", input_path, "-o", output_path);

#ifndef _WIN32
            if (strcmp(PDCURSES_PLATFORM, module_path) == 0) {
                cmd_append(cmd, "-I/usr/include/X11");
            }
#endif
            da_append(procs, cmd_run_async_and_reset(cmd));

            if (procs->count >= thread_count) {
                if (!procs_wait_and_reset(procs)) return false;
            }
        }
    }
    if (!procs_wait_and_reset(procs)) return false;
    return true;
}

bool build_pdcurses(Cmd *cmd, Procs *procs)
{
    bool result = true;

    if (!mkdir_if_not_exists(PDCURSES_BUILD_PATH));

    File_Paths obj_files = {0};

    if (!build_pdcurses_modules(cmd, procs, &obj_files, "pdcurses", pdcurses_base_modules, ARRAY_LEN(pdcurses_base_modules))) return_defer(false);
    if (!build_pdcurses_modules(cmd, procs, &obj_files, PDCURSES_PLATFORM, PDCURSES_MODULES, ARRAY_LEN(PDCURSES_MODULES))) return_defer(false);

    const char *libpdcurses_path = temp_sprintf("%s/libpdcurses.a", PDCURSES_BUILD_PATH);
    
    if (needs_rebuild(libpdcurses_path, obj_files.items, obj_files.count)) {
        cmd_append(cmd, "ar", "-crs", libpdcurses_path);

        for (size_t i = 0; i < ARRAY_LEN(pdcurses_base_modules); i++) {
            cmd_append(cmd, temp_sprintf("%s/%s.o", PDCURSES_BUILD_PATH, pdcurses_base_modules[i]));
        }
        for (size_t i = 0; i < ARRAY_LEN(PDCURSES_MODULES); i++) {
            cmd_append(cmd, temp_sprintf("%s/%s.o", PDCURSES_BUILD_PATH, PDCURSES_MODULES[i]));
        }
        if (!cmd_run_sync_and_reset(cmd)) return_defer(false);
    }

    cmd_append(cmd, "cp", "pdcurses/curses.h", PDCURSES_BUILD_PATH);
    if (!cmd_run_sync_and_reset(cmd)) return_defer(false);

defer:
    da_free(obj_files);
    return result;
}

bool build_oyulib(Cmd *cmd, Procs *procs)
{
    cmd_append(cmd, "cc", "-Wall", "-Wextra", "-I", PDCURSES_BUILD_PATH, "-o", "build/main", "src/main.c", "-L", PDCURSES_BUILD_PATH, "-lpdcurses");
#ifndef _WIN32
    cmd_append(cmd, "-lXaw", "-lXmu", "-lXt", "-lX11", "-lXpm");
#endif
    if (!cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!mkdir_if_not_exists("build")) return 1;

    int result = 0;

    Cmd cmd = {0};
    Procs procs = {0};

    if (!build_pdcurses(&cmd, &procs)) return_defer(1);
    if (!build_oyulib(&cmd, &procs)) return_defer(1);

defer:
    da_free(cmd);
    da_free(procs);
    return 0;
}
