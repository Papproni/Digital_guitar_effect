%% TD-PSOLA: time domain pitch shift overlap add
clear all;
n=1:2^14;
period=150;
index=1;
hopsize=1200;
x=1:hopsize;
overlap=50; %50% overlap
window_function=0.5*(1-cos(2*pi.*x/hopsize));


overlap=overlap/100;

plot_block_and_input_fft=0;

% input
input=sin(2*pi*n/period*1.5)+sin(2*pi*n/period/5*1.5);
% output
output=zeros(1:n);
block=zeros(1:n);

while index<length(input)-hopsize
   
    block_end=index+hopsize-1;
    block(1:hopsize/2)=input(index:2:block_end);
    block(hopsize/2+1:hopsize)=block(1:hopsize/2);
    output(index:block_end)=block(1:hopsize)+input(index:block_end);
    
    block=block.*window_function;
    if (plot_block_and_input_fft>0)
        close all;
        hold on
        %plot(abs(fft(block)))
        plot(abs(fft(input(index:block_end))));
        pause;
        
        plot(abs(fft(output(index:block_end))));
        pause;
        hold off;
    end
    index=index+hopsize*overlap-1;
end
close all;
hold on;
grid on;
plot(input(800:2500))

plot(output(800:2500))
hold off

plotOUTPUT_and_INPUT()