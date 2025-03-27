#include "net_processing_enhancements.h"
#include "validation.h"
#include "util.h"

std::unique_ptr<CPeerManager> g_peer_manager;
std::unique_ptr<CBlockPropagator> g_block_propagator;

void CPeerManager::UpdatePeerScore(NodeId nodeId, int score) {
    std::lock_guard<std::mutex> lock(peerMtx);
    peerScores[nodeId] += score;
    
    // Handle banning if score is too low
    if (peerScores[nodeId] < PEER_BAN_THRESHOLD) {
        // Ban the peer
        LOCK(cs_main);
        Misbehaving(nodeId, 100);
    }
}

void CPeerManager::UpdatePeerStats(NodeId nodeId, const PeerStats& stats) {
    std::lock_guard<std::mutex> lock(peerMtx);
    peerStats[nodeId] = stats.blocksServed + stats.txsServed - 
                       (stats.invalidBlocks * 10) - (stats.invalidTxs * 2);
}

std::vector<NodeId> CPeerManager::GetBestPeers(size_t n) {
    std::lock_guard<std::mutex> lock(peerMtx);
    std::vector<std::pair<NodeId, uint64_t>> peers;
    for (const auto& stat : peerStats) {
        peers.push_back(stat);
    }
    
    std::sort(peers.begin(), peers.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NodeId> result;
    for (size_t i = 0; i < std::min(n, peers.size()); i++) {
        result.push_back(peers[i].first);
    }
    return result;
}

void CBlockPropagator::AnnounceBlock(const uint256& hash, NodeId nodeId) {
    std::lock_guard<std::mutex> lock(propagationMtx);
    blockAnnouncements[hash].insert(nodeId);
}

void CBlockPropagator::RequestBlock(const uint256& hash, NodeId nodeId) {
    std::lock_guard<std::mutex> lock(propagationMtx);
    blockRequests[hash].insert(nodeId);
}

std::vector<NodeId> CBlockPropagator::GetBlockSources(const uint256& hash) {
    std::lock_guard<std::mutex> lock(propagationMtx);
    std::vector<NodeId> sources;
    auto it = blockAnnouncements.find(hash);
    if (it != blockAnnouncements.end()) {
        sources.insert(sources.end(), it->second.begin(), it->second.end());
    }
    return sources;
}

void CBlockPropagator::ProcessBlockAnnouncement(const uint256& hash, NodeId nodeId) {
    LOCK(cs_main);
    if (mapBlockIndex.count(hash) == 0) {
        // New block announcement
        AnnounceBlock(hash, nodeId);
        // Request from best peers
        auto peers = g_peer_manager->GetBestPeers(3);
        for (NodeId peer : peers) {
            if (peer != nodeId) {
                RequestBlock(hash, peer);
            }
        }
    }
}

bool InitEnhancedNetworking() {
    g_peer_manager = std::make_unique<CPeerManager>();
    g_block_propagator = std::make_unique<CBlockPropagator>();
    LogPrintf("Enhanced networking initialized\n");
    return true;
} 