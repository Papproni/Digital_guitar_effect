close all;
% This script reads a .wav file format 
% and shifts its frequency bins up by 2f using FFT


clear all;
%% import audio file
[input,Fs] = audioread("guitar_fast_picking.wav");
input=lowpass(input,7500,Fs);
%for i=1:length(y)

%% time domain to frequency 
sampleSize=2^12;
octave_up_1=0;
octave_up_2=0;
original=1;
octave_down_1=0;
overlap=1 % 0.5= 50% must be > 0!


%output=zeros(length(input),1);
%hold on
%plot(abs(fft_complex))
%pause;


% Set the parameters for the STFT
%win_length = 4000;
%hop_size = win_length / 4;

hop_size = 3000 ;
win_length = hop_size*2;


%window = hann(win_length); % 25x4
window=tukeywin(win_length,0.5);
window_big=tukeywin(win_length*2,0.5);
%window_2 = hann(win_length*2);



% Initialize the output signal
output = zeros(length(input)*2,1);

% Perform the STFT and pitch shifting in a loop
num_frames = floor((length(input) - win_length) / hop_size) + 1-4;
for frame_idx = 1:num_frames
    % Extract the current frame and apply the window function
    frame_start = (frame_idx - 1) * hop_size + 1;
    frame_end = frame_start + win_length - 1;
    frame = input(frame_start:frame_end).* window;
    
    %octave_up_1_buffer=[frame(1:2:length(frame));frame(1:2:length(frame))];
    %octave_up_2_buffer=[frame(1:4:length(frame));frame(1:4:length(frame));frame(1:4:length(frame));frame(1:4:length(frame))];
    octave_up_1_buffer=[frame(1:length(frame));frame(1:length(frame))];
    octave_up_1_buffer_shifted=[frame(length(frame)/2:length(frame)); frame(1:length(frame)); frame(1:length(frame)/2)];
   
   
    % Overlap-add the shifted frame to the output signal
    frame_start = (frame_idx*2 - 1) * hop_size + 1;
    difi=length(octave_up_1_buffer)+frame_start-1;
    difi=frame_end;
    new_output=octave_up_1_buffer'.*window_big';
    last_output=output(frame_start:difi);
    output(frame_start:difi) = output(frame_start:difi) + new_output' ; %+octave_up_2_buffer'.*window';
end



% Normalize

%output=lowpass(output,750,Fs);

%Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\guitar_fast_picking.wav'
%winopen(Open_Spec_File)
%pause;
audiowrite('Octave_1_UP.wav',output,2*Fs);
Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\Octave_1_UP.wav'
winopen(Open_Spec_File)
%%play(audioplayer(output,48000))