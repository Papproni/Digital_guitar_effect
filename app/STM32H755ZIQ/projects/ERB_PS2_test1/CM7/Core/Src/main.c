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
  This Code conatains the following effect: Polyphonic octave +1 and -1
  Filters that are used:
  -SubbandFilter: input is filtered into 43 subbands for analysis
  -RemoveDCfilter: Removes dc from 20Hz (highpass filter)
  -SuboctaveFIlter: Helps clear out upper harmonics from suboctave generation
  TODO LIST:
  TODO: Implement SubbandFilter Calculation    DONE!
  TODO: Imlement Subbandfilter Coefficient
  TODO: Set clock speed to 48khz
  TODO: Implement suboctave generation
  TODO: Implement Suboctave filtering
  TODO: Implement suboctave filter coefficient

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
ADC_HandleTypeDef hadc2;

DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch2;

OPAMP_HandleTypeDef hopamp1;

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC1_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM8_Init(void);
static void MX_ADC2_Init(void);
static void MX_OPAMP1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t ADC_buffer_is_full=0;
volatile uint8_t ADC_buffer_is_half_full=0;
volatile uint8_t DAC_buffer_is_full=0;
volatile uint8_t DAC_buffer_is_half_full=0;
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
	ADC_buffer_is_full=1;

}


void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac){
	DAC_buffer_is_full=1;
}

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac){
	DAC_buffer_is_half_full=1;
}
// DESIGN FILTERS
# define numberofsubbands 43
// SUBBAND FILTER VECTORS
volatile float32_t subbandfilter_input[numberofsubbands];
volatile float32_t subbandfilter_output[numberofsubbands];

// saved states
volatile float32_t subbandfilter_dn[numberofsubbands];
volatile float32_t subbandfilter_dn1[numberofsubbands];
volatile float32_t subbandfilter_dn2[numberofsubbands];

// filter coefficients fs=48000khz
float32_t subbandfilter_a1[numberofsubbands]={
  -1.99945743773269,-1.99935942906841,-1.99924062977670,-1.99909708277235,-1.99892410985062,-1.99871618287911,-1.99846677223317,-1.99816816848975,-1.99781127270592,-1.99738534980214,-1.99687773863020,-1.99627351120883,-1.99555507233332,-1.99470168928216,-1.99368893962368,-1.99248806313769,-1.99106520157565,-1.98938050735340,-1.98738709926732,-1.98502983991592,-1.98224390566903,-1.97895311574385,-1.97506798222950,-1.97048343779300,-1.96507619239353,-1.95870166479969,-1.95119042932896,-1.94234411345253,-1.93193067840176,-1.91967901366415,-1.90527277869813,-1.88834343336828,-1.86846241538006,-1.84513245237305,-1.81777804381746,-1.78573522094381,-1.74824080170997,-1.70442151569457,-1.65328359840612,-1.59370376851867,-1.52442293367053,-1.44404455593239,-1.35104038852965
};
float32_t subbandfilter_a2[numberofsubbands]={
  0.999623840823723,0.999592083812410,0.999557647639999,0.999520306796363,0.999479816929101,0.999435913307174,0.999388309169729,0.999336693954506,0.999280731400842,0.999220057523294,0.999154278453365,0.999082968148916,0.999005665973701,0.998921874153418,0.998831055119909,0.998732628762124,0.998625969611674,0.998510404002805,0.998385207262283,0.998249601004902,0.998102750636437,0.997943763199395,0.997771685739803,0.997585504428058,0.997384144736516,0.997166473064829,0.996931300315725,0.996677388064530,0.996403458142182,0.996108206671829,0.995790323872893,0.995448521284569,0.995081568475183,0.994688341807231,0.994267888432584,0.993819509407934,0.993342866651437,0.992838119401008,0.992306096857390,0.991748514743871,0.991168244482538,0.990569644389679,0.989958962429589
};

float32_t subbandfilter_b0[numberofsubbands]={
  0.000188079588138645,0.000203958093794915,0.000221176180000397,0.000239846601818621,0.000260091535449550,0.000282043346413158,0.000305845415135634,0.000331653022746955,0.000359634299578796,0.000389971238353096,0.000422860773317513,0.000458515925542157,0.000497167013149809,0.000539062923291039,0.000584472440045357,0.000633685618937812,0.000687015194163202,0.000744797998597725,0.000807396368858447,0.000875199497549016,0.000948624681781347,0.00102811840030234,0.00111415713009846,0.00120724778597086,0.00130792763174217,0.00141676346758574,0.00153434984213762,0.00166130596773492,0.00179827092890887,0.00194589666408543,0.00210483806355368,0.00227573935771526,0.00245921576240832,0.00265582909638453,0.00286605578370814,0.00309024529603291,0.00332856667428175,0.00358094029949610,0.00384695157130517,0.00412574262806443,0.00441587775873096,0.00471517780516052,0.00502051878520553};
