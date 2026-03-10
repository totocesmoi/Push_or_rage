/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Programme principal du TP Microcontrôleurs.
  * @details        : Ce fichier contient la machine à états pour les jeux de
  * Réflexe et de Mémoire, la gestion des E/S, du PWM et de
  * la communication UART avec l'IHM PC.
  * @author         : RANDRIANARIJAONA Joyce, MUZARD Thomas
  * @date           : 10/03/2026
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
 * @enum GameState
 * @brief Enumération des différents états de la machine à états du système.
 */
typedef enum {
    STATE_MENU,          /**< État du menu principal d'attente */
    STATE_GAME_REFLEX,   /**< État du jeu de réflexe en cours */
    STATE_GAME_MEMORY,   /**< État du jeu de mémoire (Simon) en cours */
    STATE_GAME_OVER      /**< État de fin de partie */
} GameState;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/** @brief État courant de la machine à états globale */
GameState currentState = STATE_MENU;

/** @brief Buffer de réception pour les commandes UART venant du PC */
char rx_buffer[50];

/** @brief Index d'écriture courant dans le buffer de réception */
uint8_t rx_index = 0;

/** @brief Drapeau (flag) levé par l'interruption UART lorsqu'un message complet ('\n') est reçu */
volatile uint8_t command_received = 0;

/** @brief Durée de la partie en millisecondes (modifiable via l'IHM) */
uint32_t activeGameDuration = 30000;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void RunMenuMode(void);
void RunReflexGame(void);
void RunMemoryGame(void);
void RunGameOver(void);
uint32_t ReadPotentiometer(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init();
  MX_TIM21_Init();
  MX_TIM22_Init();

  /* USER CODE BEGIN 2 */
  // Activation de l'interruption de réception UART
  LL_USART_EnableIT_RXNE(USART2);

  // Activation des canaux PWM et des compteurs pour les LEDs
  LL_TIM_CC_EnableChannel(TIM21, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM21, LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableCounter(TIM21);

  LL_TIM_CC_EnableChannel(TIM22, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM22, LL_TIM_CHANNEL_CH2);
  LL_TIM_EnableCounter(TIM22);

  // Extinction initiale de toutes les LEDs
  EteindreToutesLesLeds();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  switch (currentState) {
			case STATE_MENU:
				RunMenuMode();
				break;
			case STATE_GAME_REFLEX:
				RunReflexGame();
				break;
			case STATE_GAME_MEMORY:
				RunMemoryGame();
				break;
			case STATE_GAME_OVER:
				RunGameOver();
				break;
			default:
				break;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() != 0)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {

  }

  LL_Init1msTick(16000000);

  LL_SetSystemCoreClock(16000000);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Redirection de la fonction printf vers le port série UART2.
 * @param  file Fichier de sortie (non utilisé).
 * @param  ptr  Pointeur vers la chaîne de caractères à envoyer.
 * @param  len  Longueur de la chaîne.
 * @retval int  Nombre de caractères envoyés.
 */
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
        LL_USART_TransmitData8(USART2, (uint8_t)ptr[i]);
    }
    return len;
}

/**
 * @brief Analyse la commande reçue depuis le PC (IHM) via l'UART et configure le jeu en conséquence.
 * @note  Format attendu : "CMD:START:Mode:Temps" (ex: "CMD:START:Reflexe:60").
 */
void TraiterCommandePC(void) {
    if (strncmp(rx_buffer, "CMD:START:", 10) == 0) {
        char *modeStr = rx_buffer + 10;
        char *timeStr = strchr(modeStr, ':');

        if (timeStr != NULL) {
            *timeStr = '\0';
            timeStr++;

            int dureeSecondes = atoi(timeStr);
            activeGameDuration = dureeSecondes * 1000;

            if (strncmp(modeStr, "Reflexe", 7) == 0 || strncmp(modeStr, "Classique", 9) == 0) {
                currentState = STATE_GAME_REFLEX;
            }
            else if (strncmp(modeStr, "Memoire", 7) == 0 || strncmp(modeStr, "Memory", 6) == 0) {
                currentState = STATE_GAME_MEMORY;
            }

            srand(SysTick->VAL);
        }
    }
}

