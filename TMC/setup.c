#include "stm32f4xx.h"
#include "setup.h"
#include "Config/config.h"
#include "time/time.h"

static void initCLOCK(void);
static void initFPU(void);
static void initGPIO(void);
static void initTIM1(void);
static void initTIM2(void);
static void initTIM3(void);
static void initTIM5(void);
static void initSPI1(void);
static void initSPI2(void);
static void initSPI3(void);
static void initI2C2(void);
static void initUSART1(void);
static void initUSART2(void);
static void initUSART3(void);
static void initADC1(void);
static void initEXTI(void);
static void initDMA(void);
static void initNVIC(void);
static void watchdogInit(void);

void initSystem()
{
    initCLOCK();
    SystemCoreClockUpdate();
    initFPU();
    initGPIO();
    initTIM1();
    initTIM2();
    initTIM3();
    initTIM5();
    initSPI1();
    initSPI2();
    initSPI3();
    initI2C2();
    initUSART1();
    initUSART2();
    initUSART3();
    initADC1();
    initEXTI();
    initDMA();
    initNVIC();
    TicksInit();

    // watchdogInit();
}

static void initCLOCK(void)
{
    // we want to run at 168MHz using HSI
    /* HAL values
    PLLM = 8
    PLLN = 168
    PLLP = 2
    PLLQ = 7

    PLL_freq = PLL_clock_in / PLLM * PLLN / PLLP
    PLL_48 =PLL_clock_in / PLLM * PLLN / PLLQ = 48


    Real values:   // WHAT is this?
    plln = 252

    */
   
    RCC->CR |= RCC_CR_HSION; // turn HSI on
    while (!(RCC->CR & RCC_CR_HSIRDY))
    {
    };                       
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);
    while ((RCC->CFGR & RCC_CFGR_SWS_HSI))
    {
    }; 
    
    RCC->CR &= ~(RCC_CR_HSEON);
    RCC->CR &= ~(RCC_CR_PLLON | RCC_CR_PLLI2SON);
    RCC->CFGR = (uint32_t)0; // reset CFGR register - source clock is set as HSI by default

    RCC->PLLCFGR = (uint32_t)0; // reset PLLCFGR register
    RCC->CIR = (uint32_t)0;              // disable all interupts
    CLEAR_BIT(RCC->PLLCFGR,RCC_PLLCFGR_PLLSRC);
 
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); // reset PLLM
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3;  // PLLM = 8

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);                                          // reset PLLN
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7; // PLLN = 168

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); // reset PLLP, PLLP = 2

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ);                                          //    reset PLLQ
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_0 | RCC_PLLCFGR_PLLQ_1 | RCC_PLLCFGR_PLLQ_2; //    PLLQ = 7

    RCC->CR |= RCC_CR_PLLON; // turn PLL on
    while (!(RCC_CR_PLLRDY & RCC->CR))
    { // wait for PLL
    };

    FLASH->ACR &= ~0x00000007;
    FLASH->ACR |= 0x3;

    // set PLL as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC_CFGR_SWS_PLL & RCC->CFGR))
    {
    };

    RCC->CFGR |= (RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2);
    // AHB presc. = 1 (left at default)
}

// Foating-Point Unit turn on (for float number efficient computing):
static void initFPU()
{ // Foating-Point Unit turn on (for float number efficient computing):
    SCB->CPACR |= (3UL << 10 * 2) | (3UL << 11 * 2);
}

