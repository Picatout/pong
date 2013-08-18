;NAME: pic10f322_m.inc
;DESCRIPTION: macros utiles lors de l'utilisation du PIC10F322
;AUTEUR: Jacques Deschênes
;DATE: 2012-10-17
;DERNIÈRE RÉVISION: 2012-11-12

#include P10F322.INC

  constant MAXRAM = 0x7F
  constant MAXFLASH = D'512'
  constant MINRAM = 0x40
  constant BANKS = 1 ; 1 seule banque de RAM
  constant PAGES = 2 ; 2 pages de FLASH
  constant ROWSIZE = D'16' ; nombre de mots dans 1 ligne pour la programmation FLASH

#define _MCLRE_REALLY_OFF ( _MCLRE_OFF & _LVP_OFF )

#define default_config  __config _MCLRE_REALLY_OFF & _WDTE_SWDTEN & _FOSC_INTOSC

wait_hfosc_stable macro ; attend que l'oscillateur haute fréquence interne soit stable
   btfss OSCCON, HFIOFR
   goto $-1
   btfss OSCCON, HFIOFS
   goto $-1
   endm

wait_lfosc_ready macro ; attend que l'oscillateur interne basse fréquence
   btfss OSCCON, LFIOFR ; soit prêt
   goto $-1
   endm

clear_ram macro
; mise à zéro de la RAM
    movlw MINRAM
    movwf FSR
clear_ram_loop
    clrf INDF
    incf FSR
    btfss FSR,7
    goto clear_ram_loop
    endm


basic_setup macro ; désactive tous les périphériques.
   clrf INTCON ; désactivation interruption
   clrf ANSELA
   bcf CLKRCON, CLKROE ; pas de sortie de Fosc/4
   bcf FVRCON, FVREN ; désactive la référence de voltage fixe
   bcf T2CON, TMR2ON ; désactive le timer2
   bcf PWM1CON, PWM1EN ; désactive le PWM1
   bcf PWM2CON, PWM2EN ; désactive le PWM2
   bcf CLC1CON, LC1EN ; désactive CLC1
   bcf NCO1CON, N1EN ; désactive le NCO
   bcf CWG1CON0, G1EN ; désactive le CWG1
   clrf OPTION_REG
   bsf OPTION_REG, PSA ; pré-scale désactivé
   movlw 0xF
   movwf TRISA ; toutes des entrées
   movwf WPUA  ; active les pullup
   endm

  constant OUT = 0
  constant INP = 1
set_io_dir macro pin, dir ; établi une broche comme entrée ou sortie
  if dir
    bsf TRISA, pin  ; input
  else
    bcf TRISA, pin  ; output
  endif
  endm

set_clk_freq macro freq; initialize clock frequency
     movlw B'00001111'
     andwf OSCCON, F
   if freq == D'31'
      exitm
   endif
   if freq == D'250'
      movlw B'00010000'
      iorwf OSCCON, F
      exitm
   endif
   if freq == D'500'
      movlw B'00100000'
      iorwf OSCCON, F
      exitm
   endif
   if freq == D'1'
      movlw B'00110000'
      iorwf OSCCON, F
      exitm
   endif
   if freq == D'2'
      movlw B'01000000'
      iorwf OSCCON, F
      exitm
   endif
   if freq == D'4'
      movlw B'01010000'
      iorwf OSCCON, F
      exitm
   endif
   if freq == D'8'
     movlw B'01100000'
     iorwf OSCCON, F
     exitm
   endif
   if freq == D'16'
     movlw B'01110000'
     iorwf OSCCON, F
   endif
   endm

enable_clk_out macro ; active la sortie Fosc/4 sur RA2
   bcf NCO1CON, N1OE
   bsf CLKRCON, CLKROE
   endm

disable_clk_out macro ; désactive la sortie Fosc/4 sur RA2
   bcf CLKRCON, CLKROE
   endm

enable_interrupt macro ; active les interruption
   bsf INTCON, GIE
   endm

disable_interrupt macro ; désactive les interruption
   bcf INTCON, GIE
   endm

enable_periph_int macro ; active les interruptions périphériques
   bsf INTCON, PEIE
   endm

disable_periph_int macro ; désactive les interruptions périphérique
   bcf INTCON, PEIE
   endm

enable_ext_int macro ; active l'interruption externe
   bsf INTCON, INTE
   endm

disable_ext_int macro ; désactive les interruption externe
   bcf INTCON, INTE
   endm

