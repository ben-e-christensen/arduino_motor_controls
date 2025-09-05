# Corrected motor_state definition
spr = 6400  # steps per revolution
rpm = 1  # revolutions per minute
rps = rpm / 60  # revolutions per second
sps = rps * spr  # steps per second

motor_state = {
    'spr': 6400,
    'rpm': 1, # Use rpm directly as it's the user-facing value
    'revs': 3,
    'total_steps': 6400 * 3,
    'running': False,
}
