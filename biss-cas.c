/*
 * Copyright (C) 2013-2014, Andrey Dyldin <and@cesbo.com>
 */

#include "MAG-CAS-plugin.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


// #define DEBUG(msg...) printf(msg)
#define DEBUG(msg...) (void)0


static STB_MAG_Cad_t cad_interface = { NULL, NULL };
static char * custom_key_file = NULL;


static uint8_t single_char_to_hex(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    else if(c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if(c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0;
}


static uint8_t char_to_hex(const char *c)
{
    return (single_char_to_hex(c[0]) << 4) | single_char_to_hex(c[1]);
}


static uint8_t * str_to_hex(const char *str, uint8_t *data, size_t size)
{
    if(!size)
        size = ~0;

    for(int i = 0; str[0] && str[1] && i < size; str += 2, ++i)
        data[i] = char_to_hex(str);

    return data;
}


/* CAS */


static int CAS_Decrypt(uint8_t *in_buffer, int length)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    return length;
}


static void CAS_Deinit(void)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    if(custom_key_file)
        free(custom_key_file);
}


static void CAS_ResetStream(void)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    const char *key_file = "/tmp/key";
    if(custom_key_file)
        key_file = custom_key_file;

    int key_fd = 0;
    key_fd = open(key_file, O_RDONLY);

    if(key_fd > 0)
    {
        static char value[32];
        const uint32_t value_len = read(key_fd, value, sizeof(value));
        value[value_len] = 0;
        close(key_fd);

        static uint8_t key[16];
        str_to_hex(value, key, sizeof(key));

        if(cad_interface.SetScramblingKey && cad_interface.SetScramblingType)
        {
            DEBUG("Set CAS Key: %s\n", value);

            cad_interface.SetScramblingType(CAS_SCRAMBLING_TYPE_CSA);
            cad_interface.SetScramblingKey(key, 0);
            cad_interface.SetScramblingKey(key, 1);

            return;
        }
        else
        {
            DEBUG("Failed to set key: %s\n", key);
        }
    }
    else
    {
        DEBUG("Failed to read key file: %s\n", strerror(errno));
    }
}


static CAS_Flags_e CAS_GetCasFlags(void)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    return CAS_Flags_PMT_ECM;
}


static void CAS_SetAdditionalParam(const char* name, const char* value)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    if(!strcasecmp(name, "key_file"))
    {
        if(custom_key_file)
            free(custom_key_file);

        const int value_length = strlen(value);
        custom_key_file = malloc(value_length + 1);
        strcpy(custom_key_file, value);
    }
}


STB_MAG_Cas_t * CreateCasPlugin(STB_MAG_Cad_t *mag_interface, const char *ini_filename)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);

    if(mag_interface)
    {
        cad_interface.SetScramblingKey = mag_interface->SetScramblingKey;
        cad_interface.SetScramblingType = mag_interface->SetScramblingType;
    }

    static STB_MAG_Cas_t PluginInterface;
    PluginInterface.Decrypt = CAS_Decrypt;
    PluginInterface.Deinit = CAS_Deinit;
    PluginInterface.GetCasFlags = CAS_GetCasFlags;
    PluginInterface.GetEcmFormat = NULL;
    PluginInterface.GetSoID = NULL;
    PluginInterface.GetSysID = NULL;
    PluginInterface.OnEcm = NULL;
    PluginInterface.OnPmtChange = NULL;
    PluginInterface.ResetStream = CAS_ResetStream;
    PluginInterface.SetAdditionalParam = CAS_SetAdditionalParam;

    return &PluginInterface;
}


int GetCasApiVersion(void)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    return CAS_API_VERSION;
}


const char * GetCasPluginDescription(void)
{
    DEBUG("BISS: %s()\n", __FUNCTION__);
    return "BISS CAS plugin";
}
