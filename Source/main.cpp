// main.cpp - creates the system object, shows the menu and forwards each choice.
// All business logic lives in the Studio class (and the entity classes it owns).
#include "Exceptions.h"
#include "Studio.h"
#include "Validator.h"
#include <iostream>

using namespace std;

static void showMenu(const Studio& studio) {
    cout << "\n==================================================\n"
         << "  " << studio.getName() << "\n"
         << "  Open " << studio.getOpenHour() << ":00 - " << studio.getCloseHour() << ":00"
         << (studio.hasUnsavedChanges() ? "      [unsaved changes]" : "") << "\n"
         << "==================================================\n"
         << "  1. Add Record\n"
         << "  2. Display Records\n"
         << "  3. Search Record\n"
         << "  4. Update Record\n"
         << "  5. Delete Record\n"
         << "  6. Main Transaction (Book / Cancel / Complete)\n"
         << "  7. Report\n"
         << "  8. Save/Load\n"
         << "  9. Exit\n"
         << "--------------------------------------------------\n";
}

int main(int argc, char* argv[]) {
    try {
        Studio studio;                                   // system object
        if (argc > 1) studio.setDataDir(argv[1]);        // optional: custom data folder
        studio.load();                                   // START -> load existing records

        bool running = true;
        while (running) {
            try {
                showMenu(studio);
                int choice = Input::askInt("Enter choice (1-9): ", 1, 9);
                switch (choice) {
                    case 1: studio.addRecord();        break;
                    case 2: studio.displayRecords();   break;
                    case 3: studio.searchRecord();     break;
                    case 4: studio.updateRecord();     break;
                    case 5: studio.deleteRecord();     break;
                    case 6: studio.mainTransaction();  break;
                    case 7: studio.report();           break;
                    case 8: studio.saveLoadMenu();     break;
                    case 9:
                        studio.save();                   // save updated records -> EXIT
                        cout << "Goodbye!\n";
                        running = false;
                        break;
                }
            } catch (const InputClosedException&) {
                cout << "\nInput closed - saving and exiting.\n";
                studio.save();
                running = false;
            } catch (const StudioException& e) {         // validation, file, booking, not-found errors
                cout << "\n  Error: " << e.what() << "\n";
            } catch (const exception& e) {               // anything unexpected
                cout << "\n  Unexpected error: " << e.what() << "\n";
            }
        }
    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
