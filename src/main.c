/*
 *  main.c
 *  base64encoder
 *
 *  Created by Gandalf Sievers on 26.12.12.
 *  Copyright (c) 2012 Gandalf Sievers.
 *
 * MIT-License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "argsparser.h"
#include "iohelper.h"
#include "base64.h"

#define MAX_FILEBUFFER_SIZE 100000

#ifdef _M_X64
#define VERSION "1.0 x86_64 Win"
#elif __x86_64__
#define VERSION "1.0 x86_64"
#else
#define VERSION "1.0"
#endif

void usage(const char* exec)
{
    printf("Usage %s [options] input\n", exec);
    printf("-h\t--help\t\tprint this help\n");
    printf("-v\t--version\tprint version info\n");
    printf("-d\t--decode\tdecode base64 input\n");
    printf("-o\t--output\twrite output to file (instead of stdout)\n");
    printf("-s\t--stats\t\tprint stats (if output present)\n");

}

int main(int argc, const char* argv[])
{
    int ulen = 0, error = 0;
    char input = 0;
    char* output = NULL;
    char* string = NULL;
    size_t len = 0;

    struct args options[] =
    {
        {'v', "version", 0, 0, NULL},
        {'h', "help", 0, 0, NULL },
        {'d', "decode", 0, 0, NULL},
        {'s', "stats", 0, 0, NULL},
        {'o', "output", 1, 0, NULL },
        {0, NULL, 0, 0, NULL}
    };

    clock_t start = clock();

    char** unordered = argsparser(options, &ulen, &error, argc, argv);

    if(options[0].found)
    {
        printf("%s\n", VERSION);
        exit(EXIT_SUCCESS);
    }

    if(options[1].found)
    {
        usage(argv[0]);
        exit(EXIT_SUCCESS);
    }

    /* Unordered Options */
    if (ulen > 0)
    {
        int i = 0;
        for(; i < ulen; i++)
        {
            if(!getStringFromFile(unordered[i], &string, &len))
            {
                exit(EXIT_FAILURE);
            }
        }
        input = 1;
    }

    freeCharList(unordered);

    if(input == 0)
    {
        getStringFromStdin(&string, &len);
    }

    if(string != NULL)
    {
        size_t olen = 0;
        char o = 0;

        if(options[2].found)
        {
            output = decodeBase64(string, len, &olen);
        }
        else
        {
            output = encodeBase64(string, len);
        }

        free(string);

        o = writeOutput(options[4].value, output);

        free(output);

        if(!o)
        {
            fprintf(stderr, "Could write output file %s!\n", options[4].value);
            exit(EXIT_FAILURE);
        }
        if(options[4].found && options[3].found)
        {
            clock_t diff = clock() - start;
            int msec = (int)(diff * 1000 / CLOCKS_PER_SEC);
            printf("Time taken %d milliseconds %lu clocks\n", msec%1000, diff);
        }
    }

    return 0;
}