static void initGPIO(void)
{
    // enable GPIO's clocks:
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN);

    /*------------------GPIOA------------------*/
    //	set mode (00-input; 01-output; 10-alternate; 11-analog):
    GPIOA->MODER |= GPIO_MODER_MODER0; // temperature sensor (analog)

    GPIOA->MODER |= GPIO_MODER_MODER1; // humidity sensor (analog)

    GPIOA->MODER &= ~GPIO_MODER_MODER2; // USART 2 TX
    GPIOA->MODER |= GPIO_MODER_MODER2_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER3; // USART 2 RX
    GPIOA->MODER |= GPIO_MODER_MODER3_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER5; // IRONMAN fan TIM2_CH1_ETR
    GPIOA->MODER |= GPIO_MODER_MODER5_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER6; // Thruster 8 TIM3_CH1
    GPIOA->MODER |= GPIO_MODER_MODER6_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER7; // Thruster 6 TIM3_CH2
    GPIOA->MODER |= GPIO_MODER_MODER7_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER8; // FSM305 IMU BOOTN
    GPIOA->MODER |= GPIO_MODER_MODER8_0;

    GPIOA->MODER &= ~GPIO_MODER_MODER9; // USART 1 TX
    GPIOA->MODER |= GPIO_MODER_MODER9_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER10; // USART 1 RX
    GPIOA->MODER |= GPIO_MODER_MODER10_1;

    GPIOA->MODER &= ~GPIO_MODER_MODER12; // FSM305 IMU waken
    GPIOA->MODER |= GPIO_MODER_MODER12_0;

    GPIOA->MODER &= ~GPIO_MODER_MODER15; // FSM305 IMU CSN
    GPIOA->MODER |= GPIO_MODER_MODER15_0;

    // set alternate functions:
    GPIOA->AFR[0] &= ~(0xFFF0FF00);
    GPIOA->AFR[0] |= (0x22207700);

    GPIOA->AFR[1] &= ~(0x00000FF0);
    GPIOA->AFR[1] |= (0x00000770);

    //  set speed (11 - max. speed ):
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR3 |
                       GPIO_OSPEEDER_OSPEEDR5 |
                       GPIO_OSPEEDER_OSPEEDR6 |
                       GPIO_OSPEEDER_OSPEEDR7 |
                       GPIO_OSPEEDER_OSPEEDR8 |
                       GPIO_OSPEEDER_OSPEEDR9 |
                       GPIO_OSPEEDER_OSPEEDR10 |
                       GPIO_OSPEEDER_OSPEEDR12 |
                       GPIO_OSPEEDER_OSPEEDR15);

    /*------------------GPIOB------------------*/
    //	set mode (00-input; 01-output; 10-alternate; 11-analog):

    GPIOB->MODER &= ~GPIO_MODER_MODER0; // Thruster 4 TIM3_CH3
    GPIOB->MODER |= GPIO_MODER_MODER0_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER1; // Thruster 2 TIM3_CH4
    GPIOB->MODER |= GPIO_MODER_MODER1_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER3; // SPI1_SCK
    GPIOB->MODER |= GPIO_MODER_MODER3_1;

    // same pin?
    GPIOB->MODER &= ~GPIO_MODER_MODER4; // FTDI converter
    GPIOB->MODER |= GPIO_MODER_MODER4_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER4; // SPI1_MISO
    GPIOB->MODER |= GPIO_MODER_MODER4_1;

    // same pin?
    GPIOB->MODER &= ~GPIO_MODER_MODER5; // FTDI converter
    GPIOB->MODER |= GPIO_MODER_MODER5_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER5; // SPI1_MOSI
    GPIOB->MODER |= GPIO_MODER_MODER5_1;

    // same pin?
    GPIOB->MODER &= ~GPIO_MODER_MODER6; // FTDI converter
    GPIOB->MODER |= GPIO_MODER_MODER6_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER6; // SPI_CS
    GPIOB->MODER |= GPIO_MODER_MODER6_0;

    // same pin?
    GPIOB->MODER &= ~GPIO_MODER_MODER7; // FTDI converter
    GPIOB->MODER |= GPIO_MODER_MODER7_0;
    GPIOB->MODER &= ~GPIO_MODER_MODER7; // heartbeat LED
    GPIOB->MODER |= GPIO_MODER_MODER7_0;

    GPIOB->MODER &= ~GPIO_MODER_MODER10; // I2C2_CLK
    GPIOB->MODER |= GPIO_MODER_MODER10_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER11; // I2C2_SDA
    GPIOB->MODER |= GPIO_MODER_MODER11_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER12; // FLASH nWP
    GPIOB->MODER |= GPIO_MODER_MODER12_0;

    GPIOB->MODER &= ~GPIO_MODER_MODER13; // FLASH SPI2_SCK
    GPIOB->MODER |= GPIO_MODER_MODER13_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER14; // FLASH SPI2_MISO
    GPIOB->MODER |= GPIO_MODER_MODER14_1;

    GPIOB->MODER &= ~GPIO_MODER_MODER15; // FLASH SPI2_MOSI
    GPIOB->MODER |= GPIO_MODER_MODER15_1;

    // set alternate functions:
    GPIOB->AFR[0] &= ~(0x00FFF0FF);
    GPIOB->AFR[0] |= (0x00555022);

    GPIOB->AFR[1] &= ~(0xFFF0FF00);
    GPIOB->AFR[1] |= (0x55504400);

    GPIOB->ODR |= (GPIO_ODR_ODR_4 | GPIO_ODR_ODR_7);
    GPIOB->ODR |= (GPIO_ODR_ODR_5 | GPIO_ODR_ODR_6);
    GPIOB->ODR &= ~(GPIO_ODR_ODR_4 | GPIO_ODR_ODR_7);

    // set high on pin 6 (SPI1 CS) (inactive state)
    GPIOB->BSRRL |= GPIO_BSRR_BS_6;

    // (00 no pull down, no pull up; 01 pull-up; 10 pull-down ):
    // pull-up SDA and SCL line (datasheet page 13)
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR10;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR11;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR11_0;

    //	output type (0 - push-pull, 1 - open-drain)
    //	open-drain for SDA and SCL:
    GPIOB->OTYPER |= GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11;

    //  set speed (11 - max. speed ):
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 |
                       GPIO_OSPEEDER_OSPEEDR1 |
                       GPIO_OSPEEDER_OSPEEDR3 |
                       GPIO_OSPEEDER_OSPEEDR4 |
                       GPIO_OSPEEDER_OSPEEDR5 |
                       GPIO_OSPEEDER_OSPEEDR6 |
                       GPIO_OSPEEDER_OSPEEDR7 |
                       GPIO_OSPEEDER_OSPEEDR10 |
                       GPIO_OSPEEDER_OSPEEDR11 |
                       GPIO_OSPEEDER_OSPEEDR12 |
                       GPIO_OSPEEDER_OSPEEDR13 |
                       GPIO_OSPEEDER_OSPEEDR14 |
                       GPIO_OSPEEDER_OSPEEDR15);

    /*------------------GPIOC------------------*/
    //	set mode (00-input; 01-output; 10-alternate; 11-analog):

    GPIOC->MODER &= ~GPIO_MODER_MODER7; // FSM305 IMU NRST
    GPIOC->MODER |= GPIO_MODER_MODER7_0;

    GPIOC->MODER &= ~GPIO_MODER_MODER8; // LEDPin
    GPIOC->MODER |= GPIO_MODER_MODER8_0;

    GPIOC->MODER &= ~GPIO_MODER_MODER9; // FSM305 IMU interrupt (input)

    GPIOC->MODER &= ~GPIO_MODER_MODER10; // FSM305/BNO080 IMU SPI3_SCK
    GPIOC->MODER |= GPIO_MODER_MODER10_1;

    GPIOC->MODER &= ~GPIO_MODER_MODER11; // FSM305/BNO080 IMU SPI3_MISO
    GPIOC->MODER |= GPIO_MODER_MODER11_1;

    GPIOC->MODER &= ~GPIO_MODER_MODER12; // FSM305/BNO080 IMU SPI3_MOSI
    GPIOC->MODER |= GPIO_MODER_MODER12_1;

    // set alternate functions:
    GPIOC->AFR[1] &= ~(0x000FFF00);
    GPIOC->AFR[1] |= (0x00066600);

    // (00 no pull down, no pull up; 01 pull-up; 10 pull-down):
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_0;

    // set speed (11 - max. speed):
    GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR7 |
                       GPIO_OSPEEDER_OSPEEDR8 |
                       GPIO_OSPEEDER_OSPEEDR9 |
                       GPIO_OSPEEDER_OSPEEDR10 |
                       GPIO_OSPEEDER_OSPEEDR11 |
                       GPIO_OSPEEDER_OSPEEDR12);

    /*------------------GPIOD------------------*/
    //	set mode (00-input; 01-output; 10-alternate; 11-analog):

    GPIOD->MODER &= ~GPIO_MODER_MODER8; // USART 3 TX
    GPIOD->MODER |= GPIO_MODER_MODER8_1;

    GPIOD->MODER &= ~GPIO_MODER_MODER9; // USART 3 RX
    GPIOD->MODER |= GPIO_MODER_MODER9_1;

    GPIOD->MODER &= ~GPIO_MODER_MODER10; // FLASH nHOLD
    GPIOD->MODER |= GPIO_MODER_MODER10_0;

    GPIOD->MODER |= GPIO_MODER_MODER11_0; // LEDPin

    // set alternate functions:
    GPIOD->AFR[1] &= ~(0x000000FF);
    GPIOD->AFR[1] |= 0x00000077;

    // set speed (11 - max. speed):
    GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 |
                       GPIO_OSPEEDER_OSPEEDR9 |
                       GPIO_OSPEEDER_OSPEEDR10 |
                       GPIO_OSPEEDER_OSPEEDR11);

    /*------------------GPIOE------------------*/
    //	set mode (00-input; 01-output; 10-alternate; 11-analog):

    GPIOE->MODER &= ~GPIO_MODER_MODER7; //  FLASH nCE SPI2_CE
    GPIOE->MODER |= GPIO_MODER_MODER7_0;

    GPIOE->MODER &= ~GPIO_MODER_MODER9; // Thruster 7 TIM1_CH1
    GPIOE->MODER |= GPIO_MODER_MODER9_1;

    GPIOE->MODER &= ~GPIO_MODER_MODER11; // Thruster 5 TIM1_CH2
    GPIOE->MODER |= GPIO_MODER_MODER11_1;

    GPIOE->MODER &= ~GPIO_MODER_MODER13; // Thruster 3 TIM1_CH3
    GPIOE->MODER |= GPIO_MODER_MODER13_1;

    GPIOE->MODER &= ~GPIO_MODER_MODER14; // Thruster 1 TIM1_CH4
    GPIOE->MODER |= GPIO_MODER_MODER14_1;

    // set alternate functions:
    GPIOE->AFR[1] &= ~(0x0FF0F0F0);
    GPIOE->AFR[1] |= 0x01101010;

    // set speed (11 - max. speed):
    GPIOE->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR7 |
                       GPIO_OSPEEDER_OSPEEDR9 |
                       GPIO_OSPEEDER_OSPEEDR11 |
                       GPIO_OSPEEDER_OSPEEDR13 |
                       GPIO_OSPEEDER_OSPEEDR14);
}

