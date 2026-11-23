#pragma once

#include <string>
#include <vector>

struct SavedHandRecord {
    int id;
    std::string ClientId;
    std::string title;
    std::string tiles;
    int points;
};

struct StoreOperationResult {
    bool ok = false;
    std::string message;
};

class ClientStore {
public:
    ClientStore();

    std::vector<std::string> listClients() const;
    std::vector<SavedHandRecord> getHandsForClient(const std::string& activeClientId) const;

    int addHand(const std::string& activeClientId,
        const std::string& title,
        const std::string& tiles,
        int points);

    StoreOperationResult updateHandTitle(const std::string& activeClientId,
        int recordId,
        const std::string& newTitle);

private:
    std::vector<std::string> clients_;
    std::vector<SavedHandRecord> records_;
    int nextId_ = 1;
};