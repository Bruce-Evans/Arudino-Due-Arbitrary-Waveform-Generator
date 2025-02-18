// Due Arbitrary Waveform Generator by Bruce Evans 2017. (Some code adapted from the Magician, Kerry D. Wong, Mark T, MartinL, ard_newie and possibly others. Many thanks!)
// Mostly, Capitals have been used for global variables and lower case for local variables. Any constants have ALL capitals. Although unconventional, I've personally found this more helpful.
//
// To use: Upload this sketch. Connect pins 3 & 7 (preferably with a resistor in case of problems, approx 47 to 100 ohms). The square wave output is best (at high freq) taken from pin 7. The analogue wave output is at DAC0.
//
// Use the accompanying sparate program to control the Arduino from your PC. See its help file (in its data folder) for more info.
//
// or alternatively...
// If you want to use 2 pots to control frequency & duty cycle, use any value pots between 1k & 50k, and connect them between 0V & 3.3V (NOT 5V!) with their wipers connected to A0 & A1.
//  If you want to use switches to control pot operation:
//    Connect a momentary switch between the following pins: (no pull-up resistors required)
//      pin 22 & ground - toggle: enable / disable pots (& LEDs)
//      pin 24 & ground - toggle: Unsync'ed square wave: pot FREQ or PERIOD
//      pin 26 & ground - cycle thru: Unsync'ed square wave: pot freq / period RANGE
//      pin 28 & ground - toggle: Sync'ed waves: pot FREQ or PERIOD
//      pin 30 & ground - cycle thru: Sync'ed waves: pot freq / period RANGE
//      pin 32 & ground - toggle: Unsync'ed square wave: pot DUTY CYCLE or PULSE WIDTH
//      pin 34 & ground - cycle thru: Unsync'ed square wave: pot pulse width RANGE (duty cycle range is fixed at 0 - 100%)
//      pin 36 & ground - toggle: Sync'ed waves: pot DUTY CYCLE or PULSE WIDTH
//      pin 38 & ground - cycle thru: Sync'ed waves: pot pulse width RANGE (duty cycle range is fixed at 0 - 100%)
//      pin 40 & ground - cycle thru: WAVE SHAPE
//      pin 42 & ground - toggle: Exact Freq Mode ON / OFF
//      pin 44 & ground - toggle: Square Wave Sync ON / OFF
//      pin 46 & ground - cycle thru: Which wave/s to control
//  If you want to use LEDs to display pot operation: (not range or wave-shape)
//    Connect a single LED or a dual colour LED or 2 back to back LEDs (in parallel) via a single resistor (approx 560 ohms) between the following pins:
//      pins 23 & ground - indicates pots enabled (use a single LED only)
//      pins 25 & 27 - indicates: Unsync'ed square wave: pot FREQ or PERIOD
//      pins 29 & 31 - indicates: Sync'ed wave: pot FREQ or PERIOD
//      pins 33 & 35 - indicates: Unsync'ed square wave: pot DUTY CYCLE or PULSE WIDTH
//      pins 37 & 39 - indicates: Sync'ed wave: pot DUTY CYCLE or PULSE WIDTH
//      pins 41 & ground - indicates: Exact Freq Mode ON (use a single LED only) (pin 43 is low current, so not used here)
//      pins 45 & ground - indicates: Square Wave Sync ON (use a single LED only)
//      pins 47 & ground - indicates: Analogue wave is being controlled (use a single LED only)
//      pins 49 & ground - indicates: Unsynch'ed Square wave is being controlled (use a single LED only)
//  Note: Sweep and Timer functions are not controllable by the pots & switches.
//  Also note that when pots are enabled, the Arduino will accept commands from the pots when touched AND from the PC via USB when a signal is received.
// 
// or alternatively...
// TO CONTROL FROM THE ARDUINO SERIAL MONITOR:
// Set your serial monitor to 115200 baud.
// Type the following then press enter (or Send)
// Type:   a   to create a new Arbitrary wave. Enter the value (between 0 and 4095) for each waypoint. Up to 4096 points. Stepped points can also be easily entered. On-screen instructions will be given. The points will be evenly spread over the period of the wave.
// Type:   w   to toggle the analogue Wave shape between sine, triangle and arbitrary
// Type:   v   to toggle between Viewing synchronized or unsynchronized square wave                       } to toggle view and control simultaneously:
// Type:  ' '  [spacebar] to toggle between controlling synchronized waves or unsynchronized square wave  } both 'v' & ' ' can be typed (followed by enter)
// Type:   b   to control Both synchronized waves and unsynchronized square wave simultaneously
// Type:   h   to change frequency of wave/s, type required frequency in Hz followed by h. The freq range is: (synchronized waves limit: 0.00005Hz - 100kHz) (unsynchronized square wave limit: 0.00005Hz - 42MHz) via serial cconnection, or slightly less using pots
// Type:   m   to change period of wave/s, type required period in Milliseconds followed by m (synchronized waves limit: 5hrs 33mins 20secs or 20,0000 Secs - 10 MicroSecs) (unsynchronized square wave limit: 10,737Secs - 23.8 nanoSecs)
// Type:   d   to change Duty-cycle type required percentage duty-cycle (0 - 100) followed by d.
// Type:   u   to set pulse width. Type required pulse width in µseconds followed by u. PULSE WIDTH WILL REMAIN FIXED until duty-cycle (above) is set instead.
// Type:   e   to toggle Exact Freq Mode on/off (synchronized waves only) eliminating freq steps, but has lower sample rate, & dithering on synchronized sq. wave & sharp edges (view on oscilliscope with HF filter on)
// Type:   s   to set up the frequency Sweep feature. Follow on-screen instructions.
// Type:   t   to enter or exit the Timer. Follow on-screen instructions.
// Type:   p   to enable or disable the pots
// Type:   P   to toggle between pot controlling duty-cycle Percent, or Pulse width of wave. Synchronized and unsynchronized waves can be set independently by using the ' ' or 'b' command shown above
// Type:   f   to toggle between pot controlling Freq of wave, or period of wave. Synchronized and unsynchronized waves can be set independently by using the ' ' or 'b' command shown above
// Type:   r   to toggle frequency/period Range of the pots: x 1, x10, x100, x1000 & x 10000 (synchronized & unsynchronized ranges are adjustable independently by using the ' ' or 'b' command shown above)
// Type:   R   to toggle pulse width Range of the pot. (does not affect duty-cycle percent): x 1, x10, x100, x1000 & x 10000."); (synchronized & unsynchronized ranges are adjustable independently by using the ' ' or 'b' command shown above)
// Type:   ?   to display the help screen. (shortened version of the above)

/********************************************************/
// Do not adjust if working okay: Type number before delimiter (if altered, will return to default unless variables: MinMaxDuty, Delay1, 2 & 3 are altered accordingly)
// Type:   M   for setup only. Min/Max duty-cycle percentage, expressed in samples (when synchronized & above 1kHz with Exact Freq Mode off) - will return to default (4) when switching from unsynchronized to synchronized square wave (by typing 'v') [settings below 4 cause polarity reversal due to DMA timing]
// Type:   H   for setup only. delay of square wave for best phase synchronization at High sample rate. ie: 10kHz, 20kHz, 40khz & 100kHz [delay (high or low) can't be adjusted below 1.01kHz] - default is 10
// Type:   L   for setup only. delay of square wave for best phase synchronization at Low sample rate. ie: 1.1kHz, 11kHz, 21kHz, & 41kHz (adj high sample rate delay 1st) - default is 36
// Type:   D   for setup only. delay of square wave for best phase synchronization at Low duty-cycle, but not 0%. - default is 110

/********************************************************/

// Common to all Waveforms:
bool UsingGUI;  //  1 = using the Arbitrary Waveform GUI on the PC
bool PotsEnabled = 0; //  <<<<<<<<<<<<<<<<<<<<<<<< CHANGE TO 1 TO START-UP WITH POTS ENABLED <<<<<<<<<<<<<<<<<<<<<<
bool PotPulseWidth[3];   // enable / disable pulse width input from pot instead of duty-cycle % {unsynchronized wave, synchronized waves}
// Range of pots:
int   Range[]  = {1, 100, 1, 1}; // {unsynchronized wave freq/period, synchronized waves freq/period, unsynchronized wave Pulse Width Range, synchronized waves Pulse Width Range}
// Unsynchronized ^        // if PotPeriodMode[0] = 1: Range x1 = low period (24.0 nS - 40.9 µS), x10 = per. x 10, x100 = per. x 100, x1000 = per. x 1000, x10000 = per. x 10000 (up to 409.6 mS)
// Unsynchronized ^        // if PotPeriodMode[0] = 0: Range x1 = low freq   (1.26 Hz - 4.0 kHz), x10 = freq x 10, x100 = freq x 100, x1000 = freq x 1000, x10000 = freq x 10000 (up to 42 MHz)
// Sine/triangle wave  ^   // if PotPeriodMode[1] = 0: Range x1 = low freq   (10 mHz - 40.9  Hz), x10 = freq x 10, x100 = freq x 100, x1000 = freq x 1000, x10000 = freq x 10000 (up to 100 kHz)
// Sine/triangle wave  ^   // if PotPeriodMode[1] = 1: Range x1 = low period  (10 µS - 409.6 µS), x10 = per. x 10, x100 = per. x 100, x1000 = per. x 1000, x10000 = per. x 10000 (up to 4.096 Secs)
volatile boolean SquareWaveSync = LOW; // HIGH;
byte  Control  = 2;        // control of synchronized waves or unsynchronized square wave: 0 = unsync'ed, 1 = sync'ed, 2 = both
int   Pot0     = 1000;     // reading from freq pot <<<<<<<<<<<<<<<<<<<<<< DETERMINES START-UP FREQUENCY, even though pots may be disabled <<<<<<<<<<<<<<<<<<<<<
int   Pot1     = 2000;     // reading from duty-cycle pot
float DutyReading[] = {50, 50}; // duty-cycle reading from the pot {unsynchronized wave, synchronized waves}
float OldReading[3];       // freq/period reading from the pot {unsynchronized wave, synchronized waves}
bool  PotPeriodMode[3];    // {unsynchronized wave, synchronized waves}: 0 = pot adjusts freq of wave, 1 = pot adjusts period of wave
unsigned long SwitchPressedTime;    // for debouncing pot switches
unsigned long LEDupdateTime;        // update pot function LED indicators every 300 mSecs (not every cycle of the loop)
double UserInput       = 0;         // input read from serial connection
char  InputChar;                    // last input character read from serial connection (delimiter)
int   numDecimalPlaces = -1;        // counts number of input digits read from serial connection after '.' detected. Equals -1 until '.' detected
unsigned long TouchedTime = 0; // detects when enter pressed twice within 500 mSecs for triggering Status message
byte  SweepStep;                    // 0 = Sweep off, 1 =  Set SweepMinFreq, 2 = Set SweepMaxFreq, 3 = Set SweepRiseTime, 4 = Set SweepFallTime, 5 = Run
float SweepMinFreq;
float SweepMaxFreq;
int   SweepRiseTime;
int   SweepFallTime;
/********************************************************/
// For the Unsynchronized Square Wave:
uint32_t clkAFreq = 42000000ul;   // system clock 1
//uint32_t pwmFreq  = 42000000ul; // enabled later in program if freq is 1300Hz min (lowest poss is 1281)  - min (duty-cycle) pulse width:  12nSec
uint32_t pwmFreq  = 10500000ul;   // enabled later in program if freq is  650Hz min                (~641)  - min (duty-cycle) pulse width:  24nSec
//uint32_t pwmFreq  = 2;          // enabled later in program if freq is  325Hz min                (~321)  - min (duty-cycle) pulse width:  48nSec
//uint32_t pwmFreq  = 4;          // enabled later in program if freq is  163Hz min                (~161)  - min (duty-cycle) pulse width:  96nSec
volatile uint32_t PulsePeriod[] = {1010, 2020}; // {Pulse, Period}  -  if freq is < 163Hz, used by interupt handler - min (duty-cycle) pulse width: 96nSec
bool     PeriodHalf;     // identifies which half -  if freq is < 163Hz, used by interupt handler
int      uPeriodMultiplier = 2;   // for calculating microSec period at different pwm freqs
double   Period      = 42000;     // period in 'timer counts' of 24 nSecs per count, so 42,000 = 1 mSec or 1 kHz
double   Pulse       = 21000;     // pulse width in 'timer counts' of 24 nSecs per count
int      FreqReading = 1000;      // unsynchronized square wave freq (reading from pot)
double   TargetFreq  = 1000;      // unsynchronized square wave target freq
double   ActualFreq  = 1000;      // unsynchronized square wave calculated actual freq
float    TargetDuty  = 50;        // unsynchronized square wave target duty-cycle
float    ActualDuty  = 50;       // unsynchronized square wave actual duty-cycle
bool     MinOrMaxDuty;          // if duty-cycle is 0% or 100% only 1 half of unsynchronized square wave is displayed - if freq is < 163Hz, used by interupt handler
float    TargetPulseWidth;     // unsynchronized square wave target pulse width in microsecs (0 if not set)
float    uPulseWidth;         // pulse width calculated in microSeconds
float    uPeriod;            // period calculated in microSeconds
byte     TimerMode;         // time period can be entered - in days, hours, minutes, seconds (max period is 4294967296 days, or 11,767,033.6 years!) - if freq is < 163Hz, used by interupt handler
bool     TimerInvert;      // sets timer ouput neg instead of pos
bool     TimerRun;        // reset or start timer running
bool     TimeUp;         // output goes positive when time entered has passed
uint32_t TimeIncrement; // 200kHz, 5 microseconds clock rate - if freq is < 163Hz, used by interupt handler
byte     TimerSecs;    // time that has passed since resetting timer
byte     TimerMins;   // as above
byte     TimerHours;
uint32_t TimerDays;
byte     PeriodS;  // seconds - Target time period
byte     PeriodM; // minutes
byte     PeriodH; // hours
uint32_t PeriodD; // days
int      d, h, m, s, a; // used for timer input (& arbitrary uploading)
uint32_t OldTime;
bool     SecChanged; // indicates when second changed
byte     LowFreqDisplay = 0; // if freq below 0.5Hz. 1 = info ready to display, 2 = display info
byte     OldSec;

/********************************************************/