bra_ext_int macro address ; branchement sur interruption externe
   btfsc INTCON, INTF
   goto address
   endm

enable_tmr0_int macro ; active l'interruption du timer0
   bcf INTCON, TMR0IF
   bsf INTCON, TMR0IE
   endm

disable_tmr0_int macro ; désactive l'interruption du timer0
  bcf INTCON, TMR0IE
  endm

bra_tmr0_int macro address ; branchement sur interruption timer0
  btfsc INTCON, TMR0IF
  goto address
  endm

enable_tmr2_int macro ; active les interruptions sur timer2
    bsf PIE1, TMR2IE
    endm

disable_tmr2_int macro ; désactive les interruptions sur timer2
    bcf PIE1, TMR2IE
    endm

;;;;;;;;;;;;;  IOC  interrupt changement d'état de broche  ;;;;;;;;;;;;;;;

enable_ioc_int macro ; active les interruptions sur changement d'état E/S
  bcf INTCON, IOCIF
  clrf IOCAF
  bsf INTCON, IOCIE
  endm

disable_ioc_int macro ; désactive les interruptions sur changement d'état E/S
  bcf INTCON, IOCIE
  endm

  constant IOC_RISING = 0
  constant IOC_FALLING = 1
  constant IOC_BOTH = 2
set_ioc_edge macro pin, edge ; sur quel changement doit se produire l'interruption
  if edge == IOC_RISING
  bsf IOCAP, pin
  bcf IOCAN, pin
  exitm
  endif
  if edge == IOC_FALLING
  bsf IOCAN, pin
  bcf IOCAP, pin
  exitm
  endif
  bsf IOCAN, pin
  bsf IOCAP, pin
  endm

clear_ioc_flag macro ; remet à zéro les drapeaux indicateurs
   movlw 0xF0
   andwf IOCAF,F
   endm

bra_ioc_int macro address ; branche sur interruption causé par un changement
  btfsc INTCON, IOCIF     ; d'état E/S
  goto address
  endm

clear_ioc_if macro
;remet à zéro le drapeu d'interruption
    bcf INTCON, IOCIF
    movlw H'FF'
    xorwf IOCAF, W
    andwf IOCAF, F
    endm


;;;;;;;;;;    adc interrtupt ;;;;;;;;;;;;;;;;;;;

enable_adc_int macro ; active les interruptions sur ADC
  bsf PIE1, ADIE
  endm

disable_adc_int macro ; désactive les interruptions sur ADC
  bcf PIE1, ADIE
  endm

bra_adc_int macro address ; branchement sur interruption ADC
  btfsc PIR1, ADCIF
  goto address
  endm

enable_nco1_int macro ; active les interruptions sur nco1
  bsf PIE1, NCO1IE
  endm

disable_nco1_int macro ; désactive les interruptions sur nco1
  bcf PIE1, NCO1IE
  endm

bra_nco1_int macro address ; branchement sur interruption nco1
  btfsc PIR1, NCO1IF
  goto address
  endm

enable_clc1_int macro ; active les interruptions sur clc1
  bsf PIE1, CLC1IE
  endm

disable_clc1_int macro ; désactive les interruptions sur clc1
  bcf PIE1, CLC1IE
  endm

bra_clc1_int macro address ; branchement sur interruption clc1
  btfsc PIR1, CLC1IF
  goto address
  endm

bra_tmr2_int macro address ; branchement sur interruption timer2
  btfsc PIR1, TMR2IF
  goto address
  endm

  constant IOC_INT  = 0
  constant EXT_INT  = 1
  constant TMR0_INT = 2
  constant TMR2_INT = D'8'+1
  constant CLC1_INT  = D'8'+3
  constant NCO1_INT  = D'8'+4
  constant ADC_INT  = D'8'+6
bra_int macro int_src, address ;branche à l'adresse s'il la source de
                                  ; l'interruption est int_src
   if int_src < D'8'
      btfsc INTCON, int_src
      goto address
      exitm
   end if
   btfsc PIR1, (int_src - D'8')
   goto address
   endm


;;;;;;;;;;;   convertisseur analogue/numérique ;;;;;;;;;;;;;;;

disable_adc macro ; déssactive le convertisseur A/N
  bcf ADCON, ADON
  clrf ANSELA
  endm

