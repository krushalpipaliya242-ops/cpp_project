import os
from PIL import Image as PILImage
from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import cm
from reportlab.platypus import (Image, KeepTogether, PageBreak, Paragraph, Preformatted, SimpleDocTemplate,
                                Spacer, Table, TableStyle)

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
D = os.path.join(ROOT, "Diagrams"); S = os.path.join(ROOT, "Screenshots")
OUT = os.path.join(ROOT, "ProjectReport.pdf")

ss = getSampleStyleSheet()
body = ParagraphStyle("body", parent=ss["BodyText"], fontSize=10, leading=14.5, spaceAfter=6)
h1 = ParagraphStyle("h1", parent=ss["Heading1"], fontSize=16, spaceBefore=10, spaceAfter=8, textColor=colors.HexColor("#1F3A5F"))
h2 = ParagraphStyle("h2", parent=ss["Heading2"], fontSize=12, spaceBefore=8, spaceAfter=4, textColor=colors.HexColor("#1F3A5F"))
cap = ParagraphStyle("cap", parent=body, fontSize=8.5, alignment=TA_CENTER, textColor=colors.HexColor("#555555"), spaceAfter=10)
cell = ParagraphStyle("cell", parent=body, fontSize=8.6, leading=11, spaceAfter=0)
cellb = ParagraphStyle("cellb", parent=cell, fontName="Helvetica-Bold", textColor=colors.white)
code = ParagraphStyle("code", parent=ss["Code"], fontSize=7.8, leading=9.8, backColor=colors.HexColor("#F4F4F4"),
                      borderPadding=5, spaceAfter=8)
title = ParagraphStyle("title", parent=ss["Title"], fontSize=26, leading=32, textColor=colors.HexColor("#1F3A5F"))
sub = ParagraphStyle("sub", parent=body, fontSize=13, alignment=TA_CENTER, leading=18)

def P(t, st=body): return Paragraph(t, st)

def table(rows, widths, header=True):
    data = [[Paragraph(str(c), cellb if (header and i == 0) else cell) for c in r] for i, r in enumerate(rows)]
    t = Table(data, colWidths=widths, repeatRows=1 if header else 0)
    st = [("GRID", (0, 0), (-1, -1), 0.4, colors.HexColor("#999999")), ("VALIGN", (0, 0), (-1, -1), "TOP"),
          ("LEFTPADDING", (0, 0), (-1, -1), 4), ("RIGHTPADDING", (0, 0), (-1, -1), 4),
          ("TOPPADDING", (0, 0), (-1, -1), 3), ("BOTTOMPADDING", (0, 0), (-1, -1), 3)]
    if header: st.append(("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#1F3A5F")))
    for i in range(1, len(rows)):
        if i % 2 == 0: st.append(("BACKGROUND", (0, i), (-1, i), colors.HexColor("#F2F5FA")))
    t.setStyle(TableStyle(st)); return t

def img(path, maxw, maxh):
    w, h = PILImage.open(path).size
    k = min(maxw / w, maxh / h)
    return Image(path, width=w * k, height=h * k)

def footer(c, doc):
    c.saveState(); c.setFont("Helvetica", 8); c.setFillColor(colors.HexColor("#666666"))
    c.drawString(2 * cm, 1.1 * cm, "CPP31 - Photography Studio Booking  |  IU2541230464")
    c.drawRightString(A4[0] - 2 * cm, 1.1 * cm, f"Page {doc.page}"); c.restoreState()

doc = SimpleDocTemplate(OUT, pagesize=A4, leftMargin=2 * cm, rightMargin=2 * cm, topMargin=1.8 * cm, bottomMargin=1.8 * cm,
                        title="Photography Studio Booking - Project Report (CPP31)", author="IU2541230464")
W = A4[0] - 4 * cm
s = []