static void initTIM1(void)
{
    // timer clock is 168MHz
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    TIM1->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
    // PWM mode 1 and output compare 1 preload enable:
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    // PWM mode 1 and output compare 2 preload enable:
    TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;
    // PWM mode 1 and output compare 3 preload enable:
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;
    // PWM mode 1 and output compare 4 preload enable:
    TIM1->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;

    // channel 1 enable:
    TIM1->CCER |= TIM_CCER_CC1E;
    // channel 2 enable:
    TIM1->CCER |= TIM_CCER_CC2E;
    // channel 3 enable:
    TIM1->CCER |= TIM_CCER_CC3E;
    // channel 4 enable:
    TIM1->CCER |= TIM_CCER_CC4E;
    // Clock is 168 MHz
    TIM1->PSC = 168 - 1;  // counter count every 1 microsecond (typically 1 step is 1 [us] long but for better resolution and easier Dshot implementation it is 0.5 [us]. Notice that lowest motor_value 2000 step is still 1 [ms] long as in typical PWM)
    TIM1->ARR = 2500 - 1; // 1 period of PWM frequency is set to 400 Hz

    TIM1->CCR1 = 1500; // PWM length channel 1 (1 [ms])
    TIM1->CCR2 = 1500; // PWM length channel 2 (1 [ms])
    TIM1->CCR3 = 1500; // PWM length channel 3 (1 [ms])
    TIM1->CCR4 = 1500; // PWM length channel 4 (1 [ms])
    TIM1->EGR |= TIM_EGR_UG;
}

