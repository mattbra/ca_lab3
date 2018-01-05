; Export symbols
        XDEF initAD, updateAD
        

; Import symbols
         XREF clockEvent, ad_value
; Include derivative specific macros
        INCLUDE 'mc9s12dp256.inc'

; RAM: Variable data section
.data:  SECTION


; ROM: Constant data
.const: SECTION

; ROM: Code section
.init:  SECTION


initAD:

    MOVB #$C0, ATD0CTL2
    MOVB #$08, ATD0CTL3
    MOVB #$05, ATD0CTL4
    MOVB #$87, ATD0CTL5
    
    RTS
    
updateAD:
    MOVB #$87, ATD0CTL5
 
wait:           BRCLR ATD0STAT0, #$80, wait 
    
    LDD ATD0DR0
    STD ad_value
    RTS