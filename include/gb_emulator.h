#ifndef GB_EMULATOR_H_
#define GB_EMULATOR_H_

#include <cstdint>
#include <string>
#include <vector>

#include "gb_memory_map.h"
#include "gb_cpu.h"

class gb_emulator {
public:
    gb_emulator(std::string rom_filename);

    virtual ~gb_emulator();

    virtual bool go();
    virtual void tracing(bool enable);

protected:
    typedef std::vector<gb_memory_mapped_device*> gb_device_list;

    gb_memory_map      m_memory_map;
    gb_cpu             m_cpu;
    gb_device_list     m_device_list;
    uint64_t           m_cycles;
    int                m_binsize;
};

#endif // GB_EMULATOR_H_
