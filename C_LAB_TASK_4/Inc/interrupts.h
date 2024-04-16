#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_


// Handle interrupts for timer 2 (one-shot callback)
void TIM2_IRQHandler();

// Handle interrupts for timer 3 (periodic callback)
void TIM3_IRQHandler();


#endif /* INTERRUPTS_H_ */
