#ifndef BITCOIN_SECURITY_ENHANCEMENTS_H
#define BITCOIN_SECURITY_ENHANCEMENTS_H

#include "validation.h"
#include "chain.h"
#include "consensus/consensus.h"
#include <string>
#include <vector>

/** Enhanced security features for NewYorkCoin 2.0.0 */

namespace security {

/** Maximum script length in bytes */
static const unsigned int MAX_SCRIPT_SIZE = 10000;

/** Maximum number of signature check operations in an IsStandard() P2SH script */
static const unsigned int MAX_P2SH_SIGOPS = 15;

/** 
 * Enhanced transaction validation flags
 * Implements additional checks from latest Litecoin core
 */
static const unsigned int ENHANCED_VALIDATION_FLAGS = 
    SCRIPT_VERIFY_P2SH |
    SCRIPT_VERIFY_STRICTENC |
    SCRIPT_VERIFY_DERSIG |
    SCRIPT_VERIFY_LOW_S |
    SCRIPT_VERIFY_NULLDUMMY |
    SCRIPT_VERIFY_SIGPUSHONLY |
    SCRIPT_VERIFY_MINIMALDATA |
    SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_NOPS |
    SCRIPT_VERIFY_CLEANSTACK |
    SCRIPT_VERIFY_CHECKLOCKTIMEVERIFY |
    SCRIPT_VERIFY_CHECKSEQUENCEVERIFY |
    SCRIPT_VERIFY_WITNESS |
    SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_WITNESS_PROGRAM;

/**
 * Enhanced block validation
 * Additional security checks for block validation
 */
bool CheckEnhancedBlockRules(const CBlock& block, CValidationState& state);

/**
 * Enhanced transaction validation
 * Additional security checks for transaction validation
 */
bool CheckEnhancedTransactionRules(const CTransaction& tx, CValidationState& state);

/**
 * Memory pool security
 * Enhanced memory pool acceptance rules
 */
bool CheckEnhancedMempoolRules(const CTxMemPool& pool, const CTransaction& tx, CValidationState& state);

} // namespace security

#endif // BITCOIN_SECURITY_ENHANCEMENTS_H 