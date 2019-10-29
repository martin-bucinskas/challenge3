// This provides the frozen (compiled bytecode) files that are included if
// any.
#include <Python.h>

#include "nuitka/constants_blob.h"

// Blob from which modules are unstreamed.
#define stream_data constant_bin

// These modules should be loaded as bytecode. They may e.g. have to be loadable
// during "Py_Initialize" already, or for irrelevance, they are only included
// in this un-optimized form. These are not compiled by Nuitka, and therefore
// are not accelerated at all, merely bundled with the binary or module, so
// that CPython library can start out finding them.

struct frozen_desc {
    char const *name;
    ssize_t start;
    int size;
};

void copyFrozenModulesTo( struct _frozen *destination )
{
    struct frozen_desc frozen_modules[] = {
        { "_collections_abc", 6110643, 28953 },
        { "_compression", 6139596, 4135 },
        { "_weakrefset", 6143731, 7473 },
        { "abc", 6151204, 6462 },
        { "base64", 6157666, 16999 },
        { "bz2", 6174665, 11192 },
        { "codecs", 6185857, 34086 },
        { "collections", 6219943, -47087 },
        { "collections.abc", 6110643, 28953 },
        { "copyreg", 6267030, 4255 },
        { "dis", 6271285, 15216 },
        { "encodings", 6286501, -3958 },
        { "encodings.aliases", 6290459, 6307 },
        { "encodings.ascii", 6296766, 1895 },
        { "encodings.base64_codec", 6298661, 2434 },
        { "encodings.big5", 6301095, 1455 },
        { "encodings.big5hkscs", 6302550, 1465 },
        { "encodings.bz2_codec", 6304015, 3296 },
        { "encodings.charmap", 6307311, 2948 },
        { "encodings.cp037", 6310259, 2440 },
        { "encodings.cp1006", 6312699, 2516 },
        { "encodings.cp1026", 6315215, 2444 },
        { "encodings.cp1125", 6317659, 8137 },
        { "encodings.cp1140", 6325796, 2430 },
        { "encodings.cp1250", 6328226, 2467 },
        { "encodings.cp1251", 6330693, 2464 },
        { "encodings.cp1252", 6333157, 2467 },
        { "encodings.cp1253", 6335624, 2480 },
        { "encodings.cp1254", 6338104, 2469 },
        { "encodings.cp1255", 6340573, 2488 },
        { "encodings.cp1256", 6343061, 2466 },
        { "encodings.cp1257", 6345527, 2474 },
        { "encodings.cp1258", 6348001, 2472 },
        { "encodings.cp273", 6350473, 2426 },
        { "encodings.cp424", 6352899, 2470 },
        { "encodings.cp437", 6355369, 7854 },
        { "encodings.cp500", 6363223, 2440 },
        { "encodings.cp720", 6365663, 2537 },
        { "encodings.cp737", 6368200, 8176 },
        { "encodings.cp775", 6376376, 7884 },
        { "encodings.cp850", 6384260, 7515 },
        { "encodings.cp852", 6391775, 7892 },
        { "encodings.cp855", 6399667, 8145 },
        { "encodings.cp856", 6407812, 2502 },
        { "encodings.cp857", 6410314, 7497 },
        { "encodings.cp858", 6417811, 7485 },
        { "encodings.cp860", 6425296, 7833 },
        { "encodings.cp861", 6433129, 7848 },
        { "encodings.cp862", 6440977, 8037 },
        { "encodings.cp863", 6449014, 7848 },
        { "encodings.cp864", 6456862, 7994 },
        { "encodings.cp865", 6464856, 7848 },
        { "encodings.cp866", 6472704, 8181 },
        { "encodings.cp869", 6480885, 7874 },
        { "encodings.cp874", 6488759, 2568 },
        { "encodings.cp875", 6491327, 2437 },
        { "encodings.cp932", 6493764, 1457 },
        { "encodings.cp949", 6495221, 1457 },
        { "encodings.cp950", 6496678, 1457 },
        { "encodings.euc_jis_2004", 6498135, 1471 },
        { "encodings.euc_jisx0213", 6499606, 1471 },
        { "encodings.euc_jp", 6501077, 1459 },
        { "encodings.euc_kr", 6502536, 1459 },
        { "encodings.gb18030", 6503995, 1461 },
        { "encodings.gb2312", 6505456, 1459 },
        { "encodings.gbk", 6506915, 1453 },
        { "encodings.hex_codec", 6508368, 2421 },
        { "encodings.hp_roman8", 6510789, 2641 },
        { "encodings.hz", 6513430, 1451 },
        { "encodings.idna", 6514881, 5735 },
        { "encodings.iso2022_jp", 6520616, 1472 },
        { "encodings.iso2022_jp_1", 6522088, 1476 },
        { "encodings.iso2022_jp_2", 6523564, 1476 },
        { "encodings.iso2022_jp_2004", 6525040, 1482 },
        { "encodings.iso2022_jp_3", 6526522, 1476 },
        { "encodings.iso2022_jp_ext", 6527998, 1480 },
        { "encodings.iso2022_kr", 6529478, 1472 },
        { "encodings.iso8859_1", 6530950, 2439 },
        { "encodings.iso8859_10", 6533389, 2444 },
        { "encodings.iso8859_11", 6535833, 2538 },
        { "encodings.iso8859_13", 6538371, 2447 },
        { "encodings.iso8859_14", 6540818, 2465 },
        { "encodings.iso8859_15", 6543283, 2444 },
        { "encodings.iso8859_16", 6545727, 2446 },
        { "encodings.iso8859_2", 6548173, 2439 },
        { "encodings.iso8859_3", 6550612, 2446 },
        { "encodings.iso8859_4", 6553058, 2439 },
        { "encodings.iso8859_5", 6555497, 2440 },
        { "encodings.iso8859_6", 6557937, 2484 },
        { "encodings.iso8859_7", 6560421, 2447 },
        { "encodings.iso8859_8", 6562868, 2478 },
        { "encodings.iso8859_9", 6565346, 2439 },
        { "encodings.johab", 6567785, 1457 },
        { "encodings.koi8_r", 6569242, 2491 },
        { "encodings.koi8_t", 6571733, 2402 },
        { "encodings.koi8_u", 6574135, 2477 },
        { "encodings.kz1048", 6576612, 2454 },
        { "encodings.latin_1", 6579066, 1907 },
        { "encodings.mac_arabic", 6580973, 7748 },
        { "encodings.mac_centeuro", 6588721, 2478 },
        { "encodings.mac_croatian", 6591199, 2486 },
        { "encodings.mac_cyrillic", 6593685, 2476 },
        { "encodings.mac_farsi", 6596161, 2420 },
        { "encodings.mac_greek", 6598581, 2460 },
        { "encodings.mac_iceland", 6601041, 2479 },
        { "encodings.mac_latin2", 6603520, 2620 },
        { "encodings.mac_roman", 6606140, 2477 },
        { "encodings.mac_romanian", 6608617, 2487 },
        { "encodings.mac_turkish", 6611104, 2480 },
        { "encodings.palmos", 6613584, 2467 },
        { "encodings.ptcp154", 6616051, 2561 },
        { "encodings.punycode", 6618612, 6428 },
        { "encodings.quopri_codec", 6625040, 2454 },
        { "encodings.raw_unicode_escape", 6627494, 1780 },
        { "encodings.rot_13", 6629274, 3040 },
        { "encodings.shift_jis", 6632314, 1465 },
        { "encodings.shift_jis_2004", 6633779, 1475 },
        { "encodings.shift_jisx0213", 6635254, 1475 },
        { "encodings.tis_620", 6636729, 2529 },
        { "encodings.undefined", 6639258, 2174 },
        { "encodings.unicode_escape", 6641432, 1760 },
        { "encodings.unicode_internal", 6643192, 1770 },
        { "encodings.utf_16", 6644962, 4844 },
        { "encodings.utf_16_be", 6649806, 1645 },
        { "encodings.utf_16_le", 6651451, 1645 },
        { "encodings.utf_32", 6653096, 4737 },
        { "encodings.utf_32_be", 6657833, 1538 },
        { "encodings.utf_32_le", 6659371, 1538 },
        { "encodings.utf_7", 6660909, 1566 },
        { "encodings.utf_8", 6662475, 1625 },
        { "encodings.utf_8_sig", 6664100, 4527 },
        { "encodings.uu_codec", 6668627, 3236 },
        { "encodings.zlib_codec", 6671863, 3134 },
        { "enum", 6674997, 24282 },
        { "functools", 6699279, 24226 },
        { "genericpath", 6723505, 3759 },
        { "heapq", 6727264, 14373 },
        { "importlib", 6741637, -3743 },
        { "importlib._bootstrap", 6745380, 29189 },
        { "importlib._bootstrap_external", 6774569, 41829 },
        { "importlib.machinery", 6816398, 983 },
        { "inspect", 6817381, 80048 },
        { "io", 6897429, 3420 },
        { "keyword", 6900849, 1820 },
        { "linecache", 6902669, 3800 },
        { "locale", 6906469, 34566 },
        { "opcode", 6941035, 5389 },
        { "operator", 6946424, 13911 },
        { "os", 6960335, 29704 },
        { "posixpath", 6990039, 10440 },
        { "quopri", 7000479, 5782 },
        { "re", 7006261, 13815 },
        { "reprlib", 7020076, 5361 },
        { "sre_compile", 7025437, 15214 },
        { "sre_constants", 7040651, 6302 },
        { "sre_parse", 7046953, 21368 },
        { "stat", 7068321, 3884 },
        { "stringprep", 7072205, 10043 },
        { "struct", 7082248, 345 },
        { "threading", 7082593, 37913 },
        { "token", 7120506, 3610 },
        { "tokenize", 7124116, 17842 },
        { "traceback", 7141958, 19634 },
        { "types", 7161592, 8987 },
        { "warnings", 7170579, 13951 },
        { NULL, 0, 0 }
    };

    struct frozen_desc *current = frozen_modules;

    for(;;)
    {
        destination->name = (char *)current->name;
        destination->code = (unsigned char *)&constant_bin[ current->start ];
        destination->size = current->size;

        if (destination->name == NULL) break;

        current += 1;
        destination += 1;
    };
}