enable_adc macro ; active le convertisseur mais ne sélectionne pas de d'entrée
  clrf ANSELA    ; aucune entrée ANx de sélectionnée.
  movlw B'00011111' ; efface la sélection freq. d'horlorge
  andwf ADCON, F
  movlw B'01100001' ; horloge = Frc, ADON=1
  iorwf ADCON, F
  endm

  constant ADC_CLK_FOSCDIV2 = 0
  constant ADC_CLK_FOSCDIV8 = 1
  constant ADC_CLK_FOSCDIV32 = 2
  constant ADC_CLK_FRC = 3
  constant ADC_CLK_FOSCDIV4 = 4
  constant ADC_CLK_FOSCDIV16 = 5
  constant ADC_CLK_FOSCDIV64 = 6
  constant ADC_CLK_FRC2 = 7
select_adc_clk macro clk ; sélection du signal horloge pour la conversion A/N
  movlw clk << 5
  iorwf ADCON
  endm

  constant INP_MODE_DIG = 0
  constant INP_MODE_AN = 1
set_inp_mode macro pin, mode   ; sélectionne le mode d'entrée d'une broche digital ou analogue
  if mode == INP_MODE_DIG      ; pin = {RA0,RA1,RA2}
     bcf ANSELA, pin           ; mode = {INP_MODE_DIG, INP_MODE_AN}
  else
     bsf ANSELA, pin
  endif
  endm

; canaux lecture analogue/numérique
  constant CH_AN0 =  B'00000000'
  constant CH_AN1 =  B'00000100'
  constant CH_AN2 =  B'00001000'
  constant CH_TEMP = B'00011000'
  constant CH_FVR =  B'00011100'
select_adc_channel  macro ch ; sélection sur quel canal faire la lecture
   movlw B'11100011'
   andwf ADCON, F
   movlw ch
   iorwf ADCON, F
   if ch == CH_AN0 
      bsf ANSELA, 0
   endif
   if ch == CH_AN1
      bsf ANSELA, 1
   endif
   if ch == CH_AN2
      bsf ANSELA, 2
   endif
   endm


start_adc macro ; démarre une lecture mais n'attend pas la fin
  bcf PIR1, ADIF
  bsf ADCON, GO_NOT_DONE
  endm

read_adc macro ; démarre et attend que la lecture soit complétée
  btfss ADCON, GO_NOT_DONE
  bsf ADCON, GO_NOT_DONE
  btfsc ADCON, GO_NOT_DONE
  goto $-1
  endm




;;;;;;;;;;;;;;;  FVR: référence de voltage fixe ;;;;;;;;;;;;;

enable_fvr macro  ; active la référence
   bsf FVRCON, FVREN
   btfss FVRCON, FVRRDY
   goto $-1
   endm

disable_fvr macro ; désactive la référence
   bcf FVRCON, FVREN
   endm

  constant FVR_0V = 0
  constant FVR_1V = 1
  constant FVR_2V = 2
  constant FVR_4V = 3
set_fvr_volt macro gain ; défini la valeur de sortie de la référence
   movlw gain
   iorwf FVRCON, F
   endm

;;;;;;;;;;;;;  IVR, régulateur de tension interne ;;;;;;;;;;;;;
enable_power_save macro  ; active l'économie d'énergie en mode sleep
   bsf VREGCON, VREGPM1
   end

disable_power_save macro ; désactive l'économie d'énergie en mode sleep
   bcf VREGCON, VREGPM1
   endm

;;;;;;;;;;;;;        référence température ;;;;;;;;;;;;;;;;;

enable_temp macro ; active le capteur de température
   bsf FVRCON, TSEN
   endm

disable_temp macro ; désactive le capteur de températeur
   bcf FVRCON, TSEN
   endm

  constant TEMP_RANGE_LOW = 0
  constant TEMP_RANGE_HIGH = 1
set_temp_range macro range ; définie le range de lecture du capteur de températeur
   if TEMP_RANGE_LOW == 0
      bcf FVRCON, TSRNG
   else
      bsf FVRCON, TSRNG
   endif
   endm

;;;;;;;;;;;;;;;;;;     NCO générateur numériquement programmable  ;;;;;;;;;;;

enable_nco macro      ; active le générateur
  bsf NCO1CON, N1EN
  endm

disable_nco macro    ; désactive le générateur
  bcf NCO1CON, N1EN
  endm

enable_nco_output macro  ; active la sortie sur RA2
  bsf NCO1CON, N1OE
  endm