# ------------------------------------------------------------------ title page
s += [Spacer(1, 4.5 * cm), P("Photography Studio Booking", title), Spacer(1, 0.3 * cm),
      P("Project Report", sub), Spacer(1, 1.2 * cm)]
s.append(table([["Project Name", "Photography Studio Booking"], ["Project Code", "CPP31"],
                ["Enrollment Number", "IU2541230464"], ["Language / Standard", "C++17 (console application)"],
                ["Core Classes", "Customer, Photographer, Package, Booking, Studio, FileManager"],
                ["Student Name", "______________________________"]], [4.5 * cm, W - 4.5 * cm], header=False))
s.append(PageBreak())

# ------------------------------------------------------------------ 1-3
s += [P("1. Introduction and Objectives", h1),
      P("Photography Studio Booking is a console application that helps a small studio manage its customers, photographers, "
        "shoot packages and session bookings. The system prevents scheduling mistakes (double booking, bookings outside "
        "opening hours, over-filled rooms), calculates prices with membership discounts, produces summary reports and keeps all "
        "data in text files so that nothing is lost when the program is closed."),
      P("The project is also a demonstration of the required Object-Oriented Programming concepts: classes and objects, "
        "encapsulation, default and parameterised constructors, destructors, inheritance, runtime polymorphism, dynamic memory, file "
        "handling, validation, exception handling and a modular .h/.cpp design."),
      P("2. System Architecture", h1),
      P("The program follows the main flow given in the blueprint:"),
      Preformatted("USER -> MAIN MENU (main.cpp) -> SYSTEM CLASS (Studio) -> ENTITY CLASSES\n"
                   "     (Customer, Photographer, Package, Booking)\n"
                   "     -> MAIN TRANSACTION (Studio::bookSession) -> FILE MANAGER -> DATA FILES (Data/*.txt)", code),
      P("<b>main.cpp</b> only creates the <font face='Courier'>Studio</font> object, loads data, prints the menu and forwards each "
        "choice to a Studio function. It also contains the single try/catch that turns exceptions into readable messages. "
        "All business logic lives in <font face='Courier'>Studio</font> and the entity classes. "
        "<font face='Courier'>FileManager</font> is the only class that touches files; "
        "<font face='Courier'>Validator</font> / <font face='Courier'>Input</font> hold the validation rules and safe console input."),
      P("3. Class Design", h1)]
s.append(table([
    ["Class", "Responsibility", "Key members"],
    ["Record (abstract)", "Root of all stored entities. Declares the virtual interface used by generic code.",
     "id; virtual ~Record(); pure virtual typeName(), header(), display(), toFileLine(), matches()"],
    ["Person (abstract)", "Shared data of people; validates name / phone / email in its setters.",
     "name, phone, email; pure virtual roleDescription()"],
    ["Customer", "A client. Membership decides the discount.", "membership; discountRate() (Silver 5%, Gold 10%); fromFileLine()"],
    ["Photographer", "Staff member with a specialty.", "specialty, experienceYears; matches() also searches specialty"],
    ["Package", "A bookable shoot with price, length and photos.", "name, price, durationHours, numPhotos"],
    ["Booking", "A reserved session linking customer, photographer and package.",
     "ids, date, startHour, durationHours, totalPrice, status; overlaps(), isActive()"],
    ["Studio (system class)", "Owns the four dynamic collections and implements every menu operation and the booking rules.",
     "load(), save(), addRecord(), ..., bookSession(), checkAvailability(), report()"],
    ["FileManager", "All file I/O: safe write (temp file + rename), line reading, templated save/load.",
     "readLines(), writeLines(), save&lt;T&gt;(), load&lt;T&gt;()"],
    ["DynArray&lt;T&gt;", "Growable array on the heap (new[] / delete[]).", "add(), removeAt(), indexOfId(), at(), grow()"],
    ["Validator / Input", "Field validation and re-prompting console input.", "checkName(), checkDate(), askInt(), askYesNo() ..."],
    ["StudioException family", "ValidationException, FileException, RecordNotFoundException, BookingException.", "derive from std::runtime_error"],
], [3.4 * cm, 6.6 * cm, W - 10 * cm]))
s.append(PageBreak())

