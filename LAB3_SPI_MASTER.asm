;***********************************************************
;						File Header
;***********************************************************

    title "FSM"
    list p=18F2550, r=hex, n=0
    #include <p18F2550.inc>


;***********************************************************
; Reset Vector
;***********************************************************

    ORG     0x1000    ; Reset Vector
    		     ; When debugging:0x000; when loading: 0x800  1000-> because the bootloader starts at 1000 
    GOTO    START

X1 equ 0x00
;***********************************************************
; Interrupt Vector
;***********************************************************



    ORG     0x1008	; Interrupt Vector  HIGH priority
    GOTO    inter_high	; When debugging:0x008; when loading: 0x808
    ORG     0x1018	; Interrupt Vector  HIGH priority
    GOTO    inter_low	; When debugging:0x008; when loading: 0x808



;***********************************************************
; Program Code Starts Here
;***********************************************************

    ORG     0x1020		; When debugging:0x020; when loading: 0x820

START

    clrf    PORTA 		; Initialize PORTA by clearing output data latches
    movlw   0x3F 		; Value used to initialize data direction
    movwf   TRISA 		; Set RA<5:0> as inputs  0011 1111
    movlw   0x0F 		; Configure A/D for digital inputs 0000 1111
    movwf   ADCON1 		;
    movlw   0x07 		; Configure comparators for digital input
    movwf   CMCON
    clrf    PORTB 		; Initialize PORTB by clearing output data latches
    movlw   0x04		; Value used to initialize data direction
    movwf   TRISB 		; Set PORTB as output
    clrf    PORTC 		; Initialize PORTC by clearing output data latches
    movlw   0x00	        ; Value used to initialize data direction
    movwf   TRISC               
    bcf     UCON,3		
    bsf     UCFG,3
    
SSPConfig
    movlw   0x00
    movwf   SSPSTAT
    movlw   0x20        ;fosc/4         
    movwf   SSPCON1
    
TMR2Config
    movlw   0xFF              
    movwf   PR2
    movlw   0x7E              
    movwf   T2CON
    
ADCConfig
    movlw   0x03
    movwf   ADCON0                
    movlw   0x0E
    movwf   ADCON1                  ;A0 is analog
    movlw   0x3E
    movwf   ADCON2                    ;fosc/4
    
InterruptConf
    bsf     PIE1,6                  ;Enable ADC interrupt
    bsf     INTCON,7               ;GIE
    bsf     INTCON,6              ;PIE
    clrf    SSPBUF
    bcf     PIR1,6                ;ADC FLAG

main    
    goto    main
    
inter_high                        ;We dont need btfss   PIR1,6 because it is when  PIR1,6 =1 the program come here   
    nop
    
loop
    movff   ADRESH,X1
    MOVF X1, W ;WREG reg = contents of SSPBUF
    MOVWF SSPBUF
    BTFSC SSPSTAT, BF ;Has data been received (transmit complete)?
    goto loop
    bcf     PIR1,6 ;clear ad flag
    bsf     ADCON0,1 ;go!
    NOP
    RETFIE
    
inter_low
    
    nop
    retfie     
     
END
