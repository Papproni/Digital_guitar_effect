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
#include "stm32h7xx_hal_crc.h"
#include "stm32h7xx.h"
#include "arm_math.h"

#include "AD1939_driver.h"
#include "guitar_effect_delay.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#ifndef HSEM_ID_0
//#define HSEM_ID_0 (0U) /* HW semaphore 0*/
//#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2S_HandleTypeDef hi2s1;
I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi2_tx;

SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S2_Init(void);
static void MX_SPI3_Init(void);
static void MX_I2S1_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


volatile int BufSize = 6000;
volatile int Overlap = 3000;

volatile int Buf[10000];

volatile int WtrP;
volatile float Rd_P;
volatile float CrossFade;


int Do_PitchShift(int sample) {
	static float Shift = 0.5;
	int sum = sample;
	//sum up and do high-pass


	//write to ringbuffer
	Buf[WtrP] = sum;

	//read fractional readpointer and generate 0° and 180° read-pointer in integer
	int RdPtr_Int = roundf(Rd_P);
	int RdPtr_Int2 = 0;
	if (RdPtr_Int >= BufSize/2) RdPtr_Int2 = RdPtr_Int - (BufSize/2);
	else RdPtr_Int2 = RdPtr_Int + (BufSize/2);

	//read the two samples...
	float Rd0 = (float) Buf[RdPtr_Int];
	float Rd1 = (float) Buf[RdPtr_Int2];

	//Check if first readpointer starts overlap with write pointer?
	// if yes -> do cross-fade to second read-pointer
	if (Overlap >= (WtrP-RdPtr_Int) && (WtrP-RdPtr_Int) >= 0 && Shift!=1.0f) {
		int rel = WtrP-RdPtr_Int;
		CrossFade = ((float)rel)/(float)Overlap;
	}
	else if (WtrP-RdPtr_Int == 0) CrossFade = 0.0f;

	//Check if second readpointer starts overlap with write pointer?
	// if yes -> do cross-fade to first read-pointer
	if (Overlap >= (WtrP-RdPtr_Int2) && (WtrP-RdPtr_Int2) >= 0 && Shift!=1.0f) {
			int rel = WtrP-RdPtr_Int2;
			CrossFade = 1.0f - ((float)rel)/(float)Overlap;
		}
	else if (WtrP-RdPtr_Int2 == 0) CrossFade = 1.0f;


	//do cross-fading and sum up
	sum = (Rd0*CrossFade + Rd1*(1.0f-CrossFade));

	//increment fractional read-pointer and write-pointer
	Rd_P += Shift;
	WtrP++;
	if (WtrP == BufSize) WtrP = 0;
	if (roundf(Rd_P) >= BufSize) Rd_P = 0.0f;

	return sum;
}

volatile uint16_t my_data[8];// __ALIGNED(32) __attribute__((at(0x24000000)));
volatile uint16_t rx_data_i2s[8];// __ALIGNED(32) __attribute__((at(0x24000000)));

//uint8_t uartRxBuf[UART_RX_STR_SIZE] __ALIGNED(32) __attribute__((section('.ram1')));
// DESIGN FILTERS

#define numberofsubbands 55

// SUBBAND FILTER VECTORS
volatile float32_t subbandfilter_input[numberofsubbands];
volatile float32_t subbandfilter_output[numberofsubbands];

// saved states
// x[n] , X[n-1], X[n-2]
volatile float32_t subbandfilter_dn[numberofsubbands];
volatile float32_t subbandfilter_dn1[numberofsubbands];
volatile float32_t subbandfilter_dn2[numberofsubbands];
// Y[n-1] , Y[n-2]
volatile float32_t subbandfilter_yn1[numberofsubbands];
volatile float32_t subbandfilter_yn2[numberofsubbands];

// x[n] , X[n-1], X[n-2]
volatile float32_t subbandfilter_octave2_dn[numberofsubbands];
volatile float32_t subbandfilter_octave2_dn1[numberofsubbands];
volatile float32_t subbandfilter_octave2_dn2[numberofsubbands];
// Y[n-1] , Y[n-2]
volatile float32_t subbandfilter_octave2_yn1[numberofsubbands];
volatile float32_t subbandfilter_octave2_yn2[numberofsubbands];