# ------------------------------------------------------------------ 4 OOP mapping
s += [P("4. OOP Concepts and Where They Are Used", h1)]
s.append(table([
    ["Requirement", "Where / how it is implemented"],
    ["Classes and objects", "Customer, Photographer, Package, Booking, Studio, FileManager (+ Record, Person, DynArray, Validator). "
                            "Objects are created in Studio (e.g. Booking b(...) in bookSession())."],
    ["Encapsulation", "Every data member is private. Access goes through getters and validating setters, so an object can never hold "
                      "an invalid phone number, date, price, status, etc."],
    ["Default constructors", "Record(), Person(), Customer(), Photographer(), Package(), Booking(), Studio() set safe placeholder values "
                             "(required because DynArray allocates with new T[capacity])."],
    ["Parameterised constructors", "e.g. Customer(id, name, phone, email, membership), Booking(id, cid, pid, pkgId, date, start, dur, total, status). "
                                   "They call the setters, so invalid data throws ValidationException."],
    ["Destructors", "virtual ~Record() and overrides in every subclass; ~DynArray() calls delete[]; ~Studio() prints a shutdown message "
                    "while its DynArray members free the heap."],
    ["Inheritance", "Record -> Person -> Customer / Photographer, and Record -> Package, Record -> Booking (two levels deep)."],
    ["Runtime polymorphism", "Generic code calls virtual functions through a Record&: printAll() calls display(), FileManager::save() calls toFileLine(), "
                             "search calls matches(). Photographer overrides matches() to add specialty; each class overrides header()/display()."],
    ["Dynamic memory", "DynArray&lt;T&gt; (new T[capacity], growth by doubling, delete[] in destructor) and temporary arrays in Studio::report()."],
    ["File handling / persistence", "FileManager reads and writes four '|' delimited text files in Data/; load on start, save on exit or on demand."],
    ["Validation", "Validator checks name, phone, email, date (leap years), membership, status, numeric ranges; Input re-prompts until valid."],
    ["Exception handling", "Custom exception hierarchy; thrown in setters, lookups and booking rules; caught once in main()."],
    ["Modular .h/.cpp files", "One header + source per class; DynArray.h is header-only because it is a template."],
], [4.2 * cm, W - 4.2 * cm]))

# ------------------------------------------------------------------ 5 dynamic memory
s += [P("5. Dynamic Memory", h1),
      P("<b>Why it is needed.</b> The studio cannot know in advance how many customers, photographers, packages or bookings it will hold. "
        "A fixed array such as <font face='Courier'>Booking list[100]</font> would either waste memory or overflow. "
        "<font face='Courier'>DynArray&lt;T&gt;</font> solves this by keeping a heap block that grows as records are added."),
      Preformatted("T* data = new T[capacity];            // constructor: allocate\n"
                   "void grow() {                          // when count == capacity\n"
                   "    T* bigger = new T[capacity * 2];   //   allocate a block twice as large\n"
                   "    for (i...) bigger[i] = data[i];    //   copy existing records\n"
                   "    delete[] data;                     //   release the old block\n"
                   "    data = bigger;\n"
                   "}\n"
                   "~DynArray() { delete[] data; }         // destructor releases everything", code),
      P("Studio owns four of these arrays (initial capacities 8, 4, 4 and 16), so memory use always follows the number of records. "
        "Copying is deliberately disabled (<font face='Courier'>= delete</font>) because the class owns a raw pointer; an accidental copy would cause a double "
        "<font face='Courier'>delete[]</font>. Removing a record shifts the later elements left, keeping the array compact."),
      P("<b>Second use.</b> <font face='Courier'>Studio::report()</font> allocates four temporary arrays "
        "(<font face='Courier'>new int[nPhotographers]()</font>, <font face='Courier'>new double[...]()</font>, etc.) to accumulate per-photographer, "
        "per-package and per-customer totals in one pass over the bookings, then frees them with <font face='Courier'>delete[]</font>."),
      P("<b>Verification.</b> The complete scripted session was run under AddressSanitizer and UndefinedBehaviorSanitizer "
        "(<font face='Courier'>-fsanitize=address,undefined</font>) with leak detection enabled: zero memory errors and zero leaks were reported.")]