// For Sine, Triangle & Arbitrary (Analogue) Waveforms: (can be accompanied by synchronized square wave)
#define  NWAVEFULL   4096           // size of Full wave-table for slow mode
#define  NWAVETABLE   160           // size of wave-table for fast mode
int      SamplesPerCycle[] = {160, 80, 40, 16}; // {FastMode0, FastMode1, FastMode2, FastMode3}
volatile boolean WaveHalf = LOW;    // identifies current 1/2 cycle
//(1/2 cycle ID -> v) (v <---- 0,  1,  2, 3     0,  1,  2, 3 <-- FastMode)
volatile int  Duty[ ] [4] = {{80, 40, 20, 8}, {80, 40, 20, 8}}; // Samples per 1/2 cycle - changes with duty cycle
// here we are storing 12 bit samples in 16 bit ints:
uint16_t WaveFull[NWAVEFULL + 1];   // wave-table for slow mode used up to 1kHz, and in ExactFreqMode at all freq's (4096 bits)
uint16_t WaveTable[NWAVETABLE + 1]; // wave-table for copying into Wave0 to Wave3 for fast mode use (160 bits)
uint16_t Wave0[2][NWAVETABLE];      // wave used for direct DMA access up to 10kHz  - FastMode0 (160 bits)
uint16_t Wave1[2][NWAVETABLE /  2]; // wave used for direct DMA access up to 20kHz  - FastMode1 (80 bits)
uint16_t Wave2[2][NWAVETABLE /  4]; // wave used for direct DMA access up to 40kHz  - FastMode2 (40 bits)
uint16_t Wave3[2][NWAVETABLE / 10]; // wave used for direct DMA access up to 100kHz - FastMode3 (16 bits)
byte     WaveShape = 0;             // 0 = Sinewave, 1 = Triangle / Sawtooth, 2 = Arbitrary
int      TimerCounts      = 26;     // divides 42MHz (CPU timer 1) by 26 counts to produce 1.61538 MHz for DMA handler
int      Delay1           = 10;     // square wave sync delay set at high sample rate
float    Delay2           = 0.55;   // square wave sync delay set at low sample rate (set high 1st)
float    Delay3           = 110;     // square wave sync delay set at low duty
volatile int SyncDelay    = (TimerCounts - Delay1) * Delay2; // delay of square wave for synchronization with peaks of analogue wave
volatile bool MinOrMaxWaveDuty;     // if duty-cycle is 0% or 100% only 1 half of wave is displayed
volatile int FastMode     = 0;      // -1 = up to 1kHz (slow mode), 0 = up to 10kHz, 1 = up to 20kHz, 2 = up to 40khz, 3 = up to 100kHz
int      OldFastMode;               // indicates when FastMode changes
byte     MinMaxDuty       = 1;      // min & max duty-cycle limit for waves (in samples) due to DMA - will be set to 4 when viewing synchronized square wave to prevent polarity reversal due to DMA timing. (set to 1 with unsynchronized square wave as polarity reversal irrelevant)
bool     PotAdjFreq[]     = {1, 1}; // {unsynchronized wave, synchronized waves}: toggles freq adjustment by pot(1) or serial(0)
bool     PotAdjDuty[]     = {1, 1}; // {unsynchronized wave, synchronized waves}: toggles duty-cycle adjustment by pot(1) or serial(0)
float    WaveReading      = 1000;   // Target freq / period (reading from pot)
double   TargetWaveFreq   = 1000;   // synchronized waves Target freq
float    TargetWaveDuty   = 50;     // synchronized waves Target duty-cycle
double   ActualWaveFreq;            // synchronized waves Actual freq
float    ActualWaveDuty;            // synchronized waves Actual duty-cycle
float    TargetWavePulseWidth;      // synchronized waves Target pulse width in mirosecs
float    AnaPulseWidth;             // Actual Pulse Width in mSeconds
float    LastAllowedWaveDuty = 50;
volatile uint32_t WaveBit;          // the bit/sample of the wave curently being processed
double   FreqIncrement = 21475000;  // the number of wave-table bits to skip in slow mode for required freq (multiplied, so we can use an int (below) instead of a float, which is faster)
double   FreqIncrmt[3] = {21475000, 21475000}; // as above {1st half of wave, 2nd half of wave} (21475000 = half wave for 1kHz) used for calculating
volatile uint32_t Increment[]  = {21475000, 21475000}; // as above {1st half of wave, 2nd half of wave} (21475000 = half wave for 1kHz)
double   IncrProportion[] = {1.00, 1.00};   // faster way used to calculate Increment[] when freq changed but duty-cycle not changed
int16_t  DitherPoint;                       // used at very low freq's to dither the Increment to produce more accurate intermediate freq's (between freq steps)
unsigned long DitherTime;                   // indicates time to dither Increment
#define  NARBWAVE   4096                    // max number of waypoints in arbitrary wave
int16_t  ArbitraryPointNumber;              // total number of waypoints entered
int16_t  ArbitraryWave[NARBWAVE + 1];       // variable to store arbitrary wave point data - all points stored (but not the extra data for stepped points)
int16_t  ArbitraryWaveStep[NARBWAVE + 1];   // variable to store arbitrary wave point data - extra data for stepped points stored here
volatile boolean ExactFreqMode = LOW;       // high indicates Exact freq mode is on. Exact at 50% duty-cycle only, causes dithering synchronized square wave
volatile boolean ExactFreqDutyNot50 = LOW;  // high indicates when in Exact freq mode and Not at 50% duty-cycle (and not 0% or 100%) - slightly reduces the workload for the interupt handler when not at 50% duty-cycle
double   ExactFreqModeAccuracy = 0.9999925; //32; // ADJUST ACCURACY of Exact Freq Mode <<<<<<<<<<<<<<<<<<<<<<<< can be tweaked here <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int      DutyMultiplier[3];                 // used when in ExactFreqMode if not at 50% duty-cycle (& not at 0 or 100%), to TRY to maintain freq

