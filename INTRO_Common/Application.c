/**
 * \file
 * \brief Main application file
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This provides the main application entry point.
 */

#include "Platform.h"
#include "Application.h"
#include "Event.h"
#include "LED.h"
#include "WAIT1.h"
#include "CS1.h"
#include "KeyDebounce.h"
#include "CLS1.h"
#include "KIN1.h"
#if PL_CONFIG_HAS_KEYS
  #include "Keys.h"
#endif
#if PL_CONFIG_HAS_SHELL
  #include "CLS1.h"
  #include "Shell.h"
  #include "RTT1.h"
#endif
#if PL_CONFIG_HAS_BUZZER
  #include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_RTOS
  #include "FRTOS1.h"
  #include "RTOS.h"
#endif
#if PL_CONFIG_HAS_QUADRATURE
  #include "Q4CLeft.h"
  #include "Q4CRight.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_BOARD_IS_ROBO_V2
  #include "PORT_PDD.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
  #include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LCD_MENU
  #include "LCD.h"
#endif
#if PL_CONFIG_HAS_SNAKE_GAME
  #include "Snake.h"
#endif
#if PL_CONFIG_HAS_REFLECTANCE
  #include "Reflectance.h"
#endif
#if PL_CONFIG_HAS_DRIVE
  #include "Drive.h"
#endif
#if PL_CONFIG_HAS_TURN
  #include "Turn.h"
#endif
#include "Sumo.h"

#define PROGRAM_MODE 3 				// 0 = None, 1 = Primitive sumofighter , 2 = RealSumo, 3= Line following

#if PL_CONFIG_BOARD_IS_ROBO

typedef enum {
		SETUP,
		CALIB,
		READY,
		DRIVE,
		TURN,
		STOP
}DRIVER_STATE;

#endif

static xSemaphoreHandle btn1Sem = NULL;
static xSemaphoreHandle btn1LongSem = NULL;



#if PL_CONFIG_HAS_EVENTS

static void BtnMsg(int btn, const char *msg) {
#if PL_CONFIG_HAS_SHELL
  #if PL_CONFIG_HAS_SHELL_QUEUE
    uint8_t buf[48];

    UTIL1_strcpy(buf, sizeof(buf), "Button pressed: ");
    UTIL1_strcat(buf, sizeof(buf), msg);
    UTIL1_strcat(buf, sizeof(buf), ": ");
    UTIL1_strcatNum32s(buf, sizeof(buf), btn);
    UTIL1_strcat(buf, sizeof(buf), "\r\n");
    SHELL_SendString(buf);
  #else
    CLS1_SendStr("Button ", CLS1_GetStdio()->stdOut);
    CLS1_SendStr(msg, CLS1_GetStdio()->stdOut);
    CLS1_SendStr(": ", CLS1_GetStdio()->stdOut);
    CLS1_SendNum32s(btn, CLS1_GetStdio()->stdOut);
    CLS1_SendStr("\r\n", CLS1_GetStdio()->stdOut);
  #endif
#endif
#if PL_CONFIG_HAS_BUZZER
    BUZ_PlayTune(BUZ_TUNE_BUTTON);
#endif
}

