spr = 6400
root_delay = 30 / spr

motor_state = {
    'delay': root_delay,
    'spr': spr,
    'revs': 3,
    'total_steps': spr * 3,
    'running': False,
}