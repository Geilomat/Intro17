/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

/**
 * \file
 * \brief Semaphore usage
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * Module using semaphores.
 */

#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_SEMAPHORE
#include "FRTOS1.h"
#include "Sem.h"
#include "LED.h"

static xSemaphoreHandle sem = NULL;

static void vSlaveTask(void *pvParameters) {
	for(;;) {
		xSemaphoreTake(sem, portMAX_DELAY);
		LED_Neg(1);
	}
}

static void vMasterTask(void *pvParameters) {
	for(;;) {
		xSemaphoreGive(sem);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void SEM_Deinit(void) {
}

/*! \brief Initializes module */
void SEM_Init(void) {

#if 0
	BaseType_t res;

	xTaskHandle taskHandleSlave;
	res = xTaskCreate(vSlaveTask,
		  "SemaphoreSlave",
		  configMINIMAL_STACK_SIZE + 50,
		  (void*)NULL,
		  tskIDLE_PRIORITY+1,
		  &taskHandleSlave
		 );
	if(res != pdPASS) {
	  for(;;) {} // shiit
	};

	xTaskHandle taskHandleMaster;
	res = xTaskCreate(vMasterTask,
		  "SemaphoreMaster",
		  configMINIMAL_STACK_SIZE + 50,
		  (void*)NULL,
		  tskIDLE_PRIORITY+1,
		  &taskHandleMaster
		 );
	if(res != pdPASS) {
	  for(;;) {} // shiit
	};

	sem = xSemaphoreCreateBinary();
#endif
}
#endif /* PL_CONFIG_HAS_SEMAPHORE */