void setup()
{
  analogReadResolution(12);
  analogWriteResolution(12);
  Serial.begin (115200);
  Serial.println("\n     ************** Due Waveform Generator **************\n\n                Type and enter '?' for help.\n\n");
  pinMode(22, INPUT_PULLUP); // for switches
  pinMode(24, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(34, INPUT_PULLUP);
  pinMode(36, INPUT_PULLUP);
  pinMode(38, INPUT_PULLUP);
  pinMode(40, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(23, OUTPUT); // for LEDs
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(41, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(47, OUTPUT);
  pinMode(49, OUTPUT);
  pinMode(7, OUTPUT); // Square wave PWM output
  CreateWaveFull();
  CreateWaveTable();
  CreateNewWave();
  dac_setup();       
  TimerCounts = freqToTc(TargetWaveFreq);
  TC_setup();
  TC_setup4();
  TC_setup5();
  if (!PotsEnabled) SetWaveFreq(1);
  CalculateWaveDuty(1);
  if (!PotsEnabled)
  {
    SetFreqAndDuty(0, 0);
    Serial.print("   Unsync'ed Sq.Wave Freq: ");
    PrintUnsyncedSqWaveFreq(); Serial.print(", Target: "); Serial.print(TargetFreq, 3);
    Serial.print(" Hz\n   Unsync'ed Sq.Wave Period: ");
    PrintUnsyncedSqWavePeriod();
    Serial.print("   Unsync'ed Sq.Wave Duty-cycle: ");
    Serial.print(ActualDuty);
    Serial.println(" %\n");
  }
}

void CreateWaveFull() // WaveFull: (actually half a wave in full resolution) for low freq use; prevents 'sample' noise at very low audio freq's (sample-skipping used without DMA)
{
  if (WaveShape == 2) // Arbitrary wave creation - Create the wave from serial data
  {
//    Serial.print("ArbitraryPointNumber = "); Serial.println(ArbitraryPointNumber);
    float pointSpacing = 4096.00 / float(ArbitraryPointNumber); // ArbitraryPointNumber = 300 if touch screen used
 //   Serial.print("pointSpacing = "); Serial.println(pointSpacing);
    for (int16_t point = 0; point < ArbitraryPointNumber; point++) // changes for each defined point
    {
      int16_t nextPointValue;
      int16_t lastPointValue;
      float level = ArbitraryWave[point];
      if (ArbitraryWaveStep[point + 1] > -1) nextPointValue = ArbitraryWaveStep[point + 1]; // if next point is a wave-step, read level from 'step' file
      else nextPointValue = ArbitraryWave[point + 1];                                      // otherwise it's a normal point, so read level from usual file
      if (ArbitraryWaveStep[point] > -1 && point != 0) lastPointValue = ArbitraryWaveStep[point];      // if this point is a wave-step & not 1st point, read level from 'step' file
      else lastPointValue = ArbitraryWave[max(0, point - 1)];                            // otherwise read level from previous point's usual file
      byte waveStepPeak = 0; // if sharp wave peak detected (if direction changed sharply) at start of wave step value will depend on freq
      if (ArbitraryWaveStep[point] > -1) // if this point is a wave-step, 
      {
        if (abs(constrain((ArbitraryWaveStep[point] - ArbitraryWave[max(0, point - 1)]) / 100, -2, 2) - constrain((ArbitraryWave[point] - ArbitraryWaveStep[point]) / 100, -2, 2)) > 1) waveStepPeak = min(int(TargetWaveFreq / 100), min(12, int(pointSpacing))); // if sharp wave peak detected (if direction changed sharply) at start of wave step
      }
      byte wavePeak = 0; // if sharp wave peak detected (if direction changed) value will depend on freq
      if (abs(constrain((ArbitraryWave[point] - lastPointValue) / 100, -2, 2) - constrain((nextPointValue - ArbitraryWave[point]) / 100, -2, 2)) > 1) wavePeak = min(int(TargetWaveFreq / 100), min(12, int(pointSpacing))); // if sharp wave peak detected (if direction changed sharply)
      float stepValue = float(nextPointValue - ArbitraryWave[point]) / (pointSpacing - min(1, ArbitraryWaveStep[point] + 1) - wavePeak); // calculate size of steps needed to join defined points - less needed if sharp wave peak detected (if direction changed sharply)
      if (waveStepPeak > 0 && wavePeak > 0)
      {
        waveStepPeak /= 2;
        wavePeak -= waveStepPeak;
      }
      int16_t currentPointLocation = round((float(point) / ArbitraryPointNumber) * 4096); // means location of current defined point in full wave
      int16_t nextPointLocation = round((float(point + 1) / ArbitraryPointNumber) * 4096); // means location of next defined point in full wave
      for (int i = currentPointLocation; i < nextPointLocation; i++) // changes for each bit, joining defined points
      {
        if (i == currentPointLocation)
        {
          if (ArbitraryWaveStep[point] > -1 && point != 0)
          {
            if (waveStepPeak > 0) // if sharp wave peak detected at start of wave step (& freq is high now), repeat peak value (create a plateau) to make it more visible:
            {
              int16_t cl = i;
              for (i = i; i < cl + waveStepPeak; i++)
              {
                WaveFull[i] = ArbitraryWaveStep[point];
              }
            }
            WaveFull[i] = ArbitraryWaveStep[point]; // if step present, write 1st half of step
            i++; // move to next wave bit to write 2nd half of step
          }
          if (wavePeak > 0) // if sharp wave peak detected (& freq is high now), repeat peak value (create a plateau) to make it more visible:
          {
            int16_t cl = i;
            for (i = i; i < cl + wavePeak; i++)
            {
              WaveFull[i] = round(level);
            }
          }
        }
        WaveFull[i] = round(level);
        level += stepValue;
      }
    }
  }
  else
  {
  
    for(int Index = 0; Index < NWAVEFULL; Index++) // create the individual samples for the wave - half cycle, 4096 steps, 12 bit range: 4095 - 0
    {
      if (WaveShape == 0) WaveFull[Index] = (uint16_t) (((1 + sin((((2.0 * PI) / NWAVEFULL) / 2) * (Index + (NWAVEFULL / 2)))) * 4095.0) / 2); // Sine wave
      else                WaveFull[Index] = NWAVEFULL - 1 - Index;                                                                            // Triangle wave
//      Serial.print(Index); Serial.print(" WF = "); Serial.println(WaveFull[Index]);
    }
  }
}

void CreateWaveTable() // WaveTable: a smaller half wave for higher freq use; faster to copy from smaller table into NewWave (below) [It takes 50% longer to copy from full wave above!]
{
  for(int Index = 0; Index < NWAVETABLE; Index++) // create the individual samples for the wave table - half cycle, 160 steps, 12 bit range: 4095 - 0
  {
    if      (WaveShape == 0) WaveTable[Index] = (uint16_t) (((1 + sin((((2.0 * PI) / NWAVETABLE) / 2) * (Index + (NWAVETABLE / 2)))) * 4095.0) / 2); // Sine wave
    else if (WaveShape == 1) WaveTable[Index] = (uint16_t) (((1.0 / (NWAVETABLE - 1)) * (NWAVETABLE - 1 - Index)) * 4095.0);                        // Triangle wave
    else                     WaveTable[Index] = (uint16_t) WaveFull[round(Index * 25.6)];                                                          // Arbitrary wave
//    Serial.print(Index); Serial.print(" WT = "); Serial.println(WaveTable[Index]);
  }
}

// NewWave: For analogue wave frequencies above 1kHz. Samples from WaveTable to be copied here with selected duty cycle, for copying by DMA
void CreateNewWave() // create the individual samples for each FastMode wave whenever the duty-cycle is changed (copy from WaveTable)
{
  boolean wh = !WaveHalf; // idendifies which half of wave is CURRENTLY being written by DMA - (LOW = 1st half of wave [positive going half])
  byte fm = max(0, FastMode); // capture the current FastMode
  for(int cycle = 0; cycle < 4; cycle++) // update all 4 FastMode waves, current one first
  {
  byte dv = 0;
  if (TargetWaveDuty == 0 || TargetWaveDuty == 100) dv++; // if displaying one half of wave only divide by 1 less (next lines) to maintain full amplitude as both 0 & 4095 will be included in same half
    float inc0 = float(NWAVETABLE - dv) / (Duty[0][fm] - dv); // calculate increment needed for sampling 1st half wave from wavetable the required number of steps (depending on duty-cycle)
    float inc1 = float(NWAVETABLE) / (Duty[1][fm] - dv); // calculate increment needed for sampling 2nd half wave from wavetable the required number of steps (depending on duty-cycle)
    if (wh == HIGH)
    {
      if (TargetWaveDuty >   0) Create1stHalfNewWave(fm, inc0); // update next half wave cycle first (causes less disturbance to wave especially at high freq's)
      if (TargetWaveDuty < 100) Create2ndHalfNewWave(fm, inc1); // update current half wave cycle afterwards
    }
    else // if (wh ==  LOW)
    {
      if (TargetWaveDuty < 100) Create2ndHalfNewWave(fm, inc1); // update next half wave cycle first (causes less disturbance to wave especially at high freq's)
      if (TargetWaveDuty >   0) Create1stHalfNewWave(fm, inc0); // update current half wave cycle afterwards
    }
    if (fm < 3) fm++;
    else fm = 0;
  }
}
void Create1stHalfNewWave(byte fm, float inc0)
{
//  Serial.println("Half 1");
  float x = NWAVETABLE;
  for(int Index = 0; Index < Duty[0][fm]; Index++) // 1st half of cycle
  {  // separate files used (Wave0 - 3) instead of arrays to reduce memory useage: [since Wave3 is much smaller than Wave0]
    if ((TargetWaveDuty > 0 && TargetWaveDuty < 100) || Index != 0) x -= inc0;
    else x = NWAVETABLE - 1; // if displaying one half of wave only start from full amplitude at NWAVETABLE 159 instead of 160
    if      (fm == 0) Wave0[0][Index] = WaveTable[round(x)];
    else if (fm == 1) Wave1[0][Index] = WaveTable[round(x)];
    else if (fm == 2) Wave2[0][Index] = WaveTable[round(x)];
    else if (fm == 3) Wave3[0][Index] = WaveTable[round(x)];
//    if      (fm == 3) {Serial.print(Index); Serial.print("a = "); Serial.print(round(x)); Serial.print(" val = "); Serial.println(Wave3[0][Index]);}
  }
}
void Create2ndHalfNewWave(int fm, float inc1)
{
//  Serial.println("Half 2");
  float  x = 0;
  for(int Index = 0; Index < Duty[1][fm]; Index++) // 2nd half of cycle
  {  // separate files used (Wave0 - 3) instead of arrays to reduce memory useage: [since Wave3 is much smaller than Wave0]
    if ((TargetWaveDuty > 0 && TargetWaveDuty < 100) || Index != 0) x = min(NWAVETABLE - 1, x + inc1); // if displaying one half of wave only start from full amplitude
    if      (fm == 0) Wave0[1][Index] = WaveTable[round(x)];
    else if (fm == 1) Wave1[1][Index] = WaveTable[round(x)];
    else if (fm == 2) Wave2[1][Index] = WaveTable[round(x)];
    else if (fm == 3) Wave3[1][Index] = WaveTable[round(x)];
//    if      (fm == 3) {Serial.print(Index); Serial.print("b = "); Serial.print(round(x)); Serial.print(" val = "); Serial.println(Wave3[1][Index]);}
  }  
}

void loop()
{  
  if (millis() > SwitchPressedTime + 500) // check pot control switch/es:
  {
    if (!digitalRead(22))
    {
      PotsEnabled = !PotsEnabled;
      Serial.print("PotsEnabled = "); Serial.println(PotsEnabled);
      digitalWrite(23, PotsEnabled);
      if (!PotsEnabled)
      {
        digitalWrite(25, LOW);
        digitalWrite(27, LOW);
        digitalWrite(29, LOW);
        digitalWrite(31, LOW);
        digitalWrite(33, LOW);
        digitalWrite(35, LOW);
        digitalWrite(37, LOW);
        digitalWrite(39, LOW);
        digitalWrite(41, LOW);
        digitalWrite(45, LOW);
        digitalWrite(47, LOW);
        digitalWrite(49, LOW);
      }
      SwitchPressedTime = millis();
    }
    if (PotsEnabled) // check other switches:
    {
      if (!digitalRead(24)) // toggle unsync'ed wave freq / period
      {
        PotPeriodMode[0] = !PotPeriodMode[0];
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(26)) // change unsync'ed wave freq / period range
      {
        if (Range[0] < 10000) Range[0] *= 10;
        else Range[0] = 1;
        Serial.print("   Unsync'ed Sq.Wave Pot Freq Range: x "); Serial.println(Range[0]);
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(28)) // toggle sync'ed waves freq / period
      {
        PotPeriodMode[1] = !PotPeriodMode[1];
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(30)) // change unsync'ed wave freq / period range
      {
        if (Range[0] < 10000) Range[0] *= 10;
        else Range[0] = 1;
        Serial.print("   Unsync'ed Sq.Wave Pot Freq Range: x "); Serial.println(Range[0]);
        SwitchPressedTime = millis();
      }      
      else if (!digitalRead(32)) // toggle unsync'ed waves Duty Cycle / Pulse Width
      {
        PotPulseWidth[0] = !PotPulseWidth[0];
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(34)) // change unsync'ed wave Pulse Width Range
      {
        if (Range[2] < 10000) Range[2] *= 10;
        else Range[2] = 1;
        Serial.print("   Unsync'ed Sq.Wave Pot Pulse Width Range: x "); Serial.println(Range[2]); 
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(36)) // toggle unsync'ed waves Duty Cycle / Pulse Width
      {
        PotPulseWidth[1] = !PotPulseWidth[1];
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(38)) // change sync'ed wave Pulse Width Range
      {
        if (Range[3] < 10000) Range[3] *= 10;
        else Range[3] = 1;
        Serial.print("   Sync'ed Sq.Wave Pot Pulse Width Range: x "); Serial.println(Range[3]); 
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(40)) // Change Wave Shape
      {
        if (WaveShape < 2) WaveShape++;
        else WaveShape = 0;
        if      (WaveShape == 0) Serial.println("             ********** Sine Wave *********\n");
        else if (WaveShape == 1) Serial.println("             ******** Triangle Wave *******\n");
        else                     Serial.println("             ******* Arbitrary Wave *******\n");
        CreateWaveFull();
        CreateWaveTable();
        CreateNewWave();
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(42)) // toggle ExactFreqMode
      {
        ToggleExactFreqMode();
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(44)) // toggle Square Wave Sync
      {
        ToggleSquareWaveSync();
        SwitchPressedTime = millis();
      }
      else if (!digitalRead(46)) // Change which wave/s to control
      {
        if (Control < 2) Control++;
        else Control = 0;
        SwitchPressedTime = millis();
      }
      if (millis() > LEDupdateTime + 300)
      { 
        digitalWrite(25, PotPeriodMode[0]);
        digitalWrite(27, !PotPeriodMode[0]);
        digitalWrite(29, PotPeriodMode[1]);
        digitalWrite(31, !PotPeriodMode[1]);
        digitalWrite(33, !PotPulseWidth[0]);
        digitalWrite(35, PotPulseWidth[0]);
        digitalWrite(37, !PotPulseWidth[1]);
        digitalWrite(39, PotPulseWidth[1]);
        digitalWrite(41, ExactFreqMode);
        digitalWrite(45, SquareWaveSync);
        if (Control > 0) digitalWrite(47, HIGH);
        else digitalWrite(47, LOW);
        if (Control != 1) digitalWrite(49, HIGH);
        else digitalWrite(49, LOW);
        LEDupdateTime = millis();
      }
    }
  }
  if (PotsEnabled)
  {
    Pot0 = analogRead(A0);
    Pot1 = analogRead(A1);
  }
/***********************************************************************************************/
  // UNSYNC'ED SQ.WAVE FREQ / PERIOD ADJUSTMENT:
  float newReading =  Pot0 * Range[0];
  if (PotAdjFreq[0] == 1 || Control == 1)
  {
    OldReading[0] += (newReading - OldReading[0]) * constrain(0.02 * abs(newReading - OldReading[0]), 0.03, 1); // Responsive smoothing
    if (PotPeriodMode[0] == 1) FreqReading = min(84000000 / OldReading[0], 42000000); // if adjusting period convert to freq - max freq = 42MHz
    else FreqReading = min(OldReading[0]  , 42000000); // if adjusting freq - max freq = 42MHz
  }
  if ((abs(FreqReading - TargetFreq) > TargetFreq / 40 && PotAdjFreq[0] == 1) 
    || (abs(newReading - OldReading[0]) > OldReading[0] / 20 && PotAdjFreq[0] == 0))
  {
    if (Control != 1)
    {
      PotAdjFreq[0] = 1;
      if (PotPulseWidth[0] && TargetPulseWidth > 0) TargetDuty = min(100, 100 * (TargetPulseWidth / (1000000 / min(FreqReading, 42000000)))); // convert pulse width input into %
      else TargetPulseWidth = 0;
            
      TargetFreq = FreqReading;
      if (TargetFreq >= 100) Period = 84000000 / TargetFreq;
      else Period = 200000 / TargetFreq;
      SetFreqAndDuty(1, 1);
      Serial.print("   Unsync'ed Sq.Wave Freq: "); 
      PrintUnsyncedSqWaveFreq();
      Serial.print(", Unsync'ed Sq.Wave Duty Cycle: "); Serial.print(ActualDuty);
      Serial.print(" %\n   Unsync'ed Sq.Wave Period: "); 
      PrintUnsyncedSqWavePeriod();
      Serial.println("");
    }
  }
  // PULSE DUTY CYCLE ADJUSTMENT:
  float newRead;
  if (PotPulseWidth[0]) newRead = (Pot1 / 40) * Range[2];
  else newRead = Pot1 / 40;
  if (PotAdjDuty[0] == 1 || Control == 1) DutyReading[0] += (newRead - DutyReading[0]) * constrain(0.04 * abs(newRead - DutyReading[0]), 0.05, 1); // Responsive smoothing
  if ((abs(DutyReading[0] - TargetDuty) > 0.5 && PotAdjDuty[0] == 1)
    || (abs(newRead - DutyReading[0]) > 1 && PotAdjDuty[0] == 0))
  {
    if (Control != 1)
    {
      PotAdjDuty[0] = 1;
      if (PotPulseWidth[0])
      { 
        TargetPulseWidth = round(DutyReading[0]);
        TargetDuty = 100 * (round(DutyReading[0]) / (1000000 / ActualFreq)); // convert pulse width input into %
      }
      else TargetDuty = round(DutyReading[0]);
      SetFreqAndDuty(0, 1);
      Serial.print("   Unsync'ed Sq.Wave Duty-cycle: "); Serial.print(ActualDuty); Serial.print(" %,  Actual: "); Serial.print(TargetDuty); Serial.println(" %\n");
    }
  }
/***********************************************************************************************/
  // SYNC'ED WAVES FREQ ADJUSTMENT:
  float newWaveReading = max(Pot0 * Range[1] * 0.01, 0.01);
  if (PotAdjFreq[1] == 1 || Control == 0)
  {
    OldReading[1] += (newWaveReading - OldReading[1]) * constrain(0.02 * abs(newWaveReading - OldReading[1]), 0.03, 1); // Responsive smoothing
    if (PotPeriodMode[1] == 1) WaveReading = min(100000 / OldReading[1], 100000); // if adjusting period convert to freq
    else WaveReading = min(OldReading[1], 100000); // if adjust freq
  }
  if ((abs(TargetWaveFreq - WaveReading) > TargetWaveFreq / 40 && PotAdjFreq[1] == 1)
    || (abs(newWaveReading - OldReading[1]) > OldReading[1] / 20 && PotAdjFreq[1] == 0))
  {
    if (Control > 0)
    {
      PotAdjFreq[1] = 1;
      if (PotPulseWidth[1] && TargetWavePulseWidth > 0) TargetWaveDuty = min(100, 100 * (TargetWavePulseWidth / (1000000 / min(WaveReading, 100961)))); // convert pulse width input into %
      else TargetWavePulseWidth = 0;
      TargetWaveFreq = min(WaveReading, 100961);
            
      FreqIncrement = WaveReading * 21475;
//      TargetWaveFreq = WaveReading;
      SetWaveFreq(0);
      Serial.print("   Analogue Wave Freq: ");
      PrintSyncedWaveFreq();
      Serial.print(", Analogue Wave Duty Cycle: "); Serial.print(ActualWaveDuty);
      Serial.print(" %\n   Analogue Wave Period: "); 
      PrintSyncedWavePeriod();
      Serial.println("");
    }
  }
  // SYNC'ED WAVES DUTY CYCLE ADJUSTMENT:
  if (PotPulseWidth[1]) newRead = (Pot1 / 40) * Range[3];
  else newRead = Pot1 / 40;
  if (PotAdjDuty[1] == 1 || Control == 0) DutyReading[1] += (newRead - DutyReading[1]) * constrain(0.04 * abs(newRead - DutyReading[1]), 0.05, 1); // Responsive smoothing
  if ((abs(DutyReading[1] - TargetWaveDuty) > 0.5 && PotAdjDuty[1] == 1)
    || (abs(newRead - DutyReading[1]) > 1 && PotAdjDuty[1] == 0))
  {
    if (Control > 0)
    {
      PotAdjDuty[1] = 1;
      if (PotPulseWidth[1])
      { 
        TargetWavePulseWidth = round(DutyReading[1]);
        TargetWaveDuty = 100 * (round(DutyReading[1]) / (1000000 / ActualWaveFreq)); // convert pulse width input into %
      }
      else TargetWaveDuty = round(DutyReading[1]);
      CalculateWaveDuty(0);
      Serial.print("   Analogue Wave Duty-cycle: "); Serial.print(ActualWaveDuty); Serial.print(" %,  Target: "); Serial.print(TargetWaveDuty); Serial.println(" %\n");
      CreateNewWave();
    }
  }
/***********************************************************************************************/
  if (Serial.available() > 0)
  {
    InputChar = Serial.read();
    // Message Format To Set Frequency in Hz:  1000h + "send".
    if(InputChar >= '0' && InputChar <= '9')
    {
      UserInput = (UserInput * 10) + (InputChar - '0');
      if (numDecimalPlaces >= 0) numDecimalPlaces++; // if one of the previous characters was '.' count the number of decimal places typed
    }
    else if (InputChar == '.') numDecimalPlaces = 0; // prepare to start counting decimal places typed
    else
    {
      if ((InputChar == 't' && !UsingGUI && TimerMode == 0) || (InputChar == 'T' && UsingGUI)) // if entering Timer Mode:
      {
        if (SquareWaveSync) TimerMode = 2; // if SquareWaveSync was HIGH before entering timer mode
        else TimerMode = 1; // if SquareWaveSync was LOW before entering timer mode
        SquareWaveSync = LOW; // stop Synchronized Square wave
        TimerRun = 0;
        REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
        REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
        PWMC_DisableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
        NVIC_DisableIRQ(TC1_IRQn);
        pinMode(7, OUTPUT);
        if (TimerInvert) digitalWrite(7, HIGH);
        else digitalWrite(7, LOW);
        TimeUp = 0;
        if (UsingGUI) Serial.println("TimerMode");
        else
        {
          TC_setup5(); // (use timer in GUI if running)
          Serial.println(String("           ********** Timer Mode **********\n   The current time period is: ") + d + String(" days, ") + h + String(" hours, ") + m + String(" mins, ") + s + String(" secs.\n"));
          Serial.println("   To make changes enter the desired number followed by:\n   d for Days, h for Hours, m for Minutes or s for Seconds\n\n   Then type r to start running. Or type t again to exit.\n");
        }
      }
      else if (InputChar == 't' && TimerMode > 0) // if exiting Timer Mode
      {
        if (TimerMode == 2 || TargetFreq < 163) // if SquareWaveSync was HIGH before entering timer mode or low freq
        {
          pinMode(7, INPUT);
          PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PC28B_TIOA7, PIO_DEFAULT);  // pin 3
          if (TimerMode == 2) SquareWaveSync = HIGH; // if SquareWaveSync was HIGH before entering timer mode start Synchronized Square wave
          else NVIC_EnableIRQ(TC1_IRQn); // if SquareWaveSync was LOW & freq was low before entering timer mode start Unsynchronized Square wave
        }
        else SetFreqAndDuty(1, 1); // if SquareWaveSync was LOW & freq was high before entering timer mode start Unsynchronized Square wave (PWM)
        TimerSecs  = 0;
        TimerMins  = 0;
        TimerHours = 0;
        TimerDays  = 0;
        TimerMode = 0;
        TimerRun = 0;
        Serial.println("   You have exited the Timer.\n");
      }
      else if (TimerMode > 0 && UsingGUI && (InputChar == 'D' || InputChar == 'H' || InputChar == 'M' || InputChar == 'S'))
      {
        if (InputChar == 'D')
        {
          if (TimerRun) TimerDays = int(UserInput);
          else            PeriodD = int(UserInput);
        }
        else if (InputChar == 'H')
        {
          if (TimerRun) TimerHours = int(UserInput);
          else             PeriodH = int(UserInput);
        }
        else if (InputChar == 'M')
        {
          if (TimerRun) TimerMins = int(UserInput);
          else            PeriodM = int(UserInput);
        }
        else if (InputChar == 'S')
        {
          if (TimerRun) TimerSecs = int(UserInput);
          else            PeriodS = int(UserInput);
        }
      }
      else if (TimerMode > 0 && !UsingGUI && (InputChar == 'd' || InputChar == 'h' || InputChar == 'm' || InputChar == 's'))
      {
        if      (InputChar == 'd') d = int(UserInput);
        else if (InputChar == 'h') h = int(UserInput);
        else if (InputChar == 'm') m = int(UserInput);
        else if (InputChar == 's') s = int(UserInput);
        Serial.println(String(" * You typed: ") + d + String(" days, ") + h + String(" hours, ") + m + String(" mins, ") + s + String(" secs."));
        if (h > 23 || m > 59 || s > 59) Serial.println("   Sorry but that is invalid. Please try again.\n");
        else Serial.println("   To enter this time period type e\n");
      }
      else if (TimerMode > 0 && !UsingGUI && InputChar == 'e') // TIMER PERIOD ADJUSTMENT:
      {
     // PERIOD ADJUSTMENT: in Time Period Entry Mode
        PeriodD = d;
        if (h <= 23) PeriodH = h;
        if (m <= 59) PeriodM = m;
        if (s <= 59) PeriodS = s;
        Serial.println(String("** You entered: ") + PeriodD + String(" days, ") + PeriodH + String(" hours, ") + PeriodM + String(" mins, ") + PeriodS + String(" secs.\n\n   Type r to start Running. Type r again to Reset.\n"));
      } 
      else if (TimerMode > 0 && UsingGUI && InputChar == 'U')
      {
        TimeUp = 1;
        Serial.println("   Time Up!");
        if (TimerInvert) digitalWrite(7, LOW);
        else digitalWrite(7, HIGH);
      }
      else if (TimerMode > 0 && UsingGUI && InputChar == 'R')
      {
        TimerRun = 1;
        Serial.println("   Timer Running...");
      }
      else if (TimerMode > 0 && InputChar == 'r')
      {
        TimeUp = 0;
        if (TimerInvert) digitalWrite(7, HIGH); // Restart cycle
        else digitalWrite(7, LOW);
        TimerSecs    = 0; // Reset timers
        TimerMins   = 0;
        TimerHours = 0;
        TimerDays = 0;
        if (UsingGUI) TimerRun = 0; // reset only
        else
        {
          TimerRun = !TimerRun;
          TC_Start(TC1, 1);
        }
        if (TimerRun == 1)
        {
          Serial.print("   Timer Running...");
          if (PeriodD + PeriodH + PeriodM + PeriodS == 0) Serial.println("  WARNING! Timer is set to zero!\n");
          else Serial.println("\n");
        }
        else Serial.println("   Timer Reset!\n");
      }
      else if (TimerMode > 0 && InputChar == 'i')
      {
        TimerInvert = false;
        digitalWrite(7, LOW);
        Serial.println("  Positive Timer Mode.\n");
      }
      else if (TimerMode > 0 && InputChar == 'I')
      {
        TimerInvert = true;
        digitalWrite(7, HIGH);
        Serial.println("  Negative Timer Mode.\n");
      }
      else if (InputChar == 'h' || InputChar == 'm') // FREQ or PERIOD ADJUSTMENT:  in Hertz or Milliseconds
      {
        UserInput = UserInput * pow(10, min(0, -numDecimalPlaces)); // multiply input by 10 to the power of minus the number of decimal places
        numDecimalPlaces = -1; // reset
        if (InputChar == 'm') UserInput = 1000 / UserInput; // convert period into freq
        if (UserInput >= 0.00001999 && UserInput <= 42000000)
        {
          if (Control > 0) // sync'ed waves
          {
            PotAdjFreq[1] = 0;
            if (TargetWavePulseWidth > 0) TargetWaveDuty = min(100, 100 * (TargetWavePulseWidth / (1000000 / min(UserInput, 100961.54)))); // convert pulse width input into %
            TargetWaveFreq = min(UserInput, 100961.54);
            SetWaveFreq(1);
          }
          if (Control != 1) // unsync'ed sq.wave
          {
            PotAdjFreq[0] = 0;
            if (TargetPulseWidth > 0) TargetDuty = min(100, 100 * (TargetPulseWidth / (1000000 / min(UserInput, 42000000)))); // convert pulse width input into %
            TargetFreq = UserInput;
            SetFreqAndDuty(1, 1);
            Serial.print("   Unsync'ed Sq.Wave Freq: "); PrintUnsyncedSqWaveFreq(); Serial.print(", Target: "); Serial.print(UserInput, 3);
            Serial.print(" Hz\n   Unsync'ed Sq.Wave Period: ");
            PrintUnsyncedSqWavePeriod(); // PrintUnsyncedSqWaveDuty();
            Serial.print("   Unsync'ed Sq.Wave Duty-cycle: "); Serial.print(ActualDuty); Serial.println(" %\n");
          }
          if ((!SquareWaveSync && TargetFreq < 0.5) || (SquareWaveSync && TargetWaveFreq < 0.5))
          {
            if (millis() - 500 < TouchedTime) // if freq or period entered twice (if double clicked)
            {
              if (SquareWaveSync) // sync'ed waves
              {
                WaveBit = 1; // reset sync'ed waves
                if (TargetWaveDuty == 0)
                {
                  WaveHalf = 0;
                  TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
                }
                else
                {
                  WaveHalf = 1;
                  TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
                }
              }
              else // unsync'ed sq.wave
              {
                TimeIncrement = 0; // reset unsync'ed sq wave
                if (TargetDuty < 100) PeriodHalf = 0;
                else PeriodHalf = 1;
                if (TargetDuty == 0) TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
                else TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
              }
              TC_Start(TC1, 1); // Reset timer
              TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // the counter is reset and the clock is started
              LowFreqDisplay = 1;
              TouchedTime = 0;
            }
            else TouchedTime = millis();
          }
        }
        else if (InputChar == 'h') {Serial.print("   "); Serial.print(UserInput); Serial.println(" Hz is outside required freq range\n");}
        else                       {Serial.print("   "); Serial.print(1 / UserInput); Serial.println(" Secs is outside required period range\n");}
      } 
      else if (InputChar == 'd' || InputChar == 'u') // DUTY CYCLE or PULSE WIDTH ADJUSTMENT: in % or microseconds
      {
        UserInput = UserInput * pow(10, min(0, -numDecimalPlaces)); // multiply input by 10 to the power of minus the number of decimal places
        numDecimalPlaces = -1; // reset
        if ((InputChar == 'd' && UserInput >= 0 && UserInput <= 100) || InputChar == 'u')
        {
          if (Control > 0) // synchronized waves
          {
            PotAdjDuty[1] = 0;
            if (InputChar == 'u')
            {
              TargetWavePulseWidth = UserInput;
              TargetWaveDuty = 100 * (UserInput / (1000000 / ActualWaveFreq)); // convert pulse width input into %
            }
            else
            {
              TargetWavePulseWidth = 0;
              TargetWaveDuty = UserInput;
            }
            CalculateWaveDuty(0);
            if (MinOrMaxWaveDuty) CalculateWaveDuty(0); // if at 0 or 100% duty-cycle re-calculate (for stability)
            CreateNewWave();
            Serial.print("   Analogue Wave Duty-cycle: "); Serial.print(ActualWaveDuty); Serial.print(" %, Target: "); Serial.print(TargetWaveDuty); Serial.println(" %");
            Serial.print("   Analogue Wave Period: "); PrintSyncedWavePeriod(); Serial.println("\n");
          }
          if (Control != 1) // unsynchronized Square wave
          {
            PotAdjDuty[0] = 0;
            if (InputChar == 'u')
            {
              TargetPulseWidth = UserInput;
              TargetDuty = 100 * (UserInput / (1000000 / ActualFreq)); // convert pulse width input into %
            }
            else
            {
              TargetPulseWidth = 0;
              TargetDuty = UserInput;
            }
            SetFreqAndDuty(0, 1);
            Serial.print("   Unsync'ed Sq.Wave Duty-cycle: "); Serial.print(ActualDuty); Serial.print(" %, Target: "); Serial.print(TargetDuty); Serial.println(" %");
            Serial.print("   Unsync'ed Sq.Wave Period: "); PrintUnsyncedSqWavePeriod(); Serial.println("\n");
          }
        }
      }
      else if (InputChar == 'w') // Change Wave Shape
      {
        if (WaveShape < 2) WaveShape++;
        else WaveShape = 0;
        CreateWaveFull();
        CreateWaveTable();
        CreateNewWave();
        if      (WaveShape == 0) Serial.println("             ********** Sine Wave *********\n"); 
        else if (WaveShape == 1) Serial.println("             ******** Triangle Wave *******\n"); 
        else                     Serial.println("             ******* Arbitrary Wave *******\n"); 
      }
      else if (InputChar == 'M') // Min & max duty
      {
        if (UserInput >= 1 && UserInput <= 7) MinMaxDuty = UserInput;
        else MinMaxDuty = 4;
        Serial.print("   You have temporarily set MinMaxDuty to "); Serial.print(MinMaxDuty); Serial.println("  It will be reset to 4 by typing 'M' or 'v')\n");
      }
      else if (InputChar == 'H') // square wave sync delay at High sample rate
      {
        if (UserInput >= 1) Delay1 = min(UserInput, 25);
        else Delay1 = 10; // 10;
        SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty));
        Serial.print("   You have set Delay1 to "); Serial.print(Delay1); Serial.println("  It should be set to 10 (Type 'H')\n");
      }
      else if (InputChar == 'L') // square wave sync delay at Low sample rate
      {
        if (UserInput >= 1) Delay2 = min(UserInput * 0.01, 50);
        else Delay2 = 0.55; // 0.36;
        SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty));
        Serial.print("   You have set Delay2 to "); Serial.print(Delay2 * 100, 0); Serial.println("  It should be set to 55 (Type 'L')\n");
      }
      else if (InputChar == 'D') // square wave sync delay at High sample rate
      {
        if (UserInput >= 50) Delay3 = min(UserInput, 200);
        else Delay3 = 110;
        SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty));
        Serial.print("   You have set Delay3 to "); Serial.print(Delay3, 0); Serial.println("  It should be set to 110 (Type 'D')\n");
      }
      else if (InputChar == 'v' && TimerMode == 0) // toggle between Viewing sync'ed square wave or unsync'ed square wave
      {
        ToggleSquareWaveSync();
      }
      else if (PotsEnabled && InputChar == 'r') // Range of Freq / Period Pots
      {
        if (Control > 0) // synchronized waves range:
        {
          if (Range[1] < 10000) Range[1] *= 10;
          else Range[1] = 1;
          Serial.print("   Synchronized Waves Pot Freq Range: x "); Serial.println(Range[1]);
        }
        if (Control != 1) // unsynchronized sq.wave range:
        {
          if (Range[0] < 10000) Range[0] *= 10;
          else Range[0] = 1;
          Serial.print("   Unsync'ed Sq.Wave Pot Freq Range: x "); Serial.println(Range[0]); 
        }
        SwitchPressedTime = millis();
      }
      else if (PotsEnabled && InputChar == 'R') // Range of Duty-cycle / Pulse Width Pots
      {
        if (Control > 0) // synchronized waves range:
        {
          if (Range[3] < 10000) Range[3] *= 10;
          else Range[3] = 1;
          Serial.print("   Synchronized Waves Pot Pulse Width Range: x "); Serial.println(Range[3]);
        }
        if (Control != 1) // unsynchronized sq.wave range:
        {
          if (Range[2] < 10000) Range[2] *= 10;
          else Range[2] = 1;
          Serial.print("   Unsync'ed Sq.Wave Pot Pulse Width Range: x "); Serial.println(Range[2]); 
        }
        SwitchPressedTime = millis();
      }
      else if (PotsEnabled && InputChar == 'f' && TimerMode == 0) // toggle between pot controlling Freq of wave, or period of wave
      {
        if (Control > 0)
        {
          PotPeriodMode[1] = !PotPeriodMode[1]; // toggle PotPeriodMode for synch'ed waves
          Serial.print("   Synchronized Waves Pot Mode is ");
          if      (PotPeriodMode[1] == 0) Serial.println("Freq");
          else if (PotPeriodMode[1] == 1) Serial.println("Period");
        }
        if (Control != 1)
        {
          PotPeriodMode[0] = !PotPeriodMode[0]; // toggle PotPeriodMode for unsynch'ed wave
          Serial.print("   Unsynchronized Wave Pot Mode is ");
          if      (PotPeriodMode[0] == 0) Serial.println("Freq");
          else if (PotPeriodMode[0] == 1) Serial.println("Period");
        }
        Serial.println("");
        SwitchPressedTime = millis();
      }
      else if (InputChar == 'e') // Exact Freq Mode - low resolution at 50% duty-cycle only & with dithering synchronized square wave
      {
        ToggleExactFreqMode();
      }
      else if (InputChar == 'a' && TimerMode == 0) // Arbitrary wave creation - deletes old wave data, ready for new wave data
      {
        a = 1;
        ArbitraryPointNumber = 0;
        for (int16_t i = 0; i <= 4096; i++) {ArbitraryWaveStep[i] = -1;} // resets any stepped points to 'unstepped' state
        Serial.println("");
        Serial.println("         ************************* NEW ARBITRARY WAVE CREATION *************************        \n");
        Serial.println("   Please type the value of each point you wish to define. (Any old data has been deleted.)");
        Serial.println("   Separate each value (0 to 4095) with a comma. Use no spaces. Finish with a semi-colon.\n");
        Serial.println("   For example:\n   2047,2150,3800,4095,3800,400,200,400,2510,2700,2510,1800,1700,1800,2040,2150,2050,1980,1960,2000;\n");
        Serial.println("   You can create steps in the wave by dividing points into two values.\n   For example, an 'M' wave:  0,0-4095,1600,4095-0;\n");
        Serial.println("   You can define up to 300 points, but enter them no more than 40 at a time, ending with a comma.");
        Serial.println("   Use a semi-colon at the end to trigger wave creation. Points can also be added later. (300 total)\n");
      }
      else if (InputChar == '-' && TimerMode == 0) // Arbitrary wave creation - stepped point separator
      {
        ArbitraryWaveStep[ArbitraryPointNumber] = UserInput; // save Wave Step data
        if (ArbitraryPointNumber == 0) Serial.print("   You entered:\n   "); 
        Serial.print(ArbitraryWaveStep[ArbitraryPointNumber]); Serial.print("-");
      }
      else if (InputChar == ',' && TimerMode == 0) // Arbitrary wave creation - point separator / counter
      {
        if (ArbitraryPointNumber > 0 && ArbitraryPointNumber % 10 == 0) Serial.print("\n   "); // start a new line after every 10 points entered
        ArbitraryWave[ArbitraryPointNumber] = UserInput; // save Wave point data
        if (ArbitraryPointNumber == 0 && ArbitraryWaveStep[ArbitraryPointNumber] == -1) Serial.print("   You entered:\n   "); 
        Serial.print(ArbitraryWave[ArbitraryPointNumber]); Serial.print(",");
        if (ArbitraryPointNumber < NARBWAVE - 1) ArbitraryPointNumber++;
      }
      else if (InputChar == ';' && TimerMode == 0) // Arbitrary wave creation - last point detector / signal to create wave
      {
        ArbitraryWave[ArbitraryPointNumber] = UserInput;
        Serial.print(ArbitraryWave[ArbitraryPointNumber]); Serial.println(";");
        if (ArbitraryPointNumber < NARBWAVE) ArbitraryPointNumber++;
        ArbitraryWave[ArbitraryPointNumber] = ArbitraryWave[0]; // make last point same as first
        ArbitraryWaveStep[ArbitraryPointNumber] = ArbitraryWaveStep[0];
        Serial.print("   ..... a total of "); Serial.print(ArbitraryPointNumber);
        if (ArbitraryPointNumber < NARBWAVE - 1) Serial.println(" points.\n");
        else Serial.println(" points. THIS IS THE MAXIMUM LIMIT\n");
        WaveShape = 2;
        CreateWaveFull();
        CreateWaveTable();
        CreateNewWave();
        Serial.println("   Set duty-cycle to 0% (Type 0d). Other duty-cycle settings create a 'mirrored' effect.");
        Serial.println("   Typing 'a' again (to enter a new arbitrary wave) will clear the current wave from memory.");
        if (ArbitraryPointNumber < 300) Serial.println("   Or you can just add more points!\n   ");
        else Serial.println("\n   ");
        a = 0;
      }
      else if (InputChar == ' ' && TimerMode == 0) // space-bar - toggle control between sync'ed waves & unsync'ed sq.wave
      {
        if (Control == 1)
        {
          Control = 0;
          Serial.println("   CONTROL >> Unsync'ed Sq.Wave\n");
        }
        else
        {
          Control = 1;
          Serial.println("   CONTROL >> Analogue Wave\n");
        }
      }
      else if (InputChar == 'b' && TimerMode == 0) // control both analogue wave & Unsync'ed sq.wave together
      {
        Control = 2;
        Serial.println("   CONTROL >> Both Waves\n");
      }
      else if (InputChar == 's' && TimerMode == 0) // Sweep freq function
      {
        UserInput = UserInput * pow(10, min(0, -numDecimalPlaces)); // multiply input by 10 to the power of minus the number of decimal places
        numDecimalPlaces = -1; // reset
        if (SweepStep > 0 && SweepStep != 3 && SweepStep != 4 && UserInput == 0) // cancel / delete entry & stop sweep if running
        {
          SweepStep = 0;
          Serial.println("   Sweep cancelled\n");
          return;
        }
        if (SweepStep == 0)
        {
          if (!UsingGUI)
          {
            Serial.println("       ************ SWEEP FREQUENCY FUNCTION ************");
            Serial.println("   Please type the minimum sweep frequency in Hz followed by s\n   (Typing s again without entering a number first will cancel)");
          }
          SweepStep = 1;
        }
        if (SweepStep == 1 && UserInput > 0)
        {
          SweepMinFreq = UserInput;
          SweepStep = 2;
          if (!UsingGUI)
          {
            Serial.print("   The minimum sweep frequency is "); Serial.print(SweepMinFreq); Serial.println(" Hz");
            Serial.println("   Please type the maximum sweep frequency in Hz followed by s");
          }
        }
        else if (SweepStep == 2 && UserInput > 0)
        {
          if (UserInput > SweepMinFreq)
          {
            SweepMaxFreq = UserInput;
            SweepStep = 3;
            if (!UsingGUI)
            {
              Serial.print("   The maximum sweep frequency is "); Serial.print(SweepMaxFreq); Serial.println(" Hz");
              Serial.println("   Please type the sweep rise time in seconds followed by s");
            }
          }
          else if (!UsingGUI)
          {
            Serial.print("\n   The maximum sweep frequency must be higher than the minimum entered which was "); Serial.print(SweepMinFreq);
            Serial.println(" Hz\n   Please re-enter the maximum sweep frequency\n");
          }
        }
        else if (SweepStep == 3 && UserInput >= 0)
        {
          SweepRiseTime = UserInput;
          SweepStep = 4;
          if (!UsingGUI)
          {
            Serial.print("   The sweep rise time is "); Serial.print(SweepRiseTime); Serial.println(" Secs");
            Serial.println("   Please type the sweep fall time in seconds followed by s");
          }
        }
        else if (SweepStep == 4 && UserInput >= 0)
        {
          if (UserInput > 0 || SweepRiseTime > 0)
          {
            SweepFallTime = UserInput;
            SweepStep = 5;
            if (!UsingGUI)
            {
              Serial.print("   The sweep fall time is "); Serial.print(SweepFallTime); Serial.println(" Secs");
              Serial.print("   To repeat this freq sweep the following string can be entered:\n\n   ");
              Serial.println(SweepMinFreq + String("s") + SweepMaxFreq + String("s") + SweepRiseTime + String("s") + SweepFallTime + String("s\n"));
              Serial.println("   Type s again to stop the sweep\n");
              delay(2000); // time to read above message
            }
            SweepFreq();
          }
          else if (!UsingGUI)
          {
            Serial.print("\n   The Sweep Rise Time and Fall Time can't both be zero");
            Serial.println("\n   Please re-enter the Sweep Fall Time\n");
          }
        }
      }
      else if (InputChar == 'p') PotsEnabled = !PotsEnabled; // enable / disable Pots
      else if (InputChar == 'P')
      {
        if (Control > 0) // synchronized waves range:
        {
          PotPulseWidth[1] = !PotPulseWidth[1]; // enable / disable pulse width input from pot instead of duty-cycle %
        }
        if (Control != 1) // unsynchronized waves range:
        {
          PotPulseWidth[0] = !PotPulseWidth[0]; // enable / disable pulse width input from pot instead of duty-cycle %
        }
        SwitchPressedTime = millis();     
      }
      else if (InputChar == '?') // Help
      {
        Serial.println("   HELP:  Type the following, then press enter:");
        Serial.println("   Type:   w   to toggle the analogue Wave shape.");
        Serial.println("   Type:   a   to create a new Arbitrary wave. Follow on-screen instructions.");
        Serial.println("   Type:   v   to toggle between Viewing synchronized or unsynchronized waves.");
        Serial.println("   Type:  ' '  [spacebar] to toggle between controlling synchronized waves or unsync'ed square wave.");
        Serial.println("   Type:   b   to control Both synchronized and unsynchronized waves simultaneously.");
        Serial.println("   Type:   h   to change frequency of wave/s, type required frequency in Hz followed by h.");
        Serial.println("   Type:   m   to change period of wave/s, type required period in Milliseconds followed by m.");
        Serial.println("   Type:   d   to change Duty-cycle type required percentage duty-cycle (0 - 100) followed by d.");
        Serial.println("   Type:   u   to set pulse width. Type required pulse width in microseconds followed by u.");
        Serial.println("   Type:   e   to toggle on/off Exact Freq Mode for synchronized waves, eliminating freq steps.");
        Serial.println("   Type:   s   to set up the frequency Sweep feature. Follow on-screen instructions.");
        Serial.println("   Type:   t   to enter or exit the Timer. Follow on-screen instructions.");
        Serial.println("   Type:   p   to enable (or disable) pots.");
        Serial.println("   Type:   f   to toggle between pot controlling Freq of wave, or period of wave.");
        Serial.println("   Type:   P   to toggle between pot controlling duty-cycle Percent, or Pulse width of wave.");
        Serial.println("   Type:   r   to toggle Range of the frequency/period pot: x 1, x10, x100, x1000 & x 10000.");
        Serial.println("   Type:   R   to toggle Range of the pulse width pot: x 1, x10, x100, x1000 & x 10000.");
        Serial.println("   Press enter twice to display the current status.\n");
      }
      else if (InputChar == 'G' && millis() < 1000) // if connecting to GUI:
      {
        UsingGUI = 1;
        Serial.println("Hello GUI");
      }
      else // if Enter pressed
      {
        if (millis() - 500 < TouchedTime) // if Enter pressed twice - display status
        {
          Serial.println("   STATUS:");
          if (TimerMode > 0) Serial.println(String("   Timer period is set to: ") + PeriodD + String(" days, ") + PeriodH + String(" hours, ") + PeriodM + String(" mins, ") + PeriodS + String(" secs."));
          if  (ExactFreqMode) Serial.print("   Exact Mode is ON ");
          else                Serial.print("   Exact Mode is OFF");
          if (SquareWaveSync) Serial.println("  Waves are Synchronized");
          else                Serial.println("  Waves are Unsynchronized");
//          } ////e
          if (Control > 0 && TimerMode == 0) Serial.print(">> Analogue Wave Freq: ");
          else  Serial.print("   Analogue Wave Freq: ");
          PrintSyncedWaveFreq();
          Serial.print(", Analogue Wave Duty-cycle: "); Serial.print(ActualWaveDuty); Serial.println(" %");
          Serial.print("   Analogue Wave Period: ");
          PrintSyncedWavePeriod();
          if (Control != 1 && TimerMode == 0) Serial.print(">> Unsync'ed Sq.Wave Freq: ");
          else  Serial.print("   Unsync'ed Sq.Wave Freq: ");
          PrintUnsyncedSqWaveFreq();
          Serial.print(", Unsync'ed Sq.Wave Duty-cycle: "); Serial.print(ActualDuty); Serial.println(" %");
          Serial.print("   Unsync'ed Sq.Wave Period: ");
          PrintUnsyncedSqWavePeriod();
          if (PotsEnabled)
          {
            Serial.print("   Pot Mode - Analog Wave: ");
            if      (PotPeriodMode[1] == 0) Serial.print("Freq ");
            else if (PotPeriodMode[1] == 1) Serial.print("Period");
            Serial.print("  Unsync'ed Sq.Wave: ");
            if      (PotPeriodMode[0] == 0) Serial.println("Freq");
            else if (PotPeriodMode[0] == 1) Serial.println("Period");
            Serial.print("   Freq Pot Range - Analog Wave: x "); Serial.print(Range[1]); Serial.print("  Unsync'ed Sq.Wave: x "); Serial.println(Range[0]);
            if (PotPulseWidth) Serial.print("   Pulse Width Range - Analog Wave: x "); Serial.print(Range[3]); Serial.print("  Unsync'ed Sq.Wave: x "); Serial.println(Range[2]);
          }
          else if (millis() < 180000) Serial.println("   Press 'p' to enable / disable pots"); // only shown for 1st 3 minutes
          Serial.println();
          TouchedTime = 0;
        }
        else TouchedTime = millis();
      }
      UserInput = 0; // reset to 0 ready for the next sequence of digits
    }
  }
  if (TimerRun == 1 || (TimerMode == 0 && ((!SquareWaveSync && TargetFreq < 0.5) || (SquareWaveSync && TargetWaveFreq < 0.5)))) // if timer running or if sq.wave is less than 0.5Hz
  {
    bool waveHalfStart = 0;
    if ((SquareWaveSync && WaveBit < OldTime) || (!SquareWaveSync && TimeIncrement < OldTime)) waveHalfStart = 1;
    byte sec = TC1->TC_CHANNEL[1].TC_CV / 656250; // remaining seconds not in timer mode. 656250(Hz) * 60(seconds) = 39375000 clocks per minute
    if ((sec != OldSec) || waveHalfStart) // if remaining seconds have changed or at start of wavehalf then update
    {
      if (!waveHalfStart && LowFreqDisplay == 1)
      {
        SecChanged = 1;
        if (TimerSecs == 59)
        {
          if (TimerMins < 59) TimerMins++;
          else
          {
            TimerMins = 0;
            if (TimerHours < 23) TimerHours++;
            else
            {
              TimerHours = 0;
              TimerDays++;
            }
          }
        }
        LowFreqDisplay = 2;
      }
      byte oldTimerSecs = TimerSecs;
      if (waveHalfStart)
      {
        TimerSecs = 0;
        OldSec = 0;
        sec = 0;
      }
      else TimerSecs = sec;
      if (TimerMode > 0 && !UsingGUI) // if in timer mode & not using GUI
      {
        if (TimeUp == 0 && TimerDays >= PeriodD && TimerHours >= PeriodH && TimerMins >= PeriodM && TimerSecs >= PeriodS && PeriodD + PeriodH + PeriodM + PeriodS > 0) // if time elapsed & if not set to zero
        {
          if (TimerInvert) digitalWrite(7, LOW);
          else digitalWrite(7, HIGH);
          TimeUp = 1; // keeps output high (or low) after time is up in timer mode
          Serial.println("   Time Is Up!\n");
        }
      }
      else if (((!SquareWaveSync && TargetFreq < 0.5) || (SquareWaveSync && TargetWaveFreq < 0.5)) && TimerMode == 0 && a == 0) // a = 1 if arbitrary wave being received
      {
        if (oldTimerSecs + TimerMins + TimerHours + TimerDays > 0 && waveHalfStart) // if at start of wave half
        {
          if ((SquareWaveSync && (WaveHalf || AnaPulseWidth < 1000)) || !SquareWaveSync) // if at start of period
          {
            TimerSecs  = 0; // Reset time at start of each cycle
            TimerMins  = 0;
            TimerHours = 0;
            TimerDays  = 0;
            LowFreqDisplay = 2;
            delay(10); // slightly late sync of timer ensures period resets reliably next time
            TC_Start(TC1, 1); // Reset timer
          }
        }
      }  
      if (UsingGUI && TimerMode == 0 && LowFreqDisplay == 2 && a == 0) Serial.print("INFO>");
      if (!UsingGUI && TimerMode > 0) Serial.println(String("   Time Elapsed: ") + TimerDays + String(" days, ") + TimerHours + String(" hours, ") + TimerMins + String(" mins, ") + TimerSecs + String(" secs\n"));
      else if (SweepStep == 0 && TimerMode == 0 && LowFreqDisplay == 2 && a == 0) Serial.println(String("   ") + TimerHours + String(" hours, ") + TimerMins + String(" mins, ") + TimerSecs + String(" secs from start of period\n"));
      if (LowFreqDisplay == 2) LowFreqDisplay = 1; // prevents half-wave being displayed
      if (SquareWaveSync) OldTime = WaveBit;
      else OldTime = TimeIncrement;
      SecChanged = 0;
    }
    OldSec = sec;
  }
  else if (TimerMode == 0) LowFreqDisplay = 0;
  // (To use 64 bit numbers (for Increment[] etc) instead of dithering a slower sample rate (in slow mode) would be needed)
  if (DitherTime > 0 && millis() >= DitherTime && TargetWaveFreq < 0.1) // add increment dithering for very low (intermediate) freq compensation:
  {
    DitherTime = millis() + 100; // how often to dither
    int numDithPoints = min(1 / TargetWaveFreq, 1000); // number of points to dither over
    if (DitherPoint < numDithPoints - 1) DitherPoint++; // current Dither point number
    else DitherPoint = 0;
    if ((round(FreqIncrmt[0] * numDithPoints) % numDithPoints) + DitherPoint > numDithPoints - 1) Increment[0] = int(FreqIncrmt[0]) + 1; // add dither to 1st half of wave Increment[0] if required
    else Increment[0] = int(FreqIncrmt[0]); // add no dither
    if ((round(FreqIncrmt[1] * numDithPoints) % numDithPoints) + DitherPoint > numDithPoints - 1) Increment[1] = int(FreqIncrmt[1]) + 1; // add dither to 2nd half of wave Increment[1] if required
    else Increment[1] = int(FreqIncrmt[1]); // add no dither
  }
  else if (DitherTime > 0 && TargetWaveFreq >= 0.1) DitherTime = 0;
}

