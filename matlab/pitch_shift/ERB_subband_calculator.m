%% this script calculates the parameters of an ERB filter ranging form 160Hz to 8kHz
%  The functions gives back the coefficients of biquad filters.
clear all;
close all;
%% FILTERBANK PARAMS
numberofSubbands=43; % number of subbands
k=1:numberofSubbands;
k=k;
fs=48000; % sampleRate
fmin=160; %minimum frequency
fmax=8000; % maximum frequency
%% ERB PARAMETERS
qerb=8; qc=6; zleft=5; BWerb=28; %28
K=43;Ga=-12 ;Gt=-12;
gamma=2;

%% erb to hz
center_frequencys=subband_centerfreq(k,qerb,qc,zleft)/gamma;
center_frequencys_BW=ERB_critical_bandwidth(center_frequencys,gamma,qerb);
Q=center_frequencys./center_frequencys_BW;

plot(center_frequencys,'.')

%[a,b]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);
[b,a]=calculate_biquad_of_subband(center_frequencys,Q,fs);

delta_K=calculate_subbandhopps(gamma,center_frequencys,zleft,qerb,qc);
delta_k_normal=calculate_subbandhopps(1,center_frequencys,zleft,qerb,qc);
delta_K=delta_K-delta_k_normal;
delta_K=round(delta_K);

% example f=160 2f and 4f filter
[b_low, a_low] = calculate_biquad_of_subband(200,28,fs);
[b_high, a_high] = calculate_biquad_of_subband(400,28,fs);
filter_b=conv(b_low,b_high);
filter_a=conv(a_low,a_high);

function [b, a] = biquad_two_passband_filter(f1, f2, f3, f4, Q, fs)
% Design a biquad filter with two passbands
% Inputs:
%   f1: lower frequency of first passband (Hz)
%   f2: upper frequency of first passband (Hz)
%   f3: lower frequency of second passband (Hz)
%   f4: upper frequency of second passband (Hz)
%   Q: quality factor of the filter
%   fs: sampling frequency (Hz)
% Outputs:
%   b: numerator coefficients of the filter
%   a: denominator coefficients of the filter

% Convert passband frequencies to normalized frequency
w1 = (2/fs) * tan(pi * f1 / fs);
w2 = (2/fs) * tan(pi * f2 / fs);
w3 = (2/fs) * tan(pi * f3 / fs);
w4 = (2/fs) * tan(pi * f4 / fs);

% Calculate filter coefficients for first passband
K1 = tan((w2 - w1)/(2*Q));
b01 = K1^2;
b11 = 0;
b21 = -K1^2;
a01 = K1^2 + K1/Q + 1;
a11 = -2*K1^2 + 2*K1^2/Q - 2;
a21 = K1^2 - K1/Q + 1;

% Calculate filter coefficients for second passband
K2 = tan((w4 - w3)/(2*Q));
b02 = K2^2;
b12 = 0;
b22 = -K2^2;
a02 = K2^2 + K2/Q + 1;
a12 = -2*K2^2 + 2*K2^2/Q - 2;
a22 = K2^2 - K2/Q + 1;

% Combine the two biquad filters
b = conv([b01 b11 b21], [b02 b12 b22]);
a = conv([a01 a11 a21], [a02 a12 a22]);

% Normalize the coefficients to have unity gain at DC
b = b / a01;
a = a / a01;
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
function bw=ERB_critical_bandwidth(ferb,qps,qerb)
    bw=24.7+0.108.*ferb;
    bw=bw*1/qps*16/qerb;
end

%% calculate the bandpass filter for subband
function [b,a]=calculate_biquad_of_subband(fc,q,fs)
    K=tan((pi.*fc)/fs);
    norm_gain = K .* K .* q + K + q;
    b0=K./(K.*K.*q+K+q);
    b1=zeros(1,length(fc));
    b2=-K./(K.*K.*q+K+q);
    a0=ones(1,length(fc));
    a1=2*q.*(K.*K-1)./(K.*K.*q+K+q);
    a2=(K.*K.*q-K+q)./(K.*K.*q+K+q);
    b=[b0' b1' b2']./norm_gain';
    a=[a0' a1' a2']./norm_gain';
end

%% calculate which subband filter should i use for which subband
% Inputs: Gamma: pitch shift factor (-2,-1,1,2), fc: center freq
% returned values are only applicapble from K>Zleft
function subband_hopps=calculate_subbandhopps(gamma,fc,zleft,qerb,qc)
    sfc=log10((gamma*fc+228.7)./(228.7));
    
    subband_hopps=21.3.*sfc-zleft;
    subband_hopps=subband_hopps*qerb/qc;
end
