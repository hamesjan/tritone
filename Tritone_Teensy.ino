#include <Audio.h>
#include <Arduino.h>
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

AudioControlSGTL5000 audioShield;

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  while (!Serial);      // Wait for serial connection
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(1);
   fft.windowFunction(AudioWindowHanning1024);
}

unsigned long lastPeakCountTime = 0;  // Time of the last peak count  
unsigned int peakCount = 0;  // Number of peaks detected in the last second
unsigned int bpm = 0;  // Beats per minute

void loop() {
  if(fft.available() && peak1.available()){
    float maxFreq = 0;
    float maxVal = 0;
    for(int i = 1; i < 512; i++){
      float val = fft.read(i);
      if(val > maxVal){
        maxVal = val;
        maxFreq = i * 44100/1024;
      }
    }
    float amplitude = peak1.read();
    if(amplitude < 0.07){
      amplitude = 0;
    }

    unsigned long currentTime = millis();
    static unsigned long intervalStartTime = currentTime;
    static float sumFreq = 0;
    static float avgFreq[5] = {0}; // Array to store 5 average frequencies
    static int sampleCount = 0;
    static float sumAmplitude = 0;
    static float avgAmplitude = 0;
    static int sumBPM = 0;
    static int avgBPM = 0;

    if(currentTime - intervalStartTime <= 5000 && sampleCount < 5){
      if(currentTime - intervalStartTime >= 1000){
        avgFreq[sampleCount] = sumFreq/86;
        sumFreq = 0;
        sampleCount++;
        intervalStartTime = currentTime;
      }

      bpm = calculateBPM(amplitude);
      sumBPM += bpm;
      avgBPM = sumBPM/430;
      sumFreq += maxFreq;
      sumAmplitude += amplitude;
      avgAmplitude = sumAmplitude/430;
    }
    else{
      String tempo;
      if (avgBPM < 60) {
        tempo = "Largo";
      } else if (avgBPM < 76) {
        tempo = "Adagio";
      } else if (avgBPM < 108) {
        tempo = "Moderato";
      } else if (avgBPM < 120) {
        tempo = "Allegro";
      } else {
        tempo = "Presto";
      } 
      Serial.printf("Average Dominant Frequencies (Hz): %.2f, %.2f, %.2f, %.2f, %.2f | Average Amplitude: %.2f\n | Average BPM: %d | Average Tempo: %s \n", avgFreq[0], avgFreq[1], avgFreq[2], avgFreq[3], avgFreq[4], avgAmplitude, avgBPM, tempo.c_str());
      sumFreq = 0;
      sampleCount = 0;
      sumAmplitude = 0;
      sumBPM = 0;
      avgBPM = 0;
      avgAmplitude = 0;
      intervalStartTime = currentTime; 
    }
  }
}

int calculateBPM(float amplitude) {
  // Count peaks in the last second
  unsigned long currentTime = millis();
  if (currentTime - lastPeakCountTime >= 1000) {
    // One second has elapsed, calculate BPM
    bpm = peakCount*60;
    // Reset peak count and update lastPeakCountTime
    peakCount = 0;
    lastPeakCountTime = currentTime;
  }
  if(amplitude < 0.9){
    amplitude = 0;
  }
  // Detect peak
  if(amplitude >= 0.9){ //fine tune 
    peakCount++;
  }
  //Serial.println(amplitude);
  return bpm;
}