void ToggleExactFreqMode()
{
  SyncDelay = 0; // remove square wave sync delay for stability in case of sudden change to high freq
  ExactFreqMode = !ExactFreqMode;
  if (ExactFreqMode) Serial.println("   Exact Mode is ON ");
  else Serial.println("   Exact Mode is OFF");
  if (ExactFreqMode && TargetWaveDuty != 50) ExactFreqDutyNot50 = 1;
  else ExactFreqDutyNot50 = 0; ////e
  SetWaveFreq(1);
  SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty)); // re-apply square wave sync delay
}

void ToggleSquareWaveSync()
{
  if (!SquareWaveSync)
  {
    NVIC_DisableIRQ(TC1_IRQn);
    PWMC_DisableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
    pinMode(7, INPUT);
    PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PC28B_TIOA7, PIO_DEFAULT);
    SquareWaveSync = HIGH; // start Synchronized Square wave
    MinMaxDuty = 4; // allow min (& max) duty-cycle of 4 samples
    CalculateWaveDuty(0);
    CreateNewWave();
    WavePolarity();
    Serial.println("   Waves are Synchronized");
  }
  else
  {
    SquareWaveSync = LOW; // stop Synchronized Square wave
    if (TargetFreq < 163)
    {
      pinMode(7, INPUT); // re-enable pin after stopping
      TC_setup3();
    }
    else
    {
      REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
      REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
      SetPWM(7, Period, Pulse); // PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
    }
    MinMaxDuty = 1; // allow min (& max) duty-cycle of 1 sample
    CalculateWaveDuty(ExactFreqMode);
    CreateNewWave();
    Serial.println("   Waves are Unsynchronized");
  }
  Serial.print("   Analogue Wave Period: ");
  PrintSyncedWavePeriod();
  Serial.print("   Analogue Wave Duty-cycle: "); Serial.print(ActualWaveDuty); Serial.println(" %\n");
  if (ExactFreqMode) SetWaveFreq(0); // ensures exact freq maintained
}

