WDKEY	.set	0x7025
DEL		.set	83 					  ;F(MHz) - 17

       .def _DelayUs
       .text

_DelayUs:
        MOVB AH, #0                   ;Zero AH
        PUSH ST1                      ;Save ST1 to preserve EALLOW setting

DelayUs1:                            ;Outer loop

;Service the watchdog in case it is active
        EALLOW
        MOVZ DP, #(WDKEY>>6)
        MOV @WDKEY, #0x0055
        MOV @WDKEY, #0x00AA
        EDIS

;Proceed with the inner loop
        RPT #DEL                      ;Inner loop
     || NOP

        SUBB ACC,#1                   ;Decrement outer loop counter
        BF DelayUs1, GT              ;Branch for outer loop

;Finish up
        POP ST1                       ;Restore ST1
        LRETR                         ;Return

;end of function DelayUs() ******************************************


;end of file DSP280x_DelayUs.asm
       .end
