fs=48000;
fc=83;
q=2;
K=tan((pi*fc)/fs);

b0=K/(K*K*q+K+q);
b1=0;
b2=-K/(K*K*q+K+q);

a1=2*q*(K*K-1)/(K*K*q+K+q);
a2=(K*K*q-K+q)/(K*K*q+K+q);


[audioIn,fs] = audioread("NoisySpeech-16-22p5-mono-5secs.wav");
win = hann(round(0.04*fs),"periodic");
noverlap = round(0.02*fs);
fftLength = 1024;

[S,F,t] = stft(audioIn,fs, ...
               "Window",win, ...
               "OverlapLength",noverlap, ...
               "FFTLength",fftLength, ...
               "FrequencyRange","onesided");
PowerSpectrum = S.*conj(S);

numBands=32;
range=[50,8000];
normalization="bandwidth";

[fb,cf] = designAuditoryFilterBank(fs, ...
    "FrequencyScale","erb", ...
    "FFTLength",fftLength, ...
    "NumBands",numBands, ...
    "FrequencyRange",range, ...
    "Normalization",normalization);

plot(F,log(fb.'));
grid on
title("ERB Filter Bank")
xlabel("Frequency (Hz)")
