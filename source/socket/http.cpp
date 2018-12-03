#include "common/runtime.h"
#include "socket/luasocket.h"

#include "socket/http.h"

// http.request{
//   url = string,
//   [sink = LTN12 sink,]
//   [method = string,]
//   [headers = header-table,]
//   [source = LTN12 source],
//   [step = LTN12 pump step,]
//   [proxy = string,]
//   [redirect = boolean]
// }

/*static size_t HeaderCallback(char * contents, size_t size, size_t nmemb, void * userdata)
{
    size_t realsize = size * nmemb;
    HTTPChunk * chunk = (HTTPChunk *)userdata;

    chunk->header = (char *)realloc(chunk->header, chunk->size + realsize + 1);

    if (chunk->header == NULL)
    {
        //out of memory!
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(chunk->header[chunk->size]), contents, realsize);
    chunk->size += realsize;
    chunk->header[chunk->size] = 0;

    return realsize;
}*/

static size_t WriteMemoryCallback(void * contents, size_t size, size_t nmemb, void * userdata)
{
    size_t realsize = size * nmemb;
    HTTPChunk * chunk = (HTTPChunk *)userdata;

    chunk->buffer = (char *)realloc(chunk->buffer, chunk->size + realsize + 1);

    if (chunk->buffer == NULL)
    {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(chunk->buffer[chunk->size]), contents, realsize);
    chunk->size += realsize;
    chunk->buffer[chunk->size] = 0;

    return realsize;
}

int httpRequest(lua_State * L)
{
    const char * host = luaL_checkstring(L, 1);

    CURL * curl;
    CURLcode res;
    //CURLINFO info; TODO: header response or whatever

    HTTPChunk chunk;
    
    chunk.buffer = (char *)malloc(1);  /* will be grown as needed by the realloc above */ 
    chunk.size = 0;    /* no data at this point */ 
 
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, host);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    if (lua_istable(L, 2)) { /*curl_easy_setopt(curl, CURLOPT_HEADER, 1L);*/ }

    /* send all data to this function  */ 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "LovePotion-agent/1.0");

    //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback); 

    //curl_easy_setopt(curl, CURLOPT_HEADERDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK || chunk.size == 0)
        lua_pushnil(L);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &chunk.code);
    
    lua_pushlstring(L, chunk.buffer, chunk.size);
    lua_pushnumber(L, chunk.code);

    /*lua_newtable(L);

    lua_pushnumber(L, 1);
    lua_pushstring(L, chunk.header);
    
    lua_settable(L, -3);*/

    curl_easy_cleanup(curl);
    
    free(chunk.buffer);

    return 2;
}