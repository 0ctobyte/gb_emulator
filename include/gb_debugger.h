#ifndef GB_DEBUGGER_H_
#define GB_DEBUGGER_H_

#include <unordered_map>

#include <ncurses.h>

#include "gb_emulator.h"

class ncurses_stream;

class gb_debugger : public gb_emulator {
public:
    gb_debugger();
    virtual ~gb_debugger() override;

    virtual void go() override;

private:
    typedef void (gb_debugger::*key_handler_t)();
    typedef std::unordered_map<int, key_handler_t> key_map_t;
    typedef std::unique_ptr<ncurses_stream> ncurses_stream_ptr;

    ncurses_stream_ptr           m_nstream;
    WINDOW*                      m_nwin;
    int                          m_nwin_pos;
    int                          m_nwin_max_lines;
    int                          m_nwin_lines;
    int                          m_nwin_cols;
    bool                         m_continue;
    key_map_t                    m_key_map = {
        {'n', &gb_debugger::_debugger_step_once},
        {'r', &gb_debugger::_debugger_dump_registers},
        {'u', &gb_debugger::_debugger_scroll_up_half_pg},
        {'d', &gb_debugger::_debugger_scroll_dn_half_pg},
        {'b', &gb_debugger::_debugger_scroll_up_full_pg},
        {'f', &gb_debugger::_debugger_scroll_dn_full_pg},
        {'G', &gb_debugger::_debugger_scroll_to_start},
        {'g', &gb_debugger::_debugger_scroll_to_end},
        {'c', &gb_debugger::_debugger_toggle_continue},
        {KEY_UP, &gb_debugger::_debugger_scroll_up_one_line},
        {KEY_DOWN, &gb_debugger::_debugger_scroll_dn_one_line}
    };

    void _debugger_step_once();
    void _debugger_dump_registers();
    void _debugger_scroll_up_half_pg();
    void _debugger_scroll_dn_half_pg();
    void _debugger_scroll_up_full_pg();
    void _debugger_scroll_dn_full_pg();
    void _debugger_scroll_to_start();
    void _debugger_scroll_to_end();
    void _debugger_scroll_up_one_line();
    void _debugger_scroll_dn_one_line();
    void _debugger_toggle_continue();
};

#endif // GB_DEBUGGER_H_