void SweepFreq()
{
  float oldFreq  = TargetFreq;
  float oldWaveFreq = TargetWaveFreq;
  int   SweepUpdatePeriod = 25; // change freq every SweepUpdatePeriod milliseconds
  float RiseIncrement = pow(float(SweepMaxFreq) / SweepMinFreq, 1 / (float(SweepRiseTime) / (SweepUpdatePeriod * 0.001))); // proportional freq increase required each SweepUpdatePeriod
  float FallIncrement = pow(float(SweepMinFreq) / SweepMaxFreq, 1 / (float(SweepFallTime) / (SweepUpdatePeriod * 0.001))); // proportional freq decrease required each SweepUpdatePeriod
  float TargetSweepFreq = SweepMinFreq;
  float SweepIncrement = RiseIncrement;
  unsigned long SweepUpdateTime = millis() + SweepUpdatePeriod;
  while(1)
  {
    if (TargetSweepFreq >= SweepMaxFreq) SweepIncrement = FallIncrement;
    else if (TargetSweepFreq <= SweepMinFreq) SweepIncrement = RiseIncrement;
    if (millis() >= SweepUpdateTime)
    {
      SweepUpdateTime = millis() + SweepUpdatePeriod; // set next SweepUpdateTime
      if (SweepFallTime == 0 && TargetSweepFreq >= SweepMaxFreq) TargetSweepFreq = SweepMinFreq;
      else if (SweepRiseTime == 0 && TargetSweepFreq <= SweepMinFreq) TargetSweepFreq = SweepMaxFreq;
      else TargetSweepFreq *= SweepIncrement;
      if (Control > 0)
      {
        TargetWaveFreq = min(TargetSweepFreq, 100000);
        SetWaveFreq(0);
        if (Control > 1 && !UsingGUI) Serial.println();
        if (!UsingGUI) Serial.print("\n   ");
        if (UsingGUI) Serial.print("AAF ");
        PrintSyncedWaveFreq();
      }
      if (Control != 1)
      {
        TargetFreq = TargetSweepFreq;
        SetFreqAndDuty(1, 1);
        if (UsingGUI) Serial.print("SAF ");
        else Serial.print("\n   ");
        PrintUnsyncedSqWaveFreq();
      }
    }
    if (Serial.available() > 0)
    {
      InputChar = Serial.read();
      if (InputChar == 's') // if signalled to stop
      {
        SweepStep = 0;
        if (Control > 0) // return to freq before sweep
        {
          TargetWaveFreq = oldWaveFreq;
          FreqIncrement = min(TargetWaveFreq, 100000) * 21475;
          SetWaveFreq(0);
          if (UsingGUI) Serial.print("AAF ");
          else Serial.print("\n   ");
          PrintSyncedWaveFreq();
        }
        if (Control != 1) // return to freq before sweep
        {
          TargetFreq = oldFreq;
          SetFreqAndDuty(1, 1);
          if (UsingGUI) Serial.print("SAF ");
          else Serial.print("\n   ");
          PrintUnsyncedSqWaveFreq();
        }
        if (!UsingGUI) Serial.println("\n   Sweep stopped\n");
        return;
      }
    }
  }
}

