//#include <string>
//#include <string_view>
//#include <vector>
//#include <random>
//#include <iostream>
//#include <chrono>
//
//// Your original functions
////int isWildcardMatch(std::string_view pattern, std::string_view str)
//bool IsWildcardMatch(const char* pat, const char* str)
//{
//    const char* locp = NULL;
//    const char* locs = NULL;
//    bool escape = false;
//
//    while (*str) {
//        escape = false;
//        /* we encounter a star */
//        if (*pat == '\\') {
//            escape = true;
//            pat++;
//        }
//        if (!escape && *pat == '*') {
//            locp = ++pat;
//            locs = str;
//            if (*pat == '\0') {
//                return true;
//            }
//            continue;
//        }
//        /* we have a mismatch */
//        if (*str != *pat && (escape || *pat != '?')) {
//            if (!locp) {
//                return false;
//            }
//            str = ++locs;
//            pat = locp;
//            continue;
//        }
//        pat++, str++;
//    }
//    /* check if the pattern's ended */
//    while (*pat == '*') {
//        pat++;
//    }
//    return (*pat == '\0');
//}
//
//int wild_match_iter(const char* pat,const char* str)
//{
//    const char* locp = NULL;
//    const char* locs = NULL;
//
//    while (*str) {
//        /* we encounter a star */
//        if (*pat == '*') {
//            locp = ++pat;
//            locs = str;
//            if (*pat == '\0') {
//                return 1;
//            }
//            continue;
//        }
//        /* we have a mismatch */
//        if (*str != *pat && *pat != '?') {
//            if (!locp) {
//                return 0;
//            }
//            str = ++locs;
//            pat = locp;
//            continue;
//        }
//        pat++, str++;
//    }
//    /* check if the pattern's ended */
//    while (*pat == '*') {
//        pat++;
//    }
//    return (*pat == '\0');
//}
//
//#define REPEAT	100000
//
//#define NPAT	87
//#define NSTR	95
//
//const char* pat[NPAT];
//const char* str[NSTR];
//
//void init_test()
//{
//    /* Patterns */
//    pat[0] = "*ccd";
//    pat[1] = "*issip*ss*";
//    pat[2] = "xxxx*zzy*fffff";
//    pat[3] = "xxx*zzy*f";
//    pat[4] = "xxxx*zzy*f";
//    pat[5] = "xy*z*xyz";
//    pat[6] = "*sip*";
//    pat[7] = "xy*xyz";
//    pat[8] = "mi*sip*";
//    pat[9] = "*abac*";
//    pat[10] = "a*zz*";
//    pat[11] = "*12*23";
//    pat[12] = "a12b";
//    pat[13] = "*12*12*";
//    pat[14] = "*";
//    pat[15] = "a*b";
//    pat[16] = "a*";
//    pat[17] = "a*aar";
//    pat[18] = "XY*Z*XYz";
//    pat[19] = "*SIP*";
//    pat[20] = "*issip*PI";
//    pat[21] = "mi*Sip*";
//    pat[22] = "*Abac*";
//    pat[23] = "*oWn*";
//    pat[24] = "bLah";
//    pat[25] = "bLaH";
//    pat[26] = "*?";
//    pat[27] = "??";
//    pat[28] = "?*?";
//    pat[29] = "*?*?*";
//    pat[30] = "?**?*?";
//    pat[31] = "?**?*&?";
//    pat[32] = "?b*??";
//    pat[33] = "?a*??";
//    pat[34] = "?**?c?";
//    pat[35] = "?**?d?";
//    pat[36] = "?*b*?*d*?";
//    pat[37] = "bL?h";
//    pat[38] = "bLa?";
//    pat[39] = "?Lah";
//    pat[40] = "?LaH";
//    pat[41] = "a*a*a*a*a*a*aa*aaa*a*a*b";
//    pat[42] = "*a*b*ba*ca*a*aa*aaa*fa*ga*b*";
//    pat[43] = "*a*b*ba*ca*a*x*aaa*fa*ga*b*";
//    pat[44] = "*a*b*ba*ca*aaaa*fa*ga*gggg*b*";
//    pat[45] = "*a*b*ba*ca*aaaa*fa*ga*ggg*b*";
//    pat[46] = "*aabbaa*a*";
//    pat[47] = "a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*";
//    pat[48] = "*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*";
//    pat[49] = "abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*"
//        "abc*abc*abc*abc*abc*";
//    pat[50] = "abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*";
//    pat[51] = "abc*abc*abc*abc*abc";
//    pat[52] = "abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abcd";
//    pat[53] = "********a********b********c********";
//    pat[54] = "abc";
//    pat[55] = "********a********b********b********";
//    pat[56] = "***a*b*c***";
//    pat[57] = "?*.?*";
//    pat[58] = "*.zip";
//    pat[59] = "*zi?";
//    pat[60] = "a?";
//    pat[61] = "ab*?*xy";
//    pat[62] = "*zip";
//    pat[63] = "a*?*?*.zip";
//    pat[64] = "*?*?.zip";
//    pat[65] = "?a?";
//    pat[66] = "*?.zip";
//    pat[67] = "*?*?*.zip";
//    pat[68] = "*zi*";
//    pat[69] = "?aa*";
//    pat[70] = "*aa*";
//    pat[71] = "?*";
//    pat[72] = "?*?.zip";
//    pat[73] = "?.";
//    pat[74] = "*.zi*";
//    pat[75] = "?.zip";
//    pat[76] = ".?";
//    pat[77] = "a*?*?";
//    pat[78] = "?.?";
//    pat[79] = "?a";
//    pat[80] = "ab*cd*xy";
//    pat[81] = "*aa?";
//    pat[82] = "?";
//    pat[83] = "*.zi?";
//    pat[84] = "*.*";
//    pat[85] = "*ab*cd*";
//    pat[86] = "Дело * чиновника мэрии ?? драке ??а Арбате "
//        "переквали??ицировали";
//
//    /* Strings */
//    str[0] = "abcccd";
//    str[1] = "mississipissippi";
//    str[2] = "xxxx*zzzzzzzzy*f";
//    str[3] = "xxxxzzzzzzzzyf";
//    str[4] = "xyxyxyzyxyz";
//    str[5] = "mississippi";
//    str[6] = "xyxyxyxyz";
//    str[7] = "ababac";
//    str[8] = "aaazz";
//    str[9] = "a12b12";
//    str[10] = "*";
//    str[11] = "a*abab";
//    str[12] = "a*r";
//    str[13] = "a*ar";
//    str[14] = "XYXYXYZYXYz";
//    str[15] = "missisSIPpi";
//    str[16] = "mississipPI";
//    str[17] = "miSsissippi";
//    str[18] = "abAbac";
//    str[19] = "aAazz";
//    str[20] = "A12b12";
//    str[21] = "a12B12";
//    str[22] = "oWn";
//    str[23] = "bLah";
//    str[24] = "a";
//    str[25] = "ab";
//    str[26] = "abc";
//    str[27] = "abcd";
//    str[28] = "abcde";
//    str[29] = "bLaaa";
//    str[30] = "bLaH";
//    str[31] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
//        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab";
//    str[32] = "abababababababababababababababababababaacacacacacacacada"
//        "eafagahaiajakalaaaaaaaaaaaaaaaaaffafagaagggagaaaaaaaab";
//    str[33] = "aaabbaabbaab";
//    str[34] = "a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*";
//    str[35] = "aaaaaaaaaaaaaaaaa";
//    str[36] = "aaaaaaaaaaaaaaaa";
//    str[37] = "abc*abcd*abcde*abcdef*abcdefg*abcdefgh*abcdefghi*abcdefghij*"
//        "abcdefghijk*abcdefghijkl*abcdefghijklm*abcdefghijklmn";
//    str[38] = "abc*abcd*abcd*abc*abcd";
//    str[39] = "abc*abcd*abcd*abc*abcd*abcd*abc*abcd*abc*abc*abcd";
//    str[40] = "********a********b********c********";
//    str[41] = "*abc*";
//    str[42] = ".a.b";
//    str[43] = ".a.bcd";
//    str[44] = ".ab";
//    str[45] = "a.b";
//    str[46] = "a.bcd";
//    str[47] = "ab.";
//    str[48] = "xyz.bcd";
//    str[49] = ".zip";
//    str[50] = "a.a.zip";
//    str[51] = "a.a.zippo";
//    str[52] = "a.zip";
//    str[53] = "aaaa.zip";
//    str[54] = "zip";
//    str[55] = ".a";
//    str[56] = "a.";
//    str[57] = "a.a";
//    str[58] = "aa";
//    str[59] = "aaa";
//    str[60] = ".a.a";
//    str[61] = ".a.aa";
//    str[62] = "aa.";
//    str[63] = "aa.a";
//    str[64] = "aa.ba.ba";
//    str[65] = "aannn";
//    str[66] = "xaab";
//    str[67] = ".aa.";
//    str[68] = "baa.";
//    str[69] = "caa.ba.ba";
//    str[70] = ".ab.ab.ab.cd.cd.";
//    str[71] = ".ab.cd.ab.cd.abcd.";
//    str[72] = ".axb.cxd.ab.cd.abcd.";
//    str[73] = ".axb.cyd.ab.cyd.axbcd.";
//    str[74] = "abanabnabncd";
//    str[75] = "abanabnabncdef";
//    str[76] = "xa";
//    str[77] = "xxa";
//    str[78] = ".a.";
//    str[79] = "xab";
//    str[80] = "xxab";
//    str[81] = "ax";
//    str[82] = ".axb.cxd.ab.cd.abcd.xy";
//    str[83] = "a.ab.ab.ab.cd.cd.xy";
//    str[84] = "ab.ab.cd.ab.cd.abcdxy.";
//    str[85] = "ab.axb.cd.xyab.cyd.axbcd.";
//    str[86] = "ab.axb.cd.xyab.cyd.axbcd.xy";
//    str[87] = "ab.xy";
//    str[88] = "abancda.bnxyabncdefxy";
//    str[89] = "abancdabnxyabncdef";
//    str[90] = "abancdabnxyabncdefxy";
//    str[91] = "abcdx_y";
//    str[92] = "abxy";
//    str[93] = "xab_anabnabncd_xy";
//    str[94] = "Дело бывшего чиновника мэрии о драке "
//        "на Арбате переквалифицировали";
//}
//
//struct TestCase {
//    std::string pattern;
//    std::string str;
//    bool expected;
//    std::string description;
//};
//
//class TestSuite {
//private:
//    std::vector<TestCase> test_cases;
//    double total_time1 = 0.0;
//    double total_time2 = 0.0;
//
//public:
//    void initialize_test_cases() {
//        test_cases = {
//            // Basic wildcard tests
//            {"hello", "hello", true, "Exact match"},
//            {"hello", "hell", false, "Partial match should fail"},
//            {"hello", "hello!", false, "Extra characters should fail"},
//
//            // Single character wildcard (?)
//            {"h?llo", "hello", true, "Single char wildcard middle"},
//            {"h?llo", "hallo", true, "Single char wildcard different char"},
//            {"h?llo", "hxllo", true, "Single char wildcard any char"},
//            {"h?llo", "hllo", false, "Single char wildcard missing char"},
//            {"h?llo", "helloo", false, "Single char wildcard extra char"},
//
//            // Multiple single character wildcards
//            {"h??lo", "hello", true, "Multiple single char wildcards"},
//            {"h??lo", "hexlo", true, "Multiple single char wildcards different"},
//            {"?e?lo", "hello", true, "Multiple single char wildcards start"},
//
//            // Star wildcard (*) - zero or more characters
//            {"h*lo", "hello", true, "Star wildcard middle"},
//            {"h*lo", "hlo", true, "Star wildcard zero chars"},
//            {"h*lo", "heeeeeello", true, "Star wildcard multiple chars"},
//            {"h*lo", "hxxxyyyzzzlo", true, "Star wildcard various chars"},
//            {"*hello", "hello", true, "Star wildcard beginning"},
//            {"hello*", "hello", true, "Star wildcard end"},
//            {"hello*", "helloworld", true, "Star wildcard end with chars"},
//            {"*world", "world", true, "Star wildcard beginning match"},
//            {"*world", "helloworld", true, "Star wildcard beginning with chars"},
//            {"he*o", "hello", true, "Star wildcard simple"},
//
//            // Multiple star wildcards
//            {"h*l*o", "hello", true, "Multiple stars match"},
//            {"h*l*o", "hlo", true, "Multiple stars zero chars"},
//            {"h*l*o", "heellloo", true, "Multiple stars multiple chars"},
//            {"*a*b*", "axb", true, "Multiple stars with chars"},
//            {"*a*b*", "ab", true, "Multiple stars minimal"},
//            {"*a*b*", "xayb", true, "Multiple stars separated"},
//
//            // Mixed wildcards
//            {"h*ll?", "hello", true, "Mixed star and question"},
//            {"h*ll?", "hellx", true, "Mixed wildcards end match"},
//            {"h*ll?", "hllx", true, "Mixed wildcards zero star"},
//            {"h*ll?", "heellx", true, "Mixed wildcards multiple star"},
//            {"?e*l*o", "hello", true, "Complex mixed wildcards"},
//            {"?e*l*o", "xellllo", true, "Complex mixed wildcards long"},
//
//            // Edge cases
//            {"", "", true, "Both empty"},
//            {"*", "", true, "Star matches empty"},
//            {"*", "a", true, "Star matches single char"},
//            {"*", "abcdefg", true, "Star matches multiple chars"},
//            {"?", "", false, "Question fails empty"},
//            {"?", "a", true, "Question matches single char"},
//            {"?", "ab", false, "Question fails multiple chars"},
//
//            // Complex patterns
//            {"*.txt", "file.txt", true, "File extension match"},
//            {"*.txt", "document.txt", true, "File extension different name"},
//            {"*.txt", "file.dat", false, "File extension mismatch"},
//            {"file*.txt", "file123.txt", true, "Prefix star suffix"},
//            {"file*.txt", "file.txt", true, "Prefix star zero chars"},
//            {"file*.txt", "file_backup.txt", true, "Prefix star with chars"},
//            {"image?.jpg", "image1.jpg", true, "Question file pattern"},
//            {"image?.jpg", "imageA.jpg", true, "Question file pattern char"},
//            {"image?.jpg", "image10.jpg", false, "Question file pattern too long"},
//
//            // Stress tests
//            {"a*b*c*d*e*f", "abcdef", true, "Multiple stars consecutive"},
//            {"a*b*c*d*e*f", "axbxcxdxexf", true, "Multiple stars separated"},
//            {"a*b*c*d*e*f", "aabbccddeeff", true, "Multiple stars groups"},
//            {"*a*a*a*a*", "aaaa", true, "Repeated stars same char"},
//            {"*a*a*a*a*", "xaxaxaxa", true, "Repeated stars separated"},
//
//            // Potential failure cases
//            {"h*llo", "hllo", true, "Star allows zero chars"},
//            {"h*llo", "hello", true, "Star allows multiple chars"},
//            {"*test*", "this is a test string", true, "Star contains match"},
//            {"*test*", "testing", true, "Star prefix match"},
//            {"*test*", "no match here", false, "Star no match"},
//
//            // Special cases
//            {"**", "anything", true, "Double star"},
//            {"***", "test", true, "Triple star"},
//            {"?*", "a", true, "Question star combination"},
//            {"*?", "a", true, "Star question combination"},
//            {"**a", "a", true, "Double star match end"},
//            {"**a", "ba", true, "Double star match middle"},
//            {"**a", "bbba", true, "Double star match long"},
//            {"a**b", "ab", true, "Double star minimal"},
//            {"a**b", "axb", true, "Double star one char"},
//            {"a**b", "axxxb", true, "Double star multiple chars"}
//        };
//    }
//
//    void run_comprehensive_tests() {
//        std::cout << "Running Comprehensive Test Suite...\n";
//        std::cout << "========================================\n";
//
//        int passed1 = 0, passed2 = 0;
//        int total = test_cases.size();
//
//        auto suite_start = std::chrono::high_resolution_clock::now();
//
//        // Test isWildcardMatch
//        auto start1 = std::chrono::high_resolution_clock::now();
//        for (size_t i = 0; i < REPEAT; i++)
//        {
//            for (const auto& test : test_cases) {
//                bool result = IsWildcardMatch(test.pattern.c_str(), test.str.c_str());
//                if (result == test.expected) {
//                    passed1++;
//                }
//            }
//        }
//        auto end1 = std::chrono::high_resolution_clock::now();
//        total_time1 = std::chrono::duration<double>(end1 - start1).count();
//
//        // Test wild_match_iter
//        auto start2 = std::chrono::high_resolution_clock::now();
//        for (size_t i = 0; i < REPEAT; i++)
//        {
//            for (const auto& test : test_cases) {
//                bool result = wild_match_iter(test.pattern.c_str(), test.str.c_str());
//                if (result == test.expected) {
//                    passed2++;
//                }
//            }
//        }
//        auto end2 = std::chrono::high_resolution_clock::now();
//        total_time2 = std::chrono::duration<double>(end2 - start2).count();
//
//        auto suite_end = std::chrono::high_resolution_clock::now();
//        double total_suite_time = std::chrono::duration<double>(suite_end - suite_start).count();
//
//        // Print results
//        std::cout << "isWildcardMatch: " << passed1 << "/" << total << " passed ("
//            << (passed1 * 100.0 / total) << "%) - Time: " << total_time1 << "s\n";
//        std::cout << "wild_match_iter: " << passed2 << "/" << total << " passed ("
//            << (passed2 * 100.0 / total) << "%) - Time: " << total_time2 << "s\n";
//        std::cout << "Total test suite time: " << total_suite_time << "s\n";
//        std::cout << "========================================\n";
//    }
//
//    void print_performance_summary() {
//        std::cout << "\nPerformance Summary\n";
//        std::cout << "========================================\n";
//        std::cout << "isWildcardMatch: " << total_time1 << " seconds\n";
//        std::cout << "wild_match_iter: " << total_time2 << " seconds\n";
//
//        if (total_time1 > 0 && total_time2 > 0) {
//            double ratio = total_time1 / total_time2;
//            std::cout << "Performance ratio: " << ratio << "x\n";
//            if (ratio > 1.1) {
//                std::cout << "wild_match_iter is " << ratio << "x faster\n";
//            }
//            else if (ratio < 0.9) {
//                std::cout << "isWildcardMatch is " << (1.0 / ratio) << "x faster\n";
//            }
//            else {
//                std::cout << "Both implementations have similar performance\n";
//            }
//        }
//    }
//};
//
//void run_benchmark() {
//    // Your existing benchmark code here
//    std::cout << "\nRunning Original Benchmark...\n";
//    std::cout << "========================================\n";
//
//    // Add your original benchmark code here if needed
//}
//
int main() {
    //std::cout << "Wildcard Matching Comprehensive Test Suite\n";
    //std::cout << "==========================================\n\n";

    //TestSuite test_suite;
    //test_suite.initialize_test_cases();

    //// Run the main test suite with timing
    //test_suite.run_comprehensive_tests();

    //// Show performance summary
    //test_suite.print_performance_summary();

    //// Run original benchmark if needed
    //// run_benchmark();

    return 0;
}