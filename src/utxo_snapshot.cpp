#include "utxo_snapshot.h"
#include "validation.h"
#include "util.h"
#include "fs.h"
#include "serialize.h"

#include <boost/filesystem.hpp>

UTXOSnapshot::UTXOSnapshot(const std::string& dir, uint32_t interval, uint32_t age)
    : snapshotDir(dir), snapshotInterval(interval), maxAge(age)
{
    boost::filesystem::create_directories(snapshotDir);
}

bool UTXOSnapshot::MaybeCreateSnapshot(const CCoinsViewCache& view, const CBlockIndex* pindex)
{
    // Only create snapshots at specified intervals
    if (pindex->nHeight % snapshotInterval != 0)
        return false;

    return CreateSnapshot(view, pindex);
}

bool UTXOSnapshot::CreateSnapshot(const CCoinsViewCache& view, const CBlockIndex* pindex)
{
    LogPrintf("Creating UTXO snapshot at height %d\n", pindex->nHeight);

    std::string fileName = GetSnapshotFileName(pindex->GetBlockHash());
    FILE* fileout = fsbridge::fopen(fileName, "wb");
    if (!fileout) {
        LogPrintf("Failed to open snapshot file %s\n", fileName);
        return false;
    }

    CAutoFile afile(fileout, SER_DISK, CLIENT_VERSION);

    try {
        // Write header
        afile << FLATDATA(Params().MessageStart());
        afile << pindex->GetBlockHash();
        afile << pindex->nHeight;

        // Write UTXO set
        for (CCoinsViewCursor* cursor = view.Cursor(); cursor->Valid(); cursor->Next()) {
            Coin coin = cursor->GetValue();
            afile << cursor->GetKey();
            afile << coin;
        }

    } catch (const std::exception& e) {
        LogPrintf("Error creating snapshot: %s\n", e.what());
        return false;
    }

    LogPrintf("Successfully created UTXO snapshot at height %d\n", pindex->nHeight);
    return true;
}

bool UTXOSnapshot::TryLoadLatestSnapshot(CCoinsViewCache& view, const CBlockIndex* pindex)
{
    int snapshotHeight = GetLatestSnapshotHeight();
    if (snapshotHeight <= 0)
        return false;

    // Find the block index for the snapshot height
    const CBlockIndex* snapshotIndex = pindex;
    while (snapshotIndex && snapshotIndex->nHeight > snapshotHeight)
        snapshotIndex = snapshotIndex->pprev;

    if (!snapshotIndex)
        return false;

    return LoadSnapshot(view, snapshotIndex);
}

bool UTXOSnapshot::LoadSnapshot(CCoinsViewCache& view, const CBlockIndex* pindex)
{
    LogPrintf("Loading UTXO snapshot from height %d\n", pindex->nHeight);

    std::string fileName = GetSnapshotFileName(pindex->GetBlockHash());
    FILE* filein = fsbridge::fopen(fileName, "rb");
    if (!filein) {
        LogPrintf("Failed to open snapshot file %s\n", fileName);
        return false;
    }

    CAutoFile afile(filein, SER_DISK, CLIENT_VERSION);
    
    try {
        // Verify header
        char pchMsgTmp[4];
        afile >> FLATDATA(pchMsgTmp);
        if (memcmp(pchMsgTmp, Params().MessageStart(), 4) != 0) {
            LogPrintf("Invalid snapshot format\n");
            return false;
        }

        uint256 hashBlock;
        int height;
        afile >> hashBlock;
        afile >> height;

        if (hashBlock != pindex->GetBlockHash() || height != pindex->nHeight) {
            LogPrintf("Snapshot block mismatch\n");
            return false;
        }

        // Read UTXO set
        while (!afile.eof()) {
            COutPoint outpoint;
            Coin coin;
            
            try {
                afile >> outpoint;
                afile >> coin;
                view.AddCoin(outpoint, std::move(coin), true);
            } catch (const std::exception&) {
                // EOF reached
                break;
            }
        }

    } catch (const std::exception& e) {
        LogPrintf("Error loading snapshot: %s\n", e.what());
        return false;
    }

    LogPrintf("Successfully loaded UTXO snapshot from height %d\n", pindex->nHeight);
    return true;
}

std::string UTXOSnapshot::GetSnapshotFileName(const uint256& blockHash) const
{
    return snapshotDir + "/utxo-" + blockHash.ToString() + ".dat";
}

int UTXOSnapshot::GetLatestSnapshotHeight() const
{
    int latestHeight = -1;
    boost::filesystem::path snapshotPath(snapshotDir);

    try {
        for (boost::filesystem::directory_iterator it(snapshotPath); it != boost::filesystem::directory_iterator(); ++it) {
            if (it->path().extension() == ".dat") {
                std::string fileName = it->path().stem().string();
                if (fileName.substr(0, 5) == "utxo-") {
                    FILE* filein = fsbridge::fopen(it->path().string(), "rb");
                    if (!filein)
                        continue;

                    CAutoFile afile(filein, SER_DISK, CLIENT_VERSION);
                    try {
                        char pchMsgTmp[4];
                        afile >> FLATDATA(pchMsgTmp);
                        if (memcmp(pchMsgTmp, Params().MessageStart(), 4) == 0) {
                            uint256 hashBlock;
                            int height;
                            afile >> hashBlock;
                            afile >> height;
                            
                            if (height > latestHeight)
                                latestHeight = height;
                        }
                    } catch (const std::exception&) {
                        // Ignore invalid files
                    }
                }
            }
        }
    } catch (const boost::filesystem::filesystem_error&) {
        return -1;
    }

    return latestHeight;
}

void UTXOSnapshot::PruneOldSnapshots(uint32_t currentHeight)
{
    boost::filesystem::path snapshotPath(snapshotDir);

    try {
        for (boost::filesystem::directory_iterator it(snapshotPath); it != boost::filesystem::directory_iterator(); ++it) {
            if (it->path().extension() == ".dat") {
                FILE* filein = fsbridge::fopen(it->path().string(), "rb");
                if (!filein)
                    continue;

                CAutoFile afile(filein, SER_DISK, CLIENT_VERSION);
                try {
                    char pchMsgTmp[4];
                    afile >> FLATDATA(pchMsgTmp);
                    if (memcmp(pchMsgTmp, Params().MessageStart(), 4) == 0) {
                        uint256 hashBlock;
                        int height;
                        afile >> hashBlock;
                        afile >> height;
                        
                        if (currentHeight - height > maxAge) {
                            boost::filesystem::remove(it->path());
                            LogPrintf("Removed old UTXO snapshot from height %d\n", height);
                        }
                    }
                } catch (const std::exception&) {
                    // Remove invalid files
                    boost::filesystem::remove(it->path());
                }
            }
        }
    } catch (const boost::filesystem::filesystem_error&) {
        // Ignore filesystem errors during pruning
    }
} 