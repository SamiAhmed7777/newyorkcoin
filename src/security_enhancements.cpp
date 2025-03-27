#include "security_enhancements.h"
#include "validation.h"
#include "consensus/validation.h"
#include "primitives/transaction.h"
#include "script/script.h"
#include "util.h"

namespace security {

bool CheckEnhancedBlockRules(const CBlock& block, CValidationState& state)
{
    // Check block size limits
    unsigned int nMaxBlockSize = MAX_BLOCK_SIZE;
    if (block.vtx.empty() || block.vtx.size() > nMaxBlockSize || ::GetSerializeSize(block, SER_NETWORK, PROTOCOL_VERSION) > nMaxBlockSize)
        return state.DoS(100, false, REJECT_INVALID, "bad-blk-length", false, "size limits failed");

    // Check timestamp not too far in future
    if (block.GetBlockTime() > GetAdjustedTime() + 2 * 60 * 60)
        return state.Invalid(false, REJECT_INVALID, "time-too-new", "block timestamp too far in the future");

    // Enhanced checks for coinbase transaction
    const CTransaction& coinbase = *block.vtx[0];
    if (!coinbase.IsCoinBase())
        return state.DoS(100, false, REJECT_INVALID, "bad-cb-missing", false, "first tx is not coinbase");

    // Check coinbase script size
    unsigned int nMaxCoinbaseScriptSize = 100;
    if (coinbase.vin[0].scriptSig.size() < 2 || coinbase.vin[0].scriptSig.size() > nMaxCoinbaseScriptSize)
        return state.DoS(100, false, REJECT_INVALID, "bad-cb-length", false, "coinbase script size");

    return true;
}

bool CheckEnhancedTransactionRules(const CTransaction& tx, CValidationState& state)
{
    // Basic checks that don't depend on any context
    if (tx.vin.empty())
        return state.DoS(10, false, REJECT_INVALID, "bad-txns-vin-empty");
    if (tx.vout.empty())
        return state.DoS(10, false, REJECT_INVALID, "bad-txns-vout-empty");

    // Size limits
    unsigned int nMaxSize = MAX_BLOCK_SIZE;
    if (::GetSerializeSize(tx, SER_NETWORK, PROTOCOL_VERSION) > nMaxSize)
        return state.DoS(100, false, REJECT_INVALID, "bad-txns-oversize");

    // Check for negative or overflow output values
    CAmount nValueOut = 0;
    for (const CTxOut& txout : tx.vout)
    {
        if (txout.nValue < 0)
            return state.DoS(100, false, REJECT_INVALID, "bad-txns-vout-negative");
        if (txout.nValue > MAX_MONEY)
            return state.DoS(100, false, REJECT_INVALID, "bad-txns-vout-toolarge");
        nValueOut += txout.nValue;
        if (!MoneyRange(nValueOut))
            return state.DoS(100, false, REJECT_INVALID, "bad-txns-txouttotal-toolarge");
    }

    // Check for duplicate inputs
    std::set<COutPoint> vInOutPoints;
    for (const CTxIn& txin : tx.vin)
    {
        if (!vInOutPoints.insert(txin.prevout).second)
            return state.DoS(100, false, REJECT_INVALID, "bad-txns-inputs-duplicate");
    }

    return true;
}

bool CheckEnhancedMempoolRules(const CTxMemPool& pool, const CTransaction& tx, CValidationState& state)
{
    // Check against previous transactions
    // This is done last to help prevent CPU exhaustion attacks
    if (!CheckEnhancedTransactionRules(tx, state))
        return false;

    // Check size limits
    unsigned int nTxSize = ::GetSerializeSize(tx, SER_NETWORK, PROTOCOL_VERSION);
    if (nTxSize > MAX_STANDARD_TX_SIZE)
        return state.DoS(0, false, REJECT_NONSTANDARD, "tx-size-large", false, strprintf("size > %u", MAX_STANDARD_TX_SIZE));

    // Check for non-standard script size
    for (const CTxOut& txout : tx.vout)
    {
        if (txout.scriptPubKey.size() > MAX_SCRIPT_SIZE)
            return state.DoS(0, false, REJECT_NONSTANDARD, "scriptpubkey-size", false, "scriptpubkey too large");
    }

    return true;
}

} // namespace security 