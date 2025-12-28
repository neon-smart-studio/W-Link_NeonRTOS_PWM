MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00100000
    SRAM  (rwx): ORIGIN = 0x20000000, LENGTH = 0x00040000
}

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        _text = .;
        *(.text .text.*)
        *(.gnu.linkonce.t.*)

        /* 常見的只讀常量 */
        *(.rodata .rodata.* .gnu.linkonce.r.*)

        /* C 入口/終結 */
        KEEP(*(.init))
        KEEP(*(.fini))

        /* C++ 靜態初始化與析構支援 */
        . = ALIGN(4);
        KEEP(*(.ctors))
        KEEP(*(.init_array*))
        KEEP(*(.dtors))
        KEEP(*(.fini_array*))

        /* 例外處理表（.eh_frame 可選） */
        *(.eh_frame*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    _etext = .;  /* FLASH 中可載入映像尾 */

    .data : AT(_etext)
    {
        _data  = .;
        _ldata = LOADADDR(.data);
        *(vtable)
        *(.data .data.* .gnu.linkonce.d.*)
        _edata = .;
    } > SRAM

    .bss (NOLOAD) :
    {
        _bss = .;
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        _ebss = .;
    } > SRAM
}
