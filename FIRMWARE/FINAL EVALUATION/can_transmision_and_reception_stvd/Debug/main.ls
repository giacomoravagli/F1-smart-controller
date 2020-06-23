   1                     ; C Compiler for STM8 (COSMIC Software)
   2                     ; Parser V4.11.13 - 05 Feb 2019
   3                     ; Generator (Limited) V4.4.10 - 25 Sep 2019
  14                     	bsct
  15  0000               _ID:
  16  0000 00000000      	dc.l	0
  17  0004               _tx_buffer:
  18  0004 00            	dc.b	0
  19  0005 00            	dc.b	0
  20  0006               _rx_buffer:
  21  0006 00            	dc.b	0
  22  0007 00            	dc.b	0
  23  0008 00            	dc.b	0
  24  0009 00            	dc.b	0
  25  000a 00            	dc.b	0
  26  000b 00            	dc.b	0
  27  000c 00            	dc.b	0
  28  000d 00            	dc.b	0
  93                     ; 25 void main(void)
  93                     ; 26 {
  95                     	switch	.text
  96  0000               _main:
  98  0000 88            	push	a
  99       00000001      OFST:	set	1
 102                     ; 28 	bool toggle = FALSE;
 104  0001 0f01          	clr	(OFST+0,sp)
 106                     ; 30 	setup();
 108  0003 ad23          	call	_setup
 110  0005               L34:
 111                     ; 61 		tx_buffer[0] = 0x00;
 113  0005 3f04          	clr	_tx_buffer
 114                     ; 63 		switch(toggle)
 116  0007 7b01          	ld	a,(OFST+0,sp)
 118                     ; 73 						break;
 119  0009 4d            	tnz	a
 120  000a 2709          	jreq	L5
 121  000c 4a            	dec	a
 122  000d 2608          	jrne	L15
 123                     ; 67 						tx_buffer[1] = 0xFF;
 125  000f 35ff0005      	mov	_tx_buffer+1,#255
 126                     ; 68 						break;
 128  0013 2002          	jra	L15
 129  0015               L5:
 130                     ; 72 						tx_buffer[1] = 0x00;
 132  0015 3f05          	clr	_tx_buffer+1
 133                     ; 73 						break;
 135  0017               L15:
 136                     ; 77 		send_data_to_CAN_bus();
 138  0017 cd00f0        	call	_send_data_to_CAN_bus
 140                     ; 80 		toggle ^= 1;
 142  001a 7b01          	ld	a,(OFST+0,sp)
 143  001c a801          	xor	a,#1
 144  001e 6b01          	ld	(OFST+0,sp),a
 146                     ; 81 		delay_ms(50);
 148  0020 ae0032        	ldw	x,#50
 149  0023 cd0000        	call	_delay_ms
 152  0026 20dd          	jra	L34
 178                     ; 121 void setup(void)
 178                     ; 122 {
 179                     	switch	.text
 180  0028               _setup:
 184                     ; 123 	setup_clock();
 186  0028 ad06          	call	_setup_clock
 188                     ; 124 	setup_GPIO();
 190  002a ad79          	call	_setup_GPIO
 192                     ; 125 	setup_CAN();
 194  002c cd00b7        	call	_setup_CAN
 196                     ; 126 }
 199  002f 81            	ret
 232                     ; 128 void setup_clock(void)
 232                     ; 129 {
 233                     	switch	.text
 234  0030               _setup_clock:
 238                     ; 130 	CLK_DeInit();
 240  0030 cd0000        	call	_CLK_DeInit
 242                     ; 132 	CLK_HSECmd(DISABLE);
 244  0033 4f            	clr	a
 245  0034 cd0000        	call	_CLK_HSECmd
 247                     ; 133 	CLK_LSICmd(DISABLE);
 249  0037 4f            	clr	a
 250  0038 cd0000        	call	_CLK_LSICmd
 252                     ; 134 	CLK_HSICmd(ENABLE);
 254  003b a601          	ld	a,#1
 255  003d cd0000        	call	_CLK_HSICmd
 258  0040               L57:
 259                     ; 135 	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
 261  0040 ae0102        	ldw	x,#258
 262  0043 cd0000        	call	_CLK_GetFlagStatus
 264  0046 4d            	tnz	a
 265  0047 27f7          	jreq	L57
 266                     ; 137 	CLK_ClockSwitchCmd(ENABLE);
 268  0049 a601          	ld	a,#1
 269  004b cd0000        	call	_CLK_ClockSwitchCmd
 271                     ; 138 	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
 273  004e 4f            	clr	a
 274  004f cd0000        	call	_CLK_HSIPrescalerConfig
 276                     ; 139 	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 278  0052 a680          	ld	a,#128
 279  0054 cd0000        	call	_CLK_SYSCLKConfig
 281                     ; 141 	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 283  0057 4b01          	push	#1
 284  0059 4b00          	push	#0
 285  005b ae01e1        	ldw	x,#481
 286  005e cd0000        	call	_CLK_ClockSwitchConfig
 288  0061 85            	popw	x
 289                     ; 143 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_CAN, ENABLE);
 291  0062 ae1701        	ldw	x,#5889
 292  0065 cd0000        	call	_CLK_PeripheralClockConfig
 294                     ; 144 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
 296  0068 ae0001        	ldw	x,#1
 297  006b cd0000        	call	_CLK_PeripheralClockConfig
 299                     ; 145 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
 301  006e ae0101        	ldw	x,#257
 302  0071 cd0000        	call	_CLK_PeripheralClockConfig
 304                     ; 146 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3, ENABLE);
 306  0074 ae0301        	ldw	x,#769
 307  0077 cd0000        	call	_CLK_PeripheralClockConfig
 309                     ; 148 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
 311  007a ae1300        	ldw	x,#4864
 312  007d cd0000        	call	_CLK_PeripheralClockConfig
 314                     ; 149 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
 316  0080 ae1200        	ldw	x,#4608
 317  0083 cd0000        	call	_CLK_PeripheralClockConfig
 319                     ; 150 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
 321  0086 ae0200        	ldw	x,#512
 322  0089 cd0000        	call	_CLK_PeripheralClockConfig
 324                     ; 151 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
 326  008c ae0700        	ldw	x,#1792
 327  008f cd0000        	call	_CLK_PeripheralClockConfig
 329                     ; 152 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
 331  0092 ae0500        	ldw	x,#1280
 332  0095 cd0000        	call	_CLK_PeripheralClockConfig
 334                     ; 153 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, DISABLE);
 336  0098 ae0600        	ldw	x,#1536
 337  009b cd0000        	call	_CLK_PeripheralClockConfig
 339                     ; 154 	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
 341  009e ae0400        	ldw	x,#1024
 342  00a1 cd0000        	call	_CLK_PeripheralClockConfig
 344                     ; 155 }
 347  00a4 81            	ret
 372                     ; 157 void setup_GPIO(void)
 372                     ; 158 {
 373                     	switch	.text
 374  00a5               _setup_GPIO:
 378                     ; 160 	GPIO_DeInit(GPIOD);
 380  00a5 ae500f        	ldw	x,#20495
 381  00a8 cd0000        	call	_GPIO_DeInit
 383                     ; 161 	GPIO_Init(GPIOD, ((GPIO_Pin_TypeDef)(GPIO_PIN_0 | GPIO_PIN_3)), GPIO_MODE_OUT_PP_LOW_FAST);
 385  00ab 4be0          	push	#224
 386  00ad 4b09          	push	#9
 387  00af ae500f        	ldw	x,#20495
 388  00b2 cd0000        	call	_GPIO_Init
 390  00b5 85            	popw	x
 391                     ; 166 }
 394  00b6 81            	ret
 421                     ; 168 void setup_CAN(void)
 421                     ; 169 {
 422                     	switch	.text
 423  00b7               _setup_CAN:
 427                     ; 170 	CAN_DeInit();
 429  00b7 cd0000        	call	_CAN_DeInit
 431                     ; 172 	CAN_Init(CAN_MasterCtrl_NoAutoReTx,
 431                     ; 173 						CAN_Mode_Normal,
 431                     ; 174 						CAN_SynJumpWidth_1TimeQuantum,
 431                     ; 175 						CAN_BitSeg1_8TimeQuantum,
 431                     ; 176 						CAN_BitSeg2_7TimeQuantum,
 431                     ; 177 						1);
 433  00ba 4b01          	push	#1
 434  00bc 4b60          	push	#96
 435  00be 4b07          	push	#7
 436  00c0 4b00          	push	#0
 437  00c2 ae1000        	ldw	x,#4096
 438  00c5 cd0000        	call	_CAN_Init
 440  00c8 5b04          	addw	sp,#4
 441                     ; 179 	CAN_FilterInit(CAN_FilterNumber_0,
 441                     ; 180 									ENABLE,
 441                     ; 181 									CAN_FilterMode_IdMask/*CAN_FilterMode_IdList*/,
 441                     ; 182 									CAN_FilterScale_32Bit,
 441                     ; 183 									0x00,  
 441                     ; 184 									0x00/*0x08*/,
 441                     ; 185 									0x00/*0x24*/,
 441                     ; 186 									0x00/*0x68*/,
 441                     ; 187 									0x00/*0x00*/,
 441                     ; 188 									0x00/*0x08*/,
 441                     ; 189 									0x00/*0x24*/,
 441                     ; 190 									0x00/*0x68*/);			//Check messages from ID: 0x1234  // Check Filter Register Table
 443  00ca 4b00          	push	#0
 444  00cc 4b00          	push	#0
 445  00ce 4b00          	push	#0
 446  00d0 4b00          	push	#0
 447  00d2 4b00          	push	#0
 448  00d4 4b00          	push	#0
 449  00d6 4b00          	push	#0
 450  00d8 4b00          	push	#0
 451  00da 4b06          	push	#6
 452  00dc 4b00          	push	#0
 453  00de ae0001        	ldw	x,#1
 454  00e1 cd0000        	call	_CAN_FilterInit
 456  00e4 5b0a          	addw	sp,#10
 457                     ; 192 	CAN_ITConfig(CAN_IT_FMP,ENABLE);
 459  00e6 4b01          	push	#1
 460  00e8 ae0002        	ldw	x,#2
 461  00eb cd0000        	call	_CAN_ITConfig
 463  00ee 84            	pop	a
 464                     ; 193 }
 467  00ef 81            	ret
 495                     ; 195 void send_data_to_CAN_bus(void)
 495                     ; 196 {
 496                     	switch	.text
 497  00f0               _send_data_to_CAN_bus:
 501                     ; 198 	CAN_Transmit(0x0496,  CAN_Id_Extended, CAN_RTR_Data, 2, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */
 503  00f0 ae0004        	ldw	x,#_tx_buffer
 504  00f3 89            	pushw	x
 505  00f4 4b02          	push	#2
 506  00f6 4b00          	push	#0
 507  00f8 4b40          	push	#64
 508  00fa ae0496        	ldw	x,#1174
 509  00fd 89            	pushw	x
 510  00fe ae0000        	ldw	x,#0
 511  0101 89            	pushw	x
 512  0102 cd0000        	call	_CAN_Transmit
 514  0105 5b09          	addw	sp,#9
 515                     ; 200 	GPIO_WriteReverse(GPIOD, GPIO_PIN_0);
 517  0107 4b01          	push	#1
 518  0109 ae500f        	ldw	x,#20495
 519  010c cd0000        	call	_GPIO_WriteReverse
 521  010f 84            	pop	a
 522                     ; 201 	delay_ms(60);
 524  0110 ae003c        	ldw	x,#60
 525  0113 cd0000        	call	_delay_ms
 527                     ; 202 }
 530  0116 81            	ret
 644                     ; 204 void receive_data_from_CAN_bus(void)
 644                     ; 205 {
 645                     	switch	.text
 646  0117               _receive_data_from_CAN_bus:
 648  0117 5204          	subw	sp,#4
 649       00000004      OFST:	set	4
 652                     ; 209 	unsigned char i = 0x00;
 654                     ; 210 	unsigned char DLC = 0x00;
 656                     ; 212 	CAN_Receive();
 658  0119 cd0000        	call	_CAN_Receive
 660                     ; 214 	ID = CAN_GetReceivedId();
 662  011c cd0000        	call	_CAN_GetReceivedId
 664  011f ae0000        	ldw	x,#_ID
 665  0122 cd0000        	call	c_rtol
 667                     ; 215 	IDE = CAN_GetReceivedIDE();
 669  0125 cd0000        	call	_CAN_GetReceivedIDE
 671  0128 6b01          	ld	(OFST-3,sp),a
 673                     ; 216 	RTR = CAN_GetReceivedRTR();
 675  012a cd0000        	call	_CAN_GetReceivedRTR
 677  012d 6b02          	ld	(OFST-2,sp),a
 679                     ; 217 	DLC = CAN_GetReceivedDLC();
 681  012f cd0000        	call	_CAN_GetReceivedDLC
 683  0132 6b03          	ld	(OFST-1,sp),a
 685                     ; 219 	for(i = 0x00; i < DLC; i++)
 687  0134 0f04          	clr	(OFST+0,sp)
 690  0136 200f          	jra	L702
 691  0138               L302:
 692                     ; 221 		rx_buffer[i] = CAN_GetReceivedData(i);
 694  0138 7b04          	ld	a,(OFST+0,sp)
 695  013a 5f            	clrw	x
 696  013b 97            	ld	xl,a
 697  013c 89            	pushw	x
 698  013d 7b06          	ld	a,(OFST+2,sp)
 699  013f cd0000        	call	_CAN_GetReceivedData
 701  0142 85            	popw	x
 702  0143 e706          	ld	(_rx_buffer,x),a
 703                     ; 219 	for(i = 0x00; i < DLC; i++)
 705  0145 0c04          	inc	(OFST+0,sp)
 707  0147               L702:
 710  0147 7b04          	ld	a,(OFST+0,sp)
 711  0149 1103          	cp	a,(OFST-1,sp)
 712  014b 25eb          	jrult	L302
 713                     ; 224 	if((IDE == CAN_Id_Extended) && (RTR == CAN_RTR_Data)) 
 715  014d 7b01          	ld	a,(OFST-3,sp)
 716  014f a140          	cp	a,#64
 717  0151 2613          	jrne	L312
 719  0153 0d02          	tnz	(OFST-2,sp)
 720  0155 260f          	jrne	L312
 721                     ; 228 		GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
 723  0157 4b08          	push	#8
 724  0159 ae500f        	ldw	x,#20495
 725  015c cd0000        	call	_GPIO_WriteReverse
 727  015f 84            	pop	a
 728                     ; 229 		delay_ms(600);
 730  0160 ae0258        	ldw	x,#600
 731  0163 cd0000        	call	_delay_ms
 733  0166               L312:
 734                     ; 232 	if((IDE == CAN_Id_Extended) && (RTR == CAN_RTR_Remote))
 736  0166 7b01          	ld	a,(OFST-3,sp)
 737  0168 a140          	cp	a,#64
 738  016a 2615          	jrne	L512
 740  016c 7b02          	ld	a,(OFST-2,sp)
 741  016e a120          	cp	a,#32
 742  0170 260f          	jrne	L512
 743                     ; 236 		GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
 745  0172 4b08          	push	#8
 746  0174 ae500f        	ldw	x,#20495
 747  0177 cd0000        	call	_GPIO_WriteReverse
 749  017a 84            	pop	a
 750                     ; 237 		delay_ms(1600);
 752  017b ae0640        	ldw	x,#1600
 753  017e cd0000        	call	_delay_ms
 755  0181               L512:
 756                     ; 239 }
 759  0181 5b04          	addw	sp,#4
 760  0183 81            	ret
 794                     ; 354 void Delay (uint16_t nCount)
 794                     ; 355 {
 795                     	switch	.text
 796  0184               _Delay:
 798  0184 89            	pushw	x
 799       00000000      OFST:	set	0
 802  0185 2007          	jra	L142
 803  0187               L532:
 804                     ; 357   for (; nCount != 0; nCount--);
 807  0187 1e01          	ldw	x,(OFST+1,sp)
 808  0189 1d0001        	subw	x,#1
 809  018c 1f01          	ldw	(OFST+1,sp),x
 810  018e               L142:
 813  018e 1e01          	ldw	x,(OFST+1,sp)
 814  0190 26f5          	jrne	L532
 815                     ; 358 }
 818  0192 85            	popw	x
 819  0193 81            	ret
 863                     	xdef	_main
 864                     	xdef	_receive_data_from_CAN_bus
 865                     	xdef	_send_data_to_CAN_bus
 866                     	xdef	_setup_CAN
 867                     	xdef	_setup_GPIO
 868                     	xdef	_setup_clock
 869                     	xdef	_setup
 870                     	xdef	_rx_buffer
 871                     	xdef	_tx_buffer
 872                     	xdef	_ID
 873                     	xref	_delay_ms
 874                     	xdef	_Delay
 875                     	xref	_GPIO_WriteReverse
 876                     	xref	_GPIO_Init
 877                     	xref	_GPIO_DeInit
 878                     	xref	_CLK_GetFlagStatus
 879                     	xref	_CLK_SYSCLKConfig
 880                     	xref	_CLK_HSIPrescalerConfig
 881                     	xref	_CLK_ClockSwitchConfig
 882                     	xref	_CLK_PeripheralClockConfig
 883                     	xref	_CLK_ClockSwitchCmd
 884                     	xref	_CLK_LSICmd
 885                     	xref	_CLK_HSICmd
 886                     	xref	_CLK_HSECmd
 887                     	xref	_CLK_DeInit
 888                     	xref	_CAN_GetReceivedData
 889                     	xref	_CAN_GetReceivedDLC
 890                     	xref	_CAN_GetReceivedRTR
 891                     	xref	_CAN_GetReceivedIDE
 892                     	xref	_CAN_GetReceivedId
 893                     	xref	_CAN_Receive
 894                     	xref	_CAN_Transmit
 895                     	xref	_CAN_ITConfig
 896                     	xref	_CAN_FilterInit
 897                     	xref	_CAN_Init
 898                     	xref	_CAN_DeInit
 917                     	xref	c_rtol
 918                     	end
