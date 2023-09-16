%% TD-PSOLA: time domain pitch shift overlap add
clear all;
fs = 48000; % Sample rate, 48 kHz
f = 200; % Signal frequency, 100 Hz
duration = 1; % Signal duration in seconds

t = 0:1/fs:(duration - 1/fs); % Time vector

input = sin(2 * pi * (f) * t)+sin(2*pi*f/4);
input_shift = sin(2 * pi * (f) * t+pi/2);
output_expected=sin(2 * pi * (f) * t)+sin(2 * pi * (2*f) * t);
%[input,fs] = audioread("guitar_fast_picking.wav");

%output=0.5*(input+2*input.*input_shifted);
gamma=2; % 2 as in octave1up

%% INPUT FILTER FOR SUBBAND
f_c=200;
K=tan((pi*f_c)/fs);
q=10;
b0=K/(K*K*q+K+q);
b1=0;
b2=-K/(K*K*q+K+q);
a1=2*q*(K*K-1)/(K*K*q+K+q);
a2=(K*K*q-K+q)/(K*K*q+K+q);
b=[b0 b1 b2];
a=[1 a1 a2];
input_subband=filter(b,a,input);
output=zeros(length(t),1);


%% PHASE SHIFT
f_max=8000;%1200 fasza
N_min=8;
N_max=200;

N = round(N_min * log2(f_max / f))
N=min(N,N_max)
N=3;
scale_factor=N*16/f+50
h = firpm(N, [0.1, 0.9], [1, 1], 'Hilbert');
input_shifted=filter(h, 1, input_subband)*scale_factor.*input_subband;

%% Filter shifted signal
f_c=f_c*gamma;
K=tan((pi*f_c)/fs);
b0=K/(K*K*q+K+q);
b1=0;
b2=-K/(K*K*q+K+q);
a1=2*q*(K*K-1)/(K*K*q+K+q);
a2=(K*K*q-K+q)/(K*K*q+K+q);
b=[b0 b1 b2];
a=[1 a1 a2];
input_shifted_filtered=filter(b,a,input_shifted);

%% Add filterd and original signal together
output=input_shifted_filtered+input_subband;

%hold off
%close all
%hold on
%plot(output(1:500));
%plot(input_shifted(1:500));
%plot(input(1:500));
%grid on
%legend({'output','input shifted','input'},'Location','southwest')
%hold off
%{
sound(input,fs)
pause(1.5);
sound(input.*input_shifted,fs)
pause(1.5);
sound(output_expected,fs)
pause(1.5);
sound(output,fs)
%}




input_fft=abs(fft(input));
input_fft=input_fft(1:500);
output_fft=abs(fft(output));
output_fft=output_fft(1:500);
close all
hold on

plot(input_fft);
plot(output_fft)
title('Shift only one frequency up 1 octave')
legend({'inputFFT','outputFFT'},'Location','northeast')
hold off
%close all
%hold on
%plot(input)