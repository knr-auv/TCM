#include "stm32f4xx.h"
#include "config.h"
#include "time/time.h"
void initCLOCK(void)
{
    //we want to run at 168MHz using HSI
    /* HAL values
    PLLM = 8
    PLLN = 168
    PLLP = 2
    PLLq = 4
    
    Real values:
    plln = 252
    */
    RCC->CR &= ~(RCC_CR_HSEON);
    RCC->CR |= RCC_CR_HSION; //turn HSI on
    while (!(RCC->CR & RCC_CR_HSIRDY))
    {
    };                       //wait for HSI ready
    RCC->CFGR = (uint32_t)0; //reset CFGR register - source clock is set as HSI by default

    RCC->PLLCFGR = (uint32_t)0x24003010; //reset PLLCFGR register
    RCC->CIR = (uint32_t)0;              //disable all interupts
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC);
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); //reset PLLM
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2;  //plm 1000 = 8 PLLM = 16, PLLN = 192 -->64MHz
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); //reset PLLN

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_7; //plln = 252

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); //reset PLLP
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_1;
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ); //reset PLLQ
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_2;

    RCC->CR |= RCC_CR_PLLON; //turn PLL on
    while (!(RCC_CR_PLLRDY & RCC->CR))
    {
    }; //wait for PLL

    FLASH->ACR &= ~0x00000007;
    FLASH->ACR |= 0x3;

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while (!(RCC_CFGR_SWS_PLL & RCC->CFGR))
    {
    };

    RCC->CFGR |= (RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2);
    
}

void initRCC(void)
{
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN);
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}
static void initGPIO(void)
{
    GPIOC->MODER |= GPIO_MODER_MODER8_0;  //LEDPin
    GPIOD->MODER |= GPIO_MODER_MODER11_0; //LEDPin
    GPIOB->MODER |= GPIO_MODER_MODER7_0;  //heartbeat LED
    //USART 3
    GPIOD->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
    GPIOD->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1);
    GPIOD->AFR[1] &= ~(0x000000FF);
    GPIOD->AFR[1] |= 0x00000077;
    
    /*USART 2*/
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
    GPIOA->AFR[0] &= ~(0x0000FF00);
    GPIOA->AFR[0] |= (0x00007700);
    
    /*USART 1*/
    GPIOA->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER9);
    GPIOA->MODER |= (GPIO_MODER_MODER10_1 | GPIO_MODER_MODER9_1);
    GPIOA->AFR[1] &= ~(0x00000FF0);
    GPIOA->AFR[1] |= (0x00000777);
    /*FTDI converter pins*/
    GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
    GPIOB->ODR |= (GPIO_ODR_ODR_4 | GPIO_ODR_ODR_7);
    GPIOB->ODR |= (GPIO_ODR_ODR_5 | GPIO_ODR_ODR_6);
    GPIOB->ODR &= ~(GPIO_ODR_ODR_4 | GPIO_ODR_ODR_7);

    /*FSM305 IMU*/
    GPIOA->MODER &= ~(GPIO_MODER_MODER15 | GPIO_MODER_MODER8 | GPIO_MODER_MODER12);
    GPIOA->MODER |= GPIO_MODER_MODER15_0; //PA15 CSN
    GPIOA->MODER |= GPIO_MODER_MODER8_0;  //output    BOOTN
    GPIOA->MODER |= GPIO_MODER_MODER12_0; //waken

    GPIOC->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER7); //PC9 interrupt - input
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR9);
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_0; //pull up mode
    GPIOC->MODER |= GPIO_MODER_MODER7_0; //output NRST

    GPIOC->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
    GPIOC->MODER |= (GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1);            //C10-12 AF
    GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12); //high speed
    GPIOC->AFR[1] |= (0x6 << 8 | 0x6 << 12 | 0x6 << 16);                                             //AF6 for spi3

    /*thrusters */
    GPIOE->MODER &= ~(GPIO_MODER_MODER14 | GPIO_MODER_MODER13 | GPIO_MODER_MODER11 | GPIO_MODER_MODER9);
    GPIOB->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER0);
    GPIOA->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);

    GPIOE->MODER |= GPIO_MODER_MODER14_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER9_1; //T1, T3, T5, T7
    GPIOB->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER0_1;                                                //T2, T4
    GPIOA->MODER |= GPIO_MODER_MODER7_1 | GPIO_MODER_MODER6_1;                                                //T6, T8
    GPIOE->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR9);
    GPIOA->AFR[0] |= ((0x2 << 24) | (0x2 << 28));
    GPIOB->AFR[0] |= (0x2 | 0x2 << 4);
    GPIOE->AFR[1] |= ((0x1 << 24) | (0x1 << 20) | (0x1 << 12) | (0x1 << 4));

    /* onboard analog sensors */
    GPIOA->MODER |= (GPIO_MODER_MODER0 | GPIO_MODER_MODER1); //temperature and humidity sensor  -   analog mode

    /* external flash module SPI2*/

    GPIOE->MODER |= GPIO_MODER_MODER7_0;      //PE7 nCE
    GPIOD->MODER |= GPIO_MODER_MODER10_0;     //PD10 nHOLD

    GPIOB->MODER |=GPIO_MODER_MODER12_0|GPIO_MODER_MODER13_1|GPIO_MODER_MODER14_1|GPIO_MODER_MODER15_1;      //PB12 nWP
    //B13 - checked, B14 miso
    GPIOB->AFR[1] |= 5<<20;
    GPIOB->AFR[1] |= (5<<24)|(5<<28);

    //GPIOC->MODER |= GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1|GPIO_MODER_MODER12_1; //PC10 clk, PC11 miso, PC12 mosi
    //alternate function 6 for pin C10,11,12

    /*BNO080 spi3*/
    GPIOC->MODER |= GPIO_MODER_MODER10_1|GPIO_MODER_MODER11_1|GPIO_MODER_MODER12_1;
    GPIOC->OSPEEDR |= 
	GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR10_0 |
	GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR11_0 |
	GPIO_OSPEEDER_OSPEEDR12_1 | GPIO_OSPEEDER_OSPEEDR12_0;
    GPIOC->AFR[1] &= ~0x000FFF00;
	GPIOC->AFR[1] |= 0x00066600;
}
static uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

