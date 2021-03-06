#include <string.h>

#include "liblobaro_coap.h"
#include "option-types/coap_option_cf.h"

#include "tcpip_adapter.h"
#include "esp_log.h"
#include "parson.h"
#include "cn-cbor/cn-cbor.h"

#include "name.h"

static const char *TAG = "CoAP Resource";

CoAP_Res_t* wifi_ipconfig_resource = NULL;

static CoAP_HandlerResult_t wifi_ipconfig_requesthandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp, CoAP_Res_t* pResource)
{
	tcpip_adapter_ip_info_t ipinfo;
	char payloadTemp[250], ip_address[16], ip_mask[16], ip_gateway[16];
	char* pStrWorking = payloadTemp;

    uint16_t accept = CoAP_GetAcceptOptionValFromMsg(pReq);

	if( tcpip_adapter_get_ip_info( TCPIP_ADAPTER_IF_STA, &ipinfo ) != ESP_OK )
    {
        ESP_LOGE( TAG, "tcpip_adapter_get_ip_info: failed" );
        return HANDLER_ERROR;
    }

    if( accept == COAP_CF_TEXT_PLAIN) 
    {
        pStrWorking += sprintf( (char*) pStrWorking, "IP: " IPSTR ", Mask: " IPSTR ", Gateway: " IPSTR, 
            IP2STR( &ipinfo.ip ),
            IP2STR( &ipinfo.netmask ),
            IP2STR( &ipinfo.gw ) );
    }
    else if( accept == COAP_CF_JSON ) 
    {
        JSON_Value *root_value = json_value_init_object();
        JSON_Object *root_object = json_value_get_object(root_value);
        
        sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.ip ) );
        json_object_dotset_string(root_object, "wifi.ip", pStrWorking );
        sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.netmask ) );
        json_object_dotset_string(root_object, "wifi.mask", pStrWorking );
        sprintf( (char*) pStrWorking, IPSTR, IP2STR( &ipinfo.gw ) );        
        json_object_dotset_string(root_object, "wifi.gateway", pStrWorking );

        json_serialize_to_buffer( root_value, payloadTemp, 250 );
        json_value_free(root_value);

        CoAP_AddCfOptionToMsg( pResp, COAP_CF_JSON );        
    }
    else if( accept == COAP_CF_CBOR ) 
    {
        cn_cbor *map, *wifi;
        size_t enc_sz;

        wifi = cn_cbor_map_create(NULL);
        
        sprintf( (char*) ip_address, IPSTR, IP2STR( &ipinfo.ip ) );
        cn_cbor_map_put(wifi,
            cn_cbor_string_create("ip", NULL),
            cn_cbor_string_create(ip_address, NULL),
            NULL
        );
        sprintf( (char*) ip_mask, IPSTR, IP2STR( &ipinfo.netmask ) );
        cn_cbor_map_put(wifi,
            cn_cbor_string_create("mask", NULL),
            cn_cbor_string_create(ip_mask, NULL),
            NULL
        );

        sprintf( (char*) ip_gateway, IPSTR, IP2STR( &ipinfo.gw ) );
        cn_cbor_map_put(wifi,
            cn_cbor_string_create("gateway", NULL),
            cn_cbor_string_create(ip_gateway, NULL),
            NULL
        );

        map = cn_cbor_map_create(NULL);
        cn_cbor_map_put( map, cn_cbor_string_create("wifi", NULL), wifi, NULL );
        
        enc_sz = cn_cbor_encoder_write( (uint8_t*) payloadTemp, 0, 250, map);
        cn_cbor_free( map );
        
        CoAP_AddCfOptionToMsg( pResp, COAP_CF_CBOR );
        CoAP_SetPayload( pResp, (uint8_t*) payloadTemp, enc_sz, true );
        return HANDLER_OK;
    }
    else
    {
        pResp->Code = RESP_BAD_OPTION_4_02;
        return HANDLER_ERROR;
    }

    CoAP_SetPayload( pResp, (uint8_t*) payloadTemp, strlen( payloadTemp ), true );
    return HANDLER_OK;
}

void coap_create_resources( void )
{
    CoAP_ResOpts_t Options = (CoAP_ResOpts_t) {
        .Cf = COAP_CF_TEXT_PLAIN, 
        .AllowedMethods = RES_OPT_GET 
    };
    
    wifi_ipconfig_resource = CoAP_CreateResource( "wifi/ipconfig", "ESP32 IP configuration", Options, wifi_ipconfig_requesthandler, NULL );

    if( wifi_ipconfig_resource == NULL)
        ESP_LOGE( TAG, "CoAP_CreateResource returned NULL" );    
}