disable_nco_output macro  ; désactive la sortie sur RA2
  bcf NCO1CON,N1OE
  endm

  constant NCO_CLK_NCO1CLK = 0 ; entrée broche NCO1
  constant NCO_CLK_FOSC = 1   ; fréquence oscillateur MCU
  constant NCO_CLK_INTOSC = 2  ;oscillateur interne haute fréquence
  constant NCO_CLK_LC1OUT = 3 ; sortie cellule CLC1
set_nco_clock  macro clk  ; défini la source horloge du NCO
  movlw 0xFC
  andwf NCO1CLK, F
  if clk > 0
  movlw clk
  iorwf NCO1CLK, F
  endif
  endm

  constant NCO_POL_LOW = 0
  constant NCO_POL_HIGH = 1
set_nco_output_polarity macro pol  ; défini la polarité active
  if pol == NCO_POL_LOW
    bcf NCO1CON, N1POL
  else
    bsf NCO1CON, N1POL
  endif
  endm

  constant NCO_MODE_SQUARE_WAVE = 0
  constant NCO_MODE_PULSE = 1
set_nco_mode macro mode ;
   if mode == NCO_MODE_PULSE
     bsf NCO1CON, N1PFM
   else
     bcf NCO1CON, N1PFM
   endif
   endm

set_nco_pulse_width macro width ; largeur de l'impulsion en multiple de signal horloge
   movlw 0x1F                   ; width {0-7} pulse_width = 2^^width
   andwf NCO1CLK, F
   movlw width << 5
   iorwf NCO1CLK, F
   endm

set_nco_incr macro incr  ; défini l'incrément du NCO
   movlw incr>>8
   movwf NCO1INCH
   movlw incr & 0xFF
   movwf NCO1INCL
   endm

;;;;;;;;;;;;;;;;;;  TIMER 0 ;;;;;;;;;;;;;;;;;;;;;;;;

  constant TMR0_EXT_CLK = 1 
  constant TMR0_FOSC4 = 0
set_timer0_clock macro clk ; signal utilisé à l'entrée du timer 0
  if   clk == TMR0_EXT_CLK
     bsf OPTION_REG, T0CS
  else
     bcf OPTION_REG, T0CS
  endif
  endm

  constant TMR0_DIV2 = 0
  constant TMR0_DIV4 = 1
  constant TMR0_DIV8 = 2
  constant TMR0_DIV16 = 3
  constant TMR0_DIV32 = 4
  constant TMR0_DIV64 = 5
  constant TMR0_DIV128 = 6
  constant TMR0_DIV256 = 7
set_timer0_prescale macro div ; défini le diviseur du timer0 { 0-7 }
   movlw 0xF0                 ; diviseur = 2^^(div+1)
   andwf OPTION_REG,F
   if div != 0
   movlw div
   iorwf OPTION_REG, F
   endif
   endm

disable_timer0_prescale macro ; désactive le diviseur
   bsf OPTION_REG, PSA
   endm


init_timer0  macro count ; initialise le TIMER 0 avec la valeur count
   movlw count           ; 0 - 255
   movwf TMR0
   endm

wait_timer0_rollover macro ; attend que le count du timer0 revienne à zéro
   movfw TMR0              ; ne fonctionne que si le diviseur >= 4
   skpz
   goto $-2
   endm

;;;;;;;;;;;;;;;;;;;;; TIMER 2 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

enable_timer2 macro    ;activation du timer 2
   bsf T2CON, TMR2ON
   endm

disable_timer2 macro  ; désactivation du timer 2
  bcf T2CON, TMR2ON
  endm

  constant TMR2_DIV1 = 0 ; pas de division
  constant TMR2_DIV4 = 1 ; divise par 4
  constant TMR2_DIV16 = 2 ; divise par 16
  constant TMR2_DIV64 = 3 ; divise par 64
set_timer2_prescale  macro div  ; ajuste le prédiviseur du timer 2
  movlw 0xFC
  andwf T2CON,F
  if div > 0
  movlw div
  iorwf T2CON, F
  endif
  endm

  constant TMR2_PS1 = 0 ; pas de divisition
  constant TMR2_PS2 = 1
  constant TMR2_PS3 = 2
  constant TMR2_PS4 = 3
  constant TMR2_PS5 = 4
  constant TMR2_PS6 = 5
  constant TMR2_PS7 = 6
  constant TMR2_PS8 = 7
  constant TMR2_PS9 = D'8'
  constant TMR2_PS10 = D'9'
  constant TMR2_PS11 = D'10'
  constant TMR2_PS12 = D'11'
  constant TMR2_PS13 = D'12'
  constant TMR2_PS14 = D'13'
  constant TMR2_PS15 = D'14'
  constant TMR2_PS16 = D'15'

