#include "ClientStore.h"

ClientStore::ClientStore() {
    clients_ = { "vilnius_club", "kaunas_academy" };

    addHand("vilnius_club", "Standartinis ron", "123m 456m 789p 234s 55p", 2000);
    addHand("kaunas_academy", "Septynios poros", "11m 22m 33p 44p 55s 66s 77z", 1600);
}

std::vector<std::string> ClientStore::listClients() const {
    return clients_;
}

std::vector<SavedHandRecord> ClientStore::getHandsForClient(const std::string& activeClientId) const {
    std::vector<SavedHandRecord> out;
    for (const auto& rec : records_) {
        if (rec.ClientId == activeClientId) {
            out.push_back(rec);
        }
    }
    return out;
}

int ClientStore::addHand(const std::string& activeClientId,
    const std::string& title,
    const std::string& tiles,
    int points) {
    int id = nextId_++;
    records_.push_back(SavedHandRecord{ id, activeClientId, title, tiles, points });
    return id;
}

StoreOperationResult ClientStore::updateHandTitle(const std::string& activeClientId,
    int recordId,
    const std::string& newTitle) {
    for (auto& rec : records_) {
        if (rec.id == recordId) {
            if (rec.ClientId != activeClientId) {
                return StoreOperationResult{
                    false,
                    "Klaida: negalima keisti kito kliento („" + rec.ClientId + "“) įrašo."
                };
            }
            rec.title = newTitle;
            return StoreOperationResult{ true, "Įrašas sėkmingai atnaujintas." };
        }
    }
    return StoreOperationResult{ false, "Klaida: įrašas nerastas." };
}