# ------------------------------------------------------------------ 6 persistence
s += [P("6. File Handling and Persistence", h1),
      P("Persistence follows the required cycle: <b>START &rarr; load existing records &rarr; perform operations &rarr; save updated records &rarr; EXIT</b>."),
      table([["File", "Record layout"],
             ["Data/customers.txt", "id|name|phone|email|membership"],
             ["Data/photographers.txt", "id|name|phone|email|specialty|experienceYears"],
             ["Data/packages.txt", "id|name|price|durationHours|numPhotos"],
             ["Data/bookings.txt", "id|customerId|photographerId|packageId|date|startHour|durationHours|totalPrice|status"]],
            [5 * cm, W - 5 * cm]), Spacer(1, 6),
      P("<b>Load.</b> <font face='Courier'>FileManager::load&lt;T&gt;()</font> reads each line and calls <font face='Courier'>T::fromFileLine()</font>, which runs "
        "every field through the same validating setters as keyboard input. A line that is malformed, fails validation or repeats an ID is "
        "skipped with a warning instead of crashing the program. A missing file simply means \"no records yet\" (first run)."),
      P("<b>Save.</b> <font face='Courier'>FileManager::save&lt;T&gt;()</font> writes via the virtual <font face='Courier'>toFileLine()</font>. "
        "To avoid losing data if the program is interrupted mid-write, each file is first written to <font face='Courier'>name.txt.tmp</font> and only then "
        "swapped in for the old file. Saving happens on Exit (menu 9), when the input stream closes, and on demand (menu 8). "
        "The menu header shows <i>[unsaved changes]</i> whenever memory differs from disk."),
      P("IDs are generated as (largest existing ID + 1), so they stay unique after reloading. "
        "Deleting a customer, photographer or package that is still used by a booking is refused, which keeps the files consistent.")]

# ------------------------------------------------------------------ 7 validation / exceptions
s += [P("7. Validation and Exception Handling", h1)]
s.append(table([
    ["Field / rule", "Check", "On failure"],
    ["Name", "2-40 chars; letters, space, . ' -", "re-prompt"],
    ["Phone", "exactly 10 digits", "re-prompt"],
    ["Email", "one '@', text before it, domain containing '.'; no spaces or '|'", "re-prompt"],
    ["Date", "YYYY-MM-DD, real day for the month incl. leap years; not in the past when booking", "re-prompt"],
    ["Package", "price 0-1,000,000; 1-8 hours; 1-1000 photos", "re-prompt"],
    ["Menu / numbers", "whole number inside the allowed range", "re-prompt"],
    ["Studio hours", "session must start and end between 09:00 and 20:00", "BookingException"],
    ["Photographer clash", "no overlapping Confirmed booking for the same photographer and date", "BookingException"],
    ["Room capacity", "at most 2 overlapping Confirmed sessions", "BookingException"],
    ["Referential integrity", "cannot delete a customer / photographer / package used by a booking", "BookingException"],
    ["Unknown ID", "lookup fails", "RecordNotFoundException"],
    ["File problems", "cannot write file; corrupt / duplicate lines on load", "FileException / skipped with warning"],
], [3.6 * cm, W - 8.2 * cm, 4.6 * cm]))
s += [Spacer(1, 6), P("Interactive prompts re-ask until the value is valid, so typing mistakes never abort an operation. Business-rule and lookup errors are thrown as "
      "exceptions, travel up to the menu loop in <font face='Courier'>main()</font> and are printed as <i>Error: ...</i>; the program then returns to the menu with no data changed "
      "(a reschedule validates everything before modifying the booking). If the input stream closes unexpectedly, the program saves and exits cleanly.")]

