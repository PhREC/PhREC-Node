import serial, time

PORT = "/dev/ttyUSB0"
BAUD = 580000

def crc8(data):
    crc = 0xFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 0x01:
                crc = (crc >> 1) ^ 0x8C
            else:
                crc >>= 1
    return crc

def send_message(bytes):
    pass

with serial.Serial(PORT, BAUD, bytesize=8, parity="N", stopbits=1, timeout=4) as s:
    try:
        while True:
            s.reset_input_buffer()
            s.write(CALL)
            s.flush()

            t0 = time.time()
            resp = s.read(4)
            dt = time.time() - t0

            if len(resp) == 4:
                raw   = int.from_bytes(resp, "little")
                msgid = raw & 0x3F                 # bits 0:5
                hwid  = (raw >> 6) & 0xFFFF        # bits 6:21
                undef = (raw >> 22) & 0x3          # bits 22:23
                crc   = (raw >> 24) & 0xFF         # bits 24:31
                ok    = "ok" if crc == crc8(resp[:3]) else f"BAD (expected {crc8(resp[:3])})"
                print(f"{msgid} {hwid} {undef:02b} {crc}   "
                      f"[id={msgid:06b} hwid=0x{hwid:04X} crc {ok}, {dt:.3f} s, raw {resp.hex(' ')}]")
            elif resp:
                print(f"partial RX {resp.hex(' ')} ({len(resp)}/4 bytes after 4 s)")