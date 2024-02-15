close all;
% This script reads a .wav file format 
% and shifts its frequency bins up by 2f using FFT


clear all;
%% import audio file
[input,Fs] = audioread("../all_of_it/guitar_fast_picking.wav");
input=lowpass(input,15000,Fs);
comp=compressor(10);
input=comp(input);
%for i=1:length(y)

%% time domain to frequency 
sampleSize=2^16;
octave_up_1=1;
octave_up_2=0;
original=1;
octave_down_1=1;
overlap=0.5% 0.5= 50% must be > 0!

window_function = window(@hann,sampleSize);
window_function = window(@blackmanharris, sampleSize);
%window_function=tukeywin(sampleSize,1);
%N=sampleSize;
%n = (0:N-1)';
%window_function= 1 - abs((n - (N-1)/2) / ((N-1)/2));
%window_function=window_function.*tukeywin(sampleSize,0.25);
output=zeros(length(input),1);
last_Phase=zeros(length(sampleSize),1);
%hold on
%plot(abs(fft_complex))
%pause;
%% shift the fft out

startposition=50000;
j=startposition;
while (j+sampleSize)<length(input)
    close all;
    block_end=j+sampleSize-1;
    %% block
    block=input(j:block_end);
    block=block.*window_function;
    %figure("name","windowed Input","Position",[1500,800,1000,500])
    %plot(block)
    %plot(startposition:startposition+sampleSize*10,input(startposition:startposition+sampleSize*10))

    %% FFT
    fft_complex=fft(block);
    %plot(abs(fft_complex))
    
    fft_complex_copy=fft_complex;
    fft_complex=zeros(sampleSize,1);
    fft_complex_copy_2=zeros(sampleSize,1);
    fft_complex_copy_3=zeros(sampleSize,1);
    %% OCTAVE UPx1
    for i=2:(length(fft_complex)/2)
        fft_complex(i+i)=fft_complex_copy(i);
        %wrapTo2Pi(last_Phase[i+i]+Fs/(2*pi)*sampleSize);
        
    end
    last_Phase=angle(fft_complex);
    
    %% OCTAVE UPx2
    for i=10:(length(fft_complex)/4)
        fft_complex_copy_2(4*i)=fft_complex_copy(i);
    end

    %% OCTAVE DOWNx1
    for i=10:(length(fft_complex)/8)
        
        fft_complex_copy_3(round(i/2))=fft_complex_copy(i);
    end

    
    %fft_complex=fft_complex*octave_up_1+fft_complex_copy_2;%+fft_complex_copy*original;
    fft_complex=fft_complex*octave_up_1+fft_complex_copy_2*octave_up_2+fft_complex_copy_3*octave_down_1;
    inverse=ifft(real(fft_complex),"symmetric");
    
    %% output
    %output(j:block_end)=output(j:block_end)+inverse; %.*window_function;;
    output(j:block_end)=input(j:block_end)*1+5*inverse.*window_function; %.*window_function; %.*window_function;;
    %plot(output(j-2:j+2))
    %pause;
   

    %plot(output(j-2:j+2))
    %pause;
    %convoluted_output=conv(input(j:block_end),inverse);
    %output(j:block_end)=convoluted_output(1:2:end);

    %figure("name","Output after iFFT","Position",[1500,250,1000,500])
    %plot(startposition:startposition+sampleSize*10,output(startposition:startposition+sampleSize*10))
    %pause;
    j=j+sampleSize*overlap;

    
    %% increment data buffer
end

%plot(abs(fft_complex))
 %hold off
%plot(output)

output = pitch_shift(input,Fs,10);

output = lowpass(output, 15000, Fs);
audiowrite('FFT_output.wav',output,Fs);
%sound(output,Fs)
%audiowrite('Octave_1_UP.wav',output,Fs);
%Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\Octave_1_UP.wav'
%winopen(Open_Spec_File)
%%play(audioplayer(output,48000))


function shifted_sound = pitch_shift(sound, fs, semitones_shift)
    % Input:
    % sound: input sound sample
    % fs: sampling frequency of the sound sample
    % semitones_shift: number of semitones to shift the pitch (positive or negative)
    % Output:
    % shifted_sound: pitch-shifted sound sample
    
    % Convert semitones shift to a pitch ratio
    pitch_ratio = 2^(semitones_shift/12);

    % Define the new length of the sound sample after pitch shifting
    new_length = ceil(length(sound) / pitch_ratio);

    % Initialize the shifted sound sample
    shifted_sound = zeros(new_length, 1);

    % Perform pitch shifting using linear interpolation
    for i = 1:new_length
        old_index = i * pitch_ratio;
        lower_index = floor(old_index);
        upper_index = min(ceil(old_index), length(sound));

        if lower_index == upper_index
            shifted_sound(i) = sound(lower_index);
        else
            shifted_sound(i) = (sound(upper_index) - sound(lower_index)) * (old_index - lower_index) + sound(lower_index);
        end
    end

    % Resample the shifted sound to match the original sampling rate
    shifted_sound = resample(shifted_sound, fs, fs * pitch_ratio);
end