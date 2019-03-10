#ifndef GB_INTERRUPT_CONTROLLER_H_
#define GB_INTERRUPT_CONTROLLER_H_

#include <cstdint>

#include "gb_memory_map.h"
#include "gb_memory_mapped_device.h"
#include "gb_interrupt_source.h"
#include "gb_cpu.h"

class gb_interrupt_controller {
public:
    gb_interrupt_controller(gb_memory_map& memory_map, gb_cpu& cpu);
    ~gb_interrupt_controller();

    void add_interrupt_source(const gb_interrupt_source_ptr& interrupt_source);
    void update(int cycles);

private:
    class gb_interrupt_flags : public gb_memory_mapped_device {
    public:
        gb_interrupt_flags();
        virtual ~gb_interrupt_flags() override;
    };

    class gb_interrupt_enable : public gb_memory_mapped_device {
    public:
        gb_interrupt_enable();
        virtual ~gb_interrupt_enable() override;
    };

    typedef std::shared_ptr<gb_interrupt_flags>  gb_interrupt_flags_ptr;
    typedef std::shared_ptr<gb_interrupt_enable> gb_interrupt_enable_ptr;

    // The gb_interrupt_flags and gb_interrupt_enable objects contain the memory mapped
    // flags and enable registers
    gb_interrupt_flags_ptr               m_iflags;
    gb_interrupt_enable_ptr              m_ienable;

    // The interrupt controller needs access to the CPU to send it interrupt sources to handle
    gb_cpu&                              m_cpu;

    // List of interrupt sources to update every iteration
    std::vector<gb_interrupt_source_ptr> m_interrupt_sources;
};

#endif // GB_INTERRUPT_CONTROLLER_H_
