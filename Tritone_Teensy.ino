#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

const int myInput = AUDIO_INPUT_MIC;
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=616.9999847412109,509.59999084472656
AudioAnalyzeFFT1024      fft;            //xy=806.2000350952148,449.0000047683716
AudioAnalyzePeak         peak1;          //xy=819.9999847412109,558.5999908447266
AudioMixer4               mixer;
AudioConnection patchCord0(i2s1, 0, mixer, 0);
AudioConnection patchCord1(mixer, fft);
AudioConnection patchCord2(i2s1, 1, peak1, 0);
// GUItool: end automatically generated code

AudioControlSGTL5000 audioShield;

void setup() {
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(1);
   fft.windowFunction(AudioWindowHanning1024);
}

void loop() {
  if(fft.available() && peak1.available()){
    float maxFreq = 0;
    float maxVal = 0;
    for (int i = 1; i < 512; i++) {
      float val = fft.read(i);
      if (val > maxVal) {
        maxVal = val;
        maxFreq = i * 44100 / 1024;
      }
    }
    float amplitude = peak1.read();
    Serial.printf("Dominant Frequency: %.2f Hz | Amplitude: %.2f\n", maxFreq, amplitude);
  }
}