// FILTER values are from audiotest4_ERB_PS2_naive2 matlab script
// filter coefficients fs=48000khz
float32_t subbandfilter_a1[numberofsubbands]={
		-1.99945743773269,
		-1.99935942906841,
		-1.99924062977670,
		-1.99909708277235,
		-1.99892410985062,
		-1.99871618287911,
		-1.99846677223317,
		-1.99816816848975,
		-1.99781127270592,
		-1.99738534980214,
		-1.99687773863020,
		-1.99627351120883,
		-1.99555507233332,
		-1.99470168928216,
		-1.99368893962368,
		-1.99248806313769,
		-1.99106520157565,
		-1.98938050735340,
		-1.98738709926732,
		-1.98502983991592,
		-1.98224390566903,
		-1.97895311574385,
		-1.97506798222950,
		-1.97048343779300,
		-1.96507619239353,
		-1.95870166479969,
		-1.95119042932896,
		-1.94234411345253,
		-1.93193067840176,
		-1.91967901366415,
		-1.90527277869813,
		-1.88834343336828,
		-1.86846241538006,
		-1.84513245237305,
		-1.81777804381746,
		-1.78573522094381,
		-1.74824080170997,
		-1.70442151569457,
		-1.65328359840612,
		-1.59370376851867,
		-1.52442293367053,
		-1.44404455593239,
		-1.35104038852965,
		-1.24376731735276,
		-1.12050035106663,
		-0.979488440212929,
		-0.819041780213102,
		-0.637661520103793,
		-0.434225203748800,
		-0.208243466153559,
		0.0397951757780011,
		0.307975350044821,
		0.592390391796649,
		0.886463172281052,
		1.18017689199105};
float32_t subbandfilter_a2[numberofsubbands]={
		0.999623840823723,
		0.999592083812410,
		0.999557647639999,
		0.999520306796363,
		0.999479816929101,
		0.999435913307174,
		0.999388309169729,
		0.999336693954506,
		0.999280731400842,
		0.999220057523294,
		0.999154278453365,
		0.999082968148916,
		0.999005665973701,
		0.998921874153418,
		0.998831055119909,
		0.998732628762124,
		0.998625969611674,
		0.998510404002805,
		0.998385207262283,
		0.998249601004902,
		0.998102750636437,
		0.997943763199395,
		0.997771685739803,
		0.997585504428058,
		0.997384144736516,
		0.997166473064829,
		0.996931300315725,
		0.996677388064530,
		0.996403458142182,
		0.996108206671829,
		0.995790323872893,
		0.995448521284569,
		0.995081568475183,
		0.994688341807231,
		0.994267888432584,
		0.993819509407934,
		0.993342866651437,
		0.992838119401008,
		0.992306096857390,
		0.991748514743871,
		0.991168244482538,
		0.990569644389679,
		0.989958962429589,
		0.989344819163852,
		0.988738776872602,
		0.988155995338887,
		0.987615964937990,
		0.987143291300058,
		0.986768479981635,
		0.986528630464629,
		0.986467891703625,
		0.986637451137907,
		0.987094720833566,
		0.987901246292733,
		0.989118701024236};

float32_t subbandfilter_b0[numberofsubbands]={
		0.000188079588138645,
		0.000203958093794915,
		0.000221176180000397,
		0.000239846601818621,
		0.000260091535449550,
		0.000282043346413158,
		0.000305845415135634,
		0.000331653022746955,
		0.000359634299578796,
		0.000389971238353096,
		0.000422860773317513,
		0.000458515925542157,
		0.000497167013149809,
		0.000539062923291039,
		0.000584472440045357,
		0.000633685618937812,
		0.000687015194163202,
		0.000744797998597725,
		0.000807396368858447,
		0.000875199497549016,
		0.000948624681781347,
		0.00102811840030234,
		0.00111415713009846,
		0.00120724778597086,
		0.00130792763174217,
		0.00141676346758574,
		0.00153434984213762,
		0.00166130596773492,
		0.00179827092890887,
		0.00194589666408543,
		0.00210483806355368,
		0.00227573935771526,
		0.00245921576240832,
		0.00265582909638453,
		0.00286605578370814,
		0.00309024529603291,
		0.00332856667428175,
		0.00358094029949610,
		0.00384695157130517,
		0.00412574262806443,
		0.00441587775873096,
		0.00471517780516052,
		0.00502051878520553,
		0.00532759041807397,
		0.00563061156369926,
		0.00592200233055667,
		0.00619201753100491,
		0.00642835434997096,
		0.00661576000918263,
		0.00673568476768567,
		0.00676605414818766,
		0.00668127443104657,
		0.00645263958321689,
		0.00604937685363332,
		0.00544064948788215
};