void APP_EventHandler(EVNT_Handle event) {
  /*! \todo handle events */
  switch(event) {
  case EVNT_STARTUP:
    {
      int i;
      for (i=0;i<5;i++) {
        LED1_Neg();
        WAIT1_Waitms(50);
      }
      LED1_Off();
    }
    break;
  case EVNT_LED_HEARTBEAT:
    LED2_Neg();
    break;
#if PL_CONFIG_NOF_KEYS>=1
  case EVNT_SW1_PRESSED:
	 xSemaphoreGive(btn1Sem);
     BtnMsg(1, "pressed");
     break;
  case EVNT_SW1_LPRESSED:
	 xSemaphoreGive(btn1LongSem);
     BtnMsg(1, "long pressed");
     break;
  case EVNT_SW1_RELEASED:
     BtnMsg(1, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=2
  case EVNT_SW2_PRESSED:
     BtnMsg(2, "pressed");
     break;
  case EVNT_SW2_LPRESSED:
     BtnMsg(2, "long pressed");
     break;
  case EVNT_SW2_RELEASED:
     BtnMsg(2, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=3
  case EVNT_SW3_PRESSED:
     BtnMsg(3, "pressed");
     break;
  case EVNT_SW3_LPRESSED:
     BtnMsg(3, "long pressed");
     break;
  case EVNT_SW3_RELEASED:
     BtnMsg(3, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=4
  case EVNT_SW4_PRESSED:
     BtnMsg(4, "pressed");
     break;
  case EVNT_SW4_LPRESSED:
     BtnMsg(4, "long pressed");
     break;
  case EVNT_SW4_RELEASED:
     BtnMsg(4, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=5
  case EVNT_SW5_PRESSED:
     BtnMsg(5, "pressed");
     break;
  case EVNT_SW5_LPRESSED:
     BtnMsg(5, "long pressed");
     break;
  case EVNT_SW5_RELEASED:
     BtnMsg(5, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=6
  case EVNT_SW6_PRESSED:
     BtnMsg(6, "pressed");
     break;
  case EVNT_SW6_LPRESSED:
     BtnMsg(6, "long pressed");
     break;
  case EVNT_SW6_RELEASED:
     BtnMsg(6, "released");
     break;
#endif
#if PL_CONFIG_NOF_KEYS>=7
  case EVNT_SW7_PRESSED:
     BtnMsg(7, "pressed");
     break;
  case EVNT_SW7_LPRESSED:
     BtnMsg(7, "long pressed");
     break;
  case EVNT_SW7_RELEASED:
     BtnMsg(7, "released");
     break;
#endif
    default:
      break;
   } /* switch */
}
#endif /* PL_CONFIG_HAS_EVENTS */

#if PL_CONFIG_HAS_MOTOR /* currently only used for robots */
static const KIN1_UID RoboIDs[] = {
  /* 0: L20, V2 */ {{0x00,0x03,0x00,0x00,0x67,0xCD,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 1: L21, V2 */ {{0x00,0x05,0x00,0x00,0x4E,0x45,0xB7,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 2: L4, V1  */ {{0x00,0x0B,0xFF,0xFF,0x4E,0x45,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x24}},
  /* 3: L23, V2 */ {{0x00,0x0A,0x00,0x00,0x67,0xCD,0xB8,0x21,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 4: L11, V2 */ {{0x00,0x19,0x00,0x00,0x67,0xCD,0xB9,0x11,0x4E,0x45,0x32,0x15,0x30,0x02,0x00,0x13}},
  /* 5: L5, V2 */  {{0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x25}},
  /* 6: L3, V1 */  {{0x00,0x33,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x0A}},
  /* 7: L1, V1 */  {{0x00,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x25}},
  /* 8: L6, V1 */  {{0x00,0x17,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x4E,0x45,0x27,0x99,0x10,0x02,0x00,0x06}},
};
#endif

static void APP_AdoptToHardware(void) {
  KIN1_UID id;
  uint8_t res;

  res = KIN1_UIDGet(&id);
  if (res!=ERR_OK) {
    for(;;); /* error */
  }
#if PL_CONFIG_HAS_MOTOR
  if (KIN1_UIDSame(&id, &RoboIDs[0])) { /* L20 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* invert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[1])) { /* V2 L21 */
    /* no change needed */
  } else if (KIN1_UIDSame(&id, &RoboIDs[2])) { /* V1 L4 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* revert left motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[3])) { /* L23 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert left motor */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), TRUE); /* invert left motor */
  } else if (KIN1_UIDSame(&id, &RoboIDs[4])) { /* L11 */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[5])) { /* L5, V2 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert right motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[6])) { /* L3, V1 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert right motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[7])) { /* L1, V1 */
    MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE); /* invert right motor */
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
    (void)Q4CRight_SwapPins(TRUE);
#endif
  } else if (KIN1_UIDSame(&id, &RoboIDs[8])) { /* L6, V1 */
	MOT_Invert(MOT_GetMotorHandle(MOT_MOTOR_LEFT), TRUE);
#if PL_CONFIG_HAS_QUADRATURE
    (void)Q4CLeft_SwapPins(TRUE);
#endif
  }
#endif
#if PL_CONFIG_HAS_QUADRATURE && PL_CONFIG_BOARD_IS_ROBO_V2
  /* pull-ups for Quadrature Encoder Pins */
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 10, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 10, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 11, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 11, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 16, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 16, PORT_PDD_PULL_ENABLE);
  PORT_PDD_SetPinPullSelect(PORTC_BASE_PTR, 17, PORT_PDD_PULL_UP);
  PORT_PDD_SetPinPullEnable(PORTC_BASE_PTR, 17, PORT_PDD_PULL_ENABLE);
#endif
}




static void Blinky(void* pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		LED1_Neg();
#if PL_CONFIG_BOARD_IS_ROBO
		if(REF_IsReady()){
			vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
		}
		else{
			vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
		}
#else
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
#endif
	}
}

static void EventHandler(void* pvParameters) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;) {

#if PL_CONFIG_HAS_DEBOUNCE
	KEYDBNC_Process();
#else
	KEY_Scan(); /* scan keys and set events */
#endif

	EVNT_HandleEvent(APP_EventHandler, TRUE);
	vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

#if PL_CONFIG_BOARD_IS_ROBO

static void PrimitiveFight(void* PcParameters){
	//uint16_t refValues[REF_NOF_SENSORS];
	//int counter = 0;
	DRIVER_STATE state = SETUP;
	TickType_t xLastWakeTime = xTaskGetTickCount();

#define SPEED 25

	while(!0){
		switch (state){
		case SETUP:
			if(xSemaphoreTake(btn1Sem, 0)){
				if(xSemaphoreTake(btn1LongSem, 600)) {
					if(REF_CalibrateStart()) {
						LED2_On();
						state = CALIB;
					}
				} else if(REF_IsReady()) {
					state = READY;
				} else {
					SHELL_SendString("Line Sensors not ready\n");
				}
			}
			break;

		case CALIB:
			if(xSemaphoreTake(btn1Sem, 0)) {
				if(REF_CalibrateStop()) {
					LED2_Off();
					state = SETUP;
				}
			}
			break;

		case READY:
			if(REF_GetLineKind()==  REF_LINE_FULL){
				DRV_SetSpeed(SPEED, SPEED);
				DRV_SetMode(DRV_MODE_SPEED);
				state = DRIVE;
			}
			break;


		case DRIVE:
			if(REF_GetLineKind() !=  REF_LINE_FULL){
				uint16_t refValues[REF_NOF_SENSORS];
				REF_GetSensorValues(refValues, REF_NOF_SENSORS);
				if(refValues[0] < 300) {
					// backup to the right
					TURN_Turn(TURN_STEP_BORDER_BW, NULL);
					TURN_Turn(TURN_RIGHT180, NULL);
					DRV_SetSpeed(SPEED, SPEED);
					DRV_SetMode(DRV_MODE_SPEED);
					/*
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -100);
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 10);
					*/
				} else {
					// back up to the right
					TURN_Turn(TURN_STEP_BORDER_BW, NULL);
					TURN_Turn(TURN_LEFT180, NULL);
					DRV_SetSpeed(SPEED, SPEED);
					DRV_SetMode(DRV_MODE_SPEED);
					/*
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 10);
					MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -100);
					*/
				}
				//state = TURN;
			}
			if(xSemaphoreTake(btn1Sem, 0)){
				DRV_SetMode(DRV_MODE_STOP);
				state = SETUP;
			}
			break;


		case TURN:
			if(REF_GetLineKind() == REF_LINE_FULL){
				vTaskDelay(pdMS_TO_TICKS(500));
				DRV_SetSpeed(SPEED, SPEED);
				DRV_SetMode(DRV_MODE_SPEED);
				state = DRIVE;
				}
			break;
		}

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
	}
}


static void LineFollowing(void* pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  DRIVER_STATE state = SETUP;

  for(;;) {
      //vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(250));
    switch(state)
    {
      case SETUP:
        if(xSemaphoreTake(btn1Sem, 0)){
          if(xSemaphoreTake(btn1LongSem, 600)) {
            if(REF_CalibrateStart())
              LED2_On();
            	SHELL_SendString("Calib start\n");
              state = CALIB;
          } else if(REF_IsReady()) {
            state = READY;
            SHELL_SendString("State = Ready \n");
          } else {
            SHELL_SendString("Line Sensors not ready\n");
          }
        }
        break;


      case CALIB:
        if(xSemaphoreTake(btn1Sem, 0)) {
          if(REF_CalibrateStop())
        	SHELL_SendString("Calib start\n");
            LED2_Off();
            state = SETUP;
        }
        break;

      case READY:
        if(REF_GetLineKind() != REF_LINE_NONE ){
          state = DRIVE;
          LF_StartFollowing();
        }
        break;

      case DRIVE:
        if(REF_GetLineKind() == REF_LINE_NONE){
          state = SETUP;
          LF_StopFollowing();
        }

        break;

      case TURN:
        break;


    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}
#endif


void APP_Start(void) {
  PL_Init();
  APP_AdoptToHardware();
  __asm volatile("cpsid i"); /* disable interrupts */
  __asm volatile("cpsie i"); /* enable interrupts */

  EVNT_SetEvent(EVNT_STARTUP);

  BaseType_t res;


  xTaskHandle taskHandleBlinky;
  res = xTaskCreate(Blinky,
   	  	  "Blinky",
 		  configMINIMAL_STACK_SIZE + 50,
 		  (void*)NULL,
 		  tskIDLE_PRIORITY+1,
 		  &taskHandleBlinky
 		 );
  if(res != pdPASS) {
	  for(;;) {} // shiit
  }

  xTaskHandle taskHandleEvnetHandler;
  res = xTaskCreate(EventHandler,
   	  	  "EventHandler",
 		  configMINIMAL_STACK_SIZE + 100,
 		  (void*)NULL,
 		  tskIDLE_PRIORITY+1,
 		  &taskHandleEvnetHandler
 		 );
  if(res != pdPASS) {
	  for(;;) {} // shiit
  }

  btn1Sem = xSemaphoreCreateBinary();
  if(btn1Sem == NULL){
  	  while(!0){} // uuuups !
  }

  btn1LongSem = xSemaphoreCreateBinary();
  if(btn1LongSem == NULL){
  	  while(!0){} // uuuups !
  }

#if PL_CONFIG_BOARD_IS_ROBO

#if PROGRAM_MODE == 1
  xTaskHandle taskHandlePrimitiveFight;
  res = xTaskCreate(PrimitiveFight,
   	  	  "PrimitiveFight",
 		  configMINIMAL_STACK_SIZE + 100,
 		  (void*)NULL,
 		  tskIDLE_PRIORITY+1,
 		  &taskHandlePrimitiveFight
 		 );
  if(res != pdPASS) {
	  for(;;) {} // shiit
  }
#endif

#if PROGRAM_MODE == 2

#endif

#if PROGRAM_MODE == 3

  xTaskHandle taskHandleLineFollowing;
  res = xTaskCreate(LineFollowing,
   	  	  "LineFollowing",
 		  configMINIMAL_STACK_SIZE + 100,
 		  (void*)NULL,
 		  tskIDLE_PRIORITY+1,
 		  &taskHandleLineFollowing
 		 );
  if(res != pdPASS) {
	  for(;;) {} // shiit
  }
#endif

#endif
}


