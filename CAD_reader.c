/* Example SPI transfert
 *
 * This sample code is in the public domain.
 */
#include "espressif/esp_common.h"
#include "esp/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "esp8266.h"
#include <stdio.h>
#include "esp/spi.h"
#include "queue.h"
#include "ssid_config.h"
#include <string.h>
#include <lwip/api.h>

#define TELNET_PORT 23

const int CS = 0;
static volatile uint16_t CADrx;
const int freq = 200; //Ponerlo en hercios
static QueueHandle_t CADcola;

void lectura_interrupt_handler(void *arg)
{

//	uint32_t time,time_aux;
	uint16_t aux;
//	time = sdk_system_get_time();

//	gpio_write(2,1); //encendemos el pin 2 para calcular con osciloscopio cuanto tarda la interrupción

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x8310); //canal 0
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor, 0x7FFF para todo
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x8710); //canal 1
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x8B10); //canal 2
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x8F10); //canal 3
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x9310); //canal 4
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x9710); //canal 5
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x9B10); //canal 6
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

	gpio_write(CS, 0);
	aux = spi_transfer_16(1,0x9F10); //canal 7
	gpio_write(CS, 1);
	CADrx = aux & 0x7FFF; //CADrx = aux & 0xFFF esto es para dejar solo el valor
	xQueueSendToBackFromISR(CADcola, &CADrx, NULL);

//	gpio_write(2,0); //apagamos el pin 2
	
//	time_aux = sdk_system_get_time() - time;
//	printf("CAD:%u\n",CADrx);
//	printf("Time:%lu\n",(unsigned long)time_aux);

}


void envio_CAD(void *pvParameters)
{

	struct netconn *nc = netconn_new(NETCONN_TCP); //Creamos un nuevo identificador de conexion
//	if (!nc)
//    	{
//        	printf("Status monitor: Failed to allocate socket.\r\n");
//        	return;
//    	}
    	netconn_bind(nc, IP_ANY_TYPE, TELNET_PORT); //Enlazamos a traves del puerto 23, cualquier IP
    	netconn_listen(nc); //Le decimos a la conexion que se ponga en modo escucha
	struct netconn *client = NULL;
        err_t err = netconn_accept(nc, &client); //Aceptamos nueva conexion
	err_t err_c = ERR_OK;
	uint16_t CAD;
	char buf[6];

	while(1) {
		

//        	if (err != ERR_OK){
//	            	if (client)
//				netconn_delete(client);
//            			continue;
//        	}
 
		
		if (err_c == ERR_OK & err == ERR_OK){
		QueueHandle_t *CADcola = (QueueHandle_t *)pvParameters;
		if(xQueueReceive(*CADcola, &CAD, 1000)) {

			snprintf(buf, sizeof(buf), "%u", CAD);
			err_c = netconn_write(client, buf, sizeof(buf), NETCONN_COPY);

//			if (CAD >> 12 == 1) {CAD = CAD & 0xFFF; printf("Canal 1:%u\n",CAD);}
//			if (CAD >> 12 == 2) {CAD = CAD & 0xFFF; printf("Canal 2:%u\n",CAD);}
//			if (CAD >> 12 == 3) {CAD = CAD & 0xFFF; printf("Canal 3:%u\n",CAD);}
//			if (CAD >> 12 == 4) {CAD = CAD & 0xFFF; printf("Canal 4:%u\n",CAD);}
//			if (CAD >> 12 == 5) {CAD = CAD & 0xFFF; printf("Canal 5:%u\n",CAD);}
//			if (CAD >> 12 == 6) {CAD = CAD & 0xFFF; printf("Canal 6:%u\n",CAD);}
//			if (CAD >> 12 == 7) {CAD = CAD & 0xFFF; printf("Canal 7:%u\n",CAD);}
//			gpio_write(2,1);
//			char buf[80];
//      		snprintf(buf, sizeof(buf), "%u", CAD);
//	    		netconn_write(client, buf, strlen(buf), NETCONN_COPY);
//			gpio_write(2,0);
			}
		}
		else {err = netconn_accept(nc, &client);
			if (err == ERR_OK) {err_c = ERR_OK;}}
	}
}



void user_init(void)
{
	uart_set_baud(0, 115200);
	sdk_system_update_cpu_freq(SYS_CPU_160MHZ) ;
	spi_init(1, SPI_MODE0, SPI_FREQ_DIV_20M, 1, SPI_BIG_ENDIAN, true); // inicialización de SPI
	
	/*
		Bus 1 
			MISO = GPIO 12
			MOSI = GPIO 13
			SCK  = GPIO 14
			CS0  = GPIO 15 (if minimal_pins is false) (GPIO 0 para control manual)
	
		Modo 0 -> CPOL = 0, CPHA = 0, Flancos de subida de SCK y MISO/MOSI entre medias.

		Velocidad de reloj a 20 Mhz

		1 = MSB, 0 = LSB

		SPI big endian 

		Minimal_pins  = poner a true para manejar manualmente en Cs
	
	*/

    //////////////////// Envio de la configuracion inicial del SPI //////////////////////
	gpio_enable(CS, GPIO_OUTPUT);
	gpio_enable(2, GPIO_OUTPUT);
	gpio_write(2,0);
	gpio_write(CS, 1);
	sdk_os_delay_us(60000); //uint16_t us
	sdk_os_delay_us(40000);
	gpio_write(CS, 0);
	CADrx = spi_transfer_16(1, 0x8310);
	gpio_write(CS, 1);
    /////////////////////////////////////////////////////////////////////////////////////

    //////////////////// Creacion y configuración de las interrupciones /////////////////

	/* Paramos ambos timers y enmascaramos como precaución */
	timer_set_interrupts(FRC1, false);
	timer_set_run(FRC1, false);

	/* Configuramos el ISRs */
	_xt_isr_attach(INUM_TIMER_FRC1, lectura_interrupt_handler, NULL);
	
	/* Configuramos la frecuencia */
	timer_set_frequency(FRC1, freq);

	/* Desenmascaramos y echamos a andar los timers */
	timer_set_interrupts(FRC1, true);
	timer_set_run(FRC1, true);


    /////////////////////////////////////////////////////////////////////////////////////

    //////////////////// Configuración de conexión WiFi /////////////////////////////////

    	struct sdk_station_config config = {
        	.ssid = "Livebox-E202",
        	.password = "4MN78K78",
    	};

    /* required to call wifi_set_opmode before station_set_config */
    	sdk_wifi_set_opmode(STATION_MODE);
    	sdk_wifi_station_set_config(&config);
    	sdk_wifi_station_connect();

    /////////////////////////////////////////////////////////////////////////////////////

	printf("Punto 5/5 hacia la gloria");
	CADcola = xQueueCreate(20, sizeof(uint16_t));
	xTaskCreate(envio_CAD, "envio_CAD", 2048, &CADcola, 2, NULL);
}
