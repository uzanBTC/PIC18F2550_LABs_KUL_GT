;***********************************************************
; File Header
;***********************************************************

    list p=18F2550, r=hex, n=0
    #include <p18F2550.inc>

X1 equ  0x00	;VALUE

;***********************************************************
; Reset Vector
;***********************************************************

    ORG     0x1000   ; Reset Vector
    		     ; When debugging:0x000; when loading: 0x800
    GOTO    START


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

    
    clrf    PORTA	;Initialize PORTA by clearing output data latches
    movlw   0xFF	; 00100000      Value used to initialize data direction!! use RA5 as input to ensure slave select
    movwf   TRISA	;Set PORTA as input
    clrf    PORTB	; Initialize PORTB by clearing output data latches
    movlw   0x03	;0000 0011  RB0,RB1 AS INPUT Value used to initialize data direction
    movwf   TRISB  
    clrf    PORTC 	; Initialize PORTB by clearing output data latches
    movlw   0x00 	; Value used to initialize data direction
    movwf   TRISC  
    movlw   0x0F 
    
ADCCONFIG
    movwf   CCP2CON		;first close ADconversation -then open it 
    movlw   0x05 		;Select A/D input channel -1 A 7 D //00001110
    movwf   T2CON		;postscale=1:1;prescale=4
    movlw   0xFF		; PR2 IS 255
    movwf   PR2		;
    
    
    movlw   0x07 		; Configure comparators for digital input 0000 0111 turn comparator off
    movwf   CMCON
   
    ; !!!!PORTA is for value to output!!! TRISA is for setting the portA!!!
    bcf     UCON,3		; to be sure to disable USB module
    bsf     UCFG,3		; disable internal USB transceiver
    
    
    movlw   0x00
    movwf   SSPSTAT
    movlw   0x25   ;0010 0101  ss disabled
    movwf   SSPCON1
    
    bsf INTCON,GIE
    BSF INTCON,PEIE
    BSF PIE1,SSPIE
    BCF PIR1,SSPIF
    
   ; goto    main
    
 
main
    clrf   CCPR2L
    goto    loop
    
loop
    goto loop;
    
inter_high
    nop
    BTFSC PIR1,SSPIF     ;0,skip
    MOVFF SSPBUF,CCPR2L
    BCF PIR1,SSPIF
    RETFIE
inter_low
    nop
    retfie

    

    END
