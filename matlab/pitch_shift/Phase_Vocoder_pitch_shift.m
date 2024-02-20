

t=0:1:100000;

input = sin(2*3.14*t/100);
input_shifted = sin(2*3.14*t/100 + 3.14/3);
output = input.* input;
a = 1
sound(output,48000);
plot(abs(fft(input)));
hold on;
plot(abs(fft(output)))
hold off;