static void initTIM2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // Timer clock is 84Mhz

    // register is buffered and only overflow generate interrupt:
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    // PWM mode 1 and output compare 1 preload enable:
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;

    // channel 1 enable:
    TIM2->CCER |= TIM_CCER_CC1E;
    // 84Mhz
    TIM2->PSC = 84 - 1;   // ticke per us
    TIM2->ARR = 1000 - 1; //  count to 2500us (it gives 400Hz)

    TIM2->CCR1 = 0; // PWM length channel 1 (1 [ms])

    //	TIM2 enabling:
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}

static void initTIM3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    // Timer clock is 84Mhz

    // register is buffered and only overflow generate interrupt:
    TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;

    // PWM mode 1 and output compare 1 preload enable:
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE;
    // PWM mode 1 and output compare 2 preload enable:
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2PE;
    // PWM mode 1 and output compare 3 preload enable:
    TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3PE;
    // PWM mode 1 and output compare 4 preload enable:
    TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE;

    // channel 1 enable:
    TIM3->CCER |= TIM_CCER_CC1E;
    // channel 2 enable:
    TIM3->CCER |= TIM_CCER_CC2E;
    // channel 3 enable:
    TIM3->CCER |= TIM_CCER_CC3E;
    // channel 4 enable:
    TIM3->CCER |= TIM_CCER_CC4E;
    // 84Mhz
    TIM3->PSC = 84 - 1;   // ticke per us
    TIM3->ARR = 2500 - 1; //  count to 2500us (it gives 400Hz)

    TIM3->CCR1 = 1500; // PWM length channel 1 (1 [ms])
    TIM3->CCR2 = 1500; // PWM length channel 2 (1 [ms])
    TIM3->CCR3 = 1500; // PWM length channel 3 (1 [ms])
    TIM3->CCR4 = 1500; // PWM length channel 4 (1 [ms])

    //	TIM2 enabling:
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}
static void initTIM5(void)
{ // free running timer for time counting
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    // Timer clock is 84Mhz
    // 32bit cnt register
    TIM5->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
    TIM5->PSC = 84 - 1;      // 1us 1 count
    TIM5->ARR = 1000000 - 1; // reload every second
    TIM5->DIER |= TIM_DIER_UIE;
    TIM5->EGR |= TIM_EGR_UG;
}

