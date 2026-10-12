"""Draws UseCase.png, Sequence1.png (book a session) and Sequence2.png (load on start / save on exit)."""
import os
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Ellipse, Rectangle, FancyArrowPatch

OUT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

# ------------------------------------------------------------------ USE CASE
def stick(ax, x, y, name, stereotype=None):
    ax.add_patch(plt.Circle((x, y + 0.75), 0.18, fill=False, lw=1.6))
    ax.plot([x, x], [y + 0.57, y], color="k", lw=1.6)
    ax.plot([x - 0.35, x + 0.35], [y + 0.4, y + 0.4], color="k", lw=1.6)
    ax.plot([x, x - 0.3], [y, y - 0.5], color="k", lw=1.6)
    ax.plot([x, x + 0.3], [y, y - 0.5], color="k", lw=1.6)
    ax.text(x, y - 0.8, name, ha="center", va="top", fontsize=11, fontweight="bold")
    if stereotype: ax.text(x, y - 1.25, stereotype, ha="center", va="top", fontsize=9, style="italic")

def usecase():
    fig, ax = plt.subplots(figsize=(12, 12)); ax.set_xlim(0, 12); ax.set_ylim(-0.9, 11.5); ax.axis("off")
    ax.text(6, 11.2, "Photography Studio Booking (CPP31) - Use Case Diagram", ha="center", fontsize=15, fontweight="bold")
    ax.add_patch(Rectangle((3.1, 0.35), 5.8, 10.35, fill=False, lw=2))
    ax.text(6, 10.45, "Photography Studio Booking System", ha="center", fontsize=11, fontweight="bold")
    ucs = ["Add Record (Customer / Photographer / Package)", "Display Records", "Search Record", "Update Record",
           "Delete Record", "Book Photo Session", "Check Availability &\nValidate Booking Rules", "Cancel Booking",
           "Mark Booking Completed", "View Schedule for a Date", "Generate Report", "Save / Load Data"]
    ys = [9.6 - i * 0.74 - (0.4 if i >= 6 else 0) for i in range(len(ucs))]
    pos = {}
    for n, y in zip(ucs, ys):
        e = Ellipse((6, y), 5.2, 0.56, fc="#EAF2FF", ec="k", lw=1.2); ax.add_patch(e)
        ax.text(6, y, n, ha="center", va="center", fontsize=9.5)
        pos[n] = y
    staff = (1.2, 5.6); cust = (10.9, 6.2); files = (10.9, 1.9)
    stick(ax, *staff, "Studio Staff", "(primary actor)")
    stick(ax, *cust, "Customer", "(primary actor)")
    stick(ax, *files, "Data Files", "(secondary actor)")
    def link(a, b, **kw): ax.plot([a[0], b[0]], [a[1], b[1]], color="#333", lw=1.0, **kw)
    sx, sy = staff[0] + 0.3, staff[1] + 0.4
    for n in ucs:
        if n.startswith("Check"): continue
        link((sx, sy), (3.4 + 0.0 if False else 3.45, pos[n]))
    for n in ["Book Photo Session", "Cancel Booking", "View Schedule for a Date"]:
        link((cust[0] - 0.35, cust[1] + 0.4), (8.6, pos[n]))
    link((files[0] - 0.35, files[1] + 0.4), (8.6, pos["Save / Load Data"]))
    link((files[0] - 0.35, files[1] + 0.4), (8.55, pos["Display Records"]), ls=":") if False else None
    # <<include>>
    y1, y2 = pos["Book Photo Session"], pos["Check Availability &\nValidate Booking Rules"]
    ax.add_patch(FancyArrowPatch((6, y1 - 0.29), (6, y2 + 0.29), arrowstyle="->", mutation_scale=14, ls="--", lw=1.2))
    ax.text(6.1, (y1 + y2) / 2, "<<include>>", fontsize=8.5, style="italic", va="center")
    ax.text(0.3, -0.35, "Customers request bookings / cancellations at the front desk; staff operate the console menu.\n"
            "The data files (customers, photographers, packages, bookings .txt) are read and written by FileManager.", fontsize=8.5, color="#444")
    fig.savefig(os.path.join(OUT, "UseCase.png"), dpi=130, bbox_inches="tight"); plt.close(fig)