void PrintSyncedWaveFreq()
{
  if      (ActualWaveFreq < 1)       {Serial.print(ActualWaveFreq * 1000, 5); Serial.print(" mHz");}
  else if (ActualWaveFreq < 1000)    {Serial.print(ActualWaveFreq, 5); Serial.print(" Hz");}
  else                           {Serial.print(ActualWaveFreq / 1000, 5); Serial.print(" kHz");}
}

void PrintSyncedWavePeriod()
{
  float AnaPeriod = 1000 / ActualWaveFreq; // in mSeconds
  if (TargetWaveDuty == 0)
  {
    if (SquareWaveSync) AnaPulseWidth = 0.000048; // in mSeconds
    else AnaPulseWidth = 0.000350; // in mSeconds
  }
  else if (TargetWaveDuty >= 100)
  {
    if (SquareWaveSync) AnaPulseWidth = AnaPeriod - 0.000096; // in mSeconds
    else AnaPulseWidth = AnaPeriod - 0.000350; // in mSeconds
  }
  else AnaPulseWidth = ActualWaveDuty * 0.01 * AnaPeriod; // in mSeconds
  if      (AnaPeriod < 1)    {Serial.print(AnaPeriod * 1000, 5);  Serial.print(" uS, Pulse Width: ");}
  else if (AnaPeriod < 1000) {Serial.print(AnaPeriod, 5);         Serial.print(" mS, Pulse Width: ");}
  else if (AnaPeriod < 10000000) {Serial.print(AnaPeriod * 0.001, 5); Serial.print(" Sec, Pulse Width: ");}
  else                           {Serial.print(AnaPeriod * 0.001, 4); Serial.print(" Sec, Pulse Width: ");}
  if      (AnaPulseWidth < 0.001) {Serial.print(AnaPulseWidth * 1000000, 5); Serial.println(" nS");}
  else if (AnaPulseWidth < 1)     {Serial.print(AnaPulseWidth * 1000, 5); Serial.println(" uS");}
  else if (AnaPulseWidth < 1000)  {Serial.print(AnaPulseWidth, 5);        Serial.println(" mS");}
  else if (AnaPulseWidth < 10000000) {Serial.print(AnaPulseWidth / 1000, 5); Serial.println(" Sec");}
  else                               {Serial.print(AnaPulseWidth / 1000, 4); Serial.println(" Sec");}
}

void PrintUnsyncedSqWaveFreq()
{
  if      (ActualFreq < 1)       {Serial.print(ActualFreq * 1000, 5); Serial.print(" mHz");}
  else if (ActualFreq < 1000)    {Serial.print(ActualFreq, 5); Serial.print(" Hz");}
  else if (ActualFreq < 1000000) {Serial.print(ActualFreq / 1000, 5); Serial.print(" kHz");}
  else                           {Serial.print(ActualFreq / 1000000, 5); Serial.print(" MHz");}
}

void PrintUnsyncedSqWavePeriod()
{
  if      (uPeriod < 1)       {Serial.print(uPeriod * 1000, 5); Serial.print(" nS, Pulse Width: ");}
  else if (uPeriod < 1000)    {Serial.print(uPeriod, 5); Serial.print(" uS, Pulse Width: ");}
  else if (uPeriod < 1000000) {Serial.print(uPeriod / 1000, 5); Serial.print(" mS, Pulse Width: ");}
  else if (uPeriod < 10000000000) {Serial.print(uPeriod / 1000000, 5); Serial.print(" Sec, Pulse Width: ");}
  else                            {Serial.print(uPeriod / 1000000, 4); Serial.print(" Sec, Pulse Width: ");}
  if      (uPulseWidth < 1)       {Serial.print(uPulseWidth * 1000, 5); Serial.println(" nS");}
  else if (uPulseWidth < 1000)    {Serial.print(uPulseWidth, 5);        Serial.println(" uS");}
  else if (uPulseWidth < 1000000) {Serial.print(uPulseWidth / 1000, 5); Serial.println(" mS");}
  else if (uPulseWidth < 10000000000) {Serial.print(uPulseWidth / 1000000, 5); Serial.println(" Sec");}
  else                                {Serial.print(uPulseWidth / 1000000, 4); Serial.println(" Sec");}
}

float tcToFreq( int tc_cntr)
{
  float freq_hz;     
  if      (tc_cntr == 0) return 1000;
  if      (FastMode == 3) freq_hz = (420000000.00 / tc_cntr) / NWAVETABLE; // display freq fractions
  else if (FastMode == 2) freq_hz = (168000000.00 / tc_cntr) / NWAVETABLE;
  else if (FastMode == 1) freq_hz = ( 84000000.00 / tc_cntr) / NWAVETABLE;
  else if (FastMode <= 0) freq_hz = ( 42000000.00 / tc_cntr) / NWAVETABLE;
  return freq_hz;
}

int freqToTc(float freq_hz)
{
  int tc_cntr = 0;
  if      (freq_hz == 0) return 25;
  if      (FastMode == 3) tc_cntr = (420000000UL / freq_hz) / NWAVETABLE;
  else if (FastMode == 2) tc_cntr = (168000000UL / freq_hz) / NWAVETABLE;
  else if (FastMode == 1) tc_cntr = ( 84000000UL / freq_hz) / NWAVETABLE;
  else if (FastMode <= 0) tc_cntr = ( 42000000UL / freq_hz) / NWAVETABLE;
  return tc_cntr;
}

void WavePolarity() // ensures the same wave polarity is maintained (relative to square wave)
{
  if (FastMode == 0)
  {
    DACC->DACC_TPR  =  (uint32_t)  Wave0[0];      // DMA buffer
    DACC->DACC_TNPR =  (uint32_t)  Wave0[1];      // next DMA buffer
  }
  else if (FastMode == 1)
  {
    DACC->DACC_TPR  =  (uint32_t)  Wave1[0];      // DMA buffer
    DACC->DACC_TNPR =  (uint32_t)  Wave1[1];      // next DMA buffer
  }
  else if (FastMode == 2)
  {
    DACC->DACC_TPR  =  (uint32_t)  Wave2[0];      // DMA buffer
    DACC->DACC_TNPR =  (uint32_t)  Wave2[1];      // next DMA buffer
  }
  else if (FastMode == 3)
  {
    DACC->DACC_TPR  =  (uint32_t)  Wave3[0];      // DMA buffer
    DACC->DACC_TNPR =  (uint32_t)  Wave3[1];      // next DMA buffer
  }
  DACC->DACC_TCR  =  Duty[0][FastMode];
  DACC->DACC_TNCR =  Duty[1][FastMode];
}

