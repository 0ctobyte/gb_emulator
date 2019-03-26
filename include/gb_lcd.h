#ifndef GB_LCD_H_
#define GB_LCD_H_

#include "gb_memory_map.h"
#include "gb_interrupt_source.h"
#include "gb_renderer.h"

class gb_lcd : public gb_memory_mapped_device, public gb_interrupt_source {
public:
    gb_lcd(gb_memory_manager& memory_manager, gb_memory_map& memory_map);
    virtual ~gb_lcd() override;

    virtual void write_byte(uint16_t addr, uint8_t val) override;
    virtual bool update(int cycles) override;

private:
    class gb_lcd_stat_register : public gb_memory_mapped_device {
    public:
        gb_lcd_stat_register(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size);
        virtual ~gb_lcd_stat_register() override;

        virtual void write_byte(uint16_t addr, uint8_t val) override;
    };

    class gb_lcd_ly_register : public gb_memory_mapped_device {
    public:
        gb_lcd_ly_register(gb_memory_manager& memory_manager, uint16_t start_addr, size_t size);
        virtual ~gb_lcd_ly_register() override;

        virtual void write_byte(uint16_t addr, uint8_t val) override;

        // Writing to the LY register through write_byte will always reset it to 0
        // This routine gives a backdoor to allow the LCD controller itself to update LY
        void set_ly(uint8_t ly);
    };

    using gb_lcd_stat_register_ptr = std::shared_ptr<gb_lcd_stat_register>;
    using gb_lcd_ly_register_ptr   = std::shared_ptr<gb_lcd_ly_register>;

    // Registers used by the LCD
    // this:          LCDC - LCD Control
    // m_lcd_stat:    LCD STAT - LCD Status
    // m_lcd_ly:      LCD LY - Current scanline; LCD LYC - Scanline to compare
    gb_lcd_stat_register_ptr    m_lcd_stat;
    gb_lcd_ly_register_ptr      m_lcd_ly;

    // Scanline clock counter
    int                         m_scanline_counter;
};

using gb_lcd_ptr = std::shared_ptr<gb_lcd>;

#endif // GB_LCD_H_