# ------------------------------------------------------------------ SEQUENCE helper
def sequence(title, parts, steps, fname, width=15, step_h=0.62):
    n = len(steps)
    H = 3.2 + n * step_h
    fig, ax = plt.subplots(figsize=(width, H * 0.95)); ax.set_xlim(0, width); ax.set_ylim(0, H); ax.axis("off")
    ax.text(width / 2, H - 0.3, title, ha="center", fontsize=14, fontweight="bold")
    xs = {}
    gap = (width - 1.6) / (len(parts) - 1)
    for i, (key, label) in enumerate(parts):
        x = 0.8 + i * gap; xs[key] = x
        ax.add_patch(Rectangle((x - 0.78, H - 1.45), 1.56, 0.7, fc="#FFF4D6", ec="k", lw=1.2))
        ax.text(x, H - 1.1, label, ha="center", va="center", fontsize=8.8, fontweight="bold")
        ax.plot([x, x], [H - 1.45, 0.2], color="#777", lw=1, ls=(0, (5, 4)))
    y = H - 2.0
    frames = []
    for s in steps:
        kind = s[0]
        if kind == "msg":
            _, a, b, label = s[:4]; style = s[4] if len(s) > 4 else "sync"
            ls = "--" if style in ("return", "create") else "-"
            ax.add_patch(FancyArrowPatch((xs[a], y), (xs[b], y), arrowstyle="->" if style in ("return",) else "-|>",
                                         mutation_scale=13, ls=ls, lw=1.3, color="k"))
            mx = (xs[a] + xs[b]) / 2
            ax.text(mx, y + 0.08, label, ha="center", va="bottom", fontsize=8.3,
                    style="italic" if style == "return" else "normal")
        elif kind == "self":
            _, a, label = s
            x = xs[a]
            ax.plot([x, x + 0.55, x + 0.55], [y + 0.15, y + 0.15, y - 0.12], color="k", lw=1.3)
            ax.add_patch(FancyArrowPatch((x + 0.55, y - 0.12), (x, y - 0.12), arrowstyle="-|>", mutation_scale=13, lw=1.3))
            ax.text(x + 0.65, y + 0.02, label, ha="left", va="center", fontsize=8.3)
        elif kind == "frame_start":
            frames.append((s[1], y + 0.33))
        elif kind == "frame_end":
            lab, y0 = frames.pop()
            yb = y + step_h - 0.3
            ax.add_patch(Rectangle((0.15, yb), width - 0.3, y0 - yb, fill=False, ec="#2a5db0", lw=1.3))
            ax.add_patch(Rectangle((0.15, y0 - 0.3), 2.2, 0.3, fc="#DDEBFF", ec="#2a5db0", lw=1.3))
            ax.text(0.22, y0 - 0.15, lab, fontsize=8.3, va="center", color="#1b3f80", fontweight="bold")
            continue
        elif kind == "divider":
            ax.plot([0.15, width - 0.15], [y + 0.1, y + 0.1], color="#2a5db0", lw=1.1, ls=":")
            ax.text(width / 2, y + 0.15, s[1], ha="center", va="bottom", fontsize=8.5, color="#1b3f80", fontweight="bold")
        y -= step_h
    fig.savefig(os.path.join(OUT, fname), dpi=130, bbox_inches="tight"); plt.close(fig)

def seq1():
    parts = [("U", ":Studio Staff"), ("M", ":main.cpp"), ("S", ":Studio"), ("C", ":Customer"),
             ("P", ":Photographer"), ("K", ":Package"), ("B", ":Booking")]
    steps = [
        ("msg", "U", "M", "choose 6 -> 1 (Book a photo session)"),
        ("msg", "M", "S", "mainTransaction()"),
        ("self", "S", "bookSession()"),
        ("msg", "S", "U", "show lists, ask customer / photographer / package IDs"),
        ("msg", "U", "S", "IDs, date, start hour", "return"),
        ("msg", "S", "C", "indexOfId() / discountRate()"),
        ("msg", "S", "P", "indexOfId() / roleDescription()"),
        ("msg", "S", "K", "indexOfId() / getPrice(), getDurationHours()"),
        ("frame_start", "alt  [slot is free]"),
        ("self", "S", "checkAvailability(date, start, end, photographerId)"),
        ("msg", "S", "U", "display booking summary + total, ask confirm?"),
        ("msg", "U", "S", "y", "return"),
        ("msg", "S", "B", "<<create>>  Booking(id, cid, pid, pkgId, date, start, dur, total)", "create"),
        ("self", "S", "bookings.add(b);  unsaved = true"),
        ("msg", "S", "U", "\"Booking confirmed! Booking ID = n\"", "return"),
        ("divider", "[else: studio closed / photographer double-booked / all rooms busy]"),
        ("msg", "S", "M", "throw BookingException(reason)", "return"),
        ("msg", "M", "U", "catch -> print \"Error: reason\" (no data changed)", "return"),
        ("frame_end",),
    ]
    sequence("Sequence Diagram 1 - Main Transaction: Book a Photo Session", parts, steps, "Sequence1.png", width=16.5, step_h=0.6)

def seq2():
    parts = [("U", ":Studio Staff"), ("M", ":main.cpp"), ("S", ":Studio"), ("F", ":FileManager"),
             ("E", ":Entity (T)"), ("A", ":DynArray<T>"), ("D", "Data/*.txt")]
    steps = [
        ("divider", "START-UP: load existing records"),
        ("msg", "M", "S", "load()"),
        ("msg", "S", "F", "load<T>(path, records)   (x4: customers, photographers, packages, bookings)"),
        ("msg", "F", "D", "readLines(path)"),
        ("msg", "D", "F", "text lines", "return"),
        ("frame_start", "loop  [each line]"),
        ("msg", "F", "E", "T::fromFileLine(line)  (validates fields)"),
        ("msg", "F", "A", "add(item)   (grows with new T[2*cap] if full)"),
        ("frame_end",),
        ("msg", "S", "M", "\"Loaded n customers, ...\"", "return"),
        ("divider", "OPERATIONS: add / update / delete / book ...  (unsaved = true)"),
        ("divider", "EXIT: menu choice 9  (or Save/Load -> 1)"),
        ("msg", "U", "M", "choose 9 (Exit)"),
        ("msg", "M", "S", "save()"),
        ("msg", "S", "F", "save<T>(path, header, records)   (x4)"),
        ("frame_start", "loop  [each record]"),
        ("msg", "F", "E", "toFileLine()  (virtual call via Record&)"),
        ("frame_end",),
        ("msg", "F", "D", "writeLines(): write path.tmp, remove old, rename"),
        ("alt_dummy",) if False else ("msg", "F", "S", "return (throws FileException if write fails)", "return"),
        ("msg", "M", "U", "\"Saved ... Goodbye!\" (~Studio frees memory)", "return"),
    ]
    sequence("Sequence Diagram 2 - File Persistence: Load on Start-up and Save on Exit", parts, steps, "Sequence2.png", width=16.5, step_h=0.6)

usecase(); seq1(); seq2(); print("diagrams written")