static void initSPI1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    //  set prescaler:
    SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0; // APB2 is 84 [MHz] and max frequency is ?
    //  NSS value of master is set by software (SSM) it has to be high so set  SSI; Master configuration; clock idle is high (CPOL); second edge data capture (CPHA):
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;
    //  for DMA usage:
    SPI1->CR2 |= SPI_CR2_RXDMAEN;
    SPI1->CR2 |= SPI_CR2_TXDMAEN;
    //  enabling SPI1:
    SPI1->CR1 |= SPI_CR1_SPE;
}

static void initSPI2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    // setup clock polarity
    SPI2->CR1 &= (uint32_t)(0x0);
    SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_CPOL | SPI_CR1_CPHA;
    SPI2->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY = 0, full-duplex

    SPI2->CR1 |= (SPI_CR1_BR_2);
    //  enabling SPI2:
    SPI2->CR1 |= SPI_CR1_SPE;
}

static void initSPI3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    // setup clock polarity
    SPI3->CR1 &= (uint32_t)(0x0);
    // SPI3->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
    SPI3->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_CPOL;
    SPI3->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY = 0, full-duplex

    SPI3->CR1 |= (SPI_CR1_BR_2);
    //  enabling SPI3:
    SPI3->CR1 |= SPI_CR1_SPE;
}

static void initI2C2(void)
{
    //	enable I2C2 clock:
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    //	reset I2C:
    I2C2->CR1 |= I2C_CR1_SWRST;
    I2C2->CR1 &= ~I2C_CR1_SWRST;
    I2C2->CR1 &= ~I2C_CR1_NOSTRETCH;
    // peripheral clock frequency (assume max. = 42 [MHz]):
    I2C2->CR2 |= 0x2A;
    // according datasheet need to be set high:
    I2C2->OAR1 |= (1UL << 14);

    //	I2C2 SM/FM mode selection (set FM):
    I2C2->CCR |= I2C_CCR_FS;
    // DMA requests enable, NACK at the end of DMA reception:
    I2C2->CR2 |= I2C_CR2_DMAEN | I2C_CR2_LAST;

    /*400kHz setting (datasheet says that main clock should have been multiply of 10 [MHz] but it should work)
     * APB1 clock has 42 [MHz] -> 1/42 [us] is main clock period
     * according datasheet:
     * in FM mode Low time (T_L) = 2*High time (T_H) so whole period is 3*T_H
     * since we want 1/400[ms]=2.5 [us] period 2.5/3 = T_H
     * T_H needs to be a multiple of main clock period so CCR=(2.5/3)/(1/42) = 35
     * so CCR =0x23
     */
    I2C2->CCR |= 0x23;
    /* max. rise time - I couldn't find much information about it
     * Only general info about rising/falling time for certain GPIO speed setting (sth. like 2.5-6 [ns] for max. speed)
     * On the other hand in the Internet I found 300 [ns] for FM,
     * 1000 [ns] (used for SM) is too much for sure (period for 400 [kHz] is 2.5 [us] so falling and rising would take 2 [us])
     * so to be safe use 300 [ns] = 0.3 [us] -> 0.3/(1/42) = 13 -> TRISE = 13+1
     */
    I2C2->TRISE |= 0x0E;

    //	peripheral enable:
    I2C2->CR1 |= I2C_CR1_PE;
}

static uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

static void initUSART1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; // AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13;       // apb2 prescaler bits 13:15
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; // fancy table stolen from std_periph
    baud_rate /= USART1_BAUD;
    USART1->BRR = (uint16_t)baud_rate;
    USART1->CR1 = USART_CR1_RE | USART_CR1_TE; // enable uart rx and tx
    USART1->CR1 |= USART_CR1_IDLEIE;           // enable idle interrupt

    USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // enable dma tx_rx
    USART1->CR1 |= USART_CR1_UE;
}

static void initUSART2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; // AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;       // apb1 prescaler bits 12:10
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; // fancy table stolen from std_periph
    baud_rate /= USART2_BAUD;
    USART2->BRR = (uint16_t)baud_rate;         // Baud rate - hardcoded to 115200
    USART2->CR1 = USART_CR1_RE | USART_CR1_TE; // enable uart rx and tx
    USART2->CR1 |= USART_CR1_IDLEIE;           // enable idle interrupt

    USART2->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // enable dma transmit and receibe
    USART2->CR1 |= USART_CR1_UE;
}

static void initUSART3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; // AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;       // apb1 prescaler bits 12:10
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; // fancy table stolen from std_periph
    baud_rate /= USART3_BAUD;
    USART3->BRR = (uint16_t)baud_rate;         // Baud rate - hardcoded to 115200
    USART3->CR1 = USART_CR1_RE | USART_CR1_TE; // enable uart rx and tx
    USART3->CR1 |= USART_CR1_IDLEIE;           // enable idle interrupt

    USART3->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // enable dma transmit and receibe
    USART3->CR1 |= USART_CR1_UE;
}

static void initADC1(void)
{ // APB2 clock is 84MHz
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    ADC->CCR |= ADC_CCR_ADCPRE_1; // divide by 6, adc clock is 14MHz now
    ADC->CCR |= ADC_CCR_TSVREFE;

    ADC1->CR1 &= ADC_CR1_RES; // set resolution to 12bit

    ADC1->CR2 &= ~ADC_CR2_CONT; // single conversion mode
    ADC1->CR1 |= ADC_CR1_SCAN;  // enable scan mode
    // ADC1->CR2 |= ADC_CR2_CONT;
    ADC1->CR2 |= ADC_CR2_EOCS;
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // conversion time = (sampling time + 12)/14MHz

    ADC1->SMPR1 |= (ADC_SMPR1_SMP16_1 | ADC_SMPR1_SMP16_2); // oboard temp sensor should have 10us so (144 +12)/14MHz = 11us
    ADC1->SMPR2 &= ~(ADC_SMPR2_SMP0 | ADC_SMPR2_SMP1);

    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_DDS;

    ADC1->SQR1 |= (ADC_SQR1_L_1); // ADC_SQR1_L_0); //3 conversions

    ADC1->SQR3 |= (ADC_SQR3_SQ1_4 | ADC_SQR3_SQ2_0); // channel 16 than 1 than
    ADC1->SQR3 &= ~(ADC_SQR3_SQ3);
}

static void initEXTI(void)
{
    // for EXTI managing enable SYSCFG clock:
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /*--------PC9 for FSM305--------*/
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PC; // PC9 for FSM305
    EXTI->IMR |= EXTI_IMR_MR9;                    // enable mask
    EXTI->FTSR &= ~EXTI_FTSR_TR9;
    EXTI->RTSR |= EXTI_RTSR_TR9; // falling edge
    /*-------------------------*/
}

