/* 
 * Copyright (c) 2019 Sekhar Bhattacharya
 *
 * SPDS-License-Identifier: MIT
 */

#include "gb_logger.h"
#include "gb_emulator_opts.h"
#include "gb_debugger.h"

int main(int argc, char **argv) {
    gb_emulator_opts options (argc, argv);

    if (!options.parse_opts()) return EXIT_FAILURE;

    gb_logger::instance().enable_tracing(options.m_tracing);
    gb_logger::instance().set_level(GB_LOG_DEBUG);

    gb_emulator emulator;

    try {
        emulator.load_rom(options.m_rom_filename);
    } catch (const std::exception& e) {
        GB_LOGGER(GB_LOG_FATAL) << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (options.m_debugger) {
        gb_debugger debugger (emulator);

        // Need to setup stream with gb_logger again since gb_emulator_debugger modifies the
        // the reference of the underlying streambuf
        gb_logger::instance().set_stream(std::cout);
        gb_logger::instance().enable_tracing(true);

        debugger.go();
    } else {
        emulator.go();
    }

    return EXIT_SUCCESS;
}
