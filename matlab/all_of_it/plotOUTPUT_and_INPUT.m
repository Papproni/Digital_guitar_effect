close all;
hold on;
fft_len=1000;
input_FFT=abs(fft(input));
output_FFT=abs(fft(output));
input_FFT=input_FFT(1:fft_len);
output_FFT=output_FFT(1:fft_len);
plot(input_FFT);
plot(output_FFT);

max_input_freq=max(input_FFT);
max_output_freq=max(output_FFT);

max_input_freq=find(input_FFT>3000)
max_output_freq=find(output_FFT>3000)
%max_output_freq=max(max_output_freq)
%pitch=max_output_freq(1)/max_input_freq(1)
hold off;