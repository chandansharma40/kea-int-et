clear
echo "Starting build"
avr-gcc -Os -D F_CPU=16000000UL -mmcu=atmega32 main.c LCD.c sensors.c kbd.c clock.c

