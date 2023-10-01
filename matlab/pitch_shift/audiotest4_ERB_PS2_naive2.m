%% This script implements the ERB-PS2 algorythm with a real guitar signal.
%% Difference v2 vs v3:
% in V2 the octave 1 was generated from 2 shifted signals, but the octave 2
% up was generated from the UNFILTERED octave 1 up, which was full fo
% intermodulation garbage. 
% In this script i will use the filtered output of Octave1UP to generate
% the 2nd octave.

clear all;
close all;

bar=waitbar(0,'Loading input data');
%% FILTERBANK PARAMS
numberofSubbands=43; % number of subbands
k=1:numberofSubbands;
k=k;
fs=44100; % sampleRate
[input,fs] = audioread("guitar_ONE_chords.wav");

input=input';
fmin=20; %minimum frequency
fmax=4000; % maximum frequency

%input=highpass(input,fmin,fs);
input=lowpass(input,fmax,fs);
waitbar(0.1,bar,'Creating filterbank');
%% ERB PARAMETERS
qerb=8; qc=6; zleft=5; BWerb=28; %28
K=43;Ga=-12 ;Gt=-12;
gamma=2;
%% erb to hz
%% input signal frequency filter bank
center_frequencys=subband_centerfreq(k,qerb,qc,zleft)/gamma;
center_frequencys_BW=ERB_critical_bandwidth(center_frequencys,gamma,qerb);
Q=center_frequencys./center_frequencys_BW;
[filterbank_b,filterbank_a]=calculate_biquad_of_subband(center_frequencys,Q,fs);

%% octave 1 UP filters
octave1up_center_frequencys=center_frequencys*2;
octave1up_center_frequencys_BW=ERB_critical_bandwidth(octave1up_center_frequencys,1,qerb);
Q=octave1up_center_frequencys./octave1up_center_frequencys_BW;
[filterbank_octave1_up_b,filterbank_octave1_up_a]=calculate_biquad_of_subband(octave1up_center_frequencys,Q,fs);

%% octave 1 DOWN filters
octave1down_center_frequencys=center_frequencys/2;
octave1down_center_frequencys_BW=ERB_critical_bandwidth(octave1down_center_frequencys,1,qerb);
Q=octave1down_center_frequencys./octave1down_center_frequencys_BW;
[filterbank_octave1_down_b,filterbank_octave1_down_a]=calculate_biquad_of_subband(octave1down_center_frequencys,28,fs);

%% INPUT SIGNAL, buffers
waitbar(0.15,bar,'Creating buffers');

input_subbands              =   zeros(numberofSubbands,length(input));

input_shifted_1up           =   zeros(numberofSubbands,length(input));
input_shifted_1up_filtered  =   zeros(numberofSubbands,length(input));

input_shifted_2up           =   zeros(numberofSubbands,length(input));
input_shifted_2up_filtered  =   zeros(numberofSubbands,length(input));

input_shifted_1down           =   zeros(numberofSubbands,length(input));
input_shifted_1down2            =   zeros(numberofSubbands,length(input));
input_shifted_1down_filtered  =   zeros(numberofSubbands,length(input));


output=zeros(1,length(input));

%% step1: input is fitlered by subbands
% isolate frequency bands
waitbar(0.3,bar,'Octave up');

for i = 1:numberofSubbands
    input_subbands(i,1:end)=filter(filterbank_b(i,1:3),filterbank_a(i,1:3),input(1,:)*4);
end


%% step2: shift the subbanded signals
%% octave1 up
input_shifted_1up=abs(input_subbands)*2;


%% octave1 down
input_square        =zeros(numberofSubbands,length(input));
input_square_div2   =zeros(numberofSubbands,length(input));
state=1e-4;
state2=1e-4;


rangeoftesting=1000;
for i = 1:numberofSubbands
    %% subbands to square
    %close all
    %hold on
    %plot(input_shifted_1down(i,1:rangeoftesting))
    for j=2:length(input)
        signal_last=input_subbands(i,j-1);
        sugnal_current=input_subbands(i,j);

        if(signal_last>0 && sugnal_current<0)
            state=-1;
        end
        if(signal_last<0 && sugnal_current>0)
            state=1;
        end
        input_shifted_1down2(i,j)=state;
    end
   
    %plot(input_shifted_1down(i,1:rangeoftesting))

    level=0.5;
    %% div freq of square wave
    state_prev=1;
    for j=2:length(input)-1000
        signal_last=input_shifted_1down2(i,j-1);
        sugnal_current=input_shifted_1down2(i,j);

        if(signal_last>0 && sugnal_current<0)
            if state_prev==0
                state=-abs(sugnal_current);
                state_prev=1;
            elseif state_prev==1
                state=abs(sugnal_current);
                state_prev=0;
            end
        end
        
        %input_shifted_1down(i,j)=state/20;
        if sum(abs(input_subbands(i,j:j+100)))>1
            input_shifted_1down(i,j)=state/20;
            %input_shifted_1down(i,j)=0;
        end
        
    end
    %plot(input_shifted_1down(i,1:rangeoftesting))
    %pause()
    %hold off

end




%% step3: Filter the shifted signal. Every signal should be filtered with their freq*2 subband filter
for i = 1:numberofSubbands
    % FILTER OCTAVE 1 UP!
    %input_shifted_1up_filtered(i,1:end)=filter(filterbank_octave1_up_b(i,1:3),filterbank_octave1_up_a(i,1:3),input_shifted_1up(i,1:end));
    %=highpass(input_shifted_1up(i,1:end),center_frequencys(1),fs);

