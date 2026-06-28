/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief SHIP Connection client state handling function declarations
 */

#ifndef SRC_SHIP_SHIP_CONNECTION_CLIENT_H_
#define SRC_SHIP_SHIP_CONNECTION_CLIENT_H_

#include "src/ship/ship_connection/ship_connection_internal.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief State machine for handling client states
 * @param self SHIP Connection insatnce to handle the state with
 */
void ClientHandleState(ShipConnection* self);

/**
 * @brief CMI_STATE_CLIENT_SEND state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void CmiStateClientSend(ShipConnection* self);

/**
 * @brief CMI_STATE_CLIENT_WAIT state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void CmiStateClientWait(ShipConnection* self);

/**
 * @brief CMI_STATE_CLIENT_EVALUATE state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void CmiStateClientEvaluate(ShipConnection* self);

/**
 * @brief SME_PROT_H_STATE_CLIENT_INIT state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void SmeProtHandshakeStateClientInit(ShipConnection* self);

/**
 * @brief SME_PROT_H_STATE_CLIENT_LISTEN_CHOICE state handling
 * @param self SHIP Connection insatnce to handle the state with
 */
void SmeProtHandshakeStateClientListenChoice(ShipConnection* self);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_SHIP_SHIP_CONNECTION_CLIENT_H_
