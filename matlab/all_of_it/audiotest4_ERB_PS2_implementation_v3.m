%% This script implements the ERB-PS2 algorythm with a real guitar signal.
%% Difference v2 vs v3:
% in V2 the octave 1 was generated from 2 shifted signals, but the octave 2
% up was generated from the UNFILTERED octave 1 up, which was full fo
% intermodulation garbage. 
% In this script i will use the filtered output of Octave1UP to generate
% the 2nd octave.
clear all;
close all;
%% FILTERBANK PARAMS
numberofSubbands=60; % number of subbands
k=1:numberofSubbands;
k=k;
fs=44100; % sampleRate
[input,fs] = audioread("sufferLittleChildren.wav");
input=input';
fmin=20; %minimum frequency
fmax=8000; % maximum frequency

input=highpass(input,fmin,fs);

%% ERB PARAMETERS
qerb=6; qc=4; zleft=5; BWerb=50; %28
K=43;Ga=-12 ;Gt=-12;
gamma=2
%% erb to hz
center_frequencys=subband_centerfreq(k,qerb,qc,zleft)/gamma;


%[a,b]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);
[b,a]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);

%% INPUT SIGNAL, buffers
f = 440; % Signal frequency, 100 Hz
duration = 1; % Signal duration in seconds

t = 0:1/fs:(duration - 1/fs); % Time vector


input_subbands              =   zeros(numberofSubbands,length(input));
input_subbands_2            =   zeros(numberofSubbands,length(input));
input_shifted_1up           =   zeros(numberofSubbands,length(input));
input_shifted_2up           =   zeros(numberofSubbands,length(input));
input_shifted_1up_filtered  =   zeros(numberofSubbands,length(input));
input_shifted_2up_filtered  =   zeros(numberofSubbands,length(input));
output=zeros(1,length(input));

%% step1: input is fitlered by subbands
% isolate frequency bands
for i = 1:numberofSubbands
    input_subbands(i,1:end)=filter(b(i,1:3),a(i,1:3),input*8);
end
% shift isolated subbands
for i = 1:numberofSubbands
    input_subbands_2(i,1:end)=filter(b(i,1:3),a(i,1:3),input_subbands(i,1:end));
end


%% step2: shift the subbanded signals
% octave_UP1
input_shifted_1up=input_subbands.*input_subbands_2;

%% step3: Filter the shifted signal. Every signal should be filtered with their freq*2 subband filter
%% ecxample 300Hz to 600 Hz signal

for i = 1:numberofSubbands
    input_shifted_1up_filtered(i,1:end)=highpass(input_shifted_1up(i,1:end),center_frequencys(i),fs);
end


% octave_UP2
input_shifted_2up=input_shifted_1up_filtered.*input_shifted_1up_filtered*10;

for i = 1:numberofSubbands
    input_shifted_2up_filtered(i,1:end)=highpass(input_shifted_2up(i,1:end),center_frequencys(i)*3,fs);
end

%% plot signals
fftrange=100000;
obsderveIndex=10; %% which subband to check on

input_fft=abs(fft(input_subbands(obsderveIndex,1:end)));
input_fft=input_fft(1:fftrange);

input_shifted_1up_fft=abs(fft(input_shifted_1up(obsderveIndex,1:end)));
input_shifted_1up_fft=input_shifted_1up_fft(1:fftrange);
input_shifted_1up_filtered_fft=abs(fft(input_shifted_1up_filtered(obsderveIndex,1:end)));
input_shifted_1up_filtered_fft=input_shifted_1up_filtered_fft(1:fftrange);

input_shifted_2up_fft=abs(fft(input_shifted_2up(obsderveIndex,1:end)));
input_shifted_2up_fft=input_shifted_2up_fft(1:fftrange);
input_shifted_2up_filtered_fft=abs(fft(input_shifted_2up_filtered(obsderveIndex,1:end)));
input_shifted_2up_filtered_fft=input_shifted_2up_filtered_fft(1:fftrange);


%% PLOT THE FFT OF SIGNALS 
close all;
nexttile
plot(input_fft);
title('input subband')
nexttile
plot(input_shifted_1up_fft);
title('Octave 1 UP')
nexttile
plot(input_shifted_1up_filtered_fft);
title('Octave 1 UP filtered')
nexttile
plot(input_shifted_2up_fft);
title('Octave 2 UP')
nexttile
plot(input_shifted_2up_filtered_fft);
title('Octave 2 UP filtered')

nexttile
plot(input_subbands(1:1:end))
title('input subbands')
nexttile
plot(input_subbands_2(1:1:end))
title('input subbands 2')
nexttile
plot(input_shifted_1up_filtered(1:1:end))
title('input shifted 1up filtered')

%% step4: SUM the frequencies together
octave1up=sum(input_shifted_1up_filtered,1);
octave2up=sum(input_shifted_2up_filtered,1);
output=octave1up+octave2up;
output=output+input;
output=lowpass(output,fmax,fs);

%sound(input_shifted_1up(1,1:end),fs)
audiowrite('ERBPS2_output.wav',output,fs);
Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\ERBPS2_output.wav'
winopen(Open_Spec_File)

audiowrite('ERBPS2_octave1up.wav',octave1up,fs);
winopen(Open_Spec_File)
audiowrite('ERBPS2_octave2up.wav',octave2up,fs);
winopen(Open_Spec_File)




















%% calculate which subband filter should i use for which subband
% Inputs: Gamma: pitch shift factor (-2,-1,1,2), fc: center freq
% returned values are only applicapble from K>Zleft
function subband_hopps=calculate_subbandhopps(gamma,fc,zleft,qerb,qc)
    sfc=log10((gamma*fc+228.7)./(228.7));
    
    subband_hopps=21.3.*sfc-zleft;
    subband_hopps=subband_hopps*qerb/qc;
end



%% gives the center freq of subband K
function fc=subband_centerfreq(n,qerb,qc,zleft)
    fc=qerbtohz(zleft+qc/qerb*n);
end
%% converts ERB to Hz
function freq=qerbtohz(z)
    freq=228.7*(10.^(z/21.3)-1);
end

%% gives the critical bandwidth of frequency
function bw=ERB_critical_bandwidth(ferb)
    bw=24.7+0.108.*ferb;
end

%% calculate the bandpass filter for subband
function [b,a]=calculate_biquad_of_subband(fc,q,fs)
    K=tan((pi.*fc)/fs);
    b0=K./(K.*K.*q+K+q);
    b1=zeros(1,length(fc));
    b2=-K./(K.*K.*q+K+q);
    a0=ones(1,length(fc));
    a1=2*q.*(K.*K-1)./(K.*K.*q+K+q);
    a2=(K.*K.*q-K+q)./(K.*K.*q+K+q);
    b=[b0' b1' b2'];
    a=[a0' a1' a2'];
end