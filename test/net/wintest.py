import socket
import struct
from math import sin, cos

s = socket.socket()
s.connect(("169.254.1.1", 5000))

def send_type(t):
    s.send(t.encode("ascii"))

def send_flags(f):
    s.send(f.encode("ascii"))

def send_str(text):
    s.send(struct.pack("!h%ds" % len(text), len(text), bytes(text, "ascii")))

def send_lm(lm):
    if lm[0] == "grid":
        s.send(struct.pack("!chh", b'\x00', lm[1], lm[2]))

def send_colour(colour):
    s.send(struct.pack("!BBBB", colour[0], colour[1], colour[2], colour[3]))

def send_border(border):
    send_colour(border[0])
    s.send(struct.pack("!h", border[1]))

def send_margin(margin):
    s.send(struct.pack("!h", margin))

def send_padding(padding):
    s.send(struct.pack("!h", padding))

def recv_status():
    return struct.unpack("!B", s.recv(1))

def recv_status_id():
    return struct.unpack("!BI", s.recv(5))

def make_window(x=0, y=0, w=600, h=400, title="Window"):
    send_type('\x00')
    send_flags('\x07')
    s.send(struct.pack("!hhhh", x, y, w, h))
    send_str(title)
    return recv_status_id()

def open_window(wid):
    s.send(struct.pack("!cI", b'\x10', wid))
    return recv_status()

def close_window(wid):
    s.send(struct.pack("!cI", b'\x11', wid))
    return recv_status()

def make_panel(lm=("grid", 1, 1), bg_colour=(255, 255, 255, 255), border=((0, 0, 0, 0), 0), margin=0, padding=0):
    send_type('\x01')
    send_flags('\x1F')
    send_lm(lm)
    send_colour(bg_colour)
    send_border(border)
    send_margin(margin)
    send_padding(padding)
    return struct.unpack("!bI", s.recv(5))

def make_button(text="Button"):
    s.send(b'\x02\x01')
    send_str(text)
    return struct.unpack("!BI", s.recv(5))

def make_label(text="", bg_colour=(0, 0, 0, 0), margin=0, padding=0):
    s.send(b'\x03\x69')
    send_str(text)
    send_colour(bg_colour)
    send_margin(margin)
    send_padding(padding)
    return struct.unpack("!BI", s.recv(5))

def make_canvas(w, h):
    send_type('\x04')
    s.send(struct.pack("!hh", w, h))
    return recv_status_id()

def make_picture(w, h, data):
    send_type('\x05')
    send_flags('\x01')
    s.send(struct.pack("!hh%ds" % (len(data)), w, h, bytes(data)))
    return recv_status_id()

def make_rectangle(x, y, w, h, colour, border=((0, 0, 0, 0), 0)):
    send_type('\x06')
    send_flags('\x01')
    s.send(struct.pack("!HHhh", x, y, w, h))
    send_colour(colour)
    send_border(border)
    return recv_status_id()

def make_ellipse(x, y, w, h, colour, border=((0, 0, 0, 0), 0)):
    send_type('\x07')
    send_flags('\x01')
    s.send(struct.pack("!HHhh", x, y, w, h))
    send_colour(colour)
    send_border(border)
    return recv_status_id()

def make_line(x0, y0, x1, y1, colour):
    send_type('\x08')
    s.send(struct.pack("!HHHH", x0, y0, x1, y1))
    send_colour(colour)
    return recv_status_id()

def destroy_widget(wid):
    send_type('\x09')
    s.send(struct.pack("!I", wid))
    return recv_status()

def destroy_shape(sid):
    send_type('\x0A')
    s.send(struct.pack("!I", sid))
    return recv_status()

def add_widget(wid1, wid2):
    s.send(struct.pack("!cII", b'\x0B', wid1, wid2))
    return struct.unpack("!B", s.recv(1))

def rm_widget(wid):
    s.send(struct.pack("!cI", b'\x0C', wid))
    return struct.unpack("!B", s.recv(1))

def draw_shape(wid, sid):
    send_type('\x0D')
    s.send(struct.pack("!II", wid, sid))
    return recv_status()

def fill_canvas(wid, colour):
    send_type('\x0E')
    s.send(struct.pack("!I", wid))
    send_colour(colour)
    return recv_status()

def set_picture_data(wid, data):
    send_type('\x0F')
    s.send(struct.pack("!h%ds" % (len(data)), len(data), bytes(data)))
    return recv_status()

