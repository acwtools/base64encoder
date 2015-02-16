/*
 *  base64.c
 *  base64encoder
 *
 *  Created by gsievers on 26.12.12
 *  Copyright (c) 2012 Gandalf Sievers. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include "base64.h"

unsigned char encode64Char1(char ci1)
{
    unsigned char c1 = ci1 >> 2;
    c1 = c1 & 63;
    return c1;
}

unsigned char encode64Char2(char ci1, char ci2)
{
    unsigned char c2 = ci1 & 3;
    c2 = c2 << 4;
    unsigned char tmp2 = ci2 >> 4;
    tmp2 = tmp2  & 15;
    c2 = c2 | tmp2;
    c2= c2 & 63;
    return c2;
}

unsigned char encode64Char3(char ci2, char ci3)
{
    unsigned char c3 = ci2 & 15;
    c3 = c3 << 2;
    unsigned char tmp3 = ci3 >> 6;
    tmp3 = tmp3 & 3;
    c3 = c3 | tmp3;
    c3 = c3 & 63;
    return c3;
}

unsigned char encode64Char4(char ci3)
{
    unsigned char c4 = ci3 & 63;
    return c4;
}

char decode64Char1(char ci1, char ci2)
{
    return ((ci1 <<2 ) | ((ci2 >> 4) & 3));
}

char decode64Char2(char ci2, char ci3)
{
    return ((ci2 << 4) | ((ci3 >> 2) & 15));
}
char decode64Char3(char ci3, char ci4)
{
    return ((ci3 << 6) | ci4);
}

char* encodeBase64(const char* cstr, size_t len)
{
    int i,o;
    i=o=0;

    size_t outputLength = (len + 2 - ((len + 2) % 3)) / 3 * 4;

    char* output = (char*) calloc(sizeof(char), outputLength+1);
    memset(output, 0, outputLength+1);

    char base64Table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

    while(i<(len-3))
    {
        output[o]=base64Table[ encode64Char1( cstr[i])];
        output[o+1]=base64Table[ encode64Char2( cstr[i], cstr[i+1]) ];
        output[o+2]=base64Table[ encode64Char3( cstr[i+1], cstr[i+2]) ];
        output[o+3]=base64Table[ encode64Char4( cstr[i+2]) ];
        i+=3;
        o+=4;
    }

    if(i<len)
    {
        char ci2,ci3;

        if((i+1)<len)
        {
            ci2=cstr[i+1];
            if((i+2)<len)
            {
                ci3=cstr[i+2];
            }
            else
            {
                ci3=0;
            }
            output[o+2]=base64Table[encode64Char3(ci2, ci3)];
        }
        else
        {
            ci2=0;
            output[o+2]='=';
        }

        if((i+2)<len)
        {
            output[o+3]=base64Table[encode64Char4(cstr[i+2])];
        }
        else
        {
            output[o+3]='=';
        }

        output[o]=base64Table[encode64Char1(cstr[i])];
        output[o+1]=base64Table[encode64Char2(cstr[i], ci2)];
    }
    output[outputLength] = '\0';
    return output;
}

char ascii2b64(char c)
{
    if((c>= 65) && (c<=90))
    {
        c-=65;
    }
    else if((c>= 97) && (c<=122))
    {
        c-=71;
    }
    else if((c >= 48) && (c <=57))
    {
        c+=4;
    }
    else if(c==43)
    {
        c=62;
    }
    else if(c==47)
    {
        c=63;
    }

    return c;
}

char* decodeBase64(const char* cstr, size_t len, size_t* olen)
{
    int i,o;
    i=o=0;

    size_t outputLength = (len/4)*3;

    int padLen = 4;

    if(len>4)
    {
        padLen = (int)(len-(len-4));
    }
    if(cstr[len-1]=='=')
    {
        outputLength--;
        padLen--;
        if (cstr[len-2]=='=') {
            outputLength--;
            padLen--;
        }
    }

    char* output = (char*) calloc(sizeof(char), outputLength+1);
    memset(output, 0, outputLength+1);

    while(i<(len-4))
    {

        char c1,c2,c3,c4;
        c1 = ascii2b64(cstr[i]);
        c2 = ascii2b64(cstr[i+1]);
        c3 = ascii2b64(cstr[i+2]);
        c4 = ascii2b64(cstr[i+3]);

        output[o] = decode64Char1(c1, c2);
        output[o+1] = decode64Char2(c2, c3);
        output[o+2] = decode64Char3(c3, c4);
        o+=3;
        i+=4;
    }

    if(padLen >0)
    {
        char c1,c2;
        c1 = ascii2b64(cstr[i]);
        c2 = ascii2b64(cstr[i+1]);
        output[o] = decode64Char1(c1, c2);
        if(padLen>2)
        {
            char c3;
            c3 = ascii2b64(cstr[i+2]);
            output[o+1] = decode64Char2(c2, c3);
            if(padLen>3)
            {
                char c4;
                c4 = ascii2b64(cstr[i+3]);
                output[o+2] = decode64Char3(c3, c4);
            }
        }
    }
    *olen = outputLength+1;
    output[outputLength] = '\0';
    return output;
}
