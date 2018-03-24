/*
    Copyright 2016 fishpepper <AT> gmail.com

    This program is free software: you can redistribute it and/ or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http:// www.gnu.org/licenses/>.

    author: fishpepper <AT> gmail.com
*/


#include "spi.h"
#include "debug.h"
#include "led.h"
#include "config.h"
#include "cc2500.h"



#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>

// internal functions
static void spi_init_gpio(void);
static void spi_init_mode(void);
static void spi_init_dma(void);
static void spi_init_rcc(void);


void spi_init(void) {
    debug("spi: init\n"); debug_flush();
    spi_init_rcc();
    spi_init_gpio();
    spi_init_mode();
    spi_init_dma();
    /*while(1) {
        delay_ms(10);
        // select device:
        cc2500_csn_lo();
        spi_tx(0xAB);
        // select device:
        cc2500_csn_hi();

    }*/
}

static void spi_init_rcc(void) {
    // enable clocks
    rcc_periph_clock_enable(CC2500_SPI_GPIO_RCC);
    rcc_periph_clock_enable(CC2500_SPI_CLK);
}

static void spi_init_mode(void) {
    debug("spi: init\n"); debug_flush();
    // SPI NVIC
    // nvic_set_priority(NVIC_SPI2_IRQ, 3);
    // nvic_enable_irq(NVIC_SPI2_IRQ);

    // clean start
    spi_reset(CC2500_SPI);

    // set up spi
    // - master mode
    // - baud prescaler = apb_clk/8 = 24/8 = 3MHz!
    // - CPOL low
    // - CPHA 1
    // - 8 bit crc (?)
    // - MSB first
    spi_init_master(CC2500_SPI,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_8,
                    SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_1,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);

    // set NSS to software
    // NOTE: setting NSS high is important! even when controling it on our
    //       own. otherwise spi will not send any data!
    spi_enable_software_slave_management(CC2500_SPI);
    spi_set_nss_high(CC2500_SPI);

    // Enable SPI periph
    spi_enable(CC2500_SPI);



    // set fifo to quarter full(=1 byte)
    // TODO spi_fifo_reception_threshold_8bit(CC2500_SPI);
}



