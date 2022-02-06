// C-language functions to implement MMBasic commands.
#if !defined(INCLUDE_COMMAND_TABLE) && !defined(INCLUDE_TOKEN_TABLE)

    void cmd_autosave(void);
    void cmd_call(void);
    void cmd_chdir(void);
    void cmd_close(void);
    void cmd_cls(void);
    void cmd_console(void);
    void cmd_copy(void);
    void cmd_cursor(void);
    void cmd_cfunction(void);
    void cmd_dummy(void);
//    void cmd_ds18b20(void);
    void cmd_edit(void);
    void cmd_execute(void);
    void cmd_files(void);
    void cmd_font(void);
    void cmd_inc(void);
    void cmd_ireturn(void);
    void cmd_kill(void);
    void cmd_load(void);
    void cmd_longstring(void);
    void cmd_math(void);
    void cmd_memory(void);
    void cmd_mid(void);
    void cmd_mkdir(void);
//    void cmd_onewire(void);
    void cmd_open(void);
    void cmd_option(void);
    void cmd_pause(void);
//    void cmd_pin(void);
    void cmd_poke(void);
    void cmd_quit(void);
    void cmd_rename(void);
    void cmd_rmdir(void);
    void cmd_seek(void);
//    void cmd_setpin(void);
    void cmd_settick(void);
    void cmd_settitle(void);
    void cmd_sort(void);
//    void cmd_spi(void);
    void cmd_system(void);
    void cmd_timer(void);
    void cmd_xmodem(void);

// draw.c for html framebuffer.c
    void cmd_refresh(void);
    void cmd_text(void);
    void cmd_scroll(void);
    void cmd_pixel(void);
    void cmd_circle(void);
    void cmd_line(void);
    void cmd_box(void);
    void cmd_rbox(void);
    void cmd_triangle(void);    // thanks to Peter Mather (matherp on the Back Shed forum)
    void cmd_cls(void);
    void cmd_font(void);
    void cmd_colour(void);

#endif

// Entries for the command table.
#if defined(INCLUDE_COMMAND_TABLE)

    { "AutoSave",   T_CMD,              0, cmd_autosave },
    { "Box",        T_CMD,              0, cmd_box      },
    { "Call",       T_CMD,              0, cmd_call     },
    { "Chdir",      T_CMD,              0, cmd_chdir    },
    { "Circle",     T_CMD,              0, cmd_circle   },
    { "Close",      T_CMD,              0, cmd_close    },
    { "Cls",        T_CMD,              0, cmd_cls      },
    { "Color",      T_CMD,              0, cmd_colour   },
    { "Colour",     T_CMD,              0, cmd_colour   },
    { "Copy",       T_CMD,              0, cmd_copy     },
    { "Console",    T_CMD,              0, cmd_console  },
    { "CSub",       T_CMD,              0, cmd_cfunction },
//    { "Ds18b20",    T_CMD,              0, cmd_ds18b20  },
    { "End CSub",   T_CMD,              0, cmd_null     },
    { "Execute",    T_CMD,              0, cmd_execute  },
    { "Files",      T_CMD,              0, cmd_files    },
    { "Font",       T_CMD,              0, cmd_font     },
    { "Kill",       T_CMD,              0, cmd_kill     },
    { "Line",       T_CMD,              0, cmd_line     },
    { "Load",       T_CMD,              0, cmd_load     },
    { "Mid$(",      T_CMD | T_FUN,      0, cmd_mid      },
    { "Mkdir",      T_CMD,              0, cmd_mkdir    },
    { "Open",       T_CMD,              0, cmd_open     },
    { "Pixel",      T_CMD,              0, cmd_pixel    },
    { "Quit",       T_CMD,              0, cmd_quit     },
    { "Rbox",       T_CMD,              0, cmd_rbox     },
    { "Refresh",    T_CMD,              0, cmd_refresh  },
    { "Rename",     T_CMD,              0, cmd_rename   },
    { "Rmdir",      T_CMD,              0, cmd_rmdir    },
    { "Scroll",     T_CMD,              0, cmd_scroll   },
    { "Seek",       T_CMD,              0, cmd_seek     },
    { "SetTick",    T_CMD,              0, cmd_settick  },
    { "System",     T_CMD,              0, cmd_system   },
    { "Text",       T_CMD,              0, cmd_text     },
    { "Triangle",   T_CMD,              0, cmd_triangle },
    { "Option",     T_CMD,              0, cmd_option   },
    { "Cat",        T_CMD,              0, cmd_inc      }, // TODO: alias of INC
    { "Cls",        T_CMD,              0, cmd_cls      },
    { "Edit",       T_CMD,              0, cmd_edit     },
    { "Font",       T_CMD,              0, cmd_dummy    },
    { "Inc",        T_CMD,              0, cmd_inc      },
    { "IReturn",    T_CMD,              0, cmd_ireturn  },
    { "LongString", T_CMD,              0, cmd_longstring },
    { "Ls",         T_CMD,              0, cmd_files    },
    { "Pause",      T_CMD,              0, cmd_pause    },
    { "Timer",      T_CMD | T_FUN,      0, cmd_timer    },
    { "Cursor",     T_CMD,              0, cmd_cursor   },
    { "Math",       T_CMD,              0, cmd_math     },
    { "Memory",     T_CMD,              0, cmd_memory },
    { "Mode",       T_CMD,              0, cmd_dummy    },
//    { "Onewire",    T_CMD,              0, cmd_onewire  },
    { "Play",       T_CMD,              0, cmd_dummy    },
//    { "Pin",        T_CMD,              0, cmd_pin      },
    { "Poke",       T_CMD,              0, cmd_poke     },
//    { "Setpin",     T_CMD,              0, cmd_setpin   },
    { "SetTitle",   T_CMD,              0, cmd_settitle },
    { "Sort",       T_CMD,              0, cmd_sort     },
//    { "Spi",        T_CMD,              0, cmd_spi      },
    { "Text",       T_CMD,              0, cmd_dummy    },
    { "XModem",     T_CMD,              0, cmd_xmodem   },

#endif
