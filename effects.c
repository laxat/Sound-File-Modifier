#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int LINE_LENGTH = 120;
char offset[10];
char bytes[48];
char askey[18];
char channel[5];
char data_size[9];
char rate[9];
int s_data; 
int s_rate; 
int s_channel; 

void reverse(char *str)
{
    if (strlen(str) == 4)
    {
        char s_temp[5];
        int end = strlen(str) - 1;
        for (int b = 0; b < strlen(str); b += 2)
        {
            s_temp[b] = str[end - 1];
            s_temp[b + 1] = str[end];
            end -= 2;
        }

        for (int i = 0; i < strlen(str); i++)
        {
            str[i] = s_temp[i];
        }
    }
    else if (strlen(str) == 8)
    {
        char s_temp[9];
        int end = strlen(str) - 1;
        for (int b = 0; b < strlen(str); b += 2)
        {
            s_temp[b] = str[end - 1];
            s_temp[b + 1] = str[end];
            end -= 2;
        }

        for (int i = 0; i < strlen(str); i++)
        {
            str[i] = s_temp[i];
        }
    }
}

void reset(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        str[i] = '\0';
        i++;
    }
}

int fade_in(int sect)
{

    int bit = 0;
    int num_c_samples;
    int samp_num;
    char stream[LINE_LENGTH];
    while (fgets(stream, LINE_LENGTH, stdin) != NULL)
    {

        strncpy(offset, &stream[0], 9);
        strncpy(bytes, &stream[strlen(offset) + 1], 47);
        strcpy(askey, &stream[strlen(bytes) + strlen(offset) + 2]);

        int i = 0;
        printf("%s", offset);
        while (i < strlen(bytes))
        { //head

            if (bit == 44)
            {
                reverse(rate);
                reverse(data_size);
                reverse(channel);

                s_data = (int)strtol(data_size, NULL, 16);
                s_channel = (int)strtol(channel, NULL, 16);
                

                if (s_channel == 1 || s_channel == 2){
                    s_rate = (int)strtol(rate, NULL, 16);
                    num_c_samples = ((double)sect / 1000.0) * s_rate;
                    samp_num = 0;
                }else{
                    printf("Error processing WAV file\n");  
                    exit(1);
                }

            }
            else if (bit > 39 && bit < 44)
            {
                strncat(data_size, &bytes[i], 2);
            }
            else if (bit > 21 && bit < 24)
            {
                strncat(channel, &bytes[i], 2);
            }
            else if (bit > 23 && bit < 28)
            {
                strncat(rate, &bytes[i], 2);
            }
            if (bit < 44)
            { // still at header

                printf(" %c%c", bytes[i], bytes[i + 1]);
            }
            else
            { // outside of the header
                if (s_channel == 1)
                {
                    char sample[5] = {};
                    strncat(sample, &bytes[i], 2);
                    strncat(sample, &bytes[i + 3], 2);
                    reverse(sample);
                    int s_sample = (short)strtol(sample, NULL, 16);
                    double changed = s_sample;

                    if (num_c_samples !=  0)
                    {
                        changed = ((double)s_sample * samp_num / (double)num_c_samples);
                    }
                    char n[5];
                    sprintf(n, "%.4hx", (short int)changed);
                    reverse(n);
                    n[4] = '\0';
                    if (bytes[i] != ' ')
                    {
                        printf(" %c%c %c%c", n[0], n[1], n[2], n[3]);
                    }
                    else
                    {
                        printf(" %c%c %c%c", bytes[i + 1], bytes[i + 2], bytes[i + 3], bytes[i + 4]);
                    }
                    i += 3;
                }
                else if (s_channel == 2)
                {
                    char left[5] = {};
                    strncat(left, &bytes[i], 2);
                    strncat(left, &bytes[i + 3], 2);

                    char right[5] = {};
                    strncat(right, &bytes[i + 6], 2);
                    strncat(right, &bytes[i + 9], 2);

                    reverse(left);
                    reverse(right);

                    int l_sample = (short)strtol(left, NULL, 16);
                    int r_sample = (short)strtol(right, NULL, 16);

                    double l_fadein = l_sample;
                    double r_fadein = r_sample;

                    if (num_c_samples !=  0)
                    {
                        l_fadein = ((double)l_sample * samp_num / (double)num_c_samples);
                        r_fadein = ((double)r_sample * samp_num / (double)num_c_samples);
                    }

                    char nl[5];
                    char nr[5];

                    sprintf(nl, "%.4hx", (short int)l_fadein);
                    reverse(nl);
                    nl[4] = '\0';

                    sprintf(nr, "%.4hx", (short int)r_fadein);
                    reverse(nr);
                    nr[4] = '\0';
                    if (bytes[i] != ' ')
                    {
                        printf(" %c%c %c%c", nl[0], nl[1], nl[2], nl[3]);
                        printf(" %c%c %c%c", nr[0], nr[1], nr[2], nr[3]);
                    }
                    else
                    {
                        printf(" %c%c %c%c", bytes[i + 1], bytes[i + 2], bytes[i + 3], bytes[i + 4]);
                        printf(" %c%c %c%c", bytes[i + 5], bytes[i + 6], bytes[i + 7], bytes[i + 8]);
                    }
                    i += 9;
                }

                if (samp_num < num_c_samples)
                {
                    samp_num++;
                }
            }
            i += 3;
            bit++;
        }
        printf("  %s", askey);
    }
    return 0;
}

