#include <stdexcept>
#include <sstream>
#include <fstream>

#include "gb_emulator.h"
#include "gb_rom.h"
#include "gb_ram.h"
#include "gb_serial_io.h"
#include "gb_timer.h"
#include "gb_lcd.h"
#include "gb_ppu.h"

#define GB_RENDERER_WIDTH  (GB_WIDTH*5)
#define GB_RENDERER_HEIGHT (GB_HEIGHT*5)

#define GB_LCDC_ADDR       (0xFF40)

gb_emulator::gb_emulator()
    : m_renderer(GB_RENDERER_WIDTH, GB_RENDERER_HEIGHT), m_memory_manager(), m_memory_map(), m_cpu(m_memory_map), m_interrupt_controller(m_memory_manager, m_memory_map, m_cpu), m_cycles(0)
{
}

gb_emulator::~gb_emulator() {
}

void gb_emulator::load_rom(const std::string& rom_filename) {
    std::ifstream rom_file (rom_filename, std::ifstream::binary);

    if (!rom_file) {
        std::ostringstream sstr;
        sstr << "gb_emulator::load_rom() - Invalid ROM file: " << rom_filename;
        throw std::runtime_error(sstr.str());
    }

    rom_file.seekg(0, rom_file.end);
    int binsize = std::min(static_cast<int>(rom_file.tellg()), 0x10000);
    rom_file.seekg(0, rom_file.beg);

    // Load first 32KB of ROM with ROM file data
    gb_rom_ptr rom = std::make_shared<gb_rom>(m_memory_manager, 0x0000, 0x8000);
    rom_file.read(reinterpret_cast<char*>(rom->get_mem()), std::min(0x8000, binsize));

    // Add ROM as readable device but not writeable...of course
    gb_address_range_t addr_range = rom->get_address_range();
    m_memory_map.add_readable_device(rom, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add 8KB of external RAM (on the cartridge)
    gb_ram_ptr ext_ram = std::make_shared<gb_ram>(m_memory_manager, 0xA000, 0x2000);
    addr_range = ext_ram->get_address_range();
    m_memory_map.add_readable_device(ext_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(ext_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Another 8KB of work RAM (on the gameboy)
    gb_ram_ptr work_ram = std::make_shared<gb_ram>(m_memory_manager, 0xC000, 0x2000);
    addr_range = work_ram->get_address_range();
    m_memory_map.add_readable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(work_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add Serial IO device for printing to terminal
    gb_serial_io_ptr sio = std::make_shared<gb_serial_io>(m_memory_manager);
    addr_range = sio->get_address_range();
    m_memory_map.add_readable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(sio, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(sio);

    // Add Timer
    gb_timer_ptr timer = std::make_shared<gb_timer>(m_memory_manager);
    addr_range = timer->get_address_range();
    m_memory_map.add_readable_device(timer, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(timer, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(timer);

    // Add 128 bytes of high RAM (used for stack and temp variables)
    gb_ram_ptr high_ram = std::make_shared<gb_ram>(m_memory_manager, 0xFF80, 0x7F);
    addr_range = high_ram->get_address_range();
    m_memory_map.add_readable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(high_ram, std::get<0>(addr_range), std::get<1>(addr_range));

    // Add the LCD controller and it's registers
    gb_lcd_ptr lcd = std::make_shared<gb_lcd>(m_memory_manager, m_memory_map);
    addr_range = lcd->get_address_range();
    m_memory_map.add_readable_device(lcd, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(lcd, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(lcd);

    // Add the Pixel Processing Unit and it's registers
    gb_ppu_ptr ppu = std::make_shared<gb_ppu>(m_memory_manager, m_memory_map, m_renderer.get_framebuffer());
    addr_range = ppu->get_address_range();
    m_memory_map.add_readable_device(ppu, std::get<0>(addr_range), std::get<1>(addr_range));
    m_memory_map.add_writeable_device(ppu, std::get<0>(addr_range), std::get<1>(addr_range));
    m_interrupt_controller.add_interrupt_source(ppu);
}

void gb_emulator::step(const int num_cycles) {
    int step_cycles = 0;
    while (step_cycles < num_cycles) {
        int cycles = m_cpu.step();
        m_cycles += static_cast<uint64_t>(cycles);
        m_interrupt_controller.update(cycles);
        step_cycles += cycles;
    }
}

void gb_emulator::go() {
    while (m_renderer.is_open()) {
        step(70224);
        m_renderer.update(((m_memory_map.read_byte(GB_LCDC_ADDR) & 0x80) != 0));
    }
}