# ------------------------------------------------------------------ 8 main transaction
s += [P("8. Main Transaction: Booking a Session", h1),
      P("Menu option 6 offers: book a session, cancel a booking, mark a booking completed, and view the schedule for a date. Booking a session proceeds as follows:"),
      table([["Step", "Action"],
             ["1", "Studio shows customers, photographers and packages and asks for one ID of each (unknown ID &rarr; RecordNotFoundException)."],
             ["2", "Asks for the session date (valid, not in the past) and start hour. End time = start + package duration."],
             ["3", "checkAvailability(): studio hours, photographer overlap, room capacity. Any violation throws BookingException with the reason."],
             ["4", "Price = package price x (1 - membership discount), rounded to 2 decimals. A summary is displayed and the user confirms (y/n)."],
             ["5", "A new Booking (status Confirmed) is created and added to the DynArray; the unsaved flag is set."],
             ["6", "Later: cancelling frees the slot; marking Completed moves the amount from 'expected' to 'earned' revenue in the report."]],
            [1.4 * cm, W - 1.4 * cm]),
      Spacer(1, 6), P("The Report (menu 7) shows record counts, bookings by status, revenue earned and expected, bookings and revenue per photographer, "
                      "package popularity, the most popular package, the top customer by spend and the next five upcoming sessions.")]
s.append(PageBreak())

# ------------------------------------------------------------------ 9 UML
s += [P("9. UML Diagrams", h1), P("9.1 Use Case Diagram", h2),
      P("Three actors (Studio Staff, Customer, Data Files) and twelve use cases. 'Book Photo Session' includes 'Check Availability &amp; Validate Booking Rules'."),
      img(os.path.join(D, "UseCase.png"), W, 17 * cm), P("Figure 1 - Use case diagram", cap), PageBreak(),
      P("9.2 Class Diagram", h2),
      img(os.path.join(D, "ClassDiagram.png"), W, 22.5 * cm),
      P("Figure 2 - Class diagram (visibility, data members, member functions, inheritance, composition, dependencies)", cap), PageBreak(),
      P("9.3 Sequence Diagram 1 - Main Transaction (Book a Photo Session)", h2),
      img(os.path.join(D, "Sequence1.png"), W, 16 * cm), P("Figure 3 - Sequence diagram of the main transaction, including the exception path", cap),      PageBreak(),
      P("9.4 Sequence Diagram 2 - Load on Start-up and Save on Exit", h2),
      img(os.path.join(D, "Sequence2.png"), W, 17 * cm), P("Figure 4 - Sequence diagram of file persistence", cap), PageBreak()]

# ------------------------------------------------------------------ 10 testing
s += [P("10. Testing", h1),
      P("The program was compiled with <font face='Courier'>g++ 13 -std=c++17 -Wall -Wextra</font> (no warnings) and exercised with scripted console sessions. "
        "Test results below come from those real runs (sample data loaded from Data/).")]