static void initUSART1(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; //AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE2) >> 13;       //apb2 prescaler bits 13:15
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; //fancy table stolen from std_periph
    baud_rate /= USART1_BAUD;
    USART1->BRR = (uint16_t)baud_rate;
    USART1->CR1 = USART_CR1_RE | USART_CR1_TE; //enable uart rx and tx
    USART1->CR1 |= USART_CR1_IDLEIE;           //enable idle interrupt

    //enable DMA channel4 stream 7 for tx -> datasheet dma assignment
    DMA2_Stream7->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; //channel4, memory incrementation, mem ->periph
    DMA2_Stream7->PAR = (uint32_t) & (USART1->DR);                        //source is always usart1 data register
    DMA2_Stream7->CR |= DMA_SxCR_TCIE;                                    //enable transfer complete interrupt

    //enable DMA channel4 stream 5 for rx
    DMA2_Stream5->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; //channel4, memory incrementation, circular buffer
    DMA2_Stream5->PAR = (uint32_t) & (USART1->DR);
    DMA2_Stream5->CR |= DMA_SxCR_TCIE;

    USART1->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; //enable dma tx_rx
    USART1->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(DMA2_Stream5_IRQn);
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}
static void initUSART2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; //AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;       //apb1 prescaler bits 12:10
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; //fancy table stolen from std_periph
    baud_rate /= USART2_BAUD;
    USART2->BRR = (uint16_t)baud_rate;         //Baud rate - hardcoded to 115200
    USART2->CR1 = USART_CR1_RE | USART_CR1_TE; //enable uart rx and tx
    USART2->CR1 |= USART_CR1_IDLEIE;           //enable idle interrupt
    //DMA1 channel 4 stream 5 for rx, 6 for tx
    //enable DMA channel4 stream 6 for tx -> datasheet dma assignment
    DMA1_Stream6->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; //channel4, memory incrementation, mem ->periph
    DMA1_Stream6->PAR = (uint32_t) & (USART2->DR);                        //source is always usart3 data register
    DMA1_Stream6->CR |= DMA_SxCR_TCIE;                                    //enable transfer complete interrupt

    //enable DMA channel4 stream 5 for rx
    DMA1_Stream5->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; //channel4, memory incrementation, circular buffer
    DMA1_Stream5->PAR = (uint32_t) & (USART2->DR);
    DMA1_Stream5->CR |= DMA_SxCR_TCIE;

    USART2->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; //enable dma transmit and receibe
    USART2->CR1 |= USART_CR1_UE;

    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    }