int fade_out(int sect)
{
    int bit = 0;
    int samp_num, m_check, s_check;
    int num_c_samples;
    char stream[LINE_LENGTH];
    while (fgets(stream, LINE_LENGTH, stdin) != NULL)
    {
        strncpy(offset, &stream[0], 9);
        strncpy(bytes, &stream[strlen(offset) + 1], 47);
        strcpy(askey, &stream[strlen(bytes) + strlen(offset) + 2]);

        int i = 0;
        printf("%s", offset);
        while (i < strlen(bytes))
        { //head

            if (bit == 44)
            {
                reverse(rate);
                reverse(data_size);
                reverse(channel);

                s_data = (int)strtol(data_size, NULL, 16);
                s_channel = (int)strtol(channel, NULL, 16);
                if (s_channel == 1 || s_channel == 2){
                    s_rate = (int)strtol(rate, NULL, 16);
                    num_c_samples = ((double)sect / 1000.0) * s_rate;
                    samp_num = 0;
                }else{
                    printf("Error processing WAV file\n");  
                    exit(1);
                }

                m_check = s_data / 2;
                s_check = s_data / 4;

            }
            else if (bit > 39 && bit < 44)
            {
                strncat(data_size, &bytes[i], 2);
            }
            else if (bit > 21 && bit < 24)
            {
                strncat(channel, &bytes[i], 2);
            }
            else if (bit > 23 && bit < 28)
            {
                strncat(rate, &bytes[i], 2);
            }
            if (bit < 44)
            { // still at header

                printf(" %c%c", bytes[i], bytes[i + 1]);
            }
            else
            { // outside of the header
                if (s_channel == 1)
                {
                    char sample[5] = {};
                    strncat(sample, &bytes[i], 2);
                    strncat(sample, &bytes[i + 3], 2);
                    reverse(sample);
                    int s_sample = (short)strtol(sample, NULL, 16);

                    double changed = s_sample;
                    int current = (m_check - samp_num) - 1;

                    if (num_c_samples != 0)
                    {
                        if (current > 0 && current <= num_c_samples)
                        {
                            changed = ((double)s_sample * current / (double)num_c_samples);
                        }
                        else if (current <= num_c_samples)
                        {
                            changed = 0;
                        }
                    }
                    char n[5];
                    sprintf(n, "%.4hx", (short int)changed);
                    reverse(n);
                    n[4] = '\0';
                    if (bytes[i] != ' ')
                    {
                        printf(" %c%c %c%c", n[0], n[1], n[2], n[3]);
                    }
                    else
                    {
                        printf(" %c%c %c%c", bytes[i + 1], bytes[i + 2], bytes[i + 3], bytes[i + 4]);
                    }

                    if (samp_num < m_check)
                    {
                        samp_num++;
                    }
                    i += 3;

                    // STEREO
                }
                else if (s_channel == 2)
                {

                    char left[5] = {};
                    strncat(left, &bytes[i], 2);
                    strncat(left, &bytes[i + 3], 2);

                    char right[5] = {};
                    strncat(right, &bytes[i + 6], 2);
                    strncat(right, &bytes[i + 9], 2);

                    reverse(left);
                    reverse(right);

                    int l_sample = (short)strtol(left, NULL, 16);
                    int r_sample = (short)strtol(right, NULL, 16);

                    double l_fadein = l_sample, r_fadein = r_sample;
                    int current = (s_check - samp_num) - 1;

                    if (num_c_samples != 0)
                    {
                        if (current > 0 && current <= num_c_samples)
                        {
                            l_fadein = ((double)l_sample * current / (double)num_c_samples);
                            r_fadein = ((double)r_sample * current / (double)num_c_samples);
                        }
                        else if (current <= num_c_samples)
                        {
                            l_fadein = 0;
                            r_fadein = 0;
                        }
                    }

                    char nl[5];
                    char nr[5];

                    sprintf(nl, "%.4hx", (short int)l_fadein);
                    reverse(nl);
                    nl[4] = '\0';

                    sprintf(nr, "%.4hx", (short int)r_fadein);
                    reverse(nr);
                    nr[4] = '\0';
                    if (bytes[i] != ' ')
                    {
                        printf(" %c%c %c%c", nl[0], nl[1], nl[2], nl[3]);
                        printf(" %c%c %c%c", nr[0], nr[1], nr[2], nr[3]);
                    }
                    else
                    {
                        printf(" %c%c %c%c", bytes[i + 1], bytes[i + 2], bytes[i + 3], bytes[i + 4]);
                        printf(" %c%c %c%c", bytes[i + 5], bytes[i + 6], bytes[i + 7], bytes[i + 8]);
                    }
                    if (samp_num < s_check)
                    {
                        samp_num++;
                    }
                    i += 9;
                }
            }
            i += 3;
            bit++;
        }
        printf("  %s", askey);
    }
    return 0;
}

