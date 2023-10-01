%% Absolute frequency test
clear all;
close all;
fs = 48000; % Sample rate, 48 kHz
f = 440; % Signal frequency, 100 Hz
duration = 1; % Signal duration in seconds

t = 0:1/fs:(duration - 1/fs); % Time vector
%% create sin wave
input = sin(2 * pi * f* t);
%% absolute
analogrange=250;
fftrange=10000;
nexttile
plot(input,"LineWidth",1);
grid on;
xlim([1 analogrange]);
title('input')

nexttile
plot(abs(input),"LineWidth",1);
grid on;
xlim([1 analogrange]);
title('abs(input)')

nexttile
plot(abs(fft(input)),"LineWidth",1);
grid on;
xlim([1 fftrange]);
title('input spectrum')

nexttile
plot(abs(fft(abs(input))),"LineWidth",1);
grid on;
xlim([1 fftrange]);
title('abs(input) spectrum')