float32_t subbandfilter_b1[numberofsubbands]={0.0};
float32_t subbandfilter_b2[numberofsubbands]={
		-0.000188079588138645,
		-0.000203958093794915,
		-0.000221176180000397,
		-0.000239846601818621,
		-0.000260091535449550,
		-0.000282043346413158,
		-0.000305845415135634,
		-0.000331653022746955,
		-0.000359634299578796,
		-0.000389971238353096,
		-0.000422860773317513,
		-0.000458515925542157,
		-0.000497167013149809,
		-0.000539062923291039,
		-0.000584472440045357,
		-0.000633685618937812,
		-0.000687015194163202,
		-0.000744797998597725,
		-0.000807396368858447,
		-0.000875199497549016,
		-0.000948624681781347,
		-0.00102811840030234,
		-0.00111415713009846,
		-0.00120724778597086,
		-0.00130792763174217,
		-0.00141676346758574,
		-0.00153434984213762,
		-0.00166130596773492,
		-0.00179827092890887,
		-0.00194589666408543,
		-0.00210483806355368,
		-0.00227573935771526,
		-0.00245921576240832,
		-0.00265582909638453,
		-0.00286605578370814,
		-0.00309024529603291,
		-0.00332856667428175,
		-0.00358094029949610,
		-0.00384695157130517,
		-0.00412574262806443,
		-0.00441587775873096,
		-0.00471517780516052,
		-0.00502051878520553,
		-0.00532759041807397,
		-0.00563061156369926,
		-0.00592200233055667,
		-0.00619201753100491,
		-0.00642835434997096,
		-0.00661576000918263,
		-0.00673568476768567,
		-0.00676605414818766,
		-0.00668127443104657,
		-0.00645263958321689,
		-0.00604937685363332,
		-0.00544064948788215
};


volatile float32_t subbandfilter_A1[numberofsubbands];
volatile float32_t subbandfilter_A2[numberofsubbands];
volatile float32_t subbandfilter_A[numberofsubbands];

volatile float32_t subbandfilter_B0[numberofsubbands];
volatile float32_t subbandfilter_B1[numberofsubbands];
volatile float32_t subbandfilter_B2[numberofsubbands];
volatile float32_t subbandfilter_B[numberofsubbands];
// SUBBAND FILTER FUNCTION - DIRECT FORM 2 - normalfunction exectime: ~6us
void subbandfilter_calculation(int32_t input){
  float32_t input_f32=(float32_t)input;
  // set d[n], d[n-1], d[n-2]
  for(int i=0;i<numberofsubbands;i++){
//			  subbandfilter_input[i]	= input_f32;
			  subbandfilter_dn2[i]		= subbandfilter_dn1[i];
			  subbandfilter_dn1[i]		= subbandfilter_dn[i];
			  subbandfilter_dn[i]		= input_f32;

  }
  // A1 = a1*y[n-1]
  arm_mult_f32(subbandfilter_a1, subbandfilter_yn1, subbandfilter_A1, numberofsubbands);
  // A2 = a2*y[n-2]
  arm_mult_f32(subbandfilter_a2, subbandfilter_yn2, subbandfilter_A2, numberofsubbands);

  // A = A1+A2
  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_A, numberofsubbands);

  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

  // B1 = b1*x[n-1]
  arm_mult_f32(subbandfilter_b1, subbandfilter_dn1, subbandfilter_B1, numberofsubbands);
  // B2 = b2*x[n-2]
  arm_mult_f32(subbandfilter_b2, subbandfilter_dn2, subbandfilter_B2, numberofsubbands);
  // B1+B2
  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_B, numberofsubbands);

  // B0 = b0*x[n]
  arm_mult_f32(subbandfilter_b0, subbandfilter_dn, subbandfilter_B0, numberofsubbands);

  // y=B0+B1+B2
  arm_add_f32(subbandfilter_B, subbandfilter_B0, subbandfilter_B, numberofsubbands);

  // y[n]= B - A
  arm_sub_f32(subbandfilter_B,subbandfilter_A, subbandfilter_output, numberofsubbands);
  for(int i=0;i<numberofsubbands;i++){
	  subbandfilter_yn2[i]		= subbandfilter_yn1[i];
	  subbandfilter_yn1[i]		= subbandfilter_output[i];
   }
}