static void initDMA()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    /*------------SPI1------------*/
    // reception:
    DMA2_Stream2->CR |= DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0 | DMA_SxCR_MINC | DMA_SxCR_CIRC | DMA_SxCR_TCIE | DMA_SxCR_PL_1;
    DMA2_Stream2->PAR = (uint32_t)(&(SPI1->DR));
    // address and number of bytes will be set before each transmission

    // transmission:
    DMA2_Stream3->CR |= DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0 | DMA_SxCR_MINC | DMA_SxCR_CIRC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE | DMA_SxCR_PL_1;
    DMA2_Stream3->PAR = (uint32_t)(&(SPI1->DR));
    // address and number of bytes will be set before each transmission

    /*------------I2C2------------*/
    // reception:
    DMA1_Stream2->CR |= DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0 | DMA_SxCR_MINC | DMA_SxCR_TCIE | DMA_SxCR_PL_1;
    DMA1_Stream2->PAR = (uint32_t)(&(I2C2->DR));
    // address and number of bytes will be set before each transmission

    /*------------USART1------------*/
    // enable DMA channel4 stream 7 for tx -> datasheet dma assignment
    DMA2_Stream7->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; // channel4, memory incrementation, mem ->periph
    DMA2_Stream7->PAR = (uint32_t) & (USART1->DR);                        // source is always usart1 data register
    DMA2_Stream7->CR |= DMA_SxCR_TCIE;                                    // enable transfer complete interrupt

    // enable DMA channel4 stream 5 for rx
    DMA2_Stream5->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; // channel4, memory incrementation, circular buffer
    DMA2_Stream5->PAR = (uint32_t) & (USART1->DR);
    DMA2_Stream5->CR |= DMA_SxCR_TCIE;

    /*------------USART2------------*/
    // DMA1 channel 4 stream 5 for rx, 6 for tx
    // enable DMA channel4 stream 6 for tx -> datasheet dma assignment
    DMA1_Stream6->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; // channel4, memory incrementation, mem ->periph
    DMA1_Stream6->PAR = (uint32_t) & (USART2->DR);                        // source is always usart3 data register
    DMA1_Stream6->CR |= DMA_SxCR_TCIE;                                    // enable transfer complete interrupt

    // enable DMA channel4 stream 5 for rx
    DMA1_Stream5->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; // channel4, memory incrementation, circular buffer
    DMA1_Stream5->PAR = (uint32_t) & (USART2->DR);
    DMA1_Stream5->CR |= DMA_SxCR_TCIE;

    /*------------USART3------------*/
    // enable DMA channel4 stream 3 for tx -> datasheet dma assignment
    DMA1_Stream3->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; // channel4, memory incrementation, mem ->periph
    DMA1_Stream3->PAR = (uint32_t) & (USART3->DR);                        // source is always usart3 data register
    DMA1_Stream3->CR |= DMA_SxCR_TCIE;                                    // enable transfer complete interrupt

    // enable DMA channel4 stream 1 for rx
    DMA1_Stream1->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; // channel4, memory incrementation, circular buffer
    DMA1_Stream1->PAR = (uint32_t) & (USART3->DR);
    DMA1_Stream1->CR |= DMA_SxCR_TCIE;

    /*------------ADC1------------*/
    DMA2_Stream0->CR &= ~DMA_SxCR_DIR;                       // peripheral to memory
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;                       // set circular mode
    DMA2_Stream0->CR |= DMA_SxCR_MINC;                       // memory increment
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0; // 1|DMA_SxCR_PSIZE_0;  //word
    DMA2_Stream0->CR &= ~(DMA_SxCR_CHSEL);                   // channel 0
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;                       // enable transfer complete interruot
}

static void initNVIC()
{
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 0);

    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 0);

    NVIC_EnableIRQ(USART3_IRQn);
    NVIC_SetPriority(USART3_IRQn, 0);

    NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    NVIC_EnableIRQ(DMA1_Stream2_IRQn);
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);

    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    NVIC_EnableIRQ(DMA2_Stream5_IRQn);
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn, 0);

    NVIC_EnableIRQ(TIM5_IRQn);
    NVIC_SetPriority(TIM5_IRQn, 6);
}

static void watchdogInit()
{
    RCC->CSR |= RCC_CSR_LSION;
    while ((RCC->CSR & RCC_CSR_LSIRDY) == 0)
    {
        ;
    };
    // LSI is 32 000 Hz
    IWDG->KR = 0x5555;                      /* Enable access to IWDG_PR */
    IWDG->PR = IWDG_PR_PR_0 | IWDG_PR_PR_1; /* Set prescaler Value*/
    IWDG->RLR = 1000;                       /* Set counter value */
    IWDG->KR = 0xaaaa;                      /* Clear data, set counter to max (In out case 1000) value */
    IWDG->KR = 0xcccc;                      /* Enable IWDG */
    // to reset watchdog do:
    IWDG->KR = 0xaaaa;
}
