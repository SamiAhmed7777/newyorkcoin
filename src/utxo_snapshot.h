#ifndef BITCOIN_UTXO_SNAPSHOT_H
#define BITCOIN_UTXO_SNAPSHOT_H

#include "chain.h"
#include "coins.h"
#include "uint256.h"
#include <string>

class CCoinsViewCache;
class CBlockIndex;

/** UTXO Snapshot Manager for fast sync */
class UTXOSnapshot {
private:
    std::string snapshotDir;
    uint32_t snapshotInterval;
    uint32_t maxAge;

    bool CreateSnapshot(const CCoinsViewCache& view, const CBlockIndex* pindex);
    bool LoadSnapshot(CCoinsViewCache& view, const CBlockIndex* pindex);
    std::string GetSnapshotFileName(const uint256& blockHash) const;
    bool ValidateSnapshot(const CCoinsViewCache& view, const CBlockIndex* pindex) const;

public:
    UTXOSnapshot(const std::string& dir, uint32_t interval, uint32_t age);

    /** Create a new UTXO snapshot if needed */
    bool MaybeCreateSnapshot(const CCoinsViewCache& view, const CBlockIndex* pindex);

    /** Try to load a UTXO snapshot for fast sync */
    bool TryLoadLatestSnapshot(CCoinsViewCache& view, const CBlockIndex* pindex);

    /** Get the latest valid snapshot height */
    int GetLatestSnapshotHeight() const;

    /** Clean up old snapshots */
    void PruneOldSnapshots(uint32_t currentHeight);
};

#endif // BITCOIN_UTXO_SNAPSHOT_H 