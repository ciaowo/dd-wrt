//==========================================================================
//
//      plf_misc.c
//
//      HAL platform miscellaneous functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jlarmour, dmoseley, jskov
// Date:         2001-03-20
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

#include <cyg/hal/hal_if.h>
#include <cyg/io/pci_hw.h>
#include <cyg/io/pci.h>

//--------------------------------------------------------------------------

externC void cyg_hal_init_superIO(void);
static void hal_init_irq(void);

//--------------------------------------------------------------------------

void hal_platform_init(void)
{
    HAL_WRITE_UINT32(HAL_DISPLAY_LEDBAR, 0xff);
#if defined(CYGPKG_CYGMON)
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS0, ' ');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS1, 'C');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS2, 'Y');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS3, 'G');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS4, 'M');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS5, 'O');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS6, 'N');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS7, ' ');
#elif defined(CYGPKG_REDBOOT)
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS0, 'R');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS1, 'e');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS2, 'd');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS3, 'B');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS4, 'o');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS5, 'o');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS6, 't');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS7, ' ');
#else
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS0, ' ');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS1, ' ');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS2, 'e');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS3, 'C');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS4, 'o');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS5, 's');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS6, ' ');
    HAL_WRITE_UINT32(HAL_DISPLAY_ASCIIPOS7, ' ');
#endif

    // Initialize PCI before VV since serial registers need to be in
    // place before that time.
    cyg_hal_plf_pci_init();

    // Init interrupt controller on PIIX4
    hal_init_irq();

    // Initialize super IO controller
    cyg_hal_init_superIO();

    // Set up eCos/ROM interfaces
    hal_if_init();

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();
}


/*------------------------------------------------------------------------*/
/* Reset support                                                          */

void hal_malta_reset(void)
{
    *HAL_MALTA_SOFTRES = HAL_MALTA_GORESET;
    for(;;);                            // wait for it
}


//--------------------------------------------------------------------------
// IRQ init
static void
hal_init_irq(void)
{
    cyg_uint32 v;

    // Enable SERIRQ on PIIX4
    v = cyg_hal_plf_pci_cfg_read_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,0),
                                       CYG_PCI_CFG_PIIX4_GENCFG);
    v |= CYG_PCI_CFG_PIIX4_GENCFG_SERIRQ;
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,0),
                                    CYG_PCI_CFG_PIIX4_GENCFG, v);

    // Enable SERIRQ and set to continous mode.
    v = cyg_hal_plf_pci_cfg_read_byte(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,0),
                                      CYG_PCI_CFG_PIIX4_SERIRQC);
    v |= CYG_PCI_CFG_PIIX4_SERIRQC_ENABLE | CYG_PCI_CFG_PIIX4_SERIRQC_CONT;
    cyg_hal_plf_pci_cfg_write_byte(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,0),
                                   CYG_PCI_CFG_PIIX4_SERIRQC, v);

    // Init master interrupt controller (4.2.2, pp 74-78)
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_ICW1, HAL_PIIX4_ICW1_SEL | HAL_PIIX4_ICW1_WR);
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_ICW2, CYGNUM_HAL_INTERRUPT_CTRL1_BASE - CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE);
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_ICW3, HAL_PIIX4_ICW3_CASCADE);
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_ICW4, HAL_PIIX4_ICW4_UPMODE);
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_OCW3, HAL_PIIX4_OCW3_SEL | HAL_PIIX4_OCW3_ESSM | HAL_PIIX4_OCW3_REQ);

    // Init slave interrupt controller
    HAL_WRITE_UINT8(HAL_PIIX4_SLAVE_ICW1, HAL_PIIX4_ICW1_SEL | HAL_PIIX4_ICW1_WR);
    HAL_WRITE_UINT8(HAL_PIIX4_SLAVE_ICW2, CYGNUM_HAL_INTERRUPT_CTRL2_BASE - CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE);
    HAL_WRITE_UINT8(HAL_PIIX4_SLAVE_ICW3, HAL_PIIX4_ICW3_SLAVE);
    HAL_WRITE_UINT8(HAL_PIIX4_SLAVE_ICW4, HAL_PIIX4_ICW4_UPMODE);

    // Mask all sources
    HAL_WRITE_UINT8(HAL_PIIX4_MASTER_OCW1, 
                    0xff & ~(1<<(CYGNUM_HAL_INTERRUPT_CASCADE-CYGNUM_HAL_INTERRUPT_CTRL1_BASE)));
    HAL_WRITE_UINT8(HAL_PIIX4_SLAVE_OCW1, 0xff);

    // Set PCI interrupt routing and set those interrupts to level
    // sense as per 4.1.10 page 59 in 82371AB doc.
    v = ( (CYGNUM_HAL_INTERRUPT_PCI_AB-CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE) <<  0
         |(CYGNUM_HAL_INTERRUPT_PCI_AB-CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE) <<  8
         |(CYGNUM_HAL_INTERRUPT_PCI_CD-CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE) << 16
          |(CYGNUM_HAL_INTERRUPT_PCI_CD-CYGNUM_HAL_INTERRUPT_EXTERNAL_BASE) << 24);
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,0),
                                    CYG_PCI_CFG_PIIX4_PIRQR, v);
    
    HAL_READ_UINT8(HAL_PIIX4_ELCR2, v);
    v |= (1 << (CYGNUM_HAL_INTERRUPT_PCI_AB-CYGNUM_HAL_INTERRUPT_CTRL2_BASE))
        |(1 << (CYGNUM_HAL_INTERRUPT_PCI_CD-CYGNUM_HAL_INTERRUPT_CTRL2_BASE));
    HAL_WRITE_UINT8(HAL_PIIX4_ELCR2, v);

    // Let south bridge interrupt
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_SOUTH_BRIDGE_INTR);
}

/*------------------------------------------------------------------------*/
/* PCI support                                                            */
#if defined(CYGPKG_IO_PCI)

#define PCIMEM_START	0x08000000 // PCI memory address
#define PCIMEM_SIZE	0x10000000 //   256 MByte
#define PCIIO_START	0x18000000 // PCI io address
#define PCIIO_SIZE	0x03E00000 //    62 MByte

static int __check_bar(cyg_uint32 addr, cyg_uint32 size)
{
    int n;

    for (n = 0; n <= 31; n++)
	if (size == (1 << n)) {
	    /* Check that address is naturally aligned */
	    if (addr != (addr & ~(size-1)))
		return 0;
	    return size - 1;
	}
    return 0;
}


// One-time PCI initialization.

void cyg_hal_plf_pci_init(void)
{
    cyg_uint32 bar_ena, start10, start32, end, size;
    cyg_uint8  next_bus;
    cyg_uint32 v;


    static int initialized = 0;
    if (initialized) return;
    initialized = 1;

    // Setup for bus mastering
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				    CYG_PCI_CFG_COMMAND,
				    CYG_PCI_CFG_COMMAND_IO |
				    CYG_PCI_CFG_COMMAND_MEMORY |
				    CYG_PCI_CFG_COMMAND_MASTER |
				    CYG_PCI_CFG_COMMAND_PARITY |
				    CYG_PCI_CFG_COMMAND_SERR);

    // Setup latency timer field
    cyg_hal_plf_pci_cfg_write_byte(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				   CYG_PCI_CFG_LATENCY_TIMER, 6);

    // Disable all BARs
    bar_ena = 0x1ff;

    // Check for active SCS10
    start10 = HAL_GALILEO_GETREG(HAL_GALILEO_SCS10_LD_OFFSET) << 21;
    end   = ((HAL_GALILEO_GETREG(HAL_GALILEO_SCS10_HD_OFFSET) & 0x7f) + 1) << 21;
    if (end > start10) {
	if ((size = __check_bar(start10, end - start10)) != 0) {
	    // Enable BAR
	    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_SCS10_SIZE_OFFSET, size);
	    bar_ena &= ~HAL_GALILEO_BAR_ENA_SCS10;
	}
    }

    // Check for active SCS32
    start32 = HAL_GALILEO_GETREG(HAL_GALILEO_SCS32_LD_OFFSET) << 21;
    end   = ((HAL_GALILEO_GETREG(HAL_GALILEO_SCS32_HD_OFFSET) & 0x7f) + 1) << 21;
    if (end > start32) {
	if ((size = __check_bar(start32, end - start32)) != 0) {
	    // Enable BAR
	    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_SCS32_SIZE_OFFSET, size);
	    bar_ena &= ~HAL_GALILEO_BAR_ENA_SCS32;
	}
    }

    bar_ena &= ~HAL_GALILEO_BAR_ENA_SCS10;
    HAL_GALILEO_PUTREG(HAL_GALILEO_BAR_ENA_OFFSET, bar_ena);


    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				    CYG_PCI_CFG_BAR_0, 0xffffffff);

    end = cyg_hal_plf_pci_cfg_read_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
					 CYG_PCI_CFG_BAR_0);
	
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				    CYG_PCI_CFG_BAR_0, start10);


    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				    CYG_PCI_CFG_BAR_1, 0xffffffff);

    end = cyg_hal_plf_pci_cfg_read_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
					 CYG_PCI_CFG_BAR_1);

    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(0,0),
				    CYG_PCI_CFG_BAR_1, start32);


    // enable ISA bridge on PIIX4
    v = cyg_hal_plf_pci_cfg_read_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_BRIDGE),
                                       CYG_PCI_CFG_PIIX4_GENCFG);
    v |= CYG_PCI_CFG_PIIX4_GENCFG_ISA;
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_BRIDGE),
                                    CYG_PCI_CFG_PIIX4_GENCFG, v);
        

    v = cyg_hal_plf_pci_cfg_read_byte(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_BRIDGE),
                                      CYG_PCI_CFG_PIIX4_TOM);
    v &= ~CYG_PCI_CFG_PIIX4_TOM_TOM_MASK;
    v |= CYG_PCI_CFG_PIIX4_TOM_TOM_16M;
    cyg_hal_plf_pci_cfg_write_byte(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_BRIDGE),
                                   CYG_PCI_CFG_PIIX4_TOM, v);

    cyg_pci_init();
    // Configure PCI bus.
    next_bus = 1;
    cyg_pci_configure_bus(0, &next_bus);
}


// Check for configuration error.
static int pci_config_errcheck(void)
{
    cyg_uint32  irq;

    // Check for master or target abort
    irq = HAL_GALILEO_GETREG(HAL_GALILEO_IRQ_CAUSE_OFFSET);

    if (irq & (HAL_GALILEO_IRQCAUSE_MASABT | HAL_GALILEO_IRQCAUSE_TARABT)) {
	// Error. Clear bits.
	HAL_GALILEO_PUTREG(HAL_GALILEO_IRQ_CAUSE_OFFSET,
			   ~(HAL_GALILEO_IRQCAUSE_MASABT | HAL_GALILEO_IRQCAUSE_TARABT));
        return 1;
    }
    return 0;
}

cyg_uint32 cyg_hal_plf_pci_cfg_read_dword (cyg_uint32 bus,
					   cyg_uint32 devfn,
					   cyg_uint32 offset)
{
    cyg_uint32 config_data;

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
                       (bus << 16) | (devfn << 8) | offset);

    config_data = HAL_GALILEO_GETREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET);

    if (pci_config_errcheck())
	return 0xffffffff;
    return config_data;
}

cyg_uint16 cyg_hal_plf_pci_cfg_read_word (cyg_uint32 bus,
					  cyg_uint32 devfn,
					  cyg_uint32 offset)
{
    cyg_uint32 config_dword;

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
		       (bus << 16) | (devfn << 8) | (offset & ~3));

    config_dword = HAL_GALILEO_GETREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET);

    if (pci_config_errcheck())
	return 0xffff;
    return (cyg_uint16)((config_dword >> ((offset & 3) * 8)) & 0xffff);
}

cyg_uint8 cyg_hal_plf_pci_cfg_read_byte (cyg_uint32 bus,
					 cyg_uint32 devfn,
					 cyg_uint32 offset)
{
    cyg_uint32 config_dword;

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
		       (bus << 16) | (devfn << 8) | (offset & ~3));

    config_dword = HAL_GALILEO_GETREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET);

    if (pci_config_errcheck())
	return 0xff;
    return (cyg_uint8)((config_dword >> ((offset & 3) * 8)) & 0xff);
}

void cyg_hal_plf_pci_cfg_write_dword (cyg_uint32 bus,
				      cyg_uint32 devfn,
				      cyg_uint32 offset,
				      cyg_uint32 data)
{
    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
		       (bus << 16) | (devfn << 8) | offset);

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET, data);

    (void)pci_config_errcheck();
}

void cyg_hal_plf_pci_cfg_write_word (cyg_uint32 bus,
				     cyg_uint32 devfn,
				     cyg_uint32 offset,
				     cyg_uint16 data)
{
    cyg_uint32 config_dword, shift;

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
		       (bus << 16) | (devfn << 8) | (offset & ~3));

    config_dword = HAL_GALILEO_GETREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET);
    if (pci_config_errcheck())
	return;

    shift = (offset & 3) * 8;
    config_dword &= ~(0xffff << shift);
    config_dword |= (data << shift);

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET, config_dword);

    (void)pci_config_errcheck();
}

void cyg_hal_plf_pci_cfg_write_byte (cyg_uint32 bus,
				     cyg_uint32 devfn,
				     cyg_uint32 offset,
				     cyg_uint8  data)
{
    cyg_uint32 config_dword, shift;

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_ADDR_OFFSET,
		       HAL_GALILEO_PCI0_CONFIG_ADDR_ENABLE |
		       (bus << 16) | (devfn << 8) | (offset & ~3));

    config_dword = HAL_GALILEO_GETREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET);
    if (pci_config_errcheck())
	return;

    shift = (offset & 3) * 8;
    config_dword &= ~(0xff << shift);
    config_dword |= (data << shift);

    HAL_GALILEO_PUTREG(HAL_GALILEO_PCI0_CONFIG_DATA_OFFSET, config_dword);

    (void)pci_config_errcheck();
}
#endif  // defined(CYGPKG_IO_PCI)


/*------------------------------------------------------------------------*/
/* IDE support                                                            */

int
cyg_hal_plf_ide_init(void)
{
    cyg_uint32 v;

    // enable IDE
    v = cyg_hal_plf_pci_cfg_read_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_IDE),
                                       CYG_PCI_CFG_PIIX4_IDETIM);
    v |= (CYG_PCI_CFG_PIIX4_IDETIM_IDE << 16) | CYG_PCI_CFG_PIIX4_IDETIM_IDE;
    cyg_hal_plf_pci_cfg_write_dword(0, CYG_PCI_DEV_MAKE_DEVFN(_PIIX4_PCI_ID,_PIIX4_IDE),
                                    CYG_PCI_CFG_PIIX4_IDETIM, v);
    return HAL_IDE_NUM_CONTROLLERS;
}


/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