void subbandfilter_octave2_calculation(int32_t input){
  float32_t input_f32=(float32_t)input;
  // set d[n], d[n-1], d[n-2]
  for(int i=0;i<numberofsubbands;i++){
			  subbandfilter_octave2_dn2[i]=subbandfilter_octave2_dn1[i];
			  subbandfilter_octave2_dn1[i]=subbandfilter_octave2_dn[i];
			  subbandfilter_octave2_dn[i] = input_f32;
  }
  // A1 = a1*y[n-1]
  arm_mult_f32(subbandfilter_a1, subbandfilter_octave2_yn1, subbandfilter_A1, numberofsubbands);
  // A2 = a2*y[n-2]
  arm_mult_f32(subbandfilter_a2, subbandfilter_octave2_yn2, subbandfilter_A2, numberofsubbands);

  // A = A1+A2
  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_A, numberofsubbands);

  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

  // B1 = b1*x[n-1]
  arm_mult_f32(subbandfilter_b1, subbandfilter_octave2_dn1, subbandfilter_B1, numberofsubbands);
  // B2 = b2*x[n-2]
  arm_mult_f32(subbandfilter_b2, subbandfilter_octave2_dn2, subbandfilter_B2, numberofsubbands);
  // B1+B2
  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_B, numberofsubbands);

  // B0 = b0*x[n]
  arm_mult_f32(subbandfilter_b0, subbandfilter_octave2_dn, subbandfilter_B0, numberofsubbands);

  // y=B0+B1+B2
  arm_add_f32(subbandfilter_B, subbandfilter_B0, subbandfilter_B, numberofsubbands);

  // y[n]= B - A
  arm_sub_f32(subbandfilter_B,subbandfilter_A, subbandfilter_output, numberofsubbands);
  for(int i=0;i<numberofsubbands;i++){
 			  subbandfilter_octave2_yn2[i] 	= subbandfilter_octave2_yn1[i];
 			  subbandfilter_octave2_yn1[i]  = subbandfilter_output[i];
   }
}

// usefull filters
volatile float32_t subband_ones[numberofsubbands];
volatile float32_t subband_absolute_value[numberofsubbands];
volatile float32_t octave1_up=0.0;
volatile float32_t octave1_up_filtered=0.0;
// octave Filter
arm_biquad_cascade_df2T_instance_f32 highpass_iir_50hz;
arm_biquad_cascade_df2T_instance_f32 highpass_iir_50hz_octave2;
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

void octave2up(){
	// get absolute values of subbands
	arm_abs_f32(subbandfilter_output, subband_absolute_value, numberofsubbands);


	// add the octave subbands together
	arm_dot_prod_f32(subband_absolute_value, subband_ones, numberofsubbands, &octave1_up);

	// filter the DC component out
	arm_biquad_cascade_df2T_f32(&highpass_iir_50hz_octave2, &octave1_up, &octave1_up_filtered, 1);
}

typedef union
{
	struct{
		int16_t raw_low;
		int16_t raw_high;
	};
	int32_t value;
}adc_data_bitfield;

volatile int32_t output_test_ac;
volatile adc_data_bitfield adc_data_bf;
volatile adc_data_bitfield output_buffer;
volatile uint8_t ADC_READY_FLAG = 0;
volatile uint8_t DAC_HALF_COMPLETE_FLAG = 0;
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
//	SCB_InvalidateDCache();
	SCB_InvalidateDCache_by_Addr(rx_data_i2s, sizeof(rx_data_i2s));
	ADC_READY_FLAG = 1;