float32_t subbandfilter_b1[numberofsubbands]={0.0};
float32_t subbandfilter_b2[numberofsubbands]={
  -0.000188079588138645,-0.000203958093794915,-0.000221176180000397,-0.000239846601818621,-0.000260091535449550,-0.000282043346413158,-0.000305845415135634,-0.000331653022746955,-0.000359634299578796,-0.000389971238353096,-0.000422860773317513,-0.000458515925542157,-0.000497167013149809,-0.000539062923291039,-0.000584472440045357,-0.000633685618937812,-0.000687015194163202,-0.000744797998597725,-0.000807396368858447,-0.000875199497549016,-0.000948624681781347,-0.00102811840030234,-0.00111415713009846,-0.00120724778597086,-0.00130792763174217,-0.00141676346758574,-0.00153434984213762,-0.00166130596773492,-0.00179827092890887,-0.00194589666408543,-0.00210483806355368,-0.00227573935771526,-0.00245921576240832,-0.00265582909638453,-0.00286605578370814,-0.00309024529603291,-0.00332856667428175,-0.00358094029949610,-0.00384695157130517,-0.00412574262806443,-0.00441587775873096,-0.00471517780516052,-0.00502051878520553
};


volatile float32_t subbandfilter_A1[numberofsubbands];
volatile float32_t subbandfilter_A2[numberofsubbands];

volatile float32_t subbandfilter_B0[numberofsubbands];
volatile float32_t subbandfilter_B1[numberofsubbands];
volatile float32_t subbandfilter_B2[numberofsubbands];

// SUBBAND FILTER FUNCTION - DIRECT FORM 2 - normalfunction exectime: ~6us
void subbandfilter_calculation(uint32_t input){
  float32_t input_f32=(float32_t)input-2048;
  // set d[n], d[n-1], d[n-2]
  for(int i=0;i<numberofsubbands;i++){
			  subbandfilter_input[i]=input_f32;
			  subbandfilter_dn2[i]=subbandfilter_dn1[i];
			  subbandfilter_dn1[i]=subbandfilter_dn[i];
  }
  // A1
  arm_mult_f32(subbandfilter_a1, subbandfilter_dn1, subbandfilter_A1, numberofsubbands);
  // A2
  arm_mult_f32(subbandfilter_a2, subbandfilter_dn2, subbandfilter_A2, numberofsubbands);

  // A1+A2
  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_dn, numberofsubbands);

  // d[n]=A0-A1-A2
  arm_sub_f32(subbandfilter_input,subbandfilter_dn, subbandfilter_dn, numberofsubbands);

  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

  // B1
  arm_mult_f32(subbandfilter_b1, subbandfilter_dn1, subbandfilter_B1, numberofsubbands);
  // B2
  arm_mult_f32(subbandfilter_b2, subbandfilter_dn2, subbandfilter_B2, numberofsubbands);
  // B1+B2
  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_output, numberofsubbands);

  // B0
  arm_mult_f32(subbandfilter_b0, subbandfilter_dn, subbandfilter_B0, numberofsubbands);

  // y=B0+B1+B2
  arm_add_f32(subbandfilter_output, subbandfilter_B0, subbandfilter_output, numberofsubbands);
}
// SUBBAND FILTER FUNCTION - DIRECT FORM 2 - macrofunction exectime: ~5us
//#define SUBBANDFILTER_CALCULATION(input_16t) do { \
//  float32_t input_f32 = (float32_t)input_16t; \
//  for (int i = 0; i < numberofsubbands; i++) { \
//    subbandfilter_input[i] = input_f32; \
//    subbandfilter_dn2[i] = subbandfilter_dn1[i]; \
//    subbandfilter_dn1[i] = subbandfilter_dn[i]; \
//  } \
//  arm_mult_f32(subbandfilter_a1, subbandfilter_dn1, subbandfilter_A1, numberofsubbands); \
//  arm_mult_f32(subbandfilter_a2, subbandfilter_dn2, subbandfilter_A2, numberofsubbands); \
//  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_dn, numberofsubbands); \
//  arm_add_f32(subbandfilter_dn, subbandfilter_input, subbandfilter_dn, numberofsubbands); \
//  arm_mult_f32(subbandfilter_b1, subbandfilter_dn1, subbandfilter_B1, numberofsubbands); \
//  arm_mult_f32(subbandfilter_b2, subbandfilter_dn2, subbandfilter_B2, numberofsubbands); \
//  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_output, numberofsubbands); \
//  arm_mult_f32(subbandfilter_b0, subbandfilter_dn, subbandfilter_B0, numberofsubbands); \
//  arm_add_f32(subbandfilter_output, subbandfilter_B0, subbandfilter_output, numberofsubbands); \
//} while (0)


