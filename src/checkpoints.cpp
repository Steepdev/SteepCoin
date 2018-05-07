// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2011-2017 The Peercoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "db.h"
#include "main.h"
#include "txdb.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;   // hardened checkpoints

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (0,uint256("0x00000e902f9cea8a855ca2b02605a23eec18c8c76505251a08dddaa91cabdcd8"))
        (1,uint256("0x000002ba7e2ee369dc51c682798d869a5b7c3e61700d9f0dc39d58b501dbf46e"))
        (2,uint256("0x00000e9dd9b0d6f13c53bbf952e98cd61b2ba87ddb8fba7d02502355504d6426"))
        (20,uint256("0x000001e58624b41beaa75cd146e5389bddd473dcd21099bcd1d380a7b9eebc5e"))
        (10000,uint256("0x00000000006d5ccdbf23a8f3784fedda4cf7e2f0ae6a4d3da3b938a3da1e9a03"))
        (30000,uint256("0x0000000000940ef181a9ceefaec9c861eea6270b014bcc7764689afc47001053"))
        (75000,uint256("0x00000000001e99c10b04464e70e83cb48fcea999478801b3d7d051b2da40a40c"))
        (100000,uint256("0x8c577e2f69f3fcb45a5bb79b642918b75f293b06337cd5ac3cb22d7f0b9856a8"))
        (125000,uint256("0x000000000036eff3aee3e2235a1c8447d61fdbc57606b1d630f1cfaac3557919"))
        (147500,uint256("0xc0bd5acf41a95ae2c4e35bd7bb9cd6683611545adb8aaf686b413bc9a79d9ccc"))

        /*( 0, hashGenesisBlockOfficial )
        ( 19080, uint256("0x000000000000bca54d9ac17881f94193fd6a270c1bb21c3bf0b37f588a40dbd7"))
        ( 30583, uint256("0xd39d1481a7eecba48932ea5913be58ad3894c7ee6d5a8ba8abeb772c66a6696e"))
        ( 99999, uint256("0x27fd5e1de16a4270eb8c68dee2754a64da6312c7c3a0e99a7e6776246be1ee3f"))
        (219999, uint256("0xab0dad4b10d2370f009ed6df6effca1ba42f01d5070d6b30afeedf6463fbe7a2"))*/
        ;
    static const CCheckpointData data = {
        &mapCheckpoints,
        1375533383, // * UNIX timestamp of last checkpoint block
        21491097,   // * total number of transactions between genesis and last checkpoint
                    //   (the tx=... number in the SetBestChain debug.log lines)
        60000.0     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        ( 0, hashGenesisBlockTestNet )
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
        1338180505,
        16341,
        300
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        // const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;
        const MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL) {
            return 0.0;
        }
        else {
            return 0.0;
        }

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        // const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;
        const MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        // const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;
        const MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    uint256 GetLatestHardenedCheckpoint()
    {
        // const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;
        const MapCheckpoints& checkpoints = (fTestNet ? mapCheckpointsTestnet : mapCheckpoints);
        return (checkpoints.rbegin()->second);
    }
}