//	//	adc_data_bitfield adc_data_bf;
	adc_data_bf.raw_low 	= rx_data_i2s[0];
	adc_data_bf.raw_high 	= rx_data_i2s[1];
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s){
	SCB_InvalidateDCache_by_Addr(rx_data_i2s, sizeof(rx_data_i2s));
//	SCB_InvalidateDCache();
	ADC_READY_FLAG = 1;
	//	adc_data_bitfield adc_data_bf;
	adc_data_bf.raw_low 	= rx_data_i2s[4];
	adc_data_bf.raw_high 	= rx_data_i2s[5];
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
	DAC_HALF_COMPLETE_FLAG = 1;
//	int32_t out2 = output_buffer.raw_low;
//	int32_t out3 = output_buffer.raw_high;
	SCB_CleanDCache_by_Addr(my_data, sizeof(my_data));
//	my_data[2] = out2;
//	my_data[3] = out3;
}
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){
	DAC_HALF_COMPLETE_FLAG = 0;
//	int32_t out2 = output_buffer.raw_low;
//	int32_t out3 = output_buffer.raw_high;
	SCB_CleanDCache_by_Addr(my_data, sizeof(my_data));
//	my_data[6] = out2;
//	my_data[7] = out3;
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

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
//  timeout = 0xFFFF;
//  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
//  if ( timeout < 0 )
//  {
//  Error_Handler();
//  }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  __HAL_RCC_SYSCFG_CLK_ENABLE();


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
//__HAL_RCC_HSEM_CLK_ENABLE();
///*Take HSEM */
//HAL_HSEM_FastTake(HSEM_ID_0);
///*Release HSEM in order to notify the CPU2(CM4)*/
//HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
	//timeout = 0xFFFF;
	//while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
	//if ( timeout < 0 )
	//{
	//Error_Handler();
	//}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  __HAL_RCC_SPI1_CLK_ENABLE();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S2_Init();
  MX_SPI3_Init();
  MX_I2S1_Init();
  /* USER CODE BEGIN 2 */



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


//	uint8_t AD1939_GLOBAL_ADDRESS = 0x08;
//	uint8_t AD1939_RW_BIT 		= 0; // 1 READ 0 WRITE
//	uint8_t AD1939_REG_ADDRESS 	= 0;// 0 CLOCK CONTROL 0
//	uint8_t AD1939_DATA 	 	= 134;// 0 CLOCK CONTROL 0A
//	uint8_t TXdata[3];
//	uint8_t RXdata[3];

  for(int i=0; i<4; i++){
	  my_data[i] = 0x0000;
	  rx_data_i2s[i]= 0x0000;
  }





  //HAL_I2S_Transmit_DMA(&hi2s2, my_data, 4);
  float n=0;
  float fract = 100;



//  HAL_I2S_Transmit_DMA(&hi2s2, my_data, 4);
  // init specaial DSP instance
  arm_biquad_cascade_df2T_init_f32(&highpass_iir_50hz, 1, &highpass_coeff, &highpass_state);
  arm_biquad_cascade_df2T_init_f32(&highpass_iir_50hz_octave2, 1, &highpass_coeff, &highpass_state);
  for(uint8_t i = 0; i<numberofsubbands;i++){
	  subband_ones[i] = 1.0;
  }
  // init AD and DA
  HAL_I2S_Transmit_DMA(	&hi2s2, 	my_data, 		4);
  HAL_I2S_Receive_DMA(	&hi2s1, 	rx_data_i2s, 	4);
  ad1939_init(&hspi3);


//  HAL_I2S_Receive(&hi2s1, rx_data_i2s, 4);
//  HAL_I2S_Transmit_DMA(&hi2s2, my_data, 120);
  //HAL_I2S_Receive_DMA(&hi2s1, rx_data_i2s, 4);

//  HAL_I2S_Transmit_DMA(&hi2s2, my_data, 2);
  uint32_t delay_time = 1; // ms

  uint32_t amp=2000;
  uint32_t ch0 = 1;
  uint32_t ch1 = 1;
  uint16_t test_val  = 25000;

  //HAL_Delay(10);
  //uint32_t timeVariable = HAL_GetTick();

  uint8_t channel = 0;

  delay_effects_tst delay_effect;
  init_guitar_effect_delay(&delay_effect);
  float32_t freq_f32 = 4.0;

	volatile float32_t n_2;
	volatile float32_t len_f32 = 48000;

	float mul_val_f32 = 0;
	int state = 1;

//	WtrP = 0;
//	Rd_P = 0.0f;
//	Shift = 0.5f;
//	CrossFade = 1.0f;
  while (1)
  {
	  if (ADC_READY_FLAG){
		  ADC_READY_FLAG  = 0;
		  // get data from ADC
		int32_t value_from_ADC = adc_data_bf.value/2; //value_from_ADC_HighByte | value_from_ADC_LowByte;

		// +1 octave
		subbandfilter_calculation(value_from_ADC);
		octave1up();
		// save result
		float32_t octave_1_up_f32 = octave1_up_filtered;

		// +2 octave
		subbandfilter_octave2_calculation((int32_t)(octave_1_up_f32*4));
		octave2up();
//		 save result
		float32_t octave_2_up_f32 = octave1_up_filtered;

		// Write to DAC
		volatile static float32_t passthrough_volume = 0.3;
		volatile static float32_t octave_1_volume = 4;
		volatile static float32_t octave_2_volume = 0;
		output_test_ac=	(int32_t)octave_1_up_f32*octave_1_volume +
						(int32_t)octave_2_up_f32*octave_2_volume +
						(int32_t)((float32_t)value_from_ADC*passthrough_volume);

		//output_buffer.value=output_test_ac;
		output_buffer.value= Do_PitchShift(value_from_ADC) + output_test_ac;


		output_buffer.value = delay_effect.callback(&delay_effect,output_buffer.value);
//
//		// tremolo
//
////		if (sin(freq_f32*6.28*n/len) > 0){
////			mul_val_f32 = 1;
////		}
//		if(value_from_ADC>0x1FFFFF00){
//			len_f32 = 500;
//		}else{
//			len_f32 += 0.1;
//		}
//		if( len_f32 > 10000){
//			len_f32  = 10000;
//		}
//
//		if(state){
//			mul_val_f32=(float)n_2/len_f32;
//			n_2=n_2+1;
//		}else{
//			mul_val_f32=(float)n_2/len_f32;
//			n_2= n_2-1;
//		}
//
//		if(n_2>=len_f32){
//			state= 0;
//		}
//		if(n_2 == 0){
//			state=1;
//		}
//
//		if(mul_val_f32 >1.0){
//			mul_val_f32 = 1.0;
//		}
//
//		//mul_val_f32 +=(sin(freq_f32*6.28*n/len_f32)>0-0.5)/(float)len;
//		output_buffer.value = (int32_t)((float32_t)output_test_ac * mul_val_f32);
//		if(abs(output_test_ac)<abs(output_buffer.value)){
//			len_f32 = 9999;
//		}


//		output_buffer.value = Do_PitchShift(value_from_ADC);
//		output_buffer.value = output_test_ac;

		if ( DAC_HALF_COMPLETE_FLAG) {
			my_data[2] = output_buffer.raw_low;
			my_data[3] = output_buffer.raw_high;
		}
		else{
			my_data[6] = output_buffer.raw_low;
			my_data[7] = output_buffer.raw_high;
		}


//		SCB_InvalidateDCache();
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2S1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S1_Init(void)
{

  /* USER CODE BEGIN I2S1_Init 0 */

  /* USER CODE END I2S1_Init 0 */

  /* USER CODE BEGIN I2S1_Init 1 */

  /* USER CODE END I2S1_Init 1 */
  hi2s1.Instance = SPI1;
  hi2s1.Init.Mode = I2S_MODE_MASTER_RX;
  hi2s1.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s1.Init.DataFormat = I2S_DATAFORMAT_24B;
  hi2s1.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s1.Init.CPOL = I2S_CPOL_LOW;
  hi2s1.Init.FirstBit = I2S_FIRSTBIT_MSB;
  hi2s1.Init.WSInversion = I2S_WS_INVERSION_DISABLE;
  hi2s1.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_LEFT;
  hi2s1.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_DISABLE;
  if (HAL_I2S_Init(&hi2s1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S1_Init 2 */

  /* USER CODE END I2S1_Init 2 */

}

/**
  * @brief I2S2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S2_Init(void)
{

  /* USER CODE BEGIN I2S2_Init 0 */

  /* USER CODE END I2S2_Init 0 */

  /* USER CODE BEGIN I2S2_Init 1 */

  /* USER CODE END I2S2_Init 1 */
  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_24B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.FirstBit = I2S_FIRSTBIT_MSB;
  hi2s2.Init.WSInversion = I2S_WS_INVERSION_ENABLE;
  hi2s2.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_LEFT;
  hi2s2.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_ENABLE;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S2_Init 2 */

  /* USER CODE END I2S2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 0x0;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi3.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : CODEC_CS_Pin */
  GPIO_InitStruct.Pin = CODEC_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(CODEC_CS_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