// usefull filters
volatile float32_t subband_ones[numberofsubbands]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
volatile float32_t subband_absolute_value[numberofsubbands];
volatile float32_t octave1_up=0.0;
volatile float32_t octave1_up_filtered=0.0;
// octave Filter
arm_biquad_cascade_df2T_instance_f32 highpass_iir_50hz;
volatile float32_t highpass_coeff[5]={0.99538200, -1.99076399, 0.99538200, 1.99074267, -0.99078531};
volatile float32_t highpass_state[10];

void octave1up(){
	// get absolute values of subbands
	arm_abs_f32(subbandfilter_output, subband_absolute_value, numberofsubbands);


	// add the octave subbands together
	arm_dot_prod_f32(subband_absolute_value, subband_ones, numberofsubbands, &octave1_up);

	// filter the DC component out
	arm_biquad_cascade_df2T_f32(&highpass_iir_50hz, &octave1_up, &octave1_up_filtered, 1);
}

volatile uint16_t state=0;
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	// Megjegyzés: valószínlőűleg a peripheral clockot kell nézni időzítéskor. APBclock 75Mhz volt ezért 75Mhz/48khz=1300 ennyi kell timer8 nak
    // Read & Update The ADC Result
	uint32_t value_from_ADC = HAL_ADC_GetValue(&hadc2);


	//value_from_ADC=value_from_ADC-2048;
	//SUBBANDFILTER_CALCULATION(value_from_ADC);

	subbandfilter_calculation(value_from_ADC);
	octave1up();

	// input+octave1up
	//DAC1->DHR12R2 = (uint16_t)octave1_up_filtered+value_from_ADC+2048;
	// octave1up
	uint32_t output_test_ac=(uint32_t)octave1_up_filtered*1;
//	DAC1->DHR12R2 = output_test_ac+2048;//+value_from_ADC-2048;

	DAC1->DHR12R2 = (uint32_t)output_test_ac+2048;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC1_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_ADC2_Init();
  MX_OPAMP1_Init();
  /* USER CODE BEGIN 2 */
  uint16_t input_ADC;
  uint16_t output_DAC;
  // start DMA for ADC and DAC
	//HAL_ADC_Start_DMA(&hadc2, (uint32_t*)input_ADC, 1);
	//HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_2, (uint32_t*)output_DAC, 1, DAC_ALIGN_12B_R);

	HAL_OPAMP_Start(&hopamp1);
	// start Timers for ADC and DAC
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_Base_Start(&htim5);
	 HAL_ADC_Start_IT(&hadc2);
	  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

  uint16_t input=10;
  float32_t output=1;


  // SubbandFilters

  arm_biquad_cascade_df2T_init_f32(&highpass_iir_50hz, 1, &highpass_coeff, &highpass_state);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  volatile uint32_t myvar;
  while (1)
  {
	  // BENCHMARK START
//	  myvar=HAL_GetTick();
//
//	  for(int i=0; i<10000;i++){
//		  input=input_ADC-2048;
//		  SUBBANDFILTER_CALCULATION(input);
//		  octave1up();
//		  output_DAC=input+octave1_up_filtered+2048;
//	  }
//
//	  myvar=HAL_GetTick()-myvar;
	  // BENCHMARK END
//	  HAL_ADC_Start(&hadc2);
//	  HAL_ADC_PollForConversion(&hadc2, 1);
//	  input_ADC = HAL_ADC_GetValue(&hadc2);
//
//	  if(1){
//		  input=input_ADC-2048;
//		  SUBBANDFILTER_CALCULATION(input);
//		  octave1up();
//		  output_DAC=input+octave1_up_filtered+2048;
//		  ADC_buffer_is_full=0;
//	  }


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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV6;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief OPAMP1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OPAMP1_Init(void)
{

  /* USER CODE BEGIN OPAMP1_Init 0 */

  /* USER CODE END OPAMP1_Init 0 */

  /* USER CODE BEGIN OPAMP1_Init 1 */

  /* USER CODE END OPAMP1_Init 1 */
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMAL;
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN OPAMP1_Init 2 */

  /* USER CODE END OPAMP1_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 3333;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 2083;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

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