void initUSART3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    volatile uint32_t baud_rate = SystemCoreClock;
    volatile uint32_t tmp = (RCC->CFGR & RCC_CFGR_HPRE) >> 4; //AHB prescaler bits 7:4
    baud_rate = baud_rate >> APBAHBPrescTable[tmp];
    tmp = (RCC->CFGR & RCC_CFGR_PPRE1) >> 10;       //apb1 prescaler bits 12:10
    baud_rate = baud_rate >> APBAHBPrescTable[tmp]; //fancy table stolen from std_periph
    baud_rate /= USART3_BAUD;
    USART3->BRR = (uint16_t)baud_rate;         //Baud rate - hardcoded to 115200
    USART3->CR1 = USART_CR1_RE | USART_CR1_TE; //enable uart rx and tx
    USART3->CR1 |= USART_CR1_IDLEIE;           //enable idle interrupt

    //enable DMA channel4 stream 3 for tx -> datasheet dma assignment
    DMA1_Stream3->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC | DMA_SxCR_DIR_0; //channel4, memory incrementation, mem ->periph
    DMA1_Stream3->PAR = (uint32_t) & (USART3->DR);                        //source is always usart3 data register
    DMA1_Stream3->CR |= DMA_SxCR_TCIE;                                    //enable transfer complete interrupt

    //enable DMA channel4 stream 1 for rx
    DMA1_Stream1->CR = DMA_SxCR_CHSEL_2 | DMA_SxCR_MINC; //channel4, memory incrementation, circular buffer
    DMA1_Stream1->PAR = (uint32_t) & (USART3->DR);
    DMA1_Stream1->CR |= DMA_SxCR_TCIE;

    USART3->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; //enable dma transmit and receibe
    USART3->CR1 |= USART_CR1_UE;
    
    NVIC_EnableIRQ(USART3_IRQn);
    NVIC_SetPriority(USART3_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Stream1_IRQn);
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);
}
void initINT(void)
{
    /*--------PC9 for FSM305--------*/
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI9_PC; //PC9 for FSM305
    EXTI->IMR |= EXTI_IMR_MR9;                    //enable mask
    EXTI->FTSR &= ~EXTI_FTSR_TR9;
    EXTI->RTSR |= EXTI_RTSR_TR9; //falling edge
    NVIC_SetPriority(EXTI9_5_IRQn, 0);
    /*-------------------------*/
}
void initSPI1(void)
{
    RCC->APB1ENR |= RCC_APB2ENR_SPI1EN;
    //setup clock polarity
    SPI2->CR1 &= (uint32_t)(0x0);
    SPI2->CR1 |= SPI_CR1_MSTR|SPI_CR1_SSI | SPI_CR1_SSM;//|SPI_CR1_CPOL;
    SPI2->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY = 0, full-duplex

    SPI2->CR1 |= (SPI_CR1_BR_2);
    SPI2->CR1 |= SPI_CR1_SPE;
}
void initSPI2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    //setup clock polarity
    SPI2->CR1 &= (uint32_t)(0x0);
    SPI2->CR1 |= SPI_CR1_MSTR|SPI_CR1_SSI | SPI_CR1_SSM|SPI_CR1_CPOL|SPI_CR1_CPHA;
    SPI2->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY = 0, full-duplex

    SPI2->CR1 |= (SPI_CR1_BR_2);
    SPI2->CR1 |= SPI_CR1_SPE;
}
void initSPI3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    //setup clock polarity
    SPI3->CR1 &= (uint32_t)(0x0);
    //SPI3->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL;
    SPI3->CR1 |= SPI_CR1_MSTR|SPI_CR1_SSI | SPI_CR1_SSM|SPI_CR1_CPOL;
    SPI3->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY = 0, full-duplex

    SPI3->CR1 |= (SPI_CR1_BR_2);
    SPI3->CR1 |= SPI_CR1_SPE;
}

