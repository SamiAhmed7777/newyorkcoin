# NewYorkCoin Core [NYC] - Version 2.0.0

NewYorkCoin is a cryptocurrency that focuses on fast, efficient, and secure transactions. This release represents a major modernization of the codebase, bringing enhanced security features and improved stability.

## Fast Sync Features

NewYorkCoin 2.0.0 introduces several optimizations to dramatically speed up initial blockchain synchronization:

1. **UTXO Snapshots**: Periodically saves the state of unspent transaction outputs, allowing new nodes to quickly sync without processing the entire blockchain history.
2. **Checkpoint System**: Trusted checkpoints at regular intervals enable fast verification of historical blocks.
3. **Headers-First Sync**: Downloads and verifies block headers before full blocks for more efficient validation.
4. **Assume-Valid Blocks**: Allows skipping signature verification for historical blocks up to a recent, known-valid block.

### Fast Sync Configuration

The following configuration options are available in newyorkcoin.conf:

```conf
# Enable/disable fast sync features (default: 1)
usecheckpoints=1
assumevalid=1
useutxosnapshots=1
headersfirst=1

# Advanced settings
assumevalidtime=1450000000  # Timestamp for assume-valid block
```

## Major Changes in 2.0.0

- Complete codebase modernization
- Enhanced security features
- Updated core dependencies
- Improved build system
- Maintained all NewYorkCoin-specific features

### Security Enhancements

- Enhanced block and transaction validation
- Improved network protocol security
- Advanced mempool management
- Modern cryptographic standards
- Robust peer verification system

### Technical Updates

- Boost 1.84.0
- OpenSSL 1.1.1w
- Berkeley DB 5.3.28
- Modern C++ standards
- Enhanced build system

## Building NewYorkCoin Core

### Prerequisites

- C++ compiler with C++11 support
- Boost 1.84.0 or later
- OpenSSL 1.1.1w or later
- Berkeley DB 5.3.28
- CMake 3.16 or later

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/NewYorkCoinNYC/newyorkcoin.git
cd newyorkcoin
```

2. Install dependencies:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3

# macOS (using Homebrew)
brew install automake berkeley-db4 libtool boost openssl pkg-config python3
```

3. Build NewYorkCoin Core:
```bash
./autogen.sh
./configure
make
```

### Running Tests

```bash
make check
```

## Upgrading from Previous Versions

This is a major release with significant changes. Please follow these steps to upgrade:

1. Backup your wallet
2. Install the new version
3. Start with -reindex flag first time

For fastest initial sync on new installations, use the following command:
```bash
newyorkcoind -usecheckpoints=1 -assumevalid=1 -useutxosnapshots=1 -headersfirst=1
```

## Maintained Features

- DigiShield difficulty adjustment
- Custom block rewards
- Unique transaction fee structure
- Fast block times
- NYC-specific consensus rules

## Contributing

We welcome contributions! Please read our contributing guidelines before submitting pull requests.

## License

NewYorkCoin Core is released under the terms of the MIT license. See [COPYING](COPYING) for more information. 