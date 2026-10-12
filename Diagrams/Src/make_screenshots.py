"""Runs the real Project binary in a pseudo-terminal, replays scripted keystrokes and renders
terminal-style PNG screenshots into Screenshots/.  (Replace with real screenshots from your own PC if required.)"""
import os, pty, re, select, shutil, subprocess, sys, time
from PIL import Image, ImageDraw, ImageFont
import matplotlib

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
BIN = os.path.join(ROOT, "Executable", "Project")
OUT = os.path.join(ROOT, "Screenshots")
FONT = os.path.join(os.path.dirname(matplotlib.__file__), "mpl-data", "fonts", "ttf", "DejaVuSansMono.ttf")

def fresh_workdir(name):
    d = f"/tmp/shot_{name}"
    shutil.rmtree(d, ignore_errors=True)
    os.makedirs(d + "/Executable")
    shutil.copytree(os.path.join(ROOT, "Data"), d + "/Data")
    return d

def run(workdir, inputs):
    pid, fd = pty.fork()
    if pid == 0:
        os.chdir(workdir + "/Executable")
        os.execv(BIN, [BIN])
    buf = b""
    def drain(t=0.25):
        nonlocal buf
        while True:
            r, _, _ = select.select([fd], [], [], t)
            if not r: return
            try: chunk = os.read(fd, 4096)
            except OSError: return
            if not chunk: return
            buf += chunk
    drain(0.5)
    for line in inputs:
        os.write(fd, (line + "\n").encode())
        drain()
    drain(0.5)
    try: os.waitpid(pid, 0)
    except ChildProcessError: pass
    text = buf.decode(errors="replace").replace("\r\n", "\n").replace("\r", "")
    return re.sub(r"\x1b\[[0-9;]*[A-Za-z]", "", text)

def crop(text, start, end, include_end_line=True, extra=0):
    lines = text.split("\n")
    s = next(i for i, l in enumerate(lines) if start in l)
    e = next(i for i in range(s, len(lines)) if end in lines[i])
    return "\n".join(lines[s:e + 1 + extra])

def render(text, title, filename):
    font = ImageFont.truetype(FONT, 15)
    lines = text.rstrip("\n").split("\n")
    cw = font.getbbox("M")[2]; lh = 21
    w = max(max(len(l) for l in lines) * cw + 40, 700)
    h = len(lines) * lh + 80
    img = Image.new("RGB", (w, h), (30, 30, 30))
    d = ImageDraw.Draw(img)
    d.rectangle([0, 0, w, 34], fill=(60, 60, 60))
    for i, c in enumerate([(255, 95, 86), (255, 189, 46), (39, 201, 63)]):
        d.ellipse([14 + i * 22, 11, 26 + i * 22, 23], fill=c)
    d.text((100, 8), title, font=ImageFont.truetype(FONT, 14), fill=(220, 220, 220))
    for i, l in enumerate(lines):
        color = (240, 130, 130) if ("Error" in l or "  ! " in l or "[warning]" in l) else (120, 220, 140) if ("confirmed" in l.lower() and "Booking" in l) or l.startswith("Saved") or l.startswith("Loaded") or "added with ID" in l or "deleted" in l else (225, 225, 225)
        d.text((20, 50 + i * lh), l, font=font, fill=color)
    img.save(os.path.join(OUT, filename))
    print("wrote", filename, f"({len(lines)} lines)")

os.makedirs(OUT, exist_ok=True)

# 1 menu + display
t = run(fresh_workdir(1), ["2", "1", "9"])
render(crop(t, "Loaded", "Isha Desai"), "Project.exe - Main menu and Display Records", "01_menu_display_customers.png")

# 2 add customer with validation
t = run(fresh_workdir(2), ["1", "1", "A1", "Neha Kapoor", "12345", "9000011122", "neha@", "neha.kapoor@gmail.com", "2", "9"])
render(crop(t, "Add which record type?", "Customer added"), "Project.exe - Add Record with input validation", "02_add_customer_validation.png")

# 3 booking success with discount
t = run(fresh_workdir(3), ["6", "1", "1", "3", "2", "2027-03-10", "10", "y", "9"])
render(crop(t, "-- Packages --", "Booking confirmed"), "Project.exe - Main transaction: book a session (Gold discount)", "03_book_session.png")

# 4 double booking
t = run(fresh_workdir(4), ["6", "1", "3", "2", "1", "2027-01-20", "11", "9"])
render(crop(t, "-- Packages --", "Error:"), "Project.exe - Double-booking prevented (exception handling)", "04_double_booking_blocked.png")

# 5 studio hours
t = run(fresh_workdir(5), ["6", "1", "1", "1", "4", "2027-05-01", "15", "9"])
render(crop(t, "Package ID:", "Error:"), "Project.exe - Studio closing time rule", "05_studio_hours_error.png")

# 6 report
t = run(fresh_workdir(6), ["7", "9"])
render(crop(t, "SUMMARY REPORT", "Next upcoming", extra=6).replace("======================================================================\n", "", 0), "Project.exe - Report", "06_report.png")

# 7 delete blocked + search
t = run(fresh_workdir(7), ["5", "1", "1", "3", "4", "diya", "9"])
render(crop(t, "Delete which record type?", "Diya Patel", extra=0).split("\n", 0)[0] if False else crop(t, "Delete which record type?", "Meera Nair"), "Project.exe - Referential integrity on Delete, and Search", "07_delete_blocked_search.png")

# 8 persistence: run, add, exit, reopen
wd = fresh_workdir(8)
t1 = run(wd, ["1", "3", "Newborn Shoot", "9500", "3", "60", "9"])
t2 = run(wd, ["2", "3", "9"])
part1 = crop(t1, "-- New package --", "Goodbye")
part2 = crop(t2, "Loaded", "Newborn Shoot")
render(part1 + "\n\n=========== program closed, then re-opened ===========\n\n" + part2, "Project.exe - File persistence (close and reopen)", "08_persistence.png")
