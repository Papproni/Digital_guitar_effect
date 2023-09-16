close all;
% This script reads a .wav file format 
% and shift in Time domain with overlaping buffers


clear all;
%% import audio file
[input,Fs] = audioread("guitar_fast_picking.wav");



input=lowpass(input,7500,Fs);
%for i=1:length(y)

%% time domain to frequency 


bufferSize=5000;

hopsize=200;
P1=1;
P2=bufferSize/2;


overlap=2500 % samples
crossfade=0;
crossfade_counter=overlap;
buffer1=zeros(bufferSize,1);
buffer2=zeros(bufferSize,1);

% Initialize the output signal
output = zeros(length(input)*2,1);

% Perform the STFT and pitch shifting in a loop
num_frames = floor((length(input) - bufferSize))/bufferSize + 1;

for frame_idx = 1:num_frames
    % Extract the current frame and apply the window function
    frame_start = (frame_idx - 1) * bufferSize + 1;
    frame_end = frame_start + bufferSize - 1;
    frame = input(frame_start:frame_end);%.*window_function;
    
    %% PITCH SHIFT
    for i=0:bufferSize*2
        if (P1>=bufferSize-overlap) && P1 <bufferSize
            crossfade_counter=crossfade_counter+1;
            crossfade=crossfade_counter/overlap;
        end
        
        if P2>=bufferSize-overlap && P2<bufferSize
            crossfade_counter=crossfade_counter-1;
            crossfade=crossfade_counter/overlap;
        end
        
        %if P2>bufferSize/2
        %    buffer1=frame;
        %end
        
        %if P1>bufferSize/2
        %    buffer2=frame;
        %end

        sample1=buffer1(P1)*crossfade;
        sample2=buffer2(P2)*(1-crossfade);
        frame_start = (frame_idx - 1) * bufferSize*2 + 1;
        output(frame_start+i)=sample1+sample2;
        %% increment buffer
        
        P1=P1+1;
        P2=P2+1;
        if P1>=bufferSize
            P1=1;
           buffer1=frame;
        end
        if P2>=bufferSize
            P2=1;
             buffer2=frame;
            
        end
    end
end



% Normalize

output=lowpass(output,750,2*Fs);
output=lowpass(output,750,2*Fs);

audiowrite('Octave_1_UP.wav',output,2*Fs);
Open_Spec_File = 'C:\ST\workspaces\stm32Audio\MATLAB\Octave_1_UP.wav'
winopen(Open_Spec_File)
%%play(audioplayer(output,48000))