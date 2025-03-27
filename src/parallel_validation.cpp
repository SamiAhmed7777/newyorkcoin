#include "parallel_validation.h"
#include "validation.h"
#include "util.h"
#include "chainparams.h"
#include "consensus/validation.h"

std::unique_ptr<CParallelValidation> parallelValidation;

CParallelValidation::CParallelValidation() {
    state.fStarted = false;
    state.fStopped = false;
    state.nThreads = 0;
    state.nBlocksInFlight = 0;
}

CParallelValidation::~CParallelValidation() {
    Stop();
}

bool CParallelValidation::Initialize(int nThreads) {
    if (state.fStarted)
        return false;

    // Cap threads to system maximum
    int nMaxThreads = std::min(nThreads, (int)std::thread::hardware_concurrency());
    nMaxThreads = std::min(nMaxThreads, MAX_PARALLEL_VALIDATION_THREADS);

    try {
        threadPool.reset(new ThreadPool(nMaxThreads));
        state.nThreads = nMaxThreads;
        state.fStarted = true;
        LogPrintf("Parallel validation initialized with %d threads\n", nMaxThreads);
        return true;
    } catch (const std::exception& e) {
        LogPrintf("Failed to initialize parallel validation: %s\n", e.what());
        return false;
    }
}

bool CParallelValidation::ValidateBlockParallel(const CBlock& block, CValidationState& state,
                                               const CChainParams& chainparams, CBlockIndex* pindex) {
    AssertLockHeld(cs_main);
    
    // Basic block checks
    if (!CheckBlock(block, state, true, true)) {
        return false;
    }

    // Context-dependent validity checks
    if (!ContextualCheckBlock(block, state, pindex->pprev)) {
        return false;
    }

    // Connect block to chain
    if (!ConnectBlock(block, state, pindex, pcoinsTip, chainparams, false)) {
        return false;
    }

    return true;
}

bool CParallelValidation::StartParallelValidation(const std::vector<std::shared_ptr<const CBlock>>& vBlocks,
                                                 const CChainParams& chainparams) {
    if (!state.fStarted || state.fStopped || vBlocks.empty())
        return false;

    // Don't parallelize if too few blocks
    if (vBlocks.size() < MIN_PARALLEL_BLOCKS)
        return false;

    LOCK(state.cs_validation);

    // Clear previous results
    validationResults.clear();
    state.nBlocksInFlight = vBlocks.size();

    // Queue validation tasks
    for (const auto& pblock : vBlocks) {
        CValidationState blockState;
        CBlockIndex* pindex = nullptr;

        // Get or create block index
        {
            LOCK(cs_main);
            pindex = LookupBlockIndex(pblock->GetHash());
            if (!pindex) {
                pindex = AddToBlockIndex(pblock->GetBlockHeader());
            }
        }

        // Queue validation task
        auto future = threadPool->enqueue([this, pblock, &blockState, &chainparams, pindex]() {
            return this->ValidateBlockParallel(*pblock, blockState, chainparams, pindex);
        });

        validationResults.push_back(std::move(future));
    }

    return true;
}

void CParallelValidation::WaitForValidation() {
    if (!state.fStarted || validationResults.empty())
        return;

    // Wait for all validations to complete
    for (auto& result : validationResults) {
        try {
            result.wait();
        } catch (const std::exception& e) {
            LogPrintf("Error waiting for validation: %s\n", e.what());
        }
    }

    state.nBlocksInFlight = 0;
}

void CParallelValidation::Stop() {
    if (!state.fStarted)
        return;

    state.fStopped = true;
    WaitForValidation();
    threadPool.reset();
    state.fStarted = false;
    state.nThreads = 0;
} 