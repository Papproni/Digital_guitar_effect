close all;
sampleSize=4096
N=1:sampleSize;
input=sin(6.28/10*N);

figure("name","Input signal")
plot(input)

figure("name","windowed Input")
w1 = window(@hamming,sampleSize); 
%input=input.*w1';
plot(input)

figure("name","FFT")
fft_input=fft(input,sampleSize);
plot(abs(fft_input(1:sampleSize/2)))

fft_input_copy=fft_input;
  for i=1:(length(fft_input)/2)
        fft_input(i+i)=fft_input_copy(i);
  end
fft_input=fft_input+fft_input_copy;
figure("name","shifted FFT")
plot(abs(fft_input(1:sampleSize/2)))

output=ifft(fft_input,"symmetric");%% only <samplesize/2 complexes matter!!!
figure("name","Output after iFFT")
plot(output)