set_timer2_postscale macro div ; ajuste le postdiviseur du timer 2
  movlw 0x7
  andwf T2CON, F
  movlw div << 3
  iorwf T2CON, F
  endm

init_timer2 macro count ; charge le registre TMR2 avec count
  movlw count
  movwf TMR2
  endm

init_timer2_period macro count ; charge le registre PR2 avec count
  movlw count
  movwf PR2
  endm

;;;;;;;;;;;;;;;;; module PWM ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  constant PWM_CH1 = 1
  constant PWM_CH2 = 2

enable_pwm_channel macro channel ; active le canal pwm 1 ou 2
   if channel == PWM_CH1
     bsf PWM1CON, PWM1EN
   else
     bsf PWM2CON, PWM2EN
   endif
   endm

disable_pwm_channel macro channel ; désactive le canal pwm 1 ou 2
  if channel == PWM_CH1
    bcf PWM1CON, PWM1EN
  else
    bcf PWM2CON, PWM2EN
  endif
  endm

enable_pwm_output macro channel ; active la sortie pwm du canal 1 ou 2
  if channel == PWM_CH1
    bsf PWM1CON, PWM1OE
  else
    bsf PWM2CON, PWM2OE
  endif
  endm

disable_pwm_output macro channel ; désactive la sortie pwm du canal 1 ou 2
  if channel == PWM_CH1
    bcf PWM1CON, PWM1OE
  else
    bcf PWM2CON, PWM2OE
  endif
  endm

  constant PWM_POL_H = 0
  constant PWM_POL_L = 1
set_pwm_polarity macro channel, pol ; défini la polarité de la sortie
  if channel == PWM_CH1
  if pol==PWM_POL_H
    bcf PWM1CON, PWM1POL
  else
    bsf PWM1CON, PWM1POL
   endif
  else
  if pol==PWM_POL_H
    bcf PWM2CON, PWM2POL
  else
    bsf PWM2CON, PWM2POL
   endif
  endif
  endm

set_pwm_duty_cycle macro channel, duty ; défini le dudy cycle
  if channel == PWM_CH1                
     movlw (duty & 0x3FF) >> 2
     movwf PWM1DCH
     movlw (duty & 0x3) << 6
     movwf PWM1DCL
  else
     movlw (duty & 0x3FF) >> 2
     movwf PWM2DCH
     movlw (duty & 0x3) << 6
     movwf PWM2DCL
  endif
  endm

;;;;;;;;;;;;;  CLC  cellule logique configurable ;;;;;;;;;;;;;;;;

enable_clc macro    ; active la cellule
  bsf CLC1CON, LC1EN
  endm

disable_clc macro   ; désactive la cellule
  bcf CLC1CON, LC1EN
  endm

enable_clc_output macro ; active la sortie
  bsf CLC1CON, LC1OE
  endm

disable_clc_output macro ; désactive la sortie
  bcf CLC1CON, LC1OE
  endm

;;;;;;;;;;;;;;;;;; CWG  générateur d'onde complémentaire  ;;;;;;;;;;;;;;

enable_cwg macro
  bsf CWG1CON0, G1EN
  endm

disable_cwg macro
  bcf CWG1CON0, G1EN
  endm

  constant CWG_OUTA = G1OEA
  constant CWG_OUTB = G1OEB
enable_cwg_output macro out ; active la sortie A ou B
  bsf CWG1CON0, out
  endm

disable_cwg_output macro out ; désactive la sortie A ou B
  bcf CWG1CON0, out
  endm


  constant CWG_INP_PWM1 = 0
  constant CWG_INP_PWM2 = 1
  constant CWG_INP_NCO  = 2
  constant CWG_INP_CLC  = 3

select_cwg_input macro inp ; sélectionne le signal d'entrée
  movlw 0xFC
  andwf CWG1CON1, F
  if inp != 0
  movlw inp
  iorwf CWG1CON1, F
  endif
  endm



;;;;;;;;;;;;;; lecture de la mémoire flash en exécution ;;;;;;;;

read_prog_mem macro addr ; fait la lecture d'un addresse
  movlw high addr        ; le résultat est dans PMDATL et PMDATH
  movwf PMADRH
  movlw low addr
  movwf PMADRL
  bcf PMCON1, CFGS
  bsf  PMCON1, RD
  nop
  nop
  endm



