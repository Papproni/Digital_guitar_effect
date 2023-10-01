%% this script calculates the parameters of an ERB filter ranging form 160Hz to 8kHz
%  The functions gives back the coefficients of biquad filters.

%% FILTERBANK PARAMS
numberofSubbands=43; % number of subbands
k=1:numberofSubbands;
k=k;
fs=48000; % sampleRate
fmin=160; %minimum frequency
fmax=8000; % maximum frequency
%% ERB PARAMETERS
qerb=6; qc=4; zleft=5; BWerb=28;
K=43;Ga=-12 ;Gt=-12;

%% erb to hz
center_frequencys=subband_centerfreq(k,qerb,qc,zleft);
plot(center_frequencys,'.')

%[a,b]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);
[b,a]=calculate_biquad_of_subband(center_frequencys,BWerb,fs);




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
    
    endb