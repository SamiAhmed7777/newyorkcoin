#ifndef BITCOIN_NET_PROCESSING_ENHANCEMENTS_H
#define BITCOIN_NET_PROCESSING_ENHANCEMENTS_H

#include "net.h"
#include "validation.h"
#include <map>
#include <set>
#include <mutex>
#include <memory>

/** Enhanced Peer Management */
class CPeerManager {
private:
    std::map<NodeId, int> peerScores;
    std::map<NodeId, uint64_t> peerStats;
    std::mutex peerMtx;

    struct PeerStats {
        uint64_t blocksServed;
        uint64_t txsServed;
        uint64_t invalidBlocks;
        uint64_t invalidTxs;
        uint64_t latency;
        time_t lastSeen;
    };

public:
    void UpdatePeerScore(NodeId nodeId, int score);
    void UpdatePeerStats(NodeId nodeId, const PeerStats& stats);
    std::vector<NodeId> GetBestPeers(size_t n);
};

/** Enhanced Block Propagation */
class CBlockPropagator {
private:
    std::mutex propagationMtx;
    std::map<uint256, std::set<NodeId>> blockAnnouncements;
    std::map<uint256, std::set<NodeId>> blockRequests;
    
public:
    void AnnounceBlock(const uint256& hash, NodeId nodeId);
    void RequestBlock(const uint256& hash, NodeId nodeId);
    std::vector<NodeId> GetBlockSources(const uint256& hash);
    void ProcessBlockAnnouncement(const uint256& hash, NodeId nodeId);
};

// Global instances
extern std::unique_ptr<CPeerManager> g_peer_manager;
extern std::unique_ptr<CBlockPropagator> g_block_propagator;

// Initialization function
bool InitEnhancedNetworking();

#endif // BITCOIN_NET_PROCESSING_ENHANCEMENTS_H 