tests = [
    ["#", "Scenario", "Input / action", "Expected", "Result"],
    ["T1", "Invalid name", "Add customer, name 'A1'", "Error message, prompt repeats", "Pass"],
    ["T2", "Invalid phone", "'12345'", "'exactly 10 digits', re-prompt", "Pass"],
    ["T3", "Invalid email", "'neha@'", "'domain must look like name.com', re-prompt", "Pass"],
    ["T4", "Past date", "Booking date 2020-01-01", "Rejected, re-prompt", "Pass"],
    ["T5", "Photographer double booking", "Meera Nair, 2027-01-20 at 11:00 (already 10-12)", "BookingException naming booking 2", "Pass"],
    ["T6", "Outside studio hours", "8-hour package starting 18:00", "'Studio hours are 09:00 to 20:00 ...'", "Pass"],
    ["T7", "Room capacity", "Third overlapping session at 10:00", "'All 2 studio rooms are occupied'", "Pass"],
    ["T8", "Discount", "Silver customer, Family Package 7500", "Total Rs. 7125.00 (5%)", "Pass"],
    ["T9", "Complete / cancel", "Complete booking 2; cancel booking 1", "Status changes; slot freed", "Pass"],
    ["T10", "Delete protection", "Delete photographer used by 2 bookings", "'Cannot delete: 2 booking(s) still refer...'", "Pass"],
    ["T11", "Search", "Customer keyword 'aarav' / 'Neha'", "Matching rows only", "Pass"],
    ["T12", "Update", "Package price to 8000, Enter keeps other fields", "Only price changes", "Pass"],
    ["T13", "Report", "Menu 7 after the above", "Counts, revenue Rs. 19125.00 earned / 10625.00 expected", "Pass"],
    ["T14", "Persistence", "Exit (9), start a new process, display records", "All changes present after reopening", "Pass"],
    ["T15", "Corrupt data file", "Bad name, duplicate ID, garbage line", "3 lines skipped with warnings; valid record kept", "Pass"],
    ["T16", "Bad menu input", "'abc', '99'", "Re-prompt, no crash", "Pass"],
    ["T17", "Input stream closed", "End of input in the middle of a prompt", "Saves and exits cleanly", "Pass"],
    ["T18", "Memory safety", "Full session under ASan + UBSan + leak check", "No errors, no leaks", "Pass"],
]
s.append(table(tests, [0.9 * cm, 3.0 * cm, 4.9 * cm, 6.2 * cm, W - 15 * cm]))
s.append(PageBreak())

# ------------------------------------------------------------------ 11 screenshots
s += [P("11. Program Screenshots", h1),
      P("The images are terminal-style renderings of real runs of the program (each produced by replaying keystrokes into the executable).")]
shots = [("01_menu_display_customers.png", "Main menu and Display Records", 8.0), ("02_add_customer_validation.png", "Add Record with input validation", 8.0),
         ("03_book_session.png", "Main transaction: booking summary with Gold discount", 8.0),
         ("04_double_booking_blocked.png", "Double booking prevented", 6.0), ("05_studio_hours_error.png", "Studio hours rule", 3.0)]
for f, c, mh in shots:
    s.append(KeepTogether([img(os.path.join(S, f), W * 0.88, mh * cm), P(c, cap)]))
s.append(PageBreak())
for f, c, mh in [("06_report.png", "Report", 9.5), ("07_delete_blocked_search.png", "Delete protection and Search", 11.0)]:
    s.append(KeepTogether([img(os.path.join(S, f), W * 0.92, mh * cm), P(c, cap)]))
s.append(PageBreak())
s += [img(os.path.join(S, "08_persistence.png"), W * 0.8, 20 * cm), P("File persistence: add a package, exit, re-open - the new package is loaded", cap)]

# ------------------------------------------------------------------ 12 conclusion
s += [P("12. Limitations and Future Work", h1),
      P("The application is a single-user console program. Possible extensions: a graphical interface, several studios or rooms with their own capacity, "
        "per-photographer working hours and days off, recurring bookings, invoice printing, password-protected staff login, and a database back end instead of text files. "
        "Package prices are copied into each booking so that a later price change does not alter past invoices."),
      P("13. Conclusion", h1),
      P("The project meets all items in the blueprint: the six required classes, a nine-option menu, dynamic memory with correct release, "
        "persistent storage that survives closing and reopening the program, thorough validation and exception handling, and a modular architecture in which "
        "<font face='Courier'>main.cpp</font> contains no business logic. The UML diagrams and tests document and verify the design.")]

doc.build(s, onFirstPage=footer, onLaterPages=footer)
print("report written", OUT)