def add_mouse_click_handler(wid):
    send_type('\x12')
    s.send(struct.pack("!Ic", wid, b'\x00'))
    return recv_status()

def add_action_handler(wid, wid2):
    send_type('\x13')
    s.send(struct.pack("!IcI", wid, b'\x01', wid2))
    return recv_status()

"""status, wid1 = make_window()
status, wid2 = make_window(200, 200, 600, 400, "Window 2")
status = open_window(wid1)
status = open_window(wid2)
status, wid3 = make_panel(lm=("grid", 2, 2), bg_colour=(255,0,0,255), margin=10)
status, wid4 = make_panel(bg_colour=(0,0,255,255))
status = add_widget(wid1, wid3)
status = add_widget(wid3, wid4)
status, wid5 = make_button()
status, wid6 = make_label("Lorem ipsum")
status = add_widget(wid4, wid5)
status = add_widget(wid3, wid6)
status, wid7 = make_panel(lm=("grid", 2, 1))
status, wid8 = make_canvas(200, 300)
status, sid1 = make_rectangle(50, 100, 100, 50, (255, 255, 0, 255))
status, sid2 = make_ellipse(75, 125, 100, 50, (255, 0, 0, 128))
status, sid3 = make_line(50, 50, 100, 100, (0, 0, 0, 255))
status = fill_canvas(wid8, (200, 200, 200, 255))
status = draw_shape(wid8, sid1)
status = add_widget(wid2, wid7)
status = add_widget(wid7, wid8)
status = draw_shape(wid8, sid2)
status = draw_shape(wid8, sid3)
status = destroy_shape(sid3)
status = draw_shape(wid8, sid3)
print("Attempting to draw deleted shape:", status)
status = destroy_widget(wid3)
input("Press enter")
status, wid9 = make_panel(lm=("grid", 2, 2), bg_colour=(255,0,0,255), margin=10)
status = add_widget(wid1, wid9)
status = add_widget(wid9, wid4)
status = add_widget(wid9, wid6)
status, wid10 = make_picture(100, 100, [255 if i % 16 in [1, 3, 4, 7] else 0 for i in range(100*100*4)])
status = add_widget(wid7, wid10)"""

pix = []

for y in range(380):
    pix.append([])
    for x in range(1000):
        pix[y].append(int((sin(y/10)+1)*127))
        pix[y].append(int((sin((x+y)/10)+1)*127))
        pix[y].append(int((cos(x/10)+1)*127))
        pix[y].append(255)

pix = [p for pi in pix for p in pi]

status, wid1 = make_window(100, 100)
status, wid2 = make_window(800, 100, 600, 600, "Window 2")
status = open_window(wid1)
status, wid3 = make_panel(lm=("grid", 2, 2), bg_colour=(150,180,220,255), margin=10)
status, wid4 = make_panel(bg_colour=(0,0,255,255))
status = add_widget(wid1, wid3)
status = add_widget(wid3, wid4)
status, wid5 = make_button("Open window 2")
status, wid6 = make_label("Lorem ipsum dolor sit amet, consectetuer adipiscing elit.\n\nDonec id auctor tellus. Suspendisse vel ante", margin=10, padding=10)
status = add_widget(wid4, wid5)
status = add_widget(wid3, wid6)
status, wid7 = make_panel(lm=("grid", 2, 1))
status, wid8 = make_canvas(200, 300)
status, sid1 = make_rectangle(50, 100, 100, 50, (255, 255, 0, 255))
status, sid2 = make_ellipse(75, 125, 100, 50, (255, 0, 0, 128))
status, sid3 = make_line(50, 50, 100, 100, (0, 0, 0, 255))
status = fill_canvas(wid8, (200, 200, 200, 255))
status = draw_shape(wid8, sid1)
status = add_widget(wid2, wid7)
status = add_widget(wid7, wid8)
status = draw_shape(wid8, sid2)
status = draw_shape(wid8, sid3)
status, wid9 = make_panel(margin=10)
status, wid10 = make_picture(1000, 380, pix)
status = add_widget(wid7, wid9)
status = add_widget(wid9, wid10)
status = add_action_handler(wid5, wid2)

#print(wid1, wid2, wid3, wid4, wid5, wid6, wid7, wid8, wid9)
#print(sid1, sid2, sid3)
#exit()
while True:
    cmd = input("$ ")
    if cmd == "":
        break
    eval(cmd)

s.close()
