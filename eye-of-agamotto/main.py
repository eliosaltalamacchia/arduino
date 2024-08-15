import time
import board
import pwmio
from adafruit_motor import servo
import neopixel
from digitalio import DigitalInOut, Direction, Pull

# servo control
servo_pin = board.A2
pwm = pwmio.PWMOut(servo_pin, duty_cycle=2 ** 15, frequency=50)
my_servo = servo.Servo(pwm)

# neopixel led 
pixel_pin = board.A3
num_pixels = 1
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=0.3, auto_write=False)
GREEN = (0, 255, 0)
OFF = (0, 0, 0)

# reed switch
reed_pin = board.A4
reed = DigitalInOut(reed_pin)
reed.direction = Direction.INPUT
reed.pull = Pull.UP
prev_state = reed.value
is_open = False

while True:
    cur_state = reed.value    
    
    if cur_state != prev_state:
        if not cur_state:
            is_open = not is_open
            if is_open:
                print("opening eye")
                pixels.fill(GREEN)
                for angle in range(0, 90, 5):  # 0 - 90 degrees, 5 degrees at a time.
                    my_servo.angle = angle
                    time.sleep(0.05)
            else:
                print("closing eye")
                pixels.fill(OFF)
                for angle in range(90, 0, -5):  # 0 - 90 degrees, 5 degrees at a time.
                    my_servo.angle = angle
                    time.sleep(0.05)
            pixels.show()                                
    prev_state = cur_state


