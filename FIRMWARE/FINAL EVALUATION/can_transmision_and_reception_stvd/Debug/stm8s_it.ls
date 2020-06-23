   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.11.13 - 05 Feb 2019
   3                     ; Generator (Limited) V4.4.10 - 25 Sep 2019
  42                     ; 65 INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
  42                     ; 66 {
  43                     	switch	.text
  44  0000               f_TRAP_IRQHandler:
  48                     ; 70 }
  51  0000 80            	iret
  73                     ; 76 INTERRUPT_HANDLER(TLI_IRQHandler, 0)
  73                     ; 77 {
  74                     	switch	.text
  75  0001               f_TLI_IRQHandler:
  79                     ; 81 }
  82  0001 80            	iret
 104                     ; 88 INTERRUPT_HANDLER(AWU_IRQHandler, 1)
 104                     ; 89 {
 105                     	switch	.text
 106  0002               f_AWU_IRQHandler:
 110                     ; 93 }
 113  0002 80            	iret
 135                     ; 100 INTERRUPT_HANDLER(CLK_IRQHandler, 2)
 135                     ; 101 {
 136                     	switch	.text
 137  0003               f_CLK_IRQHandler:
 141                     ; 105 }
 144  0003 80            	iret
 167                     ; 112 INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
 167                     ; 113 {
 168                     	switch	.text
 169  0004               f_EXTI_PORTA_IRQHandler:
 173                     ; 117 }
 176  0004 80            	iret
 199                     ; 124 INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
 199                     ; 125 {
 200                     	switch	.text
 201  0005               f_EXTI_PORTB_IRQHandler:
 205                     ; 129 }
 208  0005 80            	iret
 231                     ; 136 INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
 231                     ; 137 {
 232                     	switch	.text
 233  0006               f_EXTI_PORTC_IRQHandler:
 237                     ; 139 }
 240  0006 80            	iret
 263                     ; 146 INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
 263                     ; 147 {
 264                     	switch	.text
 265  0007               f_EXTI_PORTD_IRQHandler:
 269                     ; 151 }
 272  0007 80            	iret
 295                     ; 158 INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
 295                     ; 159 {
 296                     	switch	.text
 297  0008               f_EXTI_PORTE_IRQHandler:
 301                     ; 163 }
 304  0008 80            	iret
 326                     ; 185  @svlreg INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
 326                     ; 186 #else
 326                     ; 187  INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
 326                     ; 188 #endif
 326                     ; 189 {
 327                     	switch	.text
 328  0009               f_CAN_RX_IRQHandler:
 330  0009 be02          	ldw	x,c_lreg+2
 331  000b 89            	pushw	x
 332  000c be00          	ldw	x,c_lreg
 333  000e 89            	pushw	x
 336                     ; 200 }
 339  000f 85            	popw	x
 340  0010 bf00          	ldw	c_lreg,x
 341  0012 85            	popw	x
 342  0013 bf02          	ldw	c_lreg+2,x
 343  0015 80            	iret
 365                     ; 207  INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
 365                     ; 208 {
 366                     	switch	.text
 367  0016               f_CAN_TX_IRQHandler:
 371                     ; 212 }
 374  0016 80            	iret
 396                     ; 220 INTERRUPT_HANDLER(SPI_IRQHandler, 10)
 396                     ; 221 {
 397                     	switch	.text
 398  0017               f_SPI_IRQHandler:
 402                     ; 225 }
 405  0017 80            	iret
 428                     ; 232 INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
 428                     ; 233 {
 429                     	switch	.text
 430  0018               f_TIM1_UPD_OVF_TRG_BRK_IRQHandler:
 434                     ; 237 }
 437  0018 80            	iret
 460                     ; 244 INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
 460                     ; 245 {
 461                     	switch	.text
 462  0019               f_TIM1_CAP_COM_IRQHandler:
 466                     ; 249 }
 469  0019 80            	iret
 492                     ; 281  INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
 492                     ; 282 {
 493                     	switch	.text
 494  001a               f_TIM2_UPD_OVF_BRK_IRQHandler:
 498                     ; 286 }
 501  001a 80            	iret
 524                     ; 293  INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
 524                     ; 294 {
 525                     	switch	.text
 526  001b               f_TIM2_CAP_COM_IRQHandler:
 530                     ; 298 }
 533  001b 80            	iret
 556                     ; 308  INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
 556                     ; 309 {
 557                     	switch	.text
 558  001c               f_TIM3_UPD_OVF_BRK_IRQHandler:
 562                     ; 313 }
 565  001c 80            	iret
 588                     ; 320  INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
 588                     ; 321 {
 589                     	switch	.text
 590  001d               f_TIM3_CAP_COM_IRQHandler:
 594                     ; 325 }
 597  001d 80            	iret
 620                     ; 335  INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
 620                     ; 336 {
 621                     	switch	.text
 622  001e               f_UART1_TX_IRQHandler:
 626                     ; 340 }
 629  001e 80            	iret
 652                     ; 347  INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
 652                     ; 348 {
 653                     	switch	.text
 654  001f               f_UART1_RX_IRQHandler:
 658                     ; 352 }
 661  001f 80            	iret
 683                     ; 360 INTERRUPT_HANDLER(I2C_IRQHandler, 19)
 683                     ; 361 {
 684                     	switch	.text
 685  0020               f_I2C_IRQHandler:
 689                     ; 365 }
 692  0020 80            	iret
 715                     ; 399  INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
 715                     ; 400 {
 716                     	switch	.text
 717  0021               f_UART3_TX_IRQHandler:
 721                     ; 404   }
 724  0021 80            	iret
 747                     ; 411  INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
 747                     ; 412 {
 748                     	switch	.text
 749  0022               f_UART3_RX_IRQHandler:
 753                     ; 416   }
 756  0022 80            	iret
 778                     ; 425  INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
 778                     ; 426 {
 779                     	switch	.text
 780  0023               f_ADC2_IRQHandler:
 784                     ; 431 }
 787  0023 80            	iret
 810                     ; 465  INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
 810                     ; 466 {
 811                     	switch	.text
 812  0024               f_TIM4_UPD_OVF_IRQHandler:
 816                     ; 470 }
 819  0024 80            	iret
 842                     ; 478 INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
 842                     ; 479 {
 843                     	switch	.text
 844  0025               f_EEPROM_EEC_IRQHandler:
 848                     ; 483 }
 851  0025 80            	iret
 863                     	xdef	f_EEPROM_EEC_IRQHandler
 864                     	xdef	f_TIM4_UPD_OVF_IRQHandler
 865                     	xdef	f_ADC2_IRQHandler
 866                     	xdef	f_UART3_TX_IRQHandler
 867                     	xdef	f_UART3_RX_IRQHandler
 868                     	xdef	f_I2C_IRQHandler
 869                     	xdef	f_UART1_RX_IRQHandler
 870                     	xdef	f_UART1_TX_IRQHandler
 871                     	xdef	f_TIM3_CAP_COM_IRQHandler
 872                     	xdef	f_TIM3_UPD_OVF_BRK_IRQHandler
 873                     	xdef	f_TIM2_CAP_COM_IRQHandler
 874                     	xdef	f_TIM2_UPD_OVF_BRK_IRQHandler
 875                     	xdef	f_TIM1_UPD_OVF_TRG_BRK_IRQHandler
 876                     	xdef	f_TIM1_CAP_COM_IRQHandler
 877                     	xdef	f_SPI_IRQHandler
 878                     	xdef	f_CAN_TX_IRQHandler
 879                     	xdef	f_CAN_RX_IRQHandler
 880                     	xdef	f_EXTI_PORTE_IRQHandler
 881                     	xdef	f_EXTI_PORTD_IRQHandler
 882                     	xdef	f_EXTI_PORTC_IRQHandler
 883                     	xdef	f_EXTI_PORTB_IRQHandler
 884                     	xdef	f_EXTI_PORTA_IRQHandler
 885                     	xdef	f_CLK_IRQHandler
 886                     	xdef	f_AWU_IRQHandler
 887                     	xdef	f_TLI_IRQHandler
 888                     	xdef	f_TRAP_IRQHandler
 889                     	xref.b	c_lreg
 908                     	end