void initTIM1(void)
{
    //timer clock is 168MHz
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

    //channel 1 enable:
    TIM1->CCER |= TIM_CCER_CC1E;
    //channel 2 enable:
    TIM1->CCER |= TIM_CCER_CC2E;
    //channel 3 enable:
    TIM1->CCER |= TIM_CCER_CC3E;
    //channel 4 enable:
    TIM1->CCER |= TIM_CCER_CC4E;
    //Clock is 168 MHz
    TIM1->PSC = 168 - 1;  // counter count every 1 microsecond (typically 1 step is 1 [us] long but for better resolution and easier Dshot implementation it is 0.5 [us]. Notice that lowest motor_value 2000 step is still 1 [ms] long as in typical PWM)
    TIM1->ARR = 2500 - 1; // 1 period of PWM frequency is set to 400 Hz

    TIM1->CCR1 = 1500; // PWM length channel 1 (1 [ms])
    TIM1->CCR2 = 1500; // PWM length channel 2 (1 [ms])
    TIM1->CCR3 = 1500; // PWM length channel 3 (1 [ms])
    TIM1->CCR4 = 1500; // PWM length channel 4 (1 [ms])
    TIM1->EGR |= TIM_EGR_UG;
}
void initTIM2(void){    //free running timer for time counting
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    //Timer clock is 84Mhz
    //32bit cnt register
    TIM2->CR1 |= TIM_CR1_ARPE | TIM_CR1_URS;
    TIM2->PSC = 84 - 1; //1us 1 count
    TIM2->ARR = 1000000 - 1; //reload every second
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);
}
void initTIM3(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    //Timer clock is 84Mhz

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

    //channel 1 enable:
    TIM3->CCER |= TIM_CCER_CC1E;
    //channel 2 enable:
    TIM3->CCER |= TIM_CCER_CC2E;
    //channel 3 enable:
    TIM3->CCER |= TIM_CCER_CC3E;
    //channel 4 enable:
    TIM3->CCER |= TIM_CCER_CC4E;
    //84Mhz
    TIM3->PSC = 84 - 1;  // ticke per us
    TIM3->ARR = 2500 - 1; //  count to 2500us (it gives 400Hz)

    TIM3->CCR1 = 1500; // PWM length channel 1 (1 [ms])
    TIM3->CCR2 = 1500; // PWM length channel 2 (1 [ms])
    TIM3->CCR3 = 1500; // PWM length channel 3 (1 [ms])
    TIM3->CCR4 = 1500; // PWM length channel 4 (1 [ms])

    //	TIM2 enabling:
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}


void initADC1(void)
{   //APB2 clock i s 84MHz
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    ADC->CCR |=ADC_CCR_ADCPRE_1; //divide by 6, adc clock is 14MHz now
    ADC->CCR |= ADC_CCR_TSVREFE;

    
    ADC1->CR1 &= ADC_CR1_RES;   //set resolution to 12bit
    
    ADC1->CR2 &= ~ADC_CR2_CONT;  //single conversion mode
    ADC1->CR1 |= ADC_CR1_SCAN;  //enable scan mode 
    //ADC1->CR2 |= ADC_CR2_CONT; 
    ADC1->CR2 |= ADC_CR2_EOCS;
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    //conversion time = (sampling time + 12)/14MHz

    ADC1->SMPR1 |=(ADC_SMPR1_SMP16_1|ADC_SMPR1_SMP16_2);    //oboard temp sensor should have 10us so (144 +12)/14MHz = 11us
    ADC1->SMPR2 &=~(ADC_SMPR2_SMP0|ADC_SMPR2_SMP1);


    ADC1->CR2 |= ADC_CR2_DMA;
    ADC1->CR2 |= ADC_CR2_DDS;
    
    ADC1->SQR1 |= (ADC_SQR1_L_1);//ADC_SQR1_L_0); //3 conversions

    ADC1->SQR3 |= (ADC_SQR3_SQ1_4| ADC_SQR3_SQ2_0); //channel 16 than 1 than
    ADC1->SQR3 &=~(ADC_SQR3_SQ3);

    DMA2_Stream0->CR &= ~DMA_SxCR_DIR;  //peripheral to memory
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;  //set circular mode
    DMA2_Stream0->CR |= DMA_SxCR_MINC;  //memory increment
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0 |DMA_SxCR_PSIZE_0 ;//1|DMA_SxCR_PSIZE_0;  //word
    DMA2_Stream0->CR &= ~(DMA_SxCR_CHSEL);  //channel 0
    DMA2_Stream0->CR |= DMA_SxCR_TCIE;  //enable transfer complete interruot
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}
void initSystem()
{
    initCLOCK();
    SystemCoreClockUpdate();
    TicksInit();
    initRCC();
    initGPIO();
    initSPI2();
    //initSPI3();
    initUSART1();
    initUSART2();
    initUSART3();
    initTIM1();
    initTIM2();
    initTIM3();
    initADC1();
    initINT();
} 
