// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The SteepCoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TXDB_H
#define BITCOIN_TXDB_H

#include "txdb-leveldb.h"
#include "main.h"
#include "leveldbwrapper.h"

/** Access to the block database (blocks/index/) */
class CBlockTreeDB : public CLevelDBWrapper
{
public:
    CBlockTreeDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);
private:
    CBlockTreeDB(const CBlockTreeDB&);
    void operator=(const CBlockTreeDB&);
public:
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool ReadBestInvalidTrust(CBigNum& bnBestInvalidTrust);
    bool WriteBestInvalidTrust(const CBigNum& bnBestInvalidTrust);
    bool ReadBlockFileInfo(int nFile, CBlockFileInfo &fileinfo);
    bool WriteBlockFileInfo(int nFile, const CBlockFileInfo &fileinfo);
    bool ReadLastBlockFile(int &nFile);
    bool WriteLastBlockFile(int nFile);
    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool &fReindex);
    bool ReadTxIndex(const uint256 &txid, CDiskTxPosrr &pos);
    bool WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPosrr> > &list);
    bool WriteFlag(const std::string &name, bool fValue);
    bool ReadFlag(const std::string &name, bool &fValue);
    bool ReadSyncCheckpoint(uint256& hashCheckpoint);
    bool WriteSyncCheckpoint(uint256 hashCheckpoint);
    bool ReadCheckpointPubKey(std::string& strPubKey);
    bool WriteCheckpointPubKey(const std::string& strPubKey);
    bool LoadBlockIndexGuts();
};

#endif  // BITCOIN_TXDB_H