end

input_shifted_1up_filtered=input_shifted_1up;
suboctave_1=sum(input_shifted_1down(1:6,:),1)*1;
suboctave_2=sum(input_shifted_1down(7:14,:),1);
suboctave_3=sum(input_shifted_1down(15:23,:),1);
suboctave_4=sum(input_shifted_1down(24:33,:),1);
suboctave_5=sum(input_shifted_1down(34:43,:),1);

filterfreqs=[10
             octave1down_center_frequencys(7)
             octave1down_center_frequencys(15)
             octave1down_center_frequencys(24)
             octave1down_center_frequencys(34)
             ];

suboctave_1=bandpass(suboctave_1,[filterfreqs(1),3*filterfreqs(1)],fs,'ImpulseResponse','iir');
suboctave_2=bandpass(suboctave_2,[filterfreqs(2),3*filterfreqs(2)],fs,'ImpulseResponse','iir');
suboctave_3=bandpass(suboctave_3,[filterfreqs(3),3*filterfreqs(3)],fs,'ImpulseResponse','iir');
suboctave_4=bandpass(suboctave_4,[filterfreqs(4),3*filterfreqs(4)],fs,'ImpulseResponse','iir');
suboctave_5=bandpass(suboctave_5,[filterfreqs(5),3*filterfreqs(5)],fs,'ImpulseResponse','iir');

octave1down=suboctave_1+suboctave_2+suboctave_3+suboctave_4+suboctave_5;

for i = 1:numberofSubbands
    % FILTER OCTAVE 1 DOWN!
    %input_shifted_1down_filtered(i,1:end)=filter(filterbank_octave1_down_b(i,1:3),filterbank_octave1_down_a(i,1:3),input_shifted_1down(i,1:end));
end
%octave1down=sum(input_shifted_1down_filtered,1)*1;

 

%input_shifted_2up=input_shifted_1up_filtered.*input_shifted_1up_filtered;
%for i = 1:numberofSubbands
%    input_shifted_2up_filtered(i,1:end)=bandpass(input_shifted_2up(i,1:end),[center_frequencys(i)*3.5,center_frequencys(i)*4.5],fs,'ImpulseResponse','iir')*10;
%end



waitbar(0.8,bar,'Calculating fft...');

%% plot signals
fftrange=50000;
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

input_shifted_1down_fft=abs(fft(input_shifted_1down(obsderveIndex,1:end)));
input_shifted_1down_fft=input_shifted_1down_fft(1:fftrange);
input_shifted_1down_filtered_fft=abs(fft(input_shifted_1down_filtered(obsderveIndex,1:end)));
input_shifted_1down_filtered_fft=input_shifted_1down_filtered_fft(1:fftrange);


waitbar(0.9,bar,'Plotting...');
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
plot(input_shifted_1down_fft);
title('Octave 1 DOWN')
nexttile
plot(input_shifted_1down_filtered_fft);
title('Octave 1 DOWN filtered')
%{
nexttile
plot(input_subbands(1:1:end))
title('input subbands')
nexttile
plot(input_subbands_2(1:1:end))
title('input subbands 2')
nexttile
plot(input_shifted_1up_filtered(1:1:end))
title('input shifted 1up filtered')
%}
%% step4: SUM the frequencies together
%octave1up=sum(input_shifted_1up_filtered,1)*1;

octave1up=sum(input_shifted_1up,1)*1;
octave1up=highpass(octave1up,center_frequencys(1),fs);








octave2up=sum(input_shifted_2up_filtered,1)*1;


%octave1up=highpass(octave1up,100,fs);

output=octave1up+octave2up;
output=output; %+octave1down;
output=output*1+input(1,:)*1;

%sound(input_shifted_1up(1,1:end),fs)

audiowrite('ERBPS2_output.wav',output,fs);
audiowrite('ERBPS2_octave1up.wav',octave1up,fs);
audiowrite('ERBPS2_octave2up.wav',octave2up,fs);
audiowrite('ERBPS2_octave1down.wav',octave1down,fs);

%Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\ERBPS2_octave1down.wav'
Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\ERBPS2_output.wav'
waitbar(1,bar,'Playing...');
winopen(Open_Spec_File)





close(bar)


















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
function bw=ERB_critical_bandwidth(ferb,qps,qerb)
    bw=24.7+0.108.*ferb*2;
    bw=bw*1/qps*1/qerb;
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
    x=a(:,1);
    b=b./x;
    a=a./x;

end

function [b, a] = allpass_biquad(center_freq, sample_rate)
    Q=sqrt(2)/2; %0.707
    % Calculate the normalized angular center frequency
    omega = 2*pi.*center_freq / sample_rate;
    
    % Calculate the alpha value based on Q
    alpha = sin(omega) / (2 * Q);
    
    b0=1-alpha;
    b1=-2*cos(omega);
    b2=1+alpha;

    a0=1+alpha;
    a1=-2*cos(omega);
    a2=1-alpha;


    % Calculate the coefficients for the biquad all-pass filter
    b   =   [b0' b1' b2'];
    a   =   [a0' a1' a2'];
    
    % Normalize the coefficients
    b = b ./ (1 + alpha');
    a = a ./ (1 + alpha');
end
