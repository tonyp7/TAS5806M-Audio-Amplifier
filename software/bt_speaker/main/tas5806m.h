/*
Copyright (c) 2020 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@author Tony Pottier
@brief esp-idf compatible code to communicate with a TAS5806M audio amplifier
@see https://idyl.io
@see https://github.com/tonyp7/TAS5806M-Audio-Amplifier
*/

#ifndef TAS5806M_H_
#define TAS5806M_H_

#define I2C_MASTER_SCL_IO					17               	/*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO					16               	/*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM             			I2C_NUM_1        	/*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE  			0                	/*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 	 		0                	/*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ      	   		100000           	/*!< I2C master clock frequency */
#define WRITE_BIT                          	I2C_MASTER_WRITE	/*!< I2C master write */
#define READ_BIT                           	I2C_MASTER_READ 	/*!< I2C master read */
#define ACK_CHECK_EN                       	0x1             	/*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      	0x0              	/*!< I2C master will not check ack from slave */
#define ACK_VAL                            	0x0             	/*!< I2C ack value */
#define NACK_VAL 							0x1 				/*!< I2C nack value */

#define I2C_TAS5806M_MASTER_NUM             I2C_MASTER_NUM      /*!< I2C port number for master dev */

#define TAS5806M_ADDRESS					0x2D				/*!< 7-bit address with a 15k pull up resistor */
#define TAS5806M_GPIO_PDN    				19					/*!< GPIO used for !PDN pin */
#define TAS5806M_GPIO_PDN_MASK				((1ULL<<TAS5806M_GPIO_PDN))


#define TAS5806M_RESET_CTRL_REGISTER			0x01
#define TAS5806M_DEVICE_CTRL_1_REGISTER			0x02
#define TAS5806M_DEVICE_CTRL_2_REGISTER			0x03
#define TAS5806M_I2C_PAGE_AUTO_INC_REGISTER		0x0f
#define TAS5806M_SIG_CH_CTRL_REGISTER			0x28
#define TAS5806M_CLOCK_DET_CTRL_REGISTER		0x29
#define TAS5806M_SDOUT_SEL_REGISTER				0x30
#define TAS5806M_I2S_CTRL_REGISTER				0x31
#define TAS5806M_SAP_CTRL1_REGISTER				0x33
#define TAS5806M_SAP_CTRL2_REGISTER				0x34
#define TAS5806M_SAP_CTRL3_REGISTER				0x35
#define TAS5806M_FS_MON_REGISTER				0x37
#define TAS5806M_BCK_MON_REGISTER				0x38
#define TAS5806M_CLKDET_STATUS_REGISTER			0x39
#define TAS5806M_CHANNEL_FORCE_HIZ_REGISTER		0x40
#define TAS5806M_DIG_VOL_CTRL_REGISTER			0x4c
#define TAS5806M_DIG_VOL_CTRL2_REGISTER			0x4e
#define TAS5806M_DIG_VOL_CTRL3_REGISTER			0x4f
#define TAS5806M_AUTO_MUTE_CTRL_REGISTER		0x50
#define TAS5806M_AUTO_MUTE_TIME_REGISTER		0x51
#define TAS5806M_ANA_CTRL_REGISTER				0x53
#define TAS5806M_AGAIN_REGISTER					0x54
#define TAS5806M_BQ_WR_CTRL1_REGISTER			0x5c
#define TAS5806M_DAC_CTRL_REGISTER				0x5d
#define TAS5806M_ADR_PIN_CTRL_REGISTER			0x60
#define TAS5806M_ADR_PIN_CONFIG_REGISTER		0x61
#define TAS5806M_DSP_MISC_REGISTER				0x66
#define TAS5806M_DIE_ID_REGISTER				0x67
#define TAS5806M_POWER_STATE_REGISTER			0x68
#define TAS5806M_AUTOMUTE_STATE_REGISTER		0x69
#define TAS5806M_PHASE_CTRL_REGISTER			0x6a
#define TAS5806M_SS_CTRL0_REGISTER				0x6b
#define TAS5806M_SS_CTRL1_REGISTER				0x6c
#define TAS5806M_SS_CTRL2_REGISTER				0x6d
#define TAS5806M_SS_CTRL3_REGISTER				0x6e
#define TAS5806M_SS_CTRL4_REGISTER				0x6f
#define TAS5806M_CHAN_FAULT_REGISTER			0x70
#define TAS5806M_GLOBAL_FAULT1_REGISTER			0x71
#define TAS5806M_GLOBAL_FAULT2_REGISTER			0x72
#define TAS5806M_OT_WARNING_REGISTER			0x73
#define TAS5806M_PIN_CONTROL1_REGISTER			0x74
#define TAS5806M_PIN_CONTROL2_REGISTER			0x75
#define TAS5806M_MISC_CONTROL_REGISTER			0x76
#define TAS5806M_FAULT_CLEAR_REGISTER			0x78

esp_err_t unlock_audioamp();
esp_err_t audioamp_init();
esp_err_t i2c_master_init();
esp_err_t tas5806m_write_byte(uint8_t register_name, uint8_t value);
esp_err_t tas5806m_read(uint8_t register_name, uint8_t* data_rd, size_t size);

void audioamp_task(void *pvParameter);

#endif
