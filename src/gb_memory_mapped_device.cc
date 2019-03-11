#include <sstream>
#include <stdexcept>

#include "gb_memory_mapped_device.h"

gb_memory_mapped_device::gb_memory_mapped_device(uint16_t start_addr, size_t size)
    : m_start_addr(start_addr), m_size(size), m_mem(size, 0)
{
}

gb_memory_mapped_device::~gb_memory_mapped_device() {
}

uint8_t* gb_memory_mapped_device::get_mem() {
    return m_mem.data();
}

gb_address_range_t gb_memory_mapped_device::get_address_range() const {
    return std::make_tuple(m_start_addr, m_size);
}

bool gb_memory_mapped_device::in_range(uint16_t addr) const {
    return ((addr >= m_start_addr) && (addr < (m_start_addr + m_size)));
}

uint16_t gb_memory_mapped_device::translate(uint16_t addr) const {
    if (!in_range(addr)) {
        std::stringstream sstr;
        sstr << "gb_memory_mapped_device::translate - " << std::hex << addr;
        throw std::out_of_range(sstr.str());
    }

    return (addr - m_start_addr);
}

uint8_t gb_memory_mapped_device::read_byte(uint16_t addr) {
    uint16_t taddr = translate(addr);

    return m_mem.at(taddr);
}

void gb_memory_mapped_device::write_byte(uint16_t addr, uint8_t val) {
    uint16_t taddr = translate(addr);

    m_mem.at(taddr) = val;
}