/**
 * @brief Éteint toutes les LEDs en réglant les rapports cycliques (Duty Cycle) PWM à 0.
 */
void EteindreToutesLesLeds(void) {
	LL_TIM_OC_SetCompareCH1(TIM21, 0);
	LL_TIM_OC_SetCompareCH2(TIM21, 0);
	LL_TIM_OC_SetCompareCH1(TIM22, 0);
	LL_TIM_OC_SetCompareCH2(TIM22, 0);
}

/**
 * @brief  Allume une LED spécifique en ajustant sa luminosité grâce au potentiomètre.
 * @param  index Index de la LED à allumer (0: Bleu, 1: Jaune, 2: Vert, 3: Rouge).
 * @note   La valeur de la période du Timer (ARR) doit être configurée à 999 ou 1000 dans CubeIDE.
 */
void AllumerLed(int index) {
    EteindreToutesLesLeds();

    uint32_t adc_value = ReadPotentiometer();

    // Calcul de la luminosité pour un Timer de période (ARR) = 1000
	uint32_t brightness = ((adc_value * 1000) / 4095) + 50;
	if (brightness > 1000) brightness = 1000;

    switch(index) {
        case 0: LL_TIM_OC_SetCompareCH1(TIM21, brightness); break;
        case 1: LL_TIM_OC_SetCompareCH2(TIM21, brightness); break;
        case 2: LL_TIM_OC_SetCompareCH1(TIM22, brightness); break;
        case 3: LL_TIM_OC_SetCompareCH2(TIM22, brightness); break;
    }
}

/**
 * @brief  Scrutte l'état physique des 4 boutons d'entrée.
 * @retval int L'index du bouton pressé (0 à 3), ou -1 si aucun bouton n'est enfoncé.
 */
int LireBoutonAppuye(void) {
    if (LL_GPIO_IsInputPinSet(BTN_0_GPIO_Port, BTN_0_Pin) == 0) return 0;
    if (LL_GPIO_IsInputPinSet(BTN_1_GPIO_Port, BTN_1_Pin) == 0) return 1;
    if (LL_GPIO_IsInputPinSet(BTN_2_GPIO_Port, BTN_2_Pin) == 0) return 2;
    if (LL_GPIO_IsInputPinSet(BTN_3_GPIO_Port, BTN_3_Pin) == 0) return 3;
    return -1;
}

/**
 * @brief État MENU : Attente d'une interaction utilisateur (physique ou via UART).
 */
void RunMenuMode(void) {
    printf("MSG:Menu - BTN_0 (Jaune) = Reflexe | BTN_1 (Bleu) = Memoire\n");
    printf("MOD:0\n");
    EteindreToutesLesLeds();

    uint32_t random_seed = 0;

    while (currentState == STATE_MENU) {
        random_seed++;

        if (command_received == 1) {
			command_received = 0;
			TraiterCommandePC();
			if (currentState != STATE_MENU) break;
		}

        int btn = LireBoutonAppuye();

        if (btn == 0) {
            LL_mDelay(50);
            if (LireBoutonAppuye() == 0) {
                srand(random_seed);
                currentState = STATE_GAME_REFLEX;
                while(LireBoutonAppuye() != -1);
            }
        }
        else if (btn == 1) {
            LL_mDelay(50);
            if (LireBoutonAppuye() == 1) {
                srand(random_seed);
                currentState = STATE_GAME_MEMORY;
                while(LireBoutonAppuye() != -1);
            }
        }
    }
}

/**
 * @brief État JEU DE RÉFLEXE : Allume des LEDs aléatoirement et attend une réaction rapide.
 */
