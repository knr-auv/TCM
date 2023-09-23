#pragma once
#include "protocol_data_types.h"

const uint16_t COM_PROTO_CreateMsg(COM_PROTO_msg_t *const msg, uint8_t *const tx_buffer, const uint16_t tx_buffer_size);
bool COM_PROTO_ParseByte(uint8_t byte, COM_PROTO_msg_t *const msg);