void SetWaveFreq(bool show) // FOR ANALOGUE & SYNCHRONIZED SQUARE WAVE:
{
//  SET FREQUENCY:
  float dutyLimit = 0;
  float allowedWaveDuty = TargetWaveDuty;
  OldFastMode = FastMode; // old FastMode
  if      (!ExactFreqMode && TargetWaveFreq > 40000) FastMode =  3;
  else if (!ExactFreqMode && TargetWaveFreq > 20000) FastMode =  2;
  else if (!ExactFreqMode && TargetWaveFreq > 10000) FastMode =  1;
  else if (!ExactFreqMode && TargetWaveFreq >  1000) FastMode =  0;
  else // if slow mode (sample rate of 400kHz)
  {
    FastMode = -1;
    FreqIncrement = TargetWaveFreq * 21475;
    double FreqIncr = FreqIncrement;
//  Set Duty (if necessary):
    if (TargetWaveDuty > 0 && TargetWaveDuty < 100) dutyLimit = 1 / (4000 / TargetWaveFreq); // calculate percent duty limit of 1 sample at 400kHz (up to 25% at 100kHz)
    if (ExactFreqMode)
    {
      FreqIncr *= ExactFreqModeAccuracy; // accuracy adjustment
    }      
    allowedWaveDuty = constrain(TargetWaveDuty, dutyLimit, 100 - dutyLimit);
    if (TargetWavePulseWidth == 0 && allowedWaveDuty == LastAllowedWaveDuty) // if PulseWidth not fixed & duty limit not changed, quick freq/duty calculation is okay:
    {
      if (allowedWaveDuty == 0)
      {
        FreqIncrmt[0] = FreqIncr / 2;
        WaveHalf = 0;
        MinOrMaxWaveDuty = 1;
      }
      else FreqIncrmt[0] = constrain((1.00 / (           allowedWaveDuty  / 50.000)) * FreqIncr, 0 , 4294967295);
//      else FreqIncrmt[0] = IncrProportion[0] * FreqIncr;
      if (allowedWaveDuty == 100)
      {
        FreqIncrmt[1] = FreqIncr / 2;
        WaveHalf = 1;
        MinOrMaxWaveDuty = 1;
      }
      else FreqIncrmt[1] = constrain((1.00 / ((100.000 - allowedWaveDuty) / 50.000)) * FreqIncr, 0 , 4294967295);
 //     else FreqIncrmt[1] = IncrProportion[1] * FreqIncr;
      Increment[0] = FreqIncrmt[0]; // update quickly after calculating above
      Increment[1] = FreqIncrmt[1]; // update quickly after calculating above
      if (TargetWaveFreq < 0.1) DitherTime = millis() + 100;
    }
    else CalculateWaveDuty(0); // if duty limit changed, freq/duty will need to be fully re-calculated:
  }
  if (FastMode >= 0)               // if in fast mode
  {
    SyncDelay = 0; // remove square wave sync delay for stability in case of sudden change to high freq
    TimerCounts = freqToTc(TargetWaveFreq);
    if (OldFastMode < 0)           // if exiting slow mode
    {
      NVIC_DisableIRQ(TC0_IRQn);
      dac_setup();
      TC_setup();
      CalculateWaveDuty(0);
      CreateNewWave();
    }
    else
    {
      TC_setup();
      if (TargetWavePulseWidth > 0)
      {
        CalculateWaveDuty(0);
        CreateNewWave();
      }
    }
    if (FastMode == OldFastMode) SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty)); // if fast mode changed calculate later
    else if (FastMode >= 0) ; // do nothing - Very slight delay needed anyway before proceeding
  }  
  else // if (FastMode < 0)        // if in slow mode
  {
    TimerCounts = freqToTc(TargetWaveFreq);
    if (OldFastMode  >= 0)         // if exiting fast mode
    {
      dac_setup2();
      TC_setup2();
      NVIC_EnableIRQ(TC0_IRQn);
      CalculateWaveDuty(0);
    }
  }    
//  MEASURE FREQUENCY:
  if (ExactFreqMode) ActualWaveFreq = TargetWaveFreq; // if in ExactFreqMode - absolutely exact only at 50% duty-cycle
  else
  {
    if (TargetWaveFreq > 1000) ActualWaveFreq = tcToFreq(TimerCounts); // if in fast mode
    else                       ActualWaveFreq = 200000 / ceil(200000 / (TargetWaveFreq * 1.0000075)); // if in slow mode - previously 1.0002519
  }                                       //  increase this number if measurement is too low   ^^^
  
//  MEASURE DUTY: (if necessary, due to duty limit changing with freq)
  if (FastMode >= 0) // if in fast mode
  {
    if (FastMode != OldFastMode || TargetWaveDuty > 0 || TargetWaveDuty < 100) // duty changes only when fast mode changes or at extreme duty-cycle
    { 
//      if (TargetWaveDuty == 0 || TargetWaveDuty == 100) ActualWaveDuty = TargetWaveDuty;
      if      (TargetWaveDuty == 0)   ActualWaveDuty = 0.0048 / (1000 / ActualWaveFreq);
      else if (TargetWaveDuty == 100) ActualWaveDuty = ((1000 / ActualWaveFreq) - 0.000048) / (10 / ActualWaveFreq);
      else ActualWaveDuty = (100.0 * Duty[0][FastMode]) / (Duty[0][FastMode] + Duty[1][FastMode]); // 1st wave half / (1st wave half + 2nd wave half)
      SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty));
    }
  }
  else // if in slow mode, duty limit was calculated with freq above
  {
    if      (TargetWaveDuty == 0)   ActualWaveDuty = 0.0048 / (1000 / ActualWaveFreq); ////new6
    else if (TargetWaveDuty == 100) ActualWaveDuty = ((1000 / ActualWaveFreq) - 0.000048) / (10 / ActualWaveFreq);
    else ActualWaveDuty = constrain(TargetWaveDuty, dutyLimit, 100 - dutyLimit);
  }
  if (show)
  {
    Serial.print("   Analogue Wave Freq: ");
    PrintSyncedWaveFreq(); Serial.print(", Target: ");
    Serial.print(TargetWaveFreq, 3);
    Serial.print(" Hz\n   Analogue Wave Period: ");
    PrintSyncedWavePeriod();
    Serial.print("   Analogue Wave Duty-cycle: "); Serial.print(ActualWaveDuty); Serial.println(" %\n");
  }
  LastAllowedWaveDuty = allowedWaveDuty;
}

void CalculateWaveDuty(bool updateSlowMode) // FOR ANALOGUE & SYNCHRONIZED SQUARE WAVE:
{
  float dutyLimit = 0;
  float allowedWaveDuty = TargetWaveDuty;
  if (FastMode >= 0)
  {
//  SET DUTY:
    int du[2][5];
    if (TargetWaveDuty > 0 && TargetWaveDuty < 100)
    {
      du[0][0] = constrain(round(TargetWaveDuty *  1.6), MinMaxDuty, 160 - MinMaxDuty); // 1st half of cycle at FastMode 0 (160 = SamplesPerCycle)
      du[0][1] = constrain(round(TargetWaveDuty *  0.8), MinMaxDuty,  80 - MinMaxDuty); // 1st half of cycle at FastMode 1
      du[0][2] = constrain(round(TargetWaveDuty *  0.4), MinMaxDuty,  40 - MinMaxDuty); // 1st half of cycle at FastMode 2
      du[0][3] = constrain(round(TargetWaveDuty * 0.16), MinMaxDuty,  16 - MinMaxDuty); // 1st half of cycle at FastMode 3
      du[1][0] = 160 - du[0][0]; // 2nd half of cycle at FastMode 0
      du[1][1] =  80 - du[0][1]; // 2nd half of cycle at FastMode 1
      du[1][2] =  40 - du[0][2]; // 2nd half of cycle at FastMode 2
      du[1][3] =  16 - du[0][3]; // 2nd half of cycle at FastMode 3
      for (byte i = 0; i <= 3; i++)
      {
        Duty[0][i] = du[0][i]; // update quickly after calculating above
        Duty[1][i] = du[1][i]; // update quickly after calculating above
      }
      MinOrMaxWaveDuty = 0;
//      if (SquareWaveSync && (du[0][FastMode] <= 4 || du[1][FastMode] <= 4)) WavePolarity(); // MinMaxDuty = 4
      if (SquareWaveSync && du[0][FastMode] != du[1][FastMode]) WavePolarity();
    }
    else // if TargetWaveDuty = 0 or 100, display only one wave half:
    {
      if (TargetWaveDuty == 0) WaveHalf = 0; // display 2nd wave half only (neg going)
      else WaveHalf = 1; // display 1st wave half only (pos going)
      Duty[!WaveHalf][0] = 160;
      Duty[!WaveHalf][1] =  80;
      Duty[!WaveHalf][2] =  40;
      Duty[!WaveHalf][3] =  16;
      MinOrMaxWaveDuty = 1;
    }
//  MEASURE DUTY:
    if      (TargetWaveDuty == 0)   ActualWaveDuty = 0.0048 / (1000 / ActualWaveFreq);
    else if (TargetWaveDuty == 100) ActualWaveDuty = ((1000 / ActualWaveFreq) - 0.000048) / (10 / ActualWaveFreq);
    else ActualWaveDuty = (100.0 * Duty[0][FastMode]) / (Duty[0][FastMode] + Duty[1][FastMode]); // 1st wave half / (1st wave half + 2nd wave half)
    SyncDelay = (TimerCounts - Delay1) * Delay2 * max((Delay3 / (abs(ActualWaveDuty - 50.0) + Delay3)), int(MinOrMaxWaveDuty));    
  }
  if (FastMode < 0 || updateSlowMode)
  {
//  SET DUTY:
    MinOrMaxWaveDuty = 0;
    double FreqIncr = FreqIncrement;
    if (TargetWaveDuty > 0 && TargetWaveDuty < 100) dutyLimit = 1 / (4000 / TargetWaveFreq); // calculate percent duty limit of 1 sample at 400kHz (up to 25% at 100kHz)
    if (ExactFreqMode)
    {
      FreqIncr *= ExactFreqModeAccuracy; // accuracy adjustment
      if (TargetWaveDuty != 50) ExactFreqDutyNot50 = 1;
      else ExactFreqDutyNot50 = 0;
    }
    allowedWaveDuty = constrain(TargetWaveDuty, dutyLimit, 100 - dutyLimit);
    if (allowedWaveDuty == 0)
    {
      FreqIncrmt[0] = FreqIncr / 2;
      WaveHalf = 0;
      MinOrMaxWaveDuty = 1;
    }
    else FreqIncrmt[0] = constrain((1.00 / (           allowedWaveDuty  / 50.000)) * FreqIncr, 0 , 4294967295);
    if (allowedWaveDuty == 100)
    {
      FreqIncrmt[1] = FreqIncr / 2;
      WaveHalf = 1;
      MinOrMaxWaveDuty = 1;
    }
    else FreqIncrmt[1] = constrain((1.00 / ((100.000 - allowedWaveDuty) / 50.000)) * FreqIncr, 0 , 4294967295);
    IncrProportion[0] = FreqIncrmt[0] / FreqIncr;
    IncrProportion[1] = FreqIncrmt[1] / FreqIncr;
    int dutyMulti1    = (IncrProportion[1] / IncrProportion[0]) * 1000;
    DutyMultiplier[0] = (IncrProportion[0] / IncrProportion[1]) * 1000;
    DutyMultiplier[1] = dutyMulti1;    // update quickly after calculating above 
    Increment[0] = FreqIncrmt[0]; // update quickly after calculating above
    Increment[1] = FreqIncrmt[1]; // update quickly after calculating above
    if (TargetWaveFreq < 0.1) DitherTime = millis() + 100;
//  MEASURE DUTY:
    if      (TargetWaveDuty == 0)   ActualWaveDuty = 0.0048 / (1000 / ActualWaveFreq);
    else if (TargetWaveDuty == 100) ActualWaveDuty = ((1000 / ActualWaveFreq) - 0.000048) / (10 / ActualWaveFreq);
    else ActualWaveDuty = constrain(TargetWaveDuty, dutyLimit, 100 - dutyLimit);
  }
}

void SetFreqAndDuty(bool setFreq, bool setDuty) // Sets freq & duty-cycle and calculates measurements - FOR UNSYNCHRONIZED SQUARE WAVE:
{
  if (setFreq)
  {
    if (TargetFreq >= 1300) // Period <= 64615) // >= 1300 Hz
    {
      Period = round(84000000 / TargetFreq);
      if (!SquareWaveSync && pwmFreq == 10) // if viewing unsynch'ed sq.wave and exiting slow mode
      {
        REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
        REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
        NVIC_DisableIRQ(TC1_IRQn);
        PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
      }
      pwmFreq = 42000000;
      uPeriodMultiplier = 1;
    }
    else if (TargetFreq >= 650) // Period <= 129231) // >= 650 Hz
    {
      Period = round(42000000 / TargetFreq);
      if (!SquareWaveSync && pwmFreq == 10) // if viewing unsynch'ed sq.wave and exiting slow mode
      {
        REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
        REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
        NVIC_DisableIRQ(TC1_IRQn);
        PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
      }
      pwmFreq = 10500000;
      uPeriodMultiplier = 2;
    }
    else if (TargetFreq >= 325) // Period <= 420000) // >= 325 Hz
    {
      Period = round(21000000 / TargetFreq);
      if (!SquareWaveSync && pwmFreq == 10) // if viewing unsynch'ed sq.wave and exiting slow mode
      {
        REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
        REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
        NVIC_DisableIRQ(TC1_IRQn);
        PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
      }
      pwmFreq = 2;
      uPeriodMultiplier = 4;
    }
    else if (TargetFreq >= 163) // Period <= 420000) // >= 163 Hz
    {
      Period = round(10500000 / TargetFreq);
      if (!SquareWaveSync && pwmFreq == 10) // if viewing unsynch'ed sq.wave and exiting slow mode
      {
        REG_PIOC_PER |= PIO_PER_P28; // PIO takes control of pin 3 from peripheral - similar to pinMode(3, OUTPUT)
        REG_PIOC_ODR |= PIO_ODR_P28; // PIO disables pin 3 (C28) - similar to pinMode(3, INPUT)
        NVIC_DisableIRQ(TC1_IRQn);
        PWMC_EnableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
      }
      pwmFreq = 4;
      uPeriodMultiplier = 8;
    }
    else // if (TargetFreq < 163) // if < 163 Hz, slow mode
    {
      Period = round(200000 / TargetFreq);
      if (!SquareWaveSync && pwmFreq != 10) // if viewing unsynch'ed sq.wave and entering slow mode
      {
        PWMC_DisableChannel(PWM_INTERFACE, g_APinDescription[7].ulPWMChannel);
        pinMode(7, INPUT);
        TC_setup3();
        PIO_Configure(PIOC, PIO_PERIPH_B, PIO_PC28B_TIOA7, PIO_DEFAULT);
      }
      pwmFreq = 10; // not used but identifies mode of operation
      uPeriodMultiplier = 420; // 210;
    }
  }
  if (setDuty) Pulse = constrain(round((TargetDuty / 100.0) * Period), 1, Period - 1);
  if (setDuty && !setFreq) ; // if setting duty only skip following freq measurement
  else // if setting freq (or if only measuring. ie: !setDuty && !setFreq)
  {
    uPeriod = (float(Period) * 0.0119047619047619) * uPeriodMultiplier;
    ActualFreq = 1000000.00 / uPeriod;
  }
  if (TargetFreq < 163) // load variables for interupt handler (and calculate uPulseWidth)
  {
    if (TargetDuty > 0 && TargetDuty < 100)
    {
      PulsePeriod[0] = Pulse;
      PulsePeriod[1] = Period;
      MinOrMaxDuty = 0;
      uPulseWidth = (float(Pulse) * 0.0119047619047619) * uPeriodMultiplier;
    }
    else
    {
      PulsePeriod[0] = Period;
      PulsePeriod[1] = Period;
      MinOrMaxDuty = 1;
      if (TargetDuty == 0)
      {
        PeriodHalf = 0;
        uPulseWidth = 0.096;
      }
      else
      {
        PeriodHalf = 1;
        uPulseWidth = uPeriod - 0.12;
      }
    }
  }
  else
  {
    if (!SquareWaveSync) SetPWM(7, Period, Pulse); // if viewing unsynch'ed sq.wave and freq >= 163Hz set up PWM
    uPulseWidth = (float(Pulse) * 0.0119047619047619) * uPeriodMultiplier;
  }
  ActualDuty = (uPulseWidth * 100) / uPeriod;
}