void RunReflexGame(void) {
    printf("MSG:Preparez-vous !\n");
    printf("MOD:1\n");
    printf("SCR:0\n");

    int currentScore = 0;
    int loopCounter = 0;
    int maxLoops = activeGameDuration / 50;

    int ledActive = -1;
    int attenteAvantAllumage = 0;

    EteindreToutesLesLeds();

    while (loopCounter < maxLoops && currentState == STATE_GAME_REFLEX) {
        if (loopCounter % 20 == 0) {
            printf("TIM:%d\n", (maxLoops - loopCounter)/20);
        }

        if (ledActive == -1) {
            if (attenteAvantAllumage == 0) {
                attenteAvantAllumage = (rand() % 10) + 5;
            }
            attenteAvantAllumage--;

            if (attenteAvantAllumage == 0) {
                ledActive = rand() % 4;
                AllumerLed(ledActive);
            }
        }
		else {
			int btnAppuye = LireBoutonAppuye();

			if (btnAppuye != -1) {
				LL_mDelay(50);
				int btnConfirme = LireBoutonAppuye();

				if (btnConfirme == btnAppuye) {
					if (btnConfirme == ledActive) {
						currentScore++;
						printf("SCR:%d\n", currentScore);
					}

					EteindreToutesLesLeds();
					ledActive = -1;

					while(LireBoutonAppuye() != -1);
				}
			}
		}

        LL_mDelay(50);
        loopCounter++;
    }

    EteindreToutesLesLeds();
    currentState = STATE_GAME_OVER;
}

/**
 * @brief État JEU DE MÉMOIRE : Affiche une séquence lumineuse que le joueur doit reproduire.
 */
void RunMemoryGame(void) {
    printf("MSG:Retenez la sequence !\n");
    printf("MOD:2\n");
    printf("SCR:0\n");

    int sequence[100];
    int currentLevel = 1;
    int isGameOver = 0;

    while (!isGameOver && currentState == STATE_GAME_MEMORY) {
        EteindreToutesLesLeds();
        LL_mDelay(1000);

        sequence[currentLevel - 1] = rand() % 4;

        printf("MSG:Observez...\n");
        for (int i = 0; i < currentLevel; i++) {
            AllumerLed(sequence[i]);
            LL_mDelay(600);
            EteindreToutesLesLeds();
            LL_mDelay(300);
        }

        printf("MSG:A vous !\n");
        int joueurIndex = 0;
        int erreurSaisie = 0;

        while (joueurIndex < currentLevel && !erreurSaisie) {
            int btn = LireBoutonAppuye();

            if (btn != -1) {
                LL_mDelay(50);
                if (LireBoutonAppuye() == btn) {

                    AllumerLed(btn);

                    if (btn == sequence[joueurIndex]) {
                        joueurIndex++;
                    } else {
                        erreurSaisie = 1;
                    }

                    while(LireBoutonAppuye() != -1);
                    EteindreToutesLesLeds();
                }
            }
        }

        if (erreurSaisie) {
            printf("MSG:Erreur !\n");
            isGameOver = 1;
            for(int k=0; k<3; k++) { AllumerLed(3); LL_mDelay(200); EteindreToutesLesLeds(); LL_mDelay(200); }
        } else {
            printf("SCR:%d\n", currentLevel);
            currentLevel++;
            for(int k=0; k<2; k++) { AllumerLed(2); LL_mDelay(150); EteindreToutesLesLeds(); LL_mDelay(150); }
        }
    }

    currentState = STATE_GAME_OVER;
}

/**
 * @brief État FIN DE PARTIE : Attente d'un signal pour retourner au menu.
 */
void RunGameOver(void) {
    printf("MSG:Partie Terminee ! BTN_0 (Jaune) pour Menu\n");

    while (currentState == STATE_GAME_OVER) {
        if (LireBoutonAppuye() == 0) {
            LL_mDelay(50);
            while(LireBoutonAppuye() != -1);
            currentState = STATE_MENU;
        }
    }
}

/**
 * @brief  Initialise, démarre et lit la valeur analogique du convertisseur ADC.
 * @retval uint32_t Valeur numérique du potentiomètre (0 à 4095).
 */
uint32_t ReadPotentiometer(void) {
    int timeout;

    if (LL_ADC_IsEnabled(ADC1) == 0) {
        LL_ADC_Enable(ADC1);

        timeout = 10000;
        while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0 && timeout > 0) {
            timeout--;
        }
    }

    LL_ADC_REG_StartConversion(ADC1);

    timeout = 10000;
    while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0 && timeout > 0) {
        timeout--;
    }

    return LL_ADC_REG_ReadConversionData12(ADC1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the LL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
