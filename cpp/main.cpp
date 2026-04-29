#include "common.h"
#include "pdata.h"
#include "rnp_miner.h"
#include "rnp_b.h"
#include "rnp_d.h"
#include "rnp_pup.h"
#include "dfom_rnp.h"
#include "nosep_rnp.h"
#include "pro_rnp.h"
#include "prefixspan.h"

//ctrl+shift+b
//cd e:/vscodeG/RNPpytocpp/RNP-Miner-code/cpp
//>> ./rnp_miner_main.exe rnp-miner ../datasets/SDB1.txt 3000
//SDB1在支持度小于820时，候选暴增，在820时为12330个候选与735个频繁用时876ms
//

/*
./rnp_miner_main.exe rnp-miner ../datasets/SDB1.txt 810
NFP-Miner: ../datasets/SDB1.txt minsup=810:
Number of frequent patterns: 33772
Number of candidate patterns: 78434
Running time: 165947ms
*/

void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog << " <algorithm> <dataset> <minsup1> [minsup2 ...]" << std::endl;
    std::cerr << "Algorithms:" << std::endl;
    std::cerr << "  rnp-miner   RNP-Miner (position-based matching)" << std::endl;
    std::cerr << "  rnp-b       RNP-Baseline (iterative)" << std::endl;
    std::cerr << "  rnp-d       RNP-Depth-first (recursive)" << std::endl;
    std::cerr << "  rnp-pup     RNP-PUP (full alphabet scan)" << std::endl;
    std::cerr << "  dfom-rnp    DFOM-RNP (nettree-based matching)" << std::endl;
    std::cerr << "  nosep-rnp   NOSEP-RNP (explicit nettree)" << std::endl;
    std::cerr << "  pro-rnp     Pro-RNP (projection-based matching)" << std::endl;
    std::cerr << "  prefixspan  PrefixSpan (classic)" << std::endl;
    std::cerr << "  all         Run all Pdata-based algorithms" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    std::string algorithm = argv[1];
    std::string dataset = argv[2];

    // Algorithms that use Pdata
    if (algorithm == "rnp-miner" || algorithm == "rnp-b" || algorithm == "rnp-d" ||
        algorithm == "rnp-pup" || algorithm == "dfom-rnp" || algorithm == "pro-rnp" ||
        algorithm == "all") {

        ProcessingData pdata;
        PosDict S;
        auto [SeqNum, pos_dict, sort_item] = pdata.datap(dataset, S);

        for (int i = 3; i < argc; ++i) {
            int minsup = std::stoi(argv[i]);

            if (algorithm == "rnp-miner" || algorithm == "all") {
                std::cout << "NFP-Miner: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                RNPMiner miner(SeqNum, pos_dict, sort_item, minsup);
                miner.run();
                auto t2 = now_ms();
                std::cout << "Number of frequent patterns: " << miner.get_fp_count() << std::endl;
                std::cout << "Number of candidate patterns: " << miner.get_can_num() << std::endl;
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }

            if (algorithm == "rnp-b" || algorithm == "all") {
                std::cout << "NFP-B: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                RNPB rnpb(SeqNum, pos_dict, sort_item, minsup);
                rnpb.run();
                auto t2 = now_ms();
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }

            if (algorithm == "rnp-d" || algorithm == "all") {
                std::cout << "NFP-D: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                RNPD rnpd(SeqNum, pos_dict, sort_item, minsup);
                rnpd.run();
                auto t2 = now_ms();
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }

            if (algorithm == "rnp-pup" || algorithm == "all") {
                std::cout << "NFP-PUP: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                RNPPUP rnppup(SeqNum, pos_dict, sort_item, minsup);
                rnppup.run();
                auto t2 = now_ms();
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }

            if (algorithm == "dfom-rnp" || algorithm == "all") {
                std::cout << "DFOM-M: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                DFOMRNP dfom(SeqNum, pos_dict, sort_item, minsup);
                dfom.run();
                auto t2 = now_ms();
                std::cout << "Number of frequent patterns: " << dfom.get_fp_count() << std::endl;
                std::cout << "Number of candidate patterns: " << dfom.get_can_num() << std::endl;
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }

            if (algorithm == "pro-rnp" || algorithm == "all") {
                std::cout << "Pro-M: " << dataset << " minsup=" << minsup << ":" << std::endl;
                auto t1 = now_ms();
                ProRNP pro(SeqNum, pos_dict, sort_item, minsup);
                pro.run();
                auto t2 = now_ms();
                std::cout << "Number of frequent patterns: " << pro.get_fp_count() << std::endl;
                std::cout << "Number of candidate patterns: " << pro.get_can_num() << std::endl;
                std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
            }
        }
        return 0;
    }

    // Self-contained algorithms (no Pdata dependency)
    if (algorithm == "nosep-rnp") {
        for (int i = 3; i < argc; ++i) {
            int minsup = std::stoi(argv[i]);
            std::cout << "SNP-Miner: " << dataset << " minsup=" << minsup << ":" << std::endl;
            auto t1 = now_ms();
            NOSEPRNP nosep(dataset, minsup);
            nosep.run();
            auto t2 = now_ms();
            std::cout << "Number of frequent patterns: " << nosep.get_fp_count() << std::endl;
            std::cout << "Number of candidate patterns: " << nosep.get_can_num() << std::endl;
            std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
        }
        return 0;
    }

    if (algorithm == "prefixspan") {
        for (int i = 3; i < argc; ++i) {
            int minsup = std::stoi(argv[i]);
            std::cout << "PrefixSpan: " << dataset << " minsup=" << minsup << ":" << std::endl;
            auto t1 = now_ms();
            PrefixSpan ps(dataset, minsup);
            ps.run();
            auto t2 = now_ms();
            std::cout << "Running time: " << (t2 - t1) << "ms" << std::endl;
        }
        return 0;
    }

    print_usage(argv[0]);
    return 1;
}