void SetPWM(byte pwmPin, uint32_t maxDutyCount, uint32_t duty) // sets up PWM for Unsynchronized Square Wave when at least 163Hz
{
  if (pwmFreq > 4) // if >= 650Hz
  {
    pmc_enable_periph_clk(PWM_INTERFACE_ID);
    PWMC_ConfigureClocks(clkAFreq, 0, VARIANT_MCK);
    PIO_Configure(g_APinDescription[pwmPin].pPort,
                  g_APinDescription[pwmPin].ulPinType,
                  g_APinDescription[pwmPin].ulPin,
                  g_APinDescription[pwmPin].ulPinConfiguration); 
    uint32_t channel = g_APinDescription[pwmPin].ulPWMChannel;
    PWMC_ConfigureChannel(PWM_INTERFACE, channel , pwmFreq, 0, 0);
    PWMC_SetPeriod(PWM_INTERFACE, channel, maxDutyCount);
    PWMC_EnableChannel(PWM_INTERFACE, channel);
    PWMC_SetDutyCycle(PWM_INTERFACE, channel, maxDutyCount - duty);
//    pmc_mck_set_prescaler(2); // creates 84MHz output but can't change back!
  }
  else
  {
  // Select Instance = PWM; Signal = PWML6 (channel 6); I/O Line = PC23 (C23, Arduino pin 7, see pinout diagram) ; Peripheral = B
    PMC->PMC_PCER1 |= PMC_PCER1_PID36;                      // Enable PWM 
    REG_PIOC_ABSR |= PIO_ABSR_P23;                          // Set PWM pin perhipheral type B
    REG_PIOC_PDR |= PIO_PDR_P23;                            // Set PWM pin to an output
    REG_PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(pwmFreq);  // Set the PWM clock rate to 2MHz (84MHz/42) ; Adjust DIVA for the resolution you are looking for
    REG_PWM_CMR6 = PWM_CMR_CALG |PWM_CMR_CPRE_CLKA;        // The period is left aligned, clock source as CLKA on channel 6
    REG_PWM_CPRD6 = maxDutyCount;                         // Channel 6 : Set the PWM frequency
    REG_PWM_CDTY6 = duty;                                // Channel 6: Set the PWM duty cycle
    REG_PWM_ENA = PWM_ENA_CHID6;                        // Enable the PWM channel 6 (see datasheet page 973) 
  }
}

void TC1_Handler() // used for Unsynchronized Square Wave when below 163Hz (200,000 clocks per Sec)
{
  // We need to get the status to clear it and allow the interrupt to fire again:
  TC_GetStatus(TC0, 1);
  TimeIncrement++; // 200,000 per Sec
  if (TimeIncrement >= PulsePeriod[PeriodHalf]) // if at end of pulse or period
  {
    if (MinOrMaxDuty) // if duty set to 0 or 100
    {
      if (PeriodHalf) // if duty set to 100
      {
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR; // set TIOA (sq. wave on pin 3) LOW when triggered (on next line)
        TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // software trigger also resets the counter and restarts the clock
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET; // set TIOA (sq. wave on pin 3) HIGH again when triggered (after else statement)
      }
      else // if duty set to 0
      {
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
        TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // output is set to 1, the counter is reset and the clock is started
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
      }
      TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // output is reset (creating 96nS pulse), the counter is reset and the clock is started
      TimeIncrement = 0; // if at end of period
    }
    else // if duty not set to 0 or 100
    {
      if (PeriodHalf)
      {
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
        TimeIncrement = 0; // if at end of period
      }
      else TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
      TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG;
      PeriodHalf = !PeriodHalf; // change to other half of period
    }
  }
}

void DACC_Handler(void) // write analogue & synchronized square wave to DAC with DMA - Fast Mode
{
  if      (FastMode == 3) DACC->DACC_TNPR = (uint32_t) Wave3[!WaveHalf]; // if (FastMode == 3) // next DMA buffer
  else if (FastMode == 2) DACC->DACC_TNPR = (uint32_t) Wave2[!WaveHalf]; // if (FastMode == 2) // next DMA buffer
  else if (FastMode == 1) DACC->DACC_TNPR = (uint32_t) Wave1[!WaveHalf]; // if (FastMode == 1) // next DMA buffer
  else if (FastMode == 0) DACC->DACC_TNPR = (uint32_t) Wave0[!WaveHalf]; // if (FastMode == 0) // next DMA buffer
  DACC->DACC_TNCR = Duty[!WaveHalf][max(0, FastMode)]; // number of counts until Handler re-triggered (when next half cycle finished)
  if (MinOrMaxWaveDuty) // if duty set to 0 or 100
  {
    if (SquareWaveSync) // creates squarewave synchronized with triangle or sine wave
    {
      for(int i = 0; i < SyncDelay; i++) {while(WaveHalf != WaveHalf);} // delays of very short duration. ie: less than 1 microsecond ammounts, to sync with analogue wave
      if (WaveHalf)
      {
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR; // set TIOA (sq. wave on pin 3) LOW when triggered (on next line)
        TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // software trigger also resets the counter and restarts the clock
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET; // set TIOA (sq. wave on pin 3) HIGH again when triggered (after else statement)
      }
      else
      {
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
        TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // the counter is reset and the clock is started
        TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
      }
      TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // output is reset (creating 48nS pulse), the counter is reset and the clock is started
    }
  } 
  else // if duty not set to 0 or 100
  {
    if (SquareWaveSync) // creates squarewave synchronized with triangle or sine wave
    {
      for(int i = 0; i < SyncDelay; i++) {while(WaveHalf != WaveHalf);} // delays of very short duration. ie: less than 1 microsecond ammounts, to sync with analogue wave
      if (WaveHalf) TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
      else TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
      TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG; // the counter is reset and the clock is started
    }
    WaveHalf = !WaveHalf; // change to other wave half
  }
}

void TC0_Handler() // write analogue & synchronized square wave to DAC - Slow Mode (400,000 clocks per Sec)
{
  TC_GetStatus(TC0, 0);
  WaveBit += Increment[!WaveHalf]; // add appropriate Increment to WaveBit
  if (WaveBit < Increment[!WaveHalf]) // if end of wave half (if 4294967295 exceeded - highest number for 32 bit int before roll-over to zero)
  {
    if (ExactFreqDutyNot50) // if in exact freq mode and not at 50% duty-cycle
      WaveBit = (WaveBit / 1000) * DutyMultiplier[WaveHalf]; // if not 50% duty-cycle TRY to maintain freq
    else if (!ExactFreqMode) WaveBit = 1; // if not in exact freq mode reset to 1, allowing next update to be lower at 0 (necessary for very low duty cycle)
    if (MinOrMaxWaveDuty) // if duty set to 0 or 100
    {
      if (!WaveHalf) DACC->DACC_CDR = WaveFull[WaveBit / 1048576]; // if displaying 2nd wave half only (just passed end), write to DAC (1048576 = 4294967296 / 4096)
      else           DACC->DACC_CDR = WaveFull[(4294967295 - WaveBit) / 1048576]; // if displaying 1st wave half only (just passed end), write to DAC
      if (SquareWaveSync)
      {
        if (WaveHalf) // if duty set to 100
        {
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR; // set TIOA (sq. wave on pin 3) LOW when triggered (on next line)
          TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // software trigger also resets the counter and restarts the clock
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET; // set TIOA (sq. wave on pin 3) HIGH when triggered (after else statement)
        }
        else // if duty set to 0
        {
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET;
          TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // the counter is reset and the clock is started
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR;
        }
        TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // output is reset (creating 48nS pulse), the counter is reset and the clock is started
      }
    }
    else // if duty not set to 0 or 100
    {
      if (WaveHalf)
      {
        DACC->DACC_CDR = WaveFull[WaveBit / 1048576]; // if 2nd wave half (just passed end of 1st half), write to DAC
        if (SquareWaveSync)
        {
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_CLEAR; // set TIOA (sq. wave on pin 3) LOW when triggered (on next line)
          TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // the counter is reset and the clock is started
        }
      }
      else
      {
        DACC->DACC_CDR = WaveFull[(4294967295 - WaveBit) / 1048576]; // if 1st wave half (just passed end of 2nd half), write to DAC
        if (SquareWaveSync)
        {
          TC2->TC_CHANNEL[1].TC_CMR = TC_CMR_WAVE | TC_CMR_ASWTRG_SET; // set TIOA (sq. wave on pin 3) HIGH when triggered (on next line)
          TC2->TC_CHANNEL[1].TC_CCR = TC_CCR_SWTRG; // the counter is reset and the clock is started
        }
      }
      WaveHalf = !WaveHalf; // change to other wave half
    }
  }
  else // if not end of wave half
  {
  if (!WaveHalf) DACC->DACC_CDR = WaveFull[WaveBit / 1048576]; // if 2nd wave half, write to DAC
  else          DACC->DACC_CDR = WaveFull[(4294967295 - WaveBit) / 1048576]; // if 1st wave half, write to DAC
  }
}

void TC_setup () // system timer clock set-up for analogue wave & synchronized square wave when in fast mode
{  
  pmc_enable_periph_clk(TC_INTERFACE_ID);
  TcChannel * t = &(TC0->TC_CHANNEL)[0];           
  t->TC_CCR = TC_CCR_CLKDIS;                       
  t->TC_IDR = 0xFFFFFFFF;                           
  t->TC_SR;                                         
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |  TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC;
  t->TC_RC = TimerCounts;
  t->TC_RA = TimerCounts /2;       
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET;
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;  
}
  
void TC_setup2() // system timer clock set-up for analogue wave & triggering synchronized square wave when in slow mode
{
  pmc_set_writeprotect(false);     // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC0);   // enable peripheral clock TC0
// we want wavesel 01 with RC:
  TC_Configure(/* clock */TC0,/* channel */0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1); // select 42 MHz clock
  TC_SetRC(TC0, 0, 105); // select divisor of 105 (42 MHz / 105) - clocks DAC at 400kHz
  TC_Start(TC0, 0);  
  TC0->TC_CHANNEL[0].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC0->TC_CHANNEL[0].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register
  NVIC_EnableIRQ(TC0_IRQn);
}

void TC_setup3() // system timer clock set-up for Unsynchronized Square Wave when below 163Hz (PWM used above 163Hz)
{
//  Serial.println("Entering < 163Hz mode");
  pmc_set_writeprotect(false);     // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC1);   // enable peripheral clock TC1 (TC0, channel 1)
// we want wavesel 01 with RC:
  TC_Configure(/* clock */TC0,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1); // select 42 MHz clock
  TC_SetRC(TC0, 1, 210); // select divisor of 210 (42 MHz / 210) - clocks at 200kHz
  TC_Start(TC0, 1);  
  TC0->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC0->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register  
// Enable the interrupt in the nested vector interrupt controller
// TC1_IRQn where 1 is the timer number * timer channels (3) + the channel number (=(0*3)+1) for timer 0 channel 1:
  NVIC_EnableIRQ(TC1_IRQn);
}

void TC_setup4() // timer clock set-up for synchronized square wave
{
  pmc_enable_periph_clk (TC_INTERFACE_ID + 2*3+1);    // clock the TC2 channel 1. for pin 3
//  pmc_enable_periph_clk (TC_INTERFACE_ID + 2*3+2); // clock the TC2 channel 2. for pin 11 instead
  TcChannel * t = &(TC2->TC_CHANNEL)[1];            // pointer to TC2 registers for its channel 1. for pin 3
//  TcChannel * t = &(TC2->TC_CHANNEL)[2];         // pointer to TC2 registers for its channel 2. for pin 11 instead
  t->TC_CCR = TC_CCR_CLKDIS;                      // disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF;                        // disable interrupts
  t->TC_SR;                                     // read int status reg to clear pending
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |     // use TCLK1 (prescale by 2, = 42MHz)
              TC_CMR_WAVE |                   // waveform mode
              TC_CMR_WAVSEL_UP |             // count-up without auto RC triggering
              TC_CMR_ASWTRG_SET |           // Software Trigger effect on TIOA
              TC_CMR_CPCTRG |              // enable manual RC triggering
              TC_CMR_ACPA_NONE |          // RA effect on TIOA
              TC_CMR_ACPC_NONE;          // RC effect on TIOA
  TC_Start(TC2, 1);
}

void TC_setup5() // timer clock set-up for timer - when operated from the serial monitor
{
  pmc_set_writeprotect(false);     // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC4);   // enable peripheral clock TC1, channel 1
  TC_Configure(TC1, 1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4); // select 656.250 kHz clock
  TC_SetRC(TC1, 1, 39375000); // select divisor of 39375000 (656250Hz / 39375000) - resets at 1 minute
  TC_Start(TC1, 1);  
}

void dac_setup() // DAC set-up for analogue wave & triggering synchronized square wave when in fast mode and using DMA
{
  pmc_enable_periph_clk (DACC_INTERFACE_ID) ;   // start clocking DAC
  dacc_reset(DACC);
  dacc_set_transfer_mode(DACC, 0);
  dacc_set_power_save(DACC, 0, 1);              // sleep = 0, fast wakeup = 1
  dacc_set_analog_control(DACC, DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02) | DACC_ACR_IBCTLDACCORE(0x01));
  dacc_set_trigger(DACC, 1);
//  dacc_set_channel_selection(DACC, 1);
//  dacc_enable_channel(DACC, 1);
  dacc_set_channel_selection(DACC, 0);
  dacc_enable_channel(DACC, 0);
  NVIC_DisableIRQ(DACC_IRQn);
  NVIC_ClearPendingIRQ(DACC_IRQn);
  NVIC_EnableIRQ(DACC_IRQn);
  dacc_enable_interrupt(DACC, DACC_IER_ENDTX);
  DACC->DACC_TPR  = (uint32_t) Wave0[0];     // DMA buffer
  DACC->DACC_TCR  = NWAVETABLE / 2;
  DACC->DACC_TNPR = (uint32_t) Wave0[1];     // next DMA buffer
  DACC->DACC_TNCR = NWAVETABLE / 2;
  DACC->DACC_PTCR = 0x00000100;
}

void dac_setup2() // DAC set-up for analogue & synchronized square wave when in slow mode (and Exact Freq Mode at any freq)
{
  NVIC_DisableIRQ(DACC_IRQn);
  NVIC_ClearPendingIRQ(DACC_IRQn);
  dacc_disable_interrupt(DACC, DACC_IER_ENDTX); // disable DMA
  DACC->DACC_CR = DACC_CR_SWRST ;               // reset DAC
  DACC->DACC_CHER = DACC_CHER_CH0 << 0 ;        // enable chan0
}

