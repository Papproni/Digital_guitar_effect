/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
/* USER CODE BEGIN PFP */

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

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();
  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
  /*
  Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
  perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  /* USER CODE BEGIN 2 */
  /* Initialize all configured peripherals */
  MX_DMA_Init();
  const int numberofsubbands=100;

  float32_t input=10;
  float32_t output=1;

  float32_t subbandfilter_input[numberofsubbands];
  float32_t subbandfilter_dn[numberofsubbands];
  float32_t subbandfilter_dn1[numberofsubbands];
  float32_t subbandfilter_dn2[numberofsubbands];
  float32_t subbandfilter_y[numberofsubbands];

  float32_t subbandfilter_a1[numberofsubbands];
  float32_t subbandfilter_a2[numberofsubbands];


  float32_t subbandfilter_b0[numberofsubbands];
  float32_t subbandfilter_b1[numberofsubbands];
  float32_t subbandfilter_b2[numberofsubbands];

  float32_t subbandfilter_A1[numberofsubbands];
  float32_t subbandfilter_A2[numberofsubbands];


  float32_t subbandfilter_B0[numberofsubbands];
  float32_t subbandfilter_B1[numberofsubbands];
  float32_t subbandfilter_B2[numberofsubbands];

  float32_t output_matrix[numberofsubbands];
  float32_t pCoeffs[numberofsubbands][5];
  float32_t pState[numberofsubbands][5];
  // SubbandFilters

  arm_biquad_casd_df1_inst_f32 filterbank_subband[numberofsubbands];
  arm_biquad_cascade_df2T_instance_f32 filterbank_subband_2[numberofsubbands];
  arm_biquad_casd_df1_inst_q15 filterbank_subband_3[numberofsubbands];

  arm_matrix_instance_f32 subbandfilter_1={numberofsubbands,1,subbandfilter_a1};
  arm_matrix_instance_f32 output_matrix_f32={numberofsubbands,1,output_matrix};



  for(int i=0; i<numberofsubbands;i++){
	  arm_biquad_cascade_df1_init_f32(&(filterbank_subband[i]), 1, &(pCoeffs[i][0]), &(pState[i][0]));
  }
  for(int i=0; i<numberofsubbands;i++){
  	  arm_biquad_cascade_df2T_init_f32(&(filterbank_subband_2[i]), 1, &(pCoeffs[i][0]), &(pState[i][0]));
  }

  for(int i=0; i<numberofsubbands;i++){
	  arm_biquad_cascade_df1_init_q15(&(filterbank_subband_3[i]), 1, &(pCoeffs[i][0]), &(pState[i][0]), 0);
  }





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  volatile uint32_t myvar;
  while (1)
  {
	  myvar=HAL_GetTick();

	  for(int i=0; i<10000;i++){
			 //for(int j=0; j<numberofsubbands;j++){
			//	  arm_biquad_cascade_df1_f32(&(filterbank_subband[j]), &subbandfilter_input[j], &subbandfilter_y[j], 1);
				  //arm_biquad_cascade_df2T_f32(&(filterbank_subband_2[j]), &input, &output, 1);
				  //arm_biquad_cascade_df1_fast_q15(&(filterbank_subband_3[j]), &input, &output, 1);

			//}
		  // matrix
			  //arm_mat_scale_f32(&subbandfilter_1, input, &output_matrix_f32);

		  // vector implementation fo IIR filter
		  // d_n=input + a1*d[n-1] + a2*d[n-2]
		  // A0
		  for(int i=0;i<numberofsubbands;i++){
			  subbandfilter_input[i]=input;
			  subbandfilter_dn2[i]=subbandfilter_dn1[i];
			  subbandfilter_dn1[i]=subbandfilter_dn[i];
		  }
		  // A1
		  arm_mult_f32(subbandfilter_a1, subbandfilter_dn1, subbandfilter_A1, numberofsubbands);
		  // A2
		  arm_mult_f32(subbandfilter_a2, subbandfilter_dn2, subbandfilter_A2, numberofsubbands);

		  // A1+A2
		  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_dn, numberofsubbands);



		  // d[n]=A0+A1+A2
		  arm_add_f32(subbandfilter_dn, subbandfilter_input, subbandfilter_dn, numberofsubbands);



		  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

		  // B1
		  arm_mult_f32(subbandfilter_b1, subbandfilter_dn1, subbandfilter_B1, numberofsubbands);
		  // B2
		  arm_mult_f32(subbandfilter_b2, subbandfilter_dn2, subbandfilter_B2, numberofsubbands);
		  // B1+B2
		  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_y, numberofsubbands);

		  // B0
		  arm_mult_f32(subbandfilter_b0, subbandfilter_dn, subbandfilter_B0, numberofsubbands);



		  // y=B0+B1+B2
		  arm_add_f32(subbandfilter_y, subbandfilter_B0, subbandfilter_y, numberofsubbands);


	  }

	  myvar=HAL_GetTick()-myvar;

	  int a=1;


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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
