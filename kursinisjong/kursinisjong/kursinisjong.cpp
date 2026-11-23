#include "HandCalculator.h"
#include "ClientStore.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#define NOMINMAX
#include <windows.h>
#include <clocale>


namespace {

    void clearLine() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void clearScreen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    void waitForEnter() {
        std::cout << "\nSpauskite Enter, kad tęstumėte...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void runAutomatedTests() {
        HandCalculator calc;

        // 1. Normal case
        {
            HandContext ctx{ "123m 456m 789p 234s 55p", WinType::Ron, true, false, 0 };
            auto res = calc.evaluate(ctx);
            assert(res.valid);
            assert(res.han == 2);
            assert(res.fu == 30);
            assert(res.points == 2000);
        }

        // 2. Boundary / special case: Chiitoitsu
        {
            HandContext ctx{ "11m 22m 33p 44p 55s 66s 77z", WinType::Ron, false, false, 0 };
            auto res = calc.evaluate(ctx);
            assert(res.valid);
            assert(res.han == 2);
            assert(res.fu == 25);
            assert(res.points == 1600);
        }

        // 3. Error case: 15 tiles
        {
            HandContext ctx{ "123m 456m 789p 234s 55p 6z", WinType::Ron, false, false, 0 };
            auto res = calc.evaluate(ctx);
            assert(!res.valid);
            assert(res.error == "Klaida: laiminčioje rankoje turi būti 14 plytelių.");
        }

        // 4. Client isolation test
        {
            ClientStore store;
            auto kaunasHands = store.getHandsForClient("kaunas_academy");
            assert(!kaunasHands.empty());
            int kaunasRecordId = kaunasHands.front().id;

            auto forbidden = store.updateHandTitle("vilnius_club", kaunasRecordId, "Hacked title");
            assert(!forbidden.ok);

            auto stillSame = store.getHandsForClient("kaunas_academy");
            assert(stillSame.front().title == "Septynios poros");
        }

        std::cout << "Visi 4 automatiniai testai pavyko!\n";
    }

    void printMenu(const std::string& activeClient) {
        std::cout << "=== Riichi Mahjong prototipas [Klientas: " << activeClient << "] ===\n";
        std::cout << "1. Skaičiuoti rankos vertę\n";
        std::cout << "2. Peržiūrėti kliento išsaugotas rankas\n";
        std::cout << "3. Atnaujinti rankos pavadinimą (pagal ID)\n";
        std::cout << "4. Pakeisti aktyvų klientą\n";
        std::cout << "5. Paleisti automatinius testus\n";
        std::cout << "0. Išeiti\n";
        std::cout << "Pasirinkimas: ";
    }

} // namespace

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
    HandCalculator calc;
    ClientStore store;
    std::string activeClient = "vilnius_club";

    clearScreen();

    while (true) {
        printMenu(activeClient);
        int choice = -1;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            clearLine();
            std::cout << "Klaida: įveskite meniu numerį.\n";
            waitForEnter();
            clearScreen();
            continue;
        }
        clearLine();

        if (choice == 0) {
            clearScreen();
            std::cout << "Programa baigta.\n";
            break;
        }
        if (choice == 1) {
            HandContext ctx;
            std::cout << "Įveskite 14 plytelių (pvz., 123m 456m 789p 234s 55p): ";
            std::getline(std::cin, ctx.rawTiles);

            char winCh = 'r';
            std::cout << "Laimėjimo būdas (r = ron, t = tsumo): ";
            std::cin >> winCh;
            ctx.winType = (winCh == 't' || winCh == 'T') ? WinType::Tsumo : WinType::Ron;

            char riichiCh = 'n';
            std::cout << "Riichi deklaruotas? (t/n): ";
            std::cin >> riichiCh;
            ctx.isRiichi = (riichiCh == 't' || riichiCh == 'T');

            char dealerCh = 'n';
            std::cout << "Dalintojas? (t/n): ";
            std::cin >> dealerCh;
            ctx.isDealer = (dealerCh == 't' || dealerCh == 'T');

            std::cout << "Dora skaičius: ";
            std::cin >> ctx.doraCount;
            clearLine();

            CalculationResult res = calc.evaluate(ctx);
            if (!res.valid) {
                std::cout << res.error << "\n";
            }
            else {
                std::cout << "Yaku: ";
                for (std::size_t i = 0; i < res.yakuList.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << res.yakuList[i];
                }
                std::cout << "\nHan: " << res.han
                    << ", Fu: " << res.fu
                    << ", Taškai: " << res.points << "\n";

                char saveCh = 'n';
                std::cout << "Išsaugoti šią ranką klientui „" << activeClient << "“? (t/n): ";
                std::cin >> saveCh;
                clearLine();
                if (saveCh == 't' || saveCh == 'T') {
                    std::string title;
                    std::cout << "Pavadinimas: ";
                    std::getline(std::cin, title);
                    int id = store.addHand(activeClient, title, ctx.rawTiles, res.points);
                    std::cout << "Išsaugota (ID: " << id << ").\n";
                }
            }
            waitForEnter();
            clearScreen();
        }
        else if (choice == 2) {
            auto hands = store.getHandsForClient(activeClient);
            std::cout << "Kliento „" << activeClient << "“ rankos:\n";
            for (const auto& h : hands) {
                std::cout << "  #" << h.id << " [" << h.title << "] " << h.tiles
                    << " -> " << h.points << " tšk.\n";
            }
            waitForEnter();
            clearScreen();
        }
        else if (choice == 3) {
            int id = 0;
            std::string newTitle;
            std::cout << "Įrašo ID: ";
            std::cin >> id;
            clearLine();
            std::cout << "Naujas pavadinimas: ";
            std::getline(std::cin, newTitle);
            auto op = store.updateHandTitle(activeClient, id, newTitle);
            std::cout << op.message << "\n";
            waitForEnter();
            clearScreen();
        }
        else if (choice == 4) {
            auto clients = store.listClients();
            std::cout << "Galimi klientai:\n";
            for (std::size_t i = 0; i < clients.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << clients[i] << "\n";
            }
            std::size_t idx = 0;
            std::cout << "Pasirinkite numerį: ";
            std::cin >> idx;
            clearLine();
            if (idx >= 1 && idx <= clients.size()) {
                activeClient = clients[idx - 1];
                std::cout << "Aktyvus klientas: " << activeClient << "\n";
            }
            else {
                std::cout << "Klaida: netinkamas kliento numeris.\n";
            }
            waitForEnter();
            clearScreen();
        }
        else if (choice == 5) {
            runAutomatedTests();
            waitForEnter();
            clearScreen();
        }
        else {
            std::cout << "Klaida: nežinomas pasirinkimas.\n";
            waitForEnter();
            clearScreen();
        }
    }

    return 0;
}