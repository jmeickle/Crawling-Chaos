/**
 * @file
 * @brief Webtiles implementation of the tiles interface
**/

#ifndef TILEWEB_H
#define TILEWEB_H

#include "externs.h"
#include "tileweb-text.h"
#include "tiledoll.h"
#include "viewgeom.h"
#include "map_knowledge.h"
#include <map>
#include <sys/un.h>

class TilesFramework
{
public:
    TilesFramework();
    virtual ~TilesFramework();

    bool initialise();
    void shutdown();
    void load_dungeon(const crawl_view_buffer &vbuf, const coord_def &gc);
    void load_dungeon(const coord_def &gc);
    int getch_ck();
    void resize();
    void clrscr();

    void cgotoxy(int x, int y, GotoRegion region = GOTO_CRT);

    void update_minimap(const coord_def &gc);
    void clear_minimap();
    void update_minimap_bounds();
    void update_tabs();

    void set_need_redraw(unsigned int min_tick_delay = 0);
    bool need_redraw() const;
    void redraw();

    void place_cursor(cursor_type type, const coord_def &gc);
    void clear_text_tags(text_tag_type type);
    void add_text_tag(text_tag_type type, const std::string &tag,
                      const coord_def &gc);
    void add_text_tag(text_tag_type type, const monster* mon);

    const coord_def &get_cursor() const;

    void add_overlay(const coord_def &gc, tileidx_t idx);
    void clear_overlays();

    void draw_doll_edit();

    // Webtiles-specific
    void textcolor(int col);
    void textbackground(int col);
    void put_string(char *str);
    void put_ucs_string(ucs_t *str);
    void clear_to_end_of_line();

    void write_message(const char *format, ...);
    void finish_message();
    void send_message(const char *format, ...);

    /* Webtiles can receive input both via stdin, and on the
       socket. Also, while waiting for input, it should be
       able to handle other control messages (for example,
       requests to re-send data when a new spectator joins).

       This function waits until input is available either via
       stdin or from a control message. If the input came from
       a control message, it will be written into c; otherwise,
       it still has to be read from stdin.

       If block is false, await_input will immediately return,
       even if no input is available. The return value indicates
       whether input can be read from stdin; c will be non-zero
       if input came via a control message.
     */
    bool await_input(wint_t& c, bool block);

    void check_for_control_messages();

    /* Adds a prefix that will be written before any other
       data that is sent after this call, unless no other
       data is sent until pop_prefix is called. The suffix
       passed to pop_prefix will only be sent if the prefix
       was sent. */
    void push_prefix(std::string prefix);
    void pop_prefix(std::string suffix);
    bool prefix_popped();

    std::string m_sock_name;
    bool m_await_connection;

    void set_crt_enabled(bool value);
    bool is_crt_enabled();

protected:
    int m_sock;
    int m_max_msg_size;
    std::string m_msg_buf;
    std::vector<sockaddr_un> m_dest_addrs;

    void _await_connection();
    wint_t _handle_control_message(sockaddr_un addr, std::string data);
    wint_t _receive_control_message();

    std::vector<std::string> m_prefixes;

    enum LayerID
    {
        LAYER_NORMAL,
        LAYER_CRT,
        LAYER_TILE_CONTROL,
        LAYER_MAX,
    };
    LayerID m_active_layer;
    bool m_crt_enabled;

    unsigned int m_last_tick_redraw;
    bool m_need_redraw;

    coord_def m_origin;

    bool m_view_loaded;

    FixedArray<screen_cell_t, GXM, GYM> m_current_view;
    coord_def m_current_gc;

    FixedArray<screen_cell_t, GXM, GYM> m_next_view;
    coord_def m_next_gc;
    coord_def m_next_view_tl;
    coord_def m_next_view_br;

    int m_current_flash_colour;
    int m_next_flash_colour;

    FixedArray<map_cell, GXM, GYM> m_current_map_knowledge;
    std::map<uint32_t, coord_def> m_monster_locs;
    bool m_need_full_map;

    coord_def m_cursor[CURSOR_MAX];
    coord_def m_last_clicked_grid;

    bool m_has_overlays;

    WebTextArea m_text_crt;
    WebTextArea m_text_stat;
    WebTextArea m_text_message;

    GotoRegion m_cursor_region;

    WebTextArea *m_print_area;
    int m_print_x, m_print_y;
    int m_print_fg, m_print_bg;

    dolls_data last_player_doll;

    void _send_version();

    void _send_everything();

    void _send_map(bool force_full = false);
    void _send_cell(const coord_def &gc,
                    const screen_cell_t &current_sc, const screen_cell_t &next_sc,
                    const map_cell &current_mc, const map_cell &next_mc,
                    std::map<uint32_t, coord_def>& new_monster_locs,
                    bool force_full);
    void _send_monster(const coord_def &gc, const monster_info* m,
                       std::map<uint32_t, coord_def>& new_monster_locs,
                       bool force_full);
};

// Main interface for tiles functions
extern TilesFramework tiles;

class tiles_crt_control
{
public:
    tiles_crt_control(bool crt_enabled)
        : m_was_enabled(tiles.is_crt_enabled())
    {
        tiles.set_crt_enabled(crt_enabled);
    }

    ~tiles_crt_control()
    {
        tiles.set_crt_enabled(m_was_enabled);
    }

private:
    bool m_was_enabled;
};

#endif