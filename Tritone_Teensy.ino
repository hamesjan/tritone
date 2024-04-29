#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

const int myInput = AUDIO_INPUT_MIC;
// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=616.9999847412109,509.59999084472656
AudioAnalyzeNoteFrequency notefreq;       //xy=806.2000350952148,449.0000047683716
AudioAnalyzePeak         peak1;          //xy=819.9999847412109,558.5999908447266
AudioMixer4               mixer;
AudioConnection patchCord0(i2s1, 0, mixer, 0);
AudioConnection patchCord1(mixer, 0, notefreq, 0);
AudioConnection patchCord4(i2s1, 1, peak1, 0);
// GUItool: end automatically generated code

AudioControlSGTL5000 audioShield;

void setup() {
   AudioMemory(30);
   audioShield.enable();
   audioShield.inputSelect(myInput);
   audioShield.volume(0.5);
   notefreq.begin(0.5);
   Serial.begin(9600);

}

void loop() {
  if(notefreq.available() && peak1.available()){
    float note = notefreq.read();
    float prob = notefreq.probability();
    float amplitude = peak1.read();
    Serial.printf("Note: %3.2f | Probability: %.2f\n | Amplitude: %.2f\n", note, prob, amplitude);
  }
}