int pan(int sect)
{

    int bit = 0;
    int samp_num; 
    int num_c_samples;
    char stream[LINE_LENGTH];
    while (fgets(stream, LINE_LENGTH, stdin) != NULL)
    {
        strncpy(offset, &stream[0], 9);
        strncpy(bytes, &stream[strlen(offset) + 1], 47);
        strcpy(askey, &stream[strlen(bytes) + strlen(offset) + 2]);

        int i = 0;
        printf("%s", offset);
        while (i < strlen(bytes))
        { //head

            if (bit == 44)
            {
                reverse(rate);
                reverse(data_size);
                reverse(channel);

                s_data = (int)strtol(data_size, NULL, 16);
                s_channel = (int)strtol(channel, NULL, 16);
                if (s_channel == 1 || s_channel == 2){
                    s_rate = (int)strtol(rate, NULL, 16);
                    num_c_samples = ((double)sect / 1000.0) * s_rate;
                    samp_num = 0;
                }else{
                    printf("Error processing WAV file\n");  
                    exit(1);
                }
                
            }
            else if (bit > 39 && bit < 44)
            {
                strncat(data_size, &bytes[i], 2);
            }
            else if (bit > 21 && bit < 24)
            {
                strncat(channel, &bytes[i], 2);
            }
            else if (bit > 23 && bit < 28)
            {
                strncat(rate, &bytes[i], 2);
            }
            if (bit < 44)
            { // still at header

                printf(" %c%c", bytes[i], bytes[i + 1]);
            }
            else
            {
                if (s_channel == 1)
                {
                    printf("Error processing WAV file. \n");
                    exit(1);
                }
                if (s_channel == 2)
                {

                    char left[5] = {};
                    strncat(left, &bytes[i], 2);
                    strncat(left, &bytes[i + 3], 2);

                    char right[5] = {};
                    strncat(right, &bytes[i + 6], 2);
                    strncat(right, &bytes[i + 9], 2);

                    reverse(left);
                    reverse(right);

                    int l_sample = (short)strtol(left, NULL, 16);
                    int r_sample = (short)strtol(right, NULL, 16);

                    double l_pan = 0, r_pan = r_sample;
                    int current = (num_c_samples - samp_num) - 1;

                    if (num_c_samples !=  0)
                    {
                        if (current > 0)
                        {
                            l_pan = ((double)l_sample * current / (double)num_c_samples);
                        }

                        if (samp_num >= 0)
                        {
                            r_pan = ((double)r_sample * samp_num / (double)num_c_samples);
                        }
                    }

                    char nl[5];
                    char nr[5];

                    sprintf(nl, "%.4hx", (short int)l_pan);
                    reverse(nl);
                    nl[4] = '\0';

                    sprintf(nr, "%.4hx", (short int)r_pan);
                    reverse(nr);
                    nr[4] = '\0';
                    if (bytes[i] != ' ')
                    {
                        if (bit)
                            printf(" %c%c %c%c", nl[0], nl[1], nl[2], nl[3]);
                        printf(" %c%c %c%c", nr[0], nr[1], nr[2], nr[3]);
                    }
                    else
                    {
                        printf(" %c%c %c%c", bytes[i + 1], bytes[i + 2], bytes[i + 3], bytes[i + 4]);
                        printf(" %c%c %c%c", bytes[i + 5], bytes[i + 6], bytes[i + 7], bytes[i + 8]);
                    }
                    if (samp_num < num_c_samples)
                    {
                        samp_num++;
                    }
                    i += 9;
                }
            }
            i += 3;
            bit++;
        }
        printf("  %s", askey);
    }

    return 0;
}
int main(int argc, char *argv[3])
{
    int seconds;

    if (argc != 3)
        {
            printf("%s\n", "Error: Invalid command-line argumanets");
            exit(1);
        }

    if (strcmp(argv[1], "-fin") == 0)
    {
        seconds = strtol(argv[2], NULL, 10);
        fade_in(seconds);
    }
    else if (strcmp(argv[1], "-fout") == 0)
    {
        seconds = strtol(argv[2], NULL, 10);
        fade_out(seconds);
    }
    else if (strcmp(argv[1], "-pan") == 0)
    {
        seconds = strtol(argv[2], NULL, 10);
        pan(seconds);
    }
    else
    {
        if (strcmp(argv[1], "-pan") != 0 || strcmp(argv[1], "-fout") != 0 || strcmp(argv[1], "-fin") != 0)
        {
            printf("%s\n", "Error: Invalid command-line arguments.");
            exit(1);
        }
        
    }
    return 0;
}