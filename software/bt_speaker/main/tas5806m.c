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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "bt_app_core.h"
#include "bt_app_av.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "driver/i2s.h"


#include "driver/i2c.h"

#include "tas5806m.h"


static SemaphoreHandle_t audio_ready_mutex = NULL;
static const char TAS5806M_TAG[] = "tas5806m";



esp_err_t i2c_master_init(){
	esp_err_t ret;
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    ret = i2c_param_config(i2c_master_port, &conf);
    if(ret == ESP_OK){
    	return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    }
    return ret;
}

esp_err_t unlock_audioamp(){
	xSemaphoreGive( audio_ready_mutex );
	return ESP_OK;
}


esp_err_t tas5806m_read(uint8_t register_name, uint8_t* data_rd, size_t size){
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( TAS5806M_ADDRESS << 1 ) | READ_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_TAS5806M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t tas5806m_write_byte(uint8_t register_name, uint8_t value){
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, TAS5806M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, register_name, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, value, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_TAS5806M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}


esp_err_t audioamp_init(){
	
	esp_err_t ret;
	
	/* audio_ready_mutex */
	audio_ready_mutex = xSemaphoreCreateMutex();
	
	/* Register the PDN pin as output and write 1 to enable the TAS chip */
	gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = TAS5806M_GPIO_PDN_MASK;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	gpio_set_level(TAS5806M_GPIO_PDN, 1);

	/* initialize I2C to drive the TAS5806M */
	ret = i2c_master_init();
	if(ret != ESP_OK){
		ESP_LOGE(TAS5806M_TAG, "I2C could not be initialized.");
		return ret;
	}
	
	
	if( xSemaphoreTake( audio_ready_mutex, portMAX_DELAY ) == pdTRUE ) {
		ESP_LOGI(TAS5806M_TAG, "Locked audio_ready_mutex until I2S sound is ready");
	}
	
	return ESP_OK;
}

void audioamp_task(void *pvParameter){


	for(;;){
		
		if( xSemaphoreTake( audio_ready_mutex, portMAX_DELAY ) == pdTRUE ) {
			
			/* sound is ready */
			ESP_LOGI(TAS5806M_TAG, "I2S is ready, setup of the audio amp begin");
			vTaskDelay(1000 / portTICK_RATE_MS);
			
			/* set PDN to 1 */
			gpio_set_level(TAS5806M_GPIO_PDN, 1);
			vTaskDelay(100 / portTICK_RATE_MS);
			
			ESP_LOGI(TAS5806M_TAG, "Setting to HI Z");
			ESP_ERROR_CHECK(tas5806m_write_byte(0x03, 0x02));
			vTaskDelay(100 / portTICK_RATE_MS);
			
			ESP_LOGI(TAS5806M_TAG, "Setting to PLAY");
			ESP_ERROR_CHECK(tas5806m_write_byte(0x03, 0x03));
			
			vTaskDelay(100 / portTICK_RATE_MS);
			
			int ret;
			uint8_t h70 = 0, h71 = 0, h72 = 0;
			i2c_cmd_handle_t cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, TAS5806M_ADDRESS << 1 | WRITE_BIT, ACK_CHECK_EN);
			i2c_master_write_byte(cmd, 0x70, ACK_CHECK_EN);
			i2c_master_stop(cmd);
			ret = i2c_master_cmd_begin(I2C_TAS5806M_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
			i2c_cmd_link_delete(cmd);
			if (ret != ESP_OK) {
				ESP_LOGE(TAS5806M_TAG, "I2C ERROR");
			}
			vTaskDelay(1 / portTICK_RATE_MS);
			cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, TAS5806M_ADDRESS << 1 | READ_BIT, ACK_CHECK_EN);
			i2c_master_read_byte(cmd, &h70, ACK_VAL);
			i2c_master_read_byte(cmd, &h71, ACK_VAL);
			i2c_master_read_byte(cmd, &h72, NACK_VAL);
			i2c_master_stop(cmd);
			ret = i2c_master_cmd_begin(I2C_TAS5806M_MASTER_NUM , cmd, 1000 / portTICK_RATE_MS);
			i2c_cmd_link_delete(cmd);
			
			ESP_LOGI(TAS5806M_TAG, "0x70 Register: %d", h70);
			ESP_LOGI(TAS5806M_TAG, "0x71 Register: %d", h71);
			ESP_LOGI(TAS5806M_TAG, "0x72 Register: %d", h72);
			
			
		}
		vTaskDelay(1000 / portTICK_RATE_MS);
		

		
	}
}