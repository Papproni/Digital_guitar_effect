%% This script implements the ERB-PS2 algorythm with a real guitar signal.
%% For testing purpose i will use only one octave up to hear the effect, if its worth trying.

%% FILTERBANK PARAMS
numberofSubbands=80; % number of subbands
k=1:numberofSubbands;
k=k;
fs=48000; % sampleRate
fmin=160; %minimum frequency
fmax=8000; % maximum frequency
%% ERB PARAMETERS
qerb=6; qc=4; zleft=5; BWerb=28;
K=43;Ga=-12 ;Gt=-12;

%% erb to hz
center_frequencys=subband_centerfreq(k,qerb,qc,zleft)/gamma;


%[a,b]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);
[b,a]=calculate_biquad_of_subband(center_frequencys,50,fs);

% calculate which filter to use for octave up sound NOTE! THIS IS ONLY FOR
% 1 OCTAVE UP!!!
delta_K=calculate_subbandhopps(gamma,center_frequencys,zleft,qerb,qc);
delta_k_normal=calculate_subbandhopps(1,center_frequencys,zleft,qerb,qc);
delta_K=delta_K-delta_k_normal;
delta_K=round(delta_K);


%% INPUT SIGNAL, buffers
fs = 48000; % Sample rate, 48 kHz
f = 440; % Signal frequency, 100 Hz
duration = 1; % Signal duration in seconds

t = 0:1/fs:(duration - 1/fs); % Time vector

input = sin(2 * pi * (f*1)* t);%+sin(2 * pi * f/3 * t);
[input,fs] = audioread("guitar_chords.wav");
%[input,fs] = audioread("guitar_fast_picking.wav");
input=input';
input_subbands              =   zeros(numberofSubbands,length(input));
input_shifted_1up           =   zeros(numberofSubbands,length(input));
input_shifted_1up_filtered  =   zeros(numberofSubbands,length(input));
output=zeros(1,length(input));

%% step1: input is fitlered by subbands
for i = 1:numberofSubbands
    input_subbands(i,1:end)=filter(b(i,1:3),a(i,1:3),input*20);
end


%% step2: shift the subbanded signals
N=3;    
h = firpm(N, [0.05, 0.95], [1, 1], 'Hilbert');

input_shifted_1up=filter(h, 1, input_subbands).*input_subbands;



%% step3: Filter the shifted signal. Every signal should be filtered with their freq*2 subband filter
%% ecxample 300Hz to 600 Hz signal

for i = 6:numberofSubbands/2
    input_shifted_1up_filtered(i,1:end)=filter(b(i+delta_K(i),1:3),a(i+delta_K(i),1:3),input_shifted_1up(i,1:end));
end
obsderveIndex=10;
close all;
nexttile
plot(abs(fft(input_subbands(obsderveIndex,1:end))));
nexttile
plot(abs(fft(input_shifted_1up(obsderveIndex,1:end))));
nexttile
plot(abs(fft(input_shifted_1up_filtered(obsderveIndex,1:end))));

%% step4: SUM the frequencies together

output=sum(input_shifted_1up_filtered,1);
output=output+input;

%sound(input_shifted_1up(1,1:end),fs)
sound(input,fs)
pause(20)
sound(output,fs)























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