static void spi_init_dma(void) {
    debug("spi: init dma\n"); debug_flush();

    // enable DMA1 Peripheral Clock
    rcc_periph_clock_enable(RCC_DMA1);

    // DMA NVIC
    nvic_set_priority(NVIC_DMA1_CHANNEL4_IRQ, NVIC_PRIO_FRSKY);
    nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, NVIC_PRIO_FRSKY);
    // nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_IRQ); // NO IRQ beeing used here

    // start with clean init for RX channel
    dma_channel_reset(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    // source and destination 1Byte = 8bit
    dma_set_memory_size(DMA1, CC2500_SPI_RX_DMA_CHANNEL, DMA_CCR_MSIZE_8BIT);
    dma_set_peripheral_size(DMA1, CC2500_SPI_RX_DMA_CHANNEL, DMA_CCR_PSIZE_8BIT);
    // automatic memory destination increment enable.
    dma_enable_memory_increment_mode(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    // source address increment disable
    dma_disable_peripheral_increment_mode(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    // Location assigned to peripheral register will be source
    dma_set_read_from_peripheral(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    // source and destination start addresses
    dma_set_peripheral_address(DMA1, CC2500_SPI_RX_DMA_CHANNEL, (uint32_t)&(SPI_DR(CC2500_SPI)));
    // target address will be set later
    dma_set_memory_address(DMA1, CC2500_SPI_RX_DMA_CHANNEL, 0);
    // chunk of data to be transfered, will be set later
    dma_set_number_of_data(DMA1, CC2500_SPI_RX_DMA_CHANNEL, 1);
    // very high prio
    dma_set_priority(DMA1, CC2500_SPI_RX_DMA_CHANNEL, DMA_CCR_PL_VERY_HIGH);

    // start with clean init for TX channel
    dma_channel_reset(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
    // source and destination 1Byte = 8bit
    dma_set_memory_size(DMA1, CC2500_SPI_TX_DMA_CHANNEL, DMA_CCR_MSIZE_8BIT);
    dma_set_peripheral_size(DMA1, CC2500_SPI_TX_DMA_CHANNEL, DMA_CCR_PSIZE_8BIT);
    // automatic memory destination increment enable.
    dma_enable_memory_increment_mode(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
    // source address increment disable
    dma_disable_peripheral_increment_mode(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
    // Location assigned to peripheral register will be target
    dma_set_read_from_memory(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
    // source and destination start addresses
    dma_set_peripheral_address(DMA1, CC2500_SPI_TX_DMA_CHANNEL, (uint32_t)&(SPI_DR(CC2500_SPI)));
    // target address will be set later
    dma_set_memory_address(DMA1, CC2500_SPI_TX_DMA_CHANNEL, 0);
    // chunk of data to be transfered, will be set later
    dma_set_number_of_data(DMA1, CC2500_SPI_TX_DMA_CHANNEL, 1);
    // very high prio
    dma_set_priority(DMA1, CC2500_SPI_TX_DMA_CHANNEL, DMA_CCR_PL_VERY_HIGH);

    // start disabled
    dma_disable_channel(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    dma_disable_channel(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
}

// data in buffer will be sent and will be overwritten with
// the data read back from the spi slave
void spi_dma_xfer(uint8_t *buffer, uint8_t len) {
    debug("spi_dma_xfer "); debug_put_uint8(len); debug(")\n");

    // TX: transfer buffer to slave
    dma_set_memory_address(DMA1, CC2500_SPI_TX_DMA_CHANNEL, (uint32_t)buffer);
    dma_set_number_of_data(DMA1, CC2500_SPI_TX_DMA_CHANNEL, len);

    // RX: read back data from slave
    dma_set_memory_address(DMA1, CC2500_SPI_RX_DMA_CHANNEL, (uint32_t)buffer);
    dma_set_number_of_data(DMA1, CC2500_SPI_RX_DMA_CHANNEL, len);

    // enable both dma channels
    dma_enable_channel(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    dma_enable_channel(DMA1, CC2500_SPI_TX_DMA_CHANNEL);

    // trigger the SPI TX + RX dma
    spi_enable_tx_dma(CC2500_SPI);
    spi_enable_rx_dma(CC2500_SPI);


    // wait for completion
    while (!(SPI_SR(CC2500_SPI) & SPI_SR_TXE)) {}
    while (SPI_SR(CC2500_SPI) & SPI_SR_BSY) {}

    // disable DMA
    dma_disable_channel(DMA1, CC2500_SPI_RX_DMA_CHANNEL);
    dma_disable_channel(DMA1, CC2500_SPI_TX_DMA_CHANNEL);
}


static void spi_init_gpio(void) {
    debug("spi: init gpio\n"); debug_flush();

    // sck och mosi as outputs.
	  gpio_set_mode(CC2500_SPI_GPIO, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, CC2500_SPI_SCK_PIN | CC2500_SPI_MOSI_PIN);

    // miso as input
	  gpio_set_mode(CC2500_SPI_GPIO, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
			CC2500_SPI_MISO_PIN);

    // configure csn
	  gpio_set_mode(CC2500_SPI_GPIO, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, CC2500_SPI_CSN_PIN);
  
    // start with csn high
    delay_us(1);
    gpio_set(CC2500_SPI_GPIO, CC2500_SPI_CSN_PIN);
}

uint8_t spi_tx(uint8_t data) {
    debug("spi_tx 0x"); debug_put_hex8(data); debug(" read: 0x");
    spi_send(CC2500_SPI, data);
    uint8_t res = spi_read(CC2500_SPI);
    debug_put_hex8(res); debug("\n"); debug_flush();
    return res;
}


uint8_t spi_rx(void) {
    debug("spi_rx res: 0x");
    spi_send(CC2500_SPI, 0xFF);
    uint8_t res = spi_read(CC2500_SPI);
    debug_put_hex8(res); debug("\n"); debug_flush();
